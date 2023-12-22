#pragma once

#include <set>
#include <unordered_map>
#include <atomic>
#include "timer.h"
#include "timer_channel.h"

class Timer;

class TimerQueue
{
public:
    explicit TimerQueue(std::shared_ptr<EpollTimer> ep_ptr);
    ~TimerQueue();

    int64_t AddTimer(TimerCallback cb, TimerStamp when, double interval);
    void StopTimer(int64_t timerId);

private:
    using Entry = std::pair<TimerStamp, Timer*>;
    using TimerList = std::set<Entry>;

    void AddTimerInLoop(Timer* timer);
    void CancelInLoop(int64_t timerId);

    void HandleRead();    //在这个函数内部执行定时器的任务
    std::vector<Entry> GetExpired(TimerStamp NowTime);        //得到所有的超时定时器
    void Reset(const std::vector<Entry>& expired, TimerStamp NowTime);
    bool insert(Timer* timer);    //插入该定时器

private:
    const int timerfd_;
    ChannelTimer timerfd_channel_;
    TimerList timers_;        //这是为了 方便查找超时定时器 的容器
    std::unordered_map<int64_t, Timer*> active_timers_;        //这是为了方便进行删除的容器，以定时器唯一的标识符sequeue为key
    std::unordered_map<int64_t, Timer*> cancelingTimers_;    //为了解决定时器正在执行，却想删除该定时器的问题的
    std::atomic_bool callingExpiredTimers_;        //标识定时器是否在执行

};