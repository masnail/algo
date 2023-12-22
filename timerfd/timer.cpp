#include "timer.h"

std::atomic_int64_t Timer::num_created_;

void Timer::ReStart(TimerStamp NowTime)
{
    if (repeat_) {
        expiration_ = AddTime(NowTime, interval_);
    }
    else {
        expiration_ = TimerStamp::Invalid();
    }
}

