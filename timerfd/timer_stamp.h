#pragma once
#include <string>
#include <iostream>

#define LOG(ERROR) std::cout

class TimerStamp
{
public:
    TimerStamp()
        :micro_seconds_since_epoch_(0)
    {
    }
    explicit TimerStamp(int64_t MicroSecondsSinceEpoch)
        :micro_seconds_since_epoch_(MicroSecondsSinceEpoch)
    {
    }
    static TimerStamp NowTime();
    static TimerStamp Invalid()
    {
        return TimerStamp();
    }
    bool Valid() const { return micro_seconds_since_epoch_ > 0; }

    std::string ToString()const;
    std::string ToFormattedString(bool showMicroseconds = true) const;

    int64_t MicroSecondsSinceEpoch() const { return micro_seconds_since_epoch_; }

    static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
    int64_t micro_seconds_since_epoch_;
};

inline bool operator<(TimerStamp lhs, TimerStamp rhs)
{
    return lhs.MicroSecondsSinceEpoch() < rhs.MicroSecondsSinceEpoch();
}

inline bool operator==(TimerStamp lhs, TimerStamp rhs)
{
    return lhs.MicroSecondsSinceEpoch() == rhs.MicroSecondsSinceEpoch();
}

///
/// Gets time difference of two timestamps, result in seconds.
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microsecond
/// resolution for next 100 years.
inline double TimeDifference(TimerStamp high, TimerStamp low)
{
    int64_t diff = high.MicroSecondsSinceEpoch() - low.MicroSecondsSinceEpoch();
    return static_cast<double>(diff) / TimerStamp::kMicroSecondsPerSecond;
}


/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as TimerStamp
inline TimerStamp AddTime(TimerStamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * TimerStamp::kMicroSecondsPerSecond);
    return TimerStamp(timestamp.MicroSecondsSinceEpoch() + delta);
}