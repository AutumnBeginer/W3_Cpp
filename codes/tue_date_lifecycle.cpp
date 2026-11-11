#include <iostream>
#include <iomanip>
using namespace std;

class Date
{
private:
    int year_, month_, day_;
    static int alive_;                     // 👈 明天周四正式讲 static，今天先借用计数

    bool isValid(int y,int m,int d) const {
        static const int md[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
        bool leap = (y%4==0&&y%100!=0)||y%400==0;
        return m>=1&&m<=12 && d>=1 && d<=(m==2&&leap?29:md[m]);
    }
    void sync(int y,int m,int d) {         // 构造间的公共校验
        if (isValid(y,m,d)) { year_=y; month_=m; day_=d; }
        else { cerr<<"[警告] 非法 "<<y<<"-"<<m<<"-"<<d<<" → 回退 1970-01-01\n";
               year_=1970; month_=1; day_=1; }
    }

public:
    Date(int y=1970,int m=1,int d=1) : year_(y),month_(m),day_(d) {
        sync(y,m,d); ++alive_;
        cout<<"[构造]   ";
        this->print();
        cout<<"  存活="<<alive_<<"\n";
    }
    Date(const Date &o) : year_(o.year_), month_(o.month_), day_(o.day_) {  // ✅ 显式写拷贝构造
        ++alive_; cout<<"[拷贝构造] 来自 "<<o.year_<<"-"<<o.month_<<"-"<<o.day_<<"  存活="<<alive_<<"\n";
    }
    ~Date() { --alive_; cout<<"[析构]   "<<year_<<"-"<<month_<<"-"<<day_<<"  存活="<<alive_<<"\n"; }

    // Date& operator=(const Date& rhs) {
    //     if (this == &rhs) {                     // ① 自赋值检查
    //         cout << "[自赋值跳过]\n";
    //         return *this;
    //     }
    //     // ② 拷贝数据（对于 Date 类，只有 int，无需释放旧资源）
    //     year_ = rhs.year_;
    //     month_ = rhs.month_;
    //     day_  = rhs.day_;
    //     // ③ 注意：alive_ 不增减，因为赋值不创建也不销毁对象
    //     cout << "[拷贝赋值] " << rhs.year_ << "-" << rhs.month_ << "-" << rhs.day_
    //      << "  存活=" << alive_ << "\n";
    //     return *this;                           // ④ 返回 *this 支持链式赋值
    // }
    // 上周四的 const 成员函数，今天直接复用
    void print() const { cout<<year_<<"-"<<setfill('0')<<setw(2)<<month_<<"-"<<setw(2)<<day_<<setfill(' '); }
    int year() const{return year_;} int month() const{return month_;} int day() const{return day_;}
    static int alive() { return alive_; }   // 明天细讲
};

   int Date::alive_ = 0;                        // 👈 static 成员：类外定义 + 初始化（明天详解）

// 按值传参 → 触发拷贝构造；const 引用传参 → 不拷贝（第 2 周周三的知识复活）
void byValue(Date d)      { cout<<"  byValue  内 "; d.print(); cout<<"\n"; }
void byRef(const Date &d) { cout<<"  byRef    内 "; d.print(); cout<<"  (零拷贝)\n"; }
Date makeDate(int y)      { return Date(y, 6, 1); }   // 返回值：可能被 RVO 吃掉

    

int main() {
    cout<<"=== 1. 三种出生 ===\n";
    Date a(2026,3,2);          // 有参构造
    Date b;                    // 默认构造（走默认实参）
    Date c = a;                // ★ 拷贝构造（有 = 但不是赋值！）

    cout<<"\n=== 2. 传参方式对比 ===\n";
    byValue(a);                // ★ 拷贝一次 + 函数结束时析构一次
    byRef(a);                  // ★ 一次拷贝都没有

    cout<<"\n=== 3. 返回值 ===\n";
    Date d = makeDate(2000);   // 拷贝（或 C++17 强制 RVO 直接构造到 d 上）

    cout<<"\n=== 4. 堆上对象：delete 才析构 ===\n";
    Date *p = new Date(1999,12,31);
    delete p;                  // 立刻析构
    cout<<"--- p 已死，main 即将结束 ---\n";
    cout<<"当前存活 = "<<Date::alive()<<"\n";
    // cout << "\n=== 5. 拷贝赋值 vs 拷贝构造 ===\n";
    // Date x(2025, 12, 25);
    // Date y(2026, 1, 1);
    // cout << "赋值前：x = "; x.print(); cout << ", y = "; y.print(); cout << "\n";
    // x = y;                      // 调用 operator=
    // cout << "赋值后：x = "; x.print(); cout << "\n";

    // cout << "\n自赋值测试：\n";
    // x = x;                      // 应当触发自赋值检查
    return 0;                  // 析构 d → c → b → a（严格逆序）
}