#include <iostream>
#include "timer.h"
#include "timer_loop_mgr.h"
#include "timer_queue.h"
#include "timer_channel.h"

void myprintTest(const char* msg)
{
    printf(" %s  NowTime:%s\n",msg, TimerStamp::NowTime().ToFormattedString().c_str());
}
 
int main()
{
    TimerEventMgr::GetInstance()->AddtimerAfterTime(1, []() {myprintTest("once1"); });
    TimerEventMgr::GetInstance()->AddtimerAfterTime(1.5, []() {myprintTest("once1.5"); });
    TimerEventMgr::GetInstance()->AddtimerAfterTime(2.5, []() {myprintTest("once2.5"); });
    TimerEventMgr::GetInstance()->AddtimerAfterTime(4.8, []() {myprintTest("once4.8"); });
    TimerEventMgr::GetInstance()->AddtimerEveryTime(2, []() {myprintTest("every2"); });
    TimerEventMgr::GetInstance()->AddtimerEveryTime(5, []() {myprintTest("every5"); });
    
    TimerEventMgr::GetInstance()->AddtimerEveryTime(7, []() {myprintTest("every7"); });
    

    getchar();

    return 0;
}