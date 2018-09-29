/**
 * @brief   Module which manages the Real Time Counter (RTC) and the
 *          periodic alarm.
 */

#include "rtc.h"
#include "assert.h"
#include "stm32f4xx_hal.h"

/* RTC prescale settings to get a 1Hz clock (32KHz / (124 + 1) / (255 + 1)) */
#define RTC_ASYNCH_PREDIV               0x7C
#define RTC_SYNCH_PREDIV                0x00FF

/* Wake-up prescaled clock (32KHz / 16) */
#define RTC_WUT_PRESC_CLOCK_HZ          2000
#define RTC_SPRE_CLOCK_HZ               1

/* Maximum periodicity which the 2KHz shall be used. Any periodicity above this
 * shall use the 1Hz clock */
#define RTC_MAX_PERIORICITY_PRSC_CLK    10000

#define RTC_MSEC_TO_TICK(msec, clock)   ((msec * clock) / 1000)

static RTC_HandleTypeDef rtcHandle;
static RTC_Callback_t rtcCallbackFromISR;

/**
 * Initialize the Real Time Counter and clean the periodic alarm.
 */
void RTC_Init(void)
{
    rtcHandle.Instance = RTC;
    rtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    rtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    rtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    rtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    rtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    rtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    rtcHandle.State = HAL_RTC_STATE_RESET;

    ASSERT(HAL_RTC_Init(&rtcHandle) == HAL_OK);
}

/**
 * Set the periodic alarm to a given periodicity and the callback which shall
 * be called whenever the alarm expires.
 *
 * @param   periodicity         Periodicity in milliseconds which the alarm
 *                              shall expires.
 *
 * @param   callbackFromISR     Callback which shall be called whenever the
 *                              alarm expires. Note that this callback will
 *                              be called within an Interrupt Service
 *                              Routine (ISR).
 */
void RTC_SetPeriodicAlarm(uint32_t periodicity, RTC_Callback_t callbackFromISR)
{
    uint32_t reloadValue;
    uint32_t clock;

    ASSERT(callbackFromISR);
    rtcCallbackFromISR = callbackFromISR;

    if (periodicity > RTC_MAX_PERIORICITY_PRSC_CLK)
    {
        reloadValue = RTC_MSEC_TO_TICK(periodicity, RTC_SPRE_CLOCK_HZ);
        clock = RTC_WAKEUPCLOCK_CK_SPRE_16BITS;
    }
    else
    {
        reloadValue = RTC_MSEC_TO_TICK(periodicity, RTC_WUT_PRESC_CLOCK_HZ);
        clock = RTC_WAKEUPCLOCK_RTCCLK_DIV16;
    }

    ASSERT(HAL_RTCEx_SetWakeUpTimer_IT(&rtcHandle, reloadValue, clock)
                == HAL_OK);
}

/**
 * Initialize the RTC clock and oscillator.
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef *rtc)
{
    RCC_OscInitTypeDef oscInit;
    RCC_PeriphCLKInitTypeDef clkInit;

    oscInit.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    oscInit.PLL.PLLState = RCC_PLL_NONE;
    oscInit.LSEState = RCC_LSE_OFF;
    oscInit.LSIState = RCC_LSI_ON;

    ASSERT(HAL_RCC_OscConfig(&oscInit) == HAL_OK);

    clkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    clkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

    ASSERT(HAL_RCCEx_PeriphCLKConfig(&clkInit) == HAL_OK);

    __HAL_RCC_RTC_ENABLE();

    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    rtcCallbackFromISR();
}
