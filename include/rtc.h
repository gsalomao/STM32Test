/**
 * @brief   Module which manages the Real Time Counter (RTC) and the
 *          periodic alarm.
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>

typedef void (*RTC_Callback_t)(void);

void RTC_Init(void);
void RTC_SetPeriodicAlarm(uint32_t periodicity, RTC_Callback_t callbackFromISR);

#endif /* RTC_H_ */
