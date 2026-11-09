#include <stdio.h>
#include <stdbool.h>

struct Date 
{
    int year,month,day;
};

// 👉 这就是"手动构造函数"：C++ 会自动调的东西，C 里你得记得自己调
// bool init_date(struct Date *d, int y, int m, int d) {  // ❌ 编译错误：参数名 'd' 重复
bool init_date(struct Date *day, int y, int m, int d) {
    if (m < 1 || m > 12) return false;
    static const int md[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (d < 1 || d > md[m]) return false;
    day->year = y; day->month = m; day->day = d;   // 指针用 ->，和 C++ 一致
    return true;
}

void date_print(const struct Date *d) {      // const struct* ：周三学的写法直接用
    printf("%04d-%02d-%02d\n", d->year, d->month, d->day);
}

int main(void) {
    struct Date a;                 // ⚠️ 只是分配了内存，里面是垃圾值！
    date_print(&a);            // 还没 init 就打印 → 输出乱码，0000-563238656-32759  

    if (!init_date(&a, 2026, 3, 2)) {
        printf("init failed\n"); return 1;
    }
    date_print(&a);

    // ⚠️ C 的两大坑，C++ 构造函数正好解决：
    // 坑1：忘了调 init_date → 静默使用垃圾值，编译器一声不吭
    // 坑2：任何人都能 a.month = 99; → 没有墙，封装靠自觉

    struct Date b; 
    init_date(&b, 2026, 13, 1);  // 返回 false，但你忽略也没人管
    date_print(&b);
    b.month = 99;                                // ✅ 编译通过，对象已烂
    printf("broken: "); 
    date_print(&b);

    return 0;
}