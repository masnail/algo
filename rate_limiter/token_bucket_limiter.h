#pragma once

#include <mutex>
#include <iostream>

namespace ratelimiter {

class TokenBucketRateLimiter {
 public:
    TokenBucketRateLimiter();
    void Init(int timewindow, double max_permits);
    int32_t Aquire(double permits, uint64_t now);

    double GetRate() const;

 private:
    void Sync(uint64_t now);
    std::chrono::microseconds ClaimNext(double permits, uint64_t now);

 private:
    double interval_;               // 生成令牌的速率，即限速的速率值，单位：秒/个
    double max_permits_;            // 最大令牌桶容量
    double stored_permits_;         // 当前令牌桶内的令牌数

    uint64_t next_free_;            // 生成令牌的开始时间

    std::mutex mut_;
};
}  //  namespace ratelimiter
