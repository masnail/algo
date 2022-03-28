
#include "token_bucket_limiter.h"
#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <thread>

using std::chrono::system_clock;
using std::chrono::microseconds;
using std::chrono::duration_cast;

namespace ratelimiter {

TokenBucketRateLimiter::TokenBucketRateLimiter()
    :max_permits_(1), stored_permits_(1), next_free_(0) {
        interval_ = 1000000.0 / max_permits_;
    }

void TokenBucketRateLimiter::Init(int timewindow, double max_permits) {
    if (max_permits <= 0) {
        max_permits_ = 1;
    } else {
        max_permits_ = max_permits;
    }
    stored_permits_ = max_permits_;
    // interval_ = 1000000.0 / max_permits;
    interval_ = timewindow*1000 / max_permits;
}

int32_t TokenBucketRateLimiter::Aquire(double permits, uint64_t now) {
    if (permits <= 0) {
    }

    std::lock_guard<std::mutex> lock(mut_);
    auto wait_time = ClaimNext(permits, now);
    // std::this_thread::sleep_for(wait_time);
    return !(wait_time.count() / 1000.0);
}

void TokenBucketRateLimiter::Sync(uint64_t now) {
    if (now > next_free_) {
        double step = (now - next_free_) / interval_;
        stored_permits_ = std::min(max_permits_, stored_permits_ + step);
        next_free_ = now;
    }
}

std::chrono::microseconds TokenBucketRateLimiter::ClaimNext(double permits, uint64_t now) {
    Sync(now);

    uint64_t wait = next_free_ - now;

    double stored = std::min(permits, stored_permits_);
    double fresh = permits - stored;

    uint64_t next_free = fresh * interval_;

    next_free_ += next_free;
    stored_permits_ -= stored;

    return microseconds(wait);
}

double TokenBucketRateLimiter::GetRate() const {
    return max_permits_;
}

}  //  namespace ratelimiter
