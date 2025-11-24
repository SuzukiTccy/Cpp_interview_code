#include <iostream>
#include <condition_variable>
#include <thread>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <chrono>
using namespace std;

static mutex mtx; // 全局锁，用于打印，静态是为了限定范围，只有这个文件可用


class ThreadPool{
private:
    void worker();  // 任务执行函数
    vector<thread> threadsPool; // 线程池
    queue<function<void()>> tasks_queue; // 任务队列
    condition_variable tasks_available; // 条件变量
    atomic<bool> stop_flag{false}; // 停止标志
    mutex queue_mtx; // 互斥锁

public:
    ThreadPool(size_t thread_num){
        for(size_t i = 0; i < thread_num; ++i){
            threadsPool.emplace_back(&ThreadPool::worker, this);
        }
        cout << "ThreadPool is created having " << threadsPool.size() << " threads" << endl;
    }

    /* 析构函数 */
    ~ThreadPool(){
        stop_flag.store(true);
        tasks_available.notify_all();
        for(auto& t : threadsPool){
            if(t.joinable()){
                t.join();
            }
        }
    }

    /* 将任务加入任务队列 */
    template <class F, class... Args>
    void add_task(F&& f, Args&& ...args){
        if(!f){
            cerr << "add_task error : " << "function is null" << endl;
            return;
        }
        unique_lock<mutex> lock(queue_mtx);
        tasks_queue.emplace(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        lock.unlock();

        tasks_available.notify_one();
    }

    /* 停止线程池 */
    void stop(){
        stop_flag.store(true);
        tasks_available.notify_all();
        queue<function<void()>> empty;
        queue_mtx.lock();
        swap(tasks_queue, empty);
        queue_mtx.unlock();
    }
};


void ThreadPool::worker(){
    function<void()> task;
    while(true){
        unique_lock<mutex> lock(queue_mtx);
        /*  wait()方法会一直阻塞，直到被notify_one()或者notify_all()唤醒 */
        tasks_available.wait(lock, [this](){ return !tasks_queue.empty() || stop_flag.load(); });
        if(stop_flag.load()) return;
        task = std::move(tasks_queue.front());
        tasks_queue.pop();
        lock.unlock();
        task();
    }
}



void work_function(int task_id){
    cout << endl;
    cout << "<====== Task: " << task_id << " is running =====>" << endl;
    for(int i = 0;i < 20; ++i){
        mtx.lock();
        cout << "thread id: " << this_thread::get_id() << " task_id: " << task_id \
        << " i: " << i << endl;
        mtx.unlock();
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    mtx.lock();
    cout << "thread id: " << this_thread::get_id() << " finished" << endl;
    mtx.unlock();
}




void threadpool(){
    ThreadPool pool(10);
    for(int task_id = 0; task_id < 20; ++task_id){
        pool.add_task(work_function, task_id);
        mtx.lock();
        cout << "Task_id: " << task_id << " is ready!" << endl;
        mtx.unlock();
    }
    this_thread::sleep_for(chrono::seconds(2));
    pool.stop();
    
}