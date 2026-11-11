#include <cstring>
#include <iostream>
using namespace std;

class GoodString {
private:
    char *data_;
public:
    GoodString(const char *s = "") : data_(new char[strlen(s)+1]) { strcpy(data_, s); }

    // ① 拷贝构造：深拷贝 —— 自己开新地盘，再抄内容
    GoodString(const GoodString &other) : data_(new char[strlen(other.data_)+1]) {
        strcpy(data_, other.data_);
        cout << "[拷贝构造] 深拷贝了一份\n";
    }
    // ② 拷贝赋值：先自赋值检查 → 先 new 后 delete（异常安全）→ 返回 *this
    GoodString& operator=(const GoodString &other) {
        if (this != &other) {
            cout << "[赋值前] this->data_ = " << (void*)data_ 
                 << ", other.data_ = " << (void*)other.data_ << endl;
            char *tmp = new char[strlen(other.data_)+1];
            strcpy(tmp, other.data_);
            cout << "[tmp] = " << (void*)tmp << endl;
            delete[] data_;
            data_ = tmp;
            cout << "[赋值后] data_ = " << (void*)data_ << endl;
        }
        return *this;
    }
    // ③ 析构：唯一负责释放
    ~GoodString() { delete[] data_; }

    void print() const { cout << data_ << " @" << (void*)data_ << endl; }
};

int main() {
    GoodString x("hello");
    GoodString y = x;      // 深拷贝：地址不同 ✅
    cout << "x: "; x.print();
    cout << "y: "; y.print();
    GoodString z("zzz");
    z = x = y;             // 链式赋值 ✅
    cout << "y: ";y.print();
    cout << "z: ";z.print();             // 加上这一行，看看 z 的地址

    // 👇 新增：打印最终 x 和 z 的地址
    cout << "最终 x: "; x.print();
    cout << "最终 z: "; z.print();

    return 0;
}