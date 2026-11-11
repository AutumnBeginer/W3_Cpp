#include <iostream>
#include <iomanip>
using namespace std;

class Date
{
private:
    int year_, month_, day_;

    bool isValid(int y, int m, int d) const
    { // 私有工具函数：外部看不见
        if (m < 1 || m > 12)
            return false;
        static const int md[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int maxDay = md[m];
        // 闰年 2 月（今天先简版，周三重载 + 时会再完善）
        if (m == 2 && (y % 4 == 0 && y % 100 != 0 || y % 400 == 0))
            maxDay = 29;
        return d >= 1 && d < maxDay;
    }

public:
    // ✅ 构造函数：初始化列表 + 合法性兜底
    Date(int y = 1970, int m = 1, int d = 1) : year_(y), month_(m), day_(d) // 顺带给了默认实参 → 兼作默认构造
    {
        if (!isValid(y, m, d))
        {
            cerr << "[警告] 非法日期 " << y << "-" << m << "-" << d
                 << "，已回退到 1970-01-01\n";
             // ★ 手动回退成员变量
            year_ = 1970;
            month_ = 1;
            day_ = 1;
        }
        cout << "[构造]" << year_ << "-" << month_ << "-" << day_ << endl;
    }
    ~Date() {
        cout << "[析构] " << year_ << "-" << month_ << "-" << day_ << endl;
    }
    // 上周四的 const 成员函数，今天直接复用
    void print() const
    {
        cout << year_ << "-" << setfill('0') << setw(2) << month_ << "-"
             << setw(2) << day_ << setfill(' ') << endl;
    }
    int year() const { return year_; } // getter：只读出口
    int month() const { return month_; }
    int day() const { return day_; }

    void setMonth(int m)
    { // setter：写出口，带校验
        if (m >= 1 && m <= 12)
            month_ = m;
        else
            cerr << "[拒绝] 月份 " << m << " 非法\n";
    }

    
};

int main()
{
    cout << "--- 1. 栈上对象 ---" << endl;
    Date a(2026, 3, 2);
    a.print();

    cout << "\n--- 2. 默认构造（走默认实参）---" << endl;
    Date b;
    b.print();

    cout << "\n--- 3. 非法日期被拦截 ---" << endl;
    Date bad(2026, 13, 99);
    bad.print();

    cout << "\n--- 4. 私有成员碰不到（取消下一行注释看报错）---" << endl;
    // a.year_ = 9999;    // ❌ 'int Date::year_' is private within this context
    a.setMonth(12); // ✅ 唯一合法通道
    a.print();

    cout << "\n--- 5. 堆上对象 ---" << endl;
    Date *p = new Date(2000, 1, 1); // 第 2 周：new 返回指针
    p->print();                     // 第 2 周：指针用 -> （和 C 的 struct 一致）
    delete p;                       // 明天讲 delete 触发析构

    return 0;
}
