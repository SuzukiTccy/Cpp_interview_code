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
    #if 1
        threadpool();
        cout << endl;
    #endif

}