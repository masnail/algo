#pragma once

#include <functional>
#include <atomic>
#include "timer_stamp.h"

using TimerCallback = std::function<void()>;
class Timer
{
public:
    Timer(TimerCallback cb, TimerStamp when, double interval)
        :callback_(std::move(cb))
        ,expiration_(when)
        ,interval_(interval)
        ,repeat_(interval>0.0)
        ,sequence_(num_created_++)
    {}

    void Start()const { callback_(); }

    TimerStamp Expiration() const { return expiration_; }
    bool RepeatTimer() const { return repeat_; }
    int64_t SequenceTime() const { return sequence_; }
    void ReStart(TimerStamp NowTime);
    static int64_t NumCreated() { return num_created_; }

private:
    const TimerCallback callback_;    //定时器执行的事件
    TimerStamp expiration_;            //定时器的执行时间
    const double interval_;            //重复定时器执行事件的间隔时间，若是一次性定时器，该值为 0.0
    const bool repeat_;                //用于判断定时器是否是重复循环的
    const int64_t sequence_;        //用来辨别定时器的唯一标识

    static std::atomic_int64_t num_created_;    //用来sequence_
};