/**
 * @brief   Assessment test for Nordsense.
 */

#include "stm32f4xx_hal.h"
#include "rtc.h"

/** Periodicity which the core will wake up to read the sensor */
#define DEFAULT_ALARM_PERIODICITY_MS       1000

typedef enum
{
    FSM_STATE_A,
    FSM_STATE_B,
    FSM_STATE_C,
    FSM_STATE_D

} FSM_STATE_t;

static inline FSM_STATE_t RunFSM(FSM_STATE_t state);
static void AlarmCallbackFromISR(void);

static volatile FSM_STATE_t g_state = FSM_STATE_A;

int main(void)
{
    while (1)
    {
        g_state = RunFSM(g_state);
    }

    return 0;
}

/**
 * Run the Finite-State Machine.
 *
 * @note        The Finite-State Machine is composed by 4 states
 *              (see @ref FSM_STATE_t).
 *
 * @param       state   Current state.
 * @returns     Next state.
 */
static inline FSM_STATE_t RunFSM(FSM_STATE_t state)
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
            /* TODO: Initialize the temperature sensor */

            state = FSM_STATE_C;
            break;

        case FSM_STATE_C: /* Read temperature sensor */
            /* TODO: Read temperature from sensor */

            /* TODO: Store temperature in circular buffer */

            state = FSM_STATE_D;
            break;

        case FSM_STATE_D: /* Sleep wait for next cycle */
            HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
            break;
    }

    return state;
}

/**
 * Callback which is called by the periodic alarm.
 *
 * @note    This callback is called within an ISR context.
 */
static void AlarmCallbackFromISR(void)
{
    g_state = FSM_STATE_C;
}
