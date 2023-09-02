#include <iostream>
#include <thread>
#include "token_bucket_limiter.h"

using namespace std;

struct OrderRequest {
    unsigned int reqTimestamp;
    OrderRequest(int reqt):reqTimestamp(reqt){}
};

class OrderController
{
 public:
    OrderController(int timeWindow/*ms*/, int upperLimit){
        token_bucket.Init(timeWindow, upperLimit);
    }
    ~OrderController() {}
    bool allow(const OrderRequest& order) {
        // true 允许;false,不允许
        // param @1:    请求个数
        // param @2:    当前时间戳，单位纳秒
        bool ret = token_bucket.Aquire(1, order.reqTimestamp*1000);
        cout << ret << endl;
        return ret;
    }

private:
 ratelimiter::TokenBucketRateLimiter token_bucket;
};

int main() {
    //  假设在3ms内，最多1个请求
    //  每3ms为一个窗口
    OrderController oc(3, 1);
    oc.allow(1);    //  第 1 窗口，当前1ms，通过
    oc.allow(4);    //  第 2 窗口，当前4ms，通过
    oc.allow(1);    //  第 3 窗口，当前1，不通过
    oc.allow(3);    //  不通过
    oc.allow(5);    //  不通过
    oc.allow(6);    //  不通过
    oc.allow(7);    //  不通过
    oc.allow(8);    //  不通过
    oc.allow(9);    //  不通过
    oc.allow(10);   //  不通过
    oc.allow(1);    //  不通过
    return 0;
}