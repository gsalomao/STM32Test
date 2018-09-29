/**
 * @brief   Assessment test for Nordsense.
 */

#include "stm32f4xx_hal.h"
#include "rtc.h"
#include "lis2de12.h"

/** Periodicity which the core will wake up to read the sensor */
#define DEFAULT_ALARM_PERIODICITY_MS       1000

typedef enum
{
    FSM_STATE_A,
    FSM_STATE_B,
    FSM_STATE_C,
    FSM_STATE_D

} FSM_STATE_t;

static void AlarmCallbackFromISR(void);

static volatile FSM_STATE_t state = FSM_STATE_A;

int main(void)
{
    int16_t temp;

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

                state = FSM_STATE_C;
                break;

            case FSM_STATE_C: /* Read temperature sensor */
                LIS2DE12_ReadTemp((int *) &temp);

                /* TODO: Store temperature in circular buffer */

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
