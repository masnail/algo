/*
*   线程池头文件
*/

#pragma once

#include <vector>
#include <map>
#include <atomic>
#include <future>
#include <condition_variable>
#include <thread>
#include <functional>
#include <stdexcept>
#include <list>

using namespace std;


// Eg: ThreadPool::GetInstance()->AddTask(closeCallback_);


// 线程池最大容量,应尽量设小一点
#define THREADPOOL_MAX_NUM 16

// 线程池,可以提交变参函数或拉姆达表达式的匿名函数执行,可以获取执行返回值
// 不直接支持类成员函数, 支持类静态成员函数或全局函数,Opteron()函数等

/************************** 使用std::list作为队列 ************************************/
class ThreadPool
{
private:
    using Task = function<void()>;                                         // 定义类型
    vector<thread> pool_;                                                  // 线程池
    list<Task> tasks_;                                                     // 任务队列
    mutex lock_;                                                           // 同步
    condition_variable task_cv_;                                           // 条件阻塞
    atomic<bool> run_{true};                                               // 线程池是否执行
    atomic<int> idlThrNum_{0};                                             // 空闲线程数量

public:
    ThreadPool() {}

    static ThreadPool* GetInstance() {
        static ThreadPool thread_pool_single;
        return &thread_pool_single;
    }
    void Destory() {
        run_ = false;
        task_cv_.notify_all(); // 唤醒所有线程执行
        for (thread &thread : pool_)
        {
            // thread.detach(); // 让线程“自生自灭”
            if (thread.joinable())
                thread.join(); // 等待任务结束， 前提：线程一定会执行完
        }
    }

private:
    ThreadPool(const ThreadPool&) = default;
    // inline ThreadPool(unsigned short size = 4) { addThread(size); }
    ~ThreadPool() = default;
    ThreadPool& operator=(const ThreadPool&) = delete;

public:
    // 提交一个任务
    // 调用.get()获取返回值会等待任务执行完,获取返回值
    // 有两种方法可以实现调用类成员，
    // 一种是使用   bind： .AddTask(std::bind(&Dog::sayHello, &dog));
    // 一种是用   mem_fn： .AddTask(std::mem_fn(&Dog::sayHello), this)
    template <class F, class... Args>
    auto AddTask(F &&f, Args &&...args) -> future<decltype(f(args...))>
    {
        if (!run_) // stoped ??
            throw runtime_error("AddTask on ThreadPool is stopped.");

        // DEBUG_INFO("add pool");

        using RetType = decltype(f(args...)); // typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
        auto task = make_shared<packaged_task<RetType()>>(
            bind(forward<F>(f), forward<Args>(args)...)); // 把函数入口及参数,打包(绑定)
        future<RetType> future = task->get_future();
        // DEBUG_INFO("add thread pool task.1");
        {
            // 添加任务到队列
            lock_guard<mutex> lock{lock_}; // 对当前块的语句加锁  lock_guard 是 mutex 的 stack 封装类，构造的时候 lock()，析构的时候 unlock()
            tasks_.emplace_back(
                [task]() { // push(Task{...}) 放到队列后面
                    (*task)();
                });
        }
        // DEBUG_INFO("add thread pool task.2");
        task_cv_.notify_one(); // 唤醒一个线程执行

        return future;
    }

    // 空闲线程数量_run
    int IdlCount() { return idlThrNum_; }
    // 线程数量
    int ThrCount() { return pool_.size(); }
    // 当前任务的总数
    int TaskCount() { return tasks_.size(); }
    // 添加指定数量的线程
    void InitThreadSize(unsigned short size = 4)
    {
        for (; pool_.size() < THREADPOOL_MAX_NUM && size > 0; --size)
        {                                      // 增加线程数量,但不超过 预定义数量 THREADPOOL_MAX_NUM
            pool_.emplace_back([this, size] { // 工作线程函数
                while (run_)
                {
                    Task task; // 获取一个待执行的 task
                    // DEBUG_INFO("run thread pool task.1");
                    {
                        // unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()
                        unique_lock<mutex> lock{lock_};
                        task_cv_.wait(lock, [this]
                                      { return !run_ || !tasks_.empty(); }); // wait 直到有 task
                        if (!run_ && tasks_.empty())
                            return;
                        task = move(tasks_.front()); // 按先进先出从队列取一个 task

                        // std::__cxx11::list<pair<int,Task>>::iterator
                        tasks_.pop_front();
                    }
                    // DEBUG_INFO("run thread pool task.2");
                    idlThrNum_--;
                    task(); // 执行任务
                    idlThrNum_++;
                }
            });
            idlThrNum_++;
        }
    }
};
