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

static RTC_HandleTypeDef g_rtc_handle;
static RTC_Callback_t g_callbackFromISR;

/**
 * Initialize the Real Time Counter and clean the periodic alarm.
 */
void RTC_Init(void)
{
    g_rtc_handle.Instance = RTC;
    g_rtc_handle.Init.HourFormat = RTC_HOURFORMAT_24;
    g_rtc_handle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    g_rtc_handle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    g_rtc_handle.Init.OutPut = RTC_OUTPUT_DISABLE;
    g_rtc_handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    g_rtc_handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    g_rtc_handle.State = HAL_RTC_STATE_RESET;

    ASSERT(HAL_RTC_Init(&g_rtc_handle) == HAL_OK);
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
    uint32_t reload_value;
    uint32_t clock;

    ASSERT(callbackFromISR);
    g_callbackFromISR = callbackFromISR;

    if (periodicity > RTC_MAX_PERIORICITY_PRSC_CLK)
    {
        reload_value = RTC_MSEC_TO_TICK(periodicity, RTC_SPRE_CLOCK_HZ);
        clock = RTC_WAKEUPCLOCK_CK_SPRE_16BITS;
    }
    else
    {
        reload_value = RTC_MSEC_TO_TICK(periodicity, RTC_WUT_PRESC_CLOCK_HZ);
        clock = RTC_WAKEUPCLOCK_RTCCLK_DIV16;
    }

    ASSERT(HAL_RTCEx_SetWakeUpTimer_IT(&g_rtc_handle, reload_value, clock)
                == HAL_OK);
}

/**
 * Initialize the RTC clock and oscillator.
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef *rtc)
{
    RCC_OscInitTypeDef osc_init;
    RCC_PeriphCLKInitTypeDef clk_init;

    osc_init.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    osc_init.PLL.PLLState = RCC_PLL_NONE;
    osc_init.LSEState = RCC_LSE_OFF;
    osc_init.LSIState = RCC_LSI_ON;

    ASSERT(HAL_RCC_OscConfig(&osc_init) == HAL_OK);

    clk_init.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    clk_init.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

    ASSERT(HAL_RCCEx_PeriphCLKConfig(&clk_init) == HAL_OK);

    __HAL_RCC_RTC_ENABLE();

    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    g_callbackFromISR();
}
