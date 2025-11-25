#include <iostream>
#include "Head.h"
using namespace std;


int main(){
/*======================= 实现智能指针 ========================*/
    cout << "<=========== 实现智能指针 ===========>" << endl;
    #if 0
        my_unique_ptr();
        cout << endl;
        my_shared_ptr();
        cout << endl;
    #endif

/*======================= 实现线程池 ========================*/
    cout << "<=========== 实现线程池 ===========>" << endl;
    #if 0
        threadpool();
        cout << endl;
    #endif

/*======================= 生产者消费者模型 ========================*/
    cout << "<=========== 生产者消费者模型 ===========>" << endl;
    #if 1
        producerConsumer();
        cout << endl;
    #endif

}