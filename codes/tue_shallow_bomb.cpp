#include <cstring>
#include <iostream>
using namespace std;

// ❌ 错误示范：有裸指针成员，却不写拷贝构造
class BadString {
  private:
    char *data_;

  public:
    BadString(const char *s) {
        data_ = new char[strlen(s) + 1];// 分配足够容纳 s 的内存（+1 给 '\0'）
        strcpy(data_, s);/// 把 s 的内容复制到 data_
        cout << "[构造] " << data_ << " (data_=" << (void *)data_ << ")\n";
    }
    // 👻 没写拷贝构造 → 编译器自动生成"逐成员拷贝" → 只拷指针，不拷内容！
    ~BadString() {
        cout << "[析构] " << (data_ ? data_ : "?") << " 释放 " << (void *)data_ << endl;
        delete[] data_; // 💥 第二次析构时，同一个地址被 free 两遍
    }
    void print() const { cout << "内容: " << data_ << endl; }
};

int main() {
    BadString x("hello");
    BadString y = x; // ⚠️ 拷贝构造（编译器版）：y.data_ == x.data_  ← 同一块堆内存！
    cout << "--- 到此为止看着都正常 ---\n";
    x.print();
    y.print();
    cout << "--- 即将出作用域 ---\n";
    return 0; // 💥 析构 y → delete[] 成功；析构 x → 同一地址再 delete → 崩溃/UB
}
