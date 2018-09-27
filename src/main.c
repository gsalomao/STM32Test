/**
 * @brief   Assessment test for Nordsense.
 */

#include "stm32f4xx_hal.h"

typedef enum
{
    FSM_STATE_A,
    FSM_STATE_B,
    FSM_STATE_C,
    FSM_STATE_D

} FSM_STATE_t;

static inline void RunFSM(void);

int main(void)
{
    while (1)
    {
        /* Run the Finite-State Machine */
        RunFSM();
    }

    return 0;
}

/**
 * Run the Finite-State Machine.
 *
 * @note    The Finite-State Machine is composed by 4 states
 *          (see @ref FSM_STATE_t).
 */
static inline void RunFSM(void)
{
    static FSM_STATE_t state = FSM_STATE_A;

    switch (state)
    {
        default:
        case FSM_STATE_A: /* Initialize MCU peripherals */
            HAL_Init();

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
            /* TODO: Enter in sleep mode */

            state = FSM_STATE_C;
            break;
    }
}
