#ifndef _TIMER_API_H
#define _TIMER_API_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*timer_callback_t)(void);

void* Timer_newTimer(unsigned int period, timer_callback_t callback);
void Timer_deleteTimer(void *timer);
void Timer_start(void *timer);
void Timer_stop(void *timer);
void Timer_reset(void *timer);
void Timer_changePeriod(void *timer, unsigned int period);

#ifdef __cplusplus
}
#endif


#endif
