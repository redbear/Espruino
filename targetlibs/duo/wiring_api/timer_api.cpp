#include "ticks_api.h"
#include "spark_wiring_timer.h"
#include "timer_api.h"

#ifdef __cplusplus
extern "C" {
#endif


void* Timer_newTimer(unsigned int period, timer_callback_t callback)
{
    Timer *_timer = new Timer(period, callback);
    return ((void *)_timer);
}

void Timer_deleteTimer(void *timer)
{
    delete ((Timer *)timer);
}

void Timer_start(void *timer)
{
	((Timer *)timer)->start();
}

void Timer_stop(void *timer)
{
	((Timer *)timer)->stop();
}

void Timer_reset(void *timer)
{
	((Timer *)timer)->reset();
}

void Timer_changePeriod(void *timer, unsigned int period)
{
	((Timer *)timer)->changePeriod(period);
}


#ifdef __cplusplus
}
#endif
