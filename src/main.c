/**
 * @brief   STM32 test project.
 */

#include "stm32f4xx_hal.h"
#include "rtc.h"
#include "lis2de12.h"
#include "circbuf.h"

/** Periodicity which the core will wake up to read the sensor */
#define DEFAULT_ALARM_PERIODICITY_MS            1000

/** Number maximum of temperature the buffer can hold */
#define DEFAULT_TEMPERATURE_BUFFER_SIZE         50

typedef enum
{
    FSM_STATE_A,
    FSM_STATE_B,
    FSM_STATE_C,
    FSM_STATE_D

} FSM_STATE_t;

static void AlarmCallbackFromISR(void);

static volatile FSM_STATE_t state = FSM_STATE_A;
static int16_t temperatureBuffer[DEFAULT_TEMPERATURE_BUFFER_SIZE];

int main(void)
{
    int16_t temp;
    CircularBuffer_t buffer;

    while (1)
    {
        switch (state)
        {
            default:
            case FSM_STATE_A: /* Initialize MCU peripherals */
                HAL_Init();
                RTC_Init();
                RTC_SetPeriodicAlarm(DEFAULT_ALARM_PERIODICITY_MS,
                    AlarmCallbackFromISR);

                state = FSM_STATE_B;
                break;

            case FSM_STATE_B: /* Initialize LIS2DE12TR */
                LIS2DE12_Init();
                LIS2DE12_EnableTemp();
                CircBuf_Init(&buffer, temperatureBuffer,
                        sizeof(temperatureBuffer));

                state = FSM_STATE_C;
                break;

            case FSM_STATE_C: /* Read temperature sensor */
                LIS2DE12_ReadTemp((int *) &temp);
                CircBuf_Write(&buffer, &temp, sizeof(temp));

                state = FSM_STATE_D;
                break;

            case FSM_STATE_D: /* Sleep wait for next cycle */
                HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,
                    PWR_SLEEPENTRY_WFE);
                break;
        }
    }

    return 0;
}

/**
 * Callback which is called by the periodic alarm.
 *
 * @note    This callback is called within an ISR context.
 */
static void AlarmCallbackFromISR(void)
{
    state = FSM_STATE_C;
}
