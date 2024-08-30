#include "timer_queue.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int createTimerfd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
        TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0){
        LOG(ERROR) << "Failed in timerfd_create";
    }
    return timerfd;
}

struct timespec howMuchTimeFronNow(TimerStamp when)
{
    int64_t microseconds = when.MicroSecondsSinceEpoch() - TimerStamp::NowTime().MicroSecondsSinceEpoch();
    if (microseconds < 100) {
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / TimerStamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % TimerStamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}
void resetTimerfd(int timerfd, TimerStamp Expiration)
{
    struct itimerspec new_value;
    memset(&new_value, 0, sizeof(new_value));
    new_value.it_value = howMuchTimeFronNow(Expiration);    //这个自定义函数是 获取expiration与当前时间的间隔
    int ret = ::timerfd_settime(timerfd, 0, &new_value, nullptr);    //启动定时器
    if (ret != 0) {
        LOG(ERROR) << "timerfd_settime() error";
    }
}
void readTimerfd(int timerfd, TimerStamp NowTime)
{
    uint64_t howmany;
    auto n = ::read(timerfd, &howmany, sizeof(howmany));
    //printf("TimerQueue::HandleRead()  at %s\n", NowTime.ToString().c_str());
    if (n != sizeof(howmany)){
        LOG(ERROR) << "TimerQueue::HandleRead() reads " << n << " bytes instead of 8";
    }
}
TimerQueue::TimerQueue(std::shared_ptr<EpollTimer> ep_ptr)
    // :loop_(TimerLoop)
    : timerfd_(createTimerfd())
    ,timerfd_channel_(ep_ptr,timerfd_)
{
    timerfd_channel_.SetReadCallback([this]() {HandleRead(); });
    timerfd_channel_.EnableReading();
}
TimerQueue::~TimerQueue()
{
    timerfd_channel_.DisableAll();
    timerfd_channel_.Remove();
    close(timerfd_);

    for (const auto& timer : timers_) {
        delete timer.second;
    }
}

int64_t TimerQueue::AddTimer(TimerCallback cb, TimerStamp when, double interval)
{
    auto timer = new Timer(std::move(cb), when, interval);
    AddTimerInLoop(timer);
    return timer->SequenceTime();    //返回唯一的标识
}

void TimerQueue::StopTimer(int64_t timerId)
{
    // loop_->RunInLoop([this, timerId]() {CancelInLoop(timerId); });
    CancelInLoop(timerId);
}

//在这个函数内部执行定时器的任务
void TimerQueue::HandleRead()
{
    TimerStamp NowTime(TimerStamp::NowTime());
    readTimerfd(timerfd_, NowTime);    //只是简单的调用::read()函数回应。不回应的话，就会一直触发EPOLLIN的

    //获取已超时(大于now)的定时器 ，因为已超时的定时器可能不止一个
    auto expired = GetExpired(NowTime);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    for (const auto& it : expired) {
        it.second->Start();
    }
    callingExpiredTimers_ = false;

    Reset(expired, NowTime);  //到期的定时器处理完后,需要进行重置最早的到期时间
}

void TimerQueue::AddTimerInLoop(Timer* timer)
{
    //判断插入的定时器 的到期时间是否 小于当前最快到期时间。若是，就需要重置最快的到期时间
    bool earliestChanged = insert(timer);
    if (earliestChanged) {
        resetTimerfd(timerfd_, timer->Expiration());    //重新设置到期时间
    }
}
void TimerQueue::CancelInLoop(int64_t timerId)
{
    auto it = active_timers_.find(timerId);
    if (it != active_timers_.end()) {
        timers_.erase(Entry(it->second->Expiration(), it->second));
        delete it->second;
        active_timers_.erase(it);
    } else if (callingExpiredTimers_) {  //这个else if是为了解决定时器正在执行，却想删除该定时器的问题
        cancelingTimers_.emplace(timerId, it->second);
    }

    assert(timers_.size() == active_timers_.size());    //一定要确保这两个容器的元素个数是相等的
}

//得到所有的超时定时器
std::vector<TimerQueue::Entry> TimerQueue::GetExpired(TimerStamp NowTime)
{
    Entry sentry(NowTime, reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto end = timers_.upper_bound(sentry);    //找到第一个未到期的定时器

    std::vector<Entry> expired(timers_.begin(), end);    //把[timers_.bein(),end)拷贝给expried
    timers_.erase(timers_.begin(), end);    //在timers_容器中删除

    for (const auto& it : expired) {    
        active_timers_.erase(it.second->SequenceTime());    //在active_timers_中删除
    }
    assert(timers_.size() == active_timers_.size());
    return expired;
}
bool TimerQueue::insert(Timer* timer)
{
    assert(timers_.size() == active_timers_.size());
    bool earliestChanged = false;
    auto when = timer->Expiration();    //得到参数timer的到期时间
    auto it = timers_.begin();            //获取timers_容器中的最早的到期时间
    //若容器中没有元素 或者 要增添的定时器的到期时间 < timers_容器中的最早的到期时间(10点 < 11点)
    if (it == timers_.end() || when < it->first) {
        earliestChanged = true;    //这种情况，说明需要重置最早的到期时间
    }

    //在两个容器中都添加定时器
    timers_.emplace(Entry(when, timer));
    active_timers_.emplace(timer->SequenceTime(), timer);

    assert(timers_.size() == active_timers_.size());
    return earliestChanged;
}

void TimerQueue::Reset(const std::vector<Entry>& expired, TimerStamp NowTime)
{
    for (const auto& it : expired) {
        //该定时器是重复的 && 没有在cancelingTimers_容器中找到该定时器， 就再插入到容器中
        if (it.second->RepeatTimer() &&
            cancelingTimers_.find(it.second->SequenceTime()) == cancelingTimers_.end()) {
            it.second->ReStart(NowTime);
            insert(it.second);
        }
        else {
            delete it.second;
        }
    }

    if (!timers_.empty()) {
        TimerStamp nextExpire = timers_.begin()->second->Expiration();    //获取最早的超时时间
        if (nextExpire.Valid()) {    //若时间是有效的，就以该时间进行重置最早的超时时间
            resetTimerfd(timerfd_, nextExpire);
        }
    }
}
