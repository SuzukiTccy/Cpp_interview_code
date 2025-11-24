#include <iostream>
using namespace std;


template <typename T>
class Unique_ptr{
private:
    T* ptr; // 管理的原始指针
public:
    /* 显式构造函数，接受原始指针，explicit防止隐式转换 */
    explicit Unique_ptr(T* p = nullptr) : ptr(p) {}

    /* 析构函数：释放资源 */
    ~Unique_ptr() { delete ptr; }

    /* 禁用拷贝构造函数和拷贝赋值运算符（独占所有权） */
    Unique_ptr(const Unique_ptr&) = delete;
    Unique_ptr& operator=(const Unique_ptr&) = delete;

    /* 移动构造函数和移动赋值运算符（转移所有权） */
    Unique_ptr(Unique_ptr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    Unique_ptr& operator=(Unique_ptr&& other) noexcept {
        // 处理自赋值
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    /* 重载解引用运算符 */
    T& operator*() const { return *ptr; }

    /* 重载箭头运算符 */
    T* operator->() const { return ptr; }

    /* 重载获取原始指针 */
    T* get() const { return ptr; }

    /* 释放所有权，返回原始指针，智能指针不再管理资源 */
    T* release() noexcept {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    /* 重置管理的资源 */
    void reset(T* p = nullptr) noexcept {
        if (p != ptr) {
            delete ptr;
            ptr = p;
        }
    }

    /* 检查是否拥有资源 */
    explicit operator bool() const { return ptr != nullptr; }
};




void my_unique_ptr(){
    cout << "<=========== my_unique_ptr() ===========>" << endl;
    Unique_ptr<int> ptr1(new int(10)); // 移动构造函数
    cout << "*ptr1 = " << *ptr1 << endl;
    Unique_ptr<int> ptr2 = std::move(ptr1); // 移动构造函数
    cout << "*ptr2 = " << *ptr2 << endl;
    cout << "ptr1 == nullptr: " << (ptr1.get() == nullptr) << endl;

    ptr2.reset(new int(20)); // 重置资源
    cout << "*ptr2 = " << *ptr2 << endl;

    int* ptr3 = ptr2.release(); // 释放所有权并转移资源
    cout << "*ptr3 = " << *ptr3 << endl;
    cout << "ptr2 == nullptr: " << (ptr2.get() == nullptr) << endl;

}



template <typename T>
class Shared_ptr{
private:
    T* ptr; // 管理的原始指针
    int* ref_count; // 引用计数

    // 辅助函数：增加引用计数
    void add_ref() {
        if(ref_count) (*ref_count)++;
    }

    // 辅助函数：减少引用计数，并在计数为0时释放资源
    void release_ref() noexcept{
        /* 如果ref_count为空指针，说明资源已经释放，析构函数就不会重复delete资源了 */
        if(ref_count){
            (*ref_count)--;
            if(*ref_count == 0){
                delete ptr;
                delete ref_count;
                ptr = nullptr;
                ref_count = nullptr;
            }
        }
    }

public:
    /* 显式构造函数，接受原始指针，explicit防止隐式转换 */
    explicit Shared_ptr(T* p = nullptr) : ptr(p), ref_count(p ? new int(1) : nullptr) {}

    /* 析构函数：减少引用计数，并在计数为0时释放资源 */
    ~Shared_ptr() { release_ref(); }

    /* 拷贝构造函数：共享所有权，增加引用计数*/
    Shared_ptr(const Shared_ptr& other) : ptr(other.ptr), ref_count(other.ref_count){
        // this和other的ref_count指向同一个int*，所以this执行add_ref()后，other的ref_count也会增加
        add_ref();
    }

    /* 拷贝赋值运算符：共享所有权，增加引用计数 */
    Shared_ptr& operator=(const Shared_ptr& other){
        if(this != &other){
            this->release_ref(); // 释放旧资源的所有权
            ptr = other.ptr;
            ref_count = other.ref_count;
            add_ref(); // this和other的ref_count指向同一个int*，所以this执行add_ref()后，other的ref_count也会增加
        }
        return *this;
    }

    /* 移动构造函数：转移所有权，不改变原对象的引用计数 */
    Shared_ptr(Shared_ptr&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count){
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    /* 移动赋值运算符：转移所有权，不改变原对象的引用计数 */
    Shared_ptr& operator=(Shared_ptr&& other) noexcept {
        if (this != &other) {
            this->release_ref(); // 释放旧资源的所有权
            ptr = other.ptr; // 转移所有权
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
            // other不用执行release_ref()，因为它的所有权已经被转移了
        }
        return *this;
    }

    /* 重载解引用运算符 */
    T& operator*() const { return *ptr; }

    /* 重载箭头运算符 */
    T* operator->() const { return ptr; }

    /* 获取原始指针 */
    T* get() const { return ptr; }

    /* 获取引用计数 */
    int use_count() const { return ref_count ? *ref_count : 0; }

    /* 检查是否获得唯一资源 */
    bool unique() const {return use_count() == 1; }

    /* 重置资源 */
    void reset(T* p = nullptr) noexcept{
        if (p != ptr) {
            release_ref(); // 释放旧资源的所有权
            ptr = p;
            ref_count = p ? new int(1) : nullptr;
        }
    }

    /* 释放所有权，返回原始指针，智能指针不再管理资源 */
    T* release() noexcept {
        T* temp = ptr;
        ptr = nullptr;
        ref_count = nullptr;
        return temp;
    }
};




void my_shared_ptr(){
    cout << "<=========== my_shared_ptr() ===========>" << endl;
    Shared_ptr<int> ptr1(new int(10));
    cout << "*ptr1 = " << *ptr1 << endl; // 输出：10
    cout << "ptr1.use_count = " << ptr1.use_count() << endl; // 输出：1
    cout << "ptr1.unique = " << ptr1.unique() << endl; // 输出：1
    cout << endl;

    Shared_ptr<int> ptr2 = ptr1; // 拷贝构造函数
    cout << "*ptr2 = " << *ptr2 << endl; // 输出：10
    cout << "ptr1.use_count = " << ptr1.use_count() << endl; // 输出：2
    cout << "ptr2.use_count = " << ptr2.use_count() << endl; // 输出：2
    cout << endl;

    ptr2.reset(new int(20));
    cout << "*ptr2 = " << *ptr2 << endl; // 输出：20
    cout << "ptr1.use_count = " << ptr1.use_count() << endl; // 输出：1
    cout << "ptr2.use_count = " << ptr2.use_count() << endl; // 输出：1
    cout << endl;

    Shared_ptr<int> ptr3 = std::move(ptr2); // 移动构造函数
    cout << "*ptr3 = " << *ptr3 << endl; // 输出：20
    cout << "ptr2.use_count = " << ptr2.use_count() << endl; // 输出：0
    cout << "ptr3.use_count = " << ptr3.use_count() << endl; // 输出：1
    cout << endl;

    Shared_ptr<int> ptr4(new int(30)); // 移动构造函数
    cout << "*ptr4 = " << *ptr4 << endl; // 输出：30
    cout << "ptr3.use_count = " << ptr3.use_count() << endl; // 输出：1
    cout << "ptr4.use_count = " << ptr4.use_count() << endl; // 输出：1
    cout << endl;

    ptr4 = ptr3; // 拷贝赋值运算符
    cout << "*ptr4 = " << *ptr4 << endl; // 输出：20
    cout << "ptr3.use_count = " << ptr3.use_count() << endl; // 输出：2
    cout << "ptr4.use_count = " << ptr4.use_count() << endl; // 输出：2
    cout << endl;

    ptr4.reset(); // 重置资源
    cout << "ptr3.use_count = " << ptr3.use_count() << endl; // 输出：1
    cout << "ptr4.use_count = " << ptr4.use_count() << endl; // 输出：0
    ptr4 = std::move(ptr3); // 移动赋值运算符
    cout << "*ptr4 = " << *ptr4 << endl; // 输出：20
    cout << "ptr3.use_count = " << ptr3.use_count() << endl; // 输出：0
    cout << "ptr4.use_count = " << ptr4.use_count() << endl; // 输出：1
    cout << endl;

    int *ptr5 = ptr4.release();
    cout << "*ptr5 = " << *ptr5 << endl;
    cout << "ptr4.use_count = " << ptr4.use_count() << endl;
    delete ptr5; // 手动释放资源
    cout << endl;
}