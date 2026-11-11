#include <iostream>
#include <iomanip>
using namespace std;

class Date
{
private:
    int year_, month_, day_;
    static int alive_;

    bool isValid(int y,int m,int d) const {
        static const int md[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
        bool leap = (y%4==0&&y%100!=0)||y%400==0;
        return m>=1&&m<=12 && d>=1 && d<=(m==2&&leap?29:md[m]);
    }
    void sync(int y,int m,int d) {
        if (isValid(y,m,d)) { year_=y; month_=m; day_=d; }
        else { cerr<<"[警告] 非法 "<<y<<"-"<<m<<"-"<<d<<" → 回退 1970-01-01\n";
               year_=1970; month_=1; day_=1; }
    }

    static int daysInMonth(int y, int m) {
        static const int md[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
        if (m == 2 && ((y%4==0 && y%100!=0) || y%400==0))
            return 29;
        return md[m];
    }

public:
    Date(int y=1970,int m=1,int d=1) : year_(y),month_(m),day_(d) {
        sync(y,m,d); ++alive_;
        cout<<"[构造]   ";
        this->print();
        cout<<"  存活="<<alive_<<"\n";
    }
    Date(const Date &o) : year_(o.year_), month_(o.month_), day_(o.day_) {
        ++alive_; cout<<"[拷贝构造] 来自 "<<o.year_<<"-"<<o.month_<<"-"<<o.day_<<"  存活="<<alive_<<"\n";
    }
    ~Date() { --alive_; cout<<"[析构]   "<<year_<<"-"<<month_<<"-"<<day_<<"  存活="<<alive_<<"\n"; }

    // ---------- 运算符重载 ----------
    Date& operator+=(int n) {
        while (n > 0) {
            int dim = daysInMonth(year_, month_);
            int left = dim - day_ + 1;          // 本月剩余天数（含今天）
            if (n < left) { day_ += n; n = 0; }
            else { n -= left; day_ = 1;
                   if (++month_ > 12) { month_ = 1; ++year_; } }
        }
        return *this;
    }

    bool operator==(const Date& o) const {
        return year_==o.year_ && month_==o.month_ && day_==o.day_;
    }
    bool operator<(const Date& o) const {
        if (year_ != o.year_) return year_ < o.year_;
        if (month_ != o.month_) return month_ < o.month_;
        return day_ < o.day_;
    }

    Date& operator++()       { *this += 1; return *this; }   // 前置
    Date  operator++(int)    { Date old = *this; *this += 1; return old; } // 后置

    // ---------- 原有成员 ----------
    void print() const { cout<<year_<<"-"<<setfill('0')<<setw(2)<<month_<<"-"<<setw(2)<<day_<<setfill(' '); }
    int year() const{return year_;}
    int month() const{return month_;}
    int day() const{return day_;}
    static int alive() { return alive_; }
};

int Date::alive_ = 0;

// ---------- 非成员运算符 ----------
Date operator+(Date d, int n) { d += n; return d; }
Date operator+(int n, const Date& d) { return d + n; }

std::ostream& operator<<(std::ostream& os, const Date& d) {
    os << d.year() << '-'
       << std::setfill('0') << std::setw(2) << d.month() << '-'
       << std::setw(2) << d.day() << std::setfill(' ');
    return os;
}

bool operator!=(const Date& a, const Date& b) { return !(a==b); }
bool operator<=(const Date& a, const Date& b) { return !(b<a); }
bool operator> (const Date& a, const Date& b) { return  b<a; }
bool operator>=(const Date& a, const Date& b) { return !(a<b); }

// ---------- main ----------
int main() {
    Date a(2026, 2, 27);

    cout << "1) 输出:        " << a << "\n";                 // ← 昨天只能 a.print()
    cout << "2) 加天数:      " << a + 3 << "\n";             // 期望 2026-03-02（跨月！）
    cout << "3) 对称加法:    " << 10 + a << "\n";            // 期望 2026-03-12
    cout << "4) 原对象没变:  " << a << "\n";                 // ★ 仍是 02-27（+ 不改左值）

    a += 3;
    cout << "5) += 后:       " << a << "\n";                 // 期望 03-02（自己变了）

    Date b = a;  cout << "6) a==b ?   " << boolalpha << (a==b) << "\n";  // true
    cout << "7) a<b ?    " << (a < (b+1)) << "\n";                       // true
    cout << "8) 链式流:   " << a << " <= " << b << " ? " << (a<=b) << "\n";

    cout << "9) 前置++:   " << ++a << "\n";                  // 03-03
    cout << "10) 后置++:  " << a++ << " → 再看 a: " << a << "\n"; // 先打印03-03，a变03-04

    cout << "11) 跨年:    " << Date(2024,2,28) + 1 << "\n";  // 2024 闰年 → 02-29
    cout << "12) 跨年+天: " << Date(2026,12,30) + 5 << "\n"; // 期望 2027-01-04

    // 13) ★ 验证昨天的雷没回来：临时量也能接
    const Date& r = a + 10;  cout << "13) 临时量绑 const&: " << r << "\n"; // ✅ 合法
    // Date& bad = a + 10;   // ❌ 非 const 左值引用接不了临时量，取消注释看报错

    // 14) ★ 验证 const 对象只能用 const 成员
    const Date c(2000,1,1);
    cout << "14) const对象: " << c << " , c==a ? " << (c==a) << "\n"; // ✅ 全靠尾部 const
    // c += 1;               // ❌ 取消注释：discards qualifiers，正确！

    return 0;
}