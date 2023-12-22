#pragma once
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <sys/eventfd.h>
#include <functional>
#include "timer_stamp.h"
#include "timer.h"

class TimerQueue;
class ChannelTimer;
class EpollTimer;

class TimerEventMgr
{
public:
    using Functor = std::function<void()>;
    using channelList=std::vector<ChannelTimer*>;

private:
    TimerEventMgr(const TimerEventMgr&) = delete;
    TimerEventMgr& operator=(const TimerEventMgr&) = delete;
    ~TimerEventMgr() {}

public:
    TimerEventMgr();
    void Destory();

    //  单例模式，全局只有一个定时器管理
    static TimerEventMgr* GetInstance() {
        static TimerEventMgr timer_loop_single;
        return &timer_loop_single;
    }

public:
    void TimerLoop();

    bool IsInLoopThread()const { return threadId_ == std::this_thread::get_id(); }

    std::thread::id GetThreadId(){ return threadId_; }

    void QuitLoop();    //退出事件循环

    int64_t AddtimerAtTime(TimerStamp time, TimerCallback cb);
    int64_t AddtimerAfterTime(double delay_seconds, TimerCallback cb);
    int64_t AddtimerEveryTime(double interval_seconds, TimerCallback cb);
    void StopTimer(int64_t timerId);

private:
    void HandleRead();    //用于wake up


private:
    std::thread::id threadId_;
    std::atomic_bool quit_;    //标志退出loop循环


    channelList activeChannels_;    //保存当前活跃事件的Channel列表

    std::shared_ptr<EpollTimer> ep_ptr_;
    int wakeupFd_;
    std::unique_ptr<ChannelTimer> wakeupChannel_;
    std::unique_ptr<TimerQueue> timer_queue_;
};