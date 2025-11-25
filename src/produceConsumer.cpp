#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <condition_variable>
using namespace std;

static mutex mtx; // 全局锁，用以终端打印数据

class ProducerConsumer{
private:
    queue<int> buffer; // 共享缓冲区
    size_t capacity = 30; // 缓冲区大小
    mutex buffer_mtx; // 互斥锁
    condition_variable cond_producer; // 生产者条件变量
    condition_variable cond_consumer; // 消费者条件变量
    atomic<bool> stop_flag{false}; // 停止标志

public:
    /* 获取缓冲区大小 */
    size_t get_capacity() const{
        return capacity;
    }

    /* 获取停止标志 */
    bool get_stop_flag() const{
        return static_cast<bool>(stop_flag.load());
    }

    /* 生产者函数 */
    void produce(int data){
        unique_lock<mutex> lock(buffer_mtx);
        cond_producer.wait(lock, [this](){ return buffer.size() < capacity || stop_flag.load(); });
        if(stop_flag.load()) return;
        buffer.push(data);
        lock.unlock();

        mtx.lock();
        cout << "生产者已生产数据：" << data << endl;
        mtx.unlock();

        cond_consumer.notify_one();
    }

    /* 消费者函数 */
    void consume(){
        unique_lock<mutex> lock(buffer_mtx);
        cond_consumer.wait(lock, [this](){ return !buffer.empty() || stop_flag.load(); });
        if(stop_flag.load()) return;
        int data = std::move(buffer.front());
        buffer.pop();
        lock.unlock();

        mtx.lock();
        cout << "消费者已消费数据：" << data << endl;
        mtx.unlock();

        cond_producer.notify_one();
    }

    void stop(){
        stop_flag.store(true);
        queue<int> empty;
        swap(empty, buffer);
        cond_producer.notify_all();
        cond_consumer.notify_all();
    }
};



void producerConsumer(){
    ProducerConsumer pc;
    
    cout << "=== 开始演示生产者消费者模型 ===" << endl;

    /* 生产者线程 */
    thread producer([&pc](){
        for(int i = 0; i < 60; ++i){
            pc.produce(i);
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    });

    /* 消费者线程 */
    thread consumer([&pc](){
        while(!pc.get_stop_flag()){
            pc.consume();
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    });

    // 让系统运行一段时间
    this_thread::sleep_for(chrono::seconds(5));

    // 停止生产和消费
    cout << endl;
    cout << "=== 停止生产和消费 ===" << endl;
    pc.stop();

    // 等待所有线程完成
    producer.join();
    consumer.join();

    cout << "=== 生产者消费者模型演示结束 ===" << endl;

}