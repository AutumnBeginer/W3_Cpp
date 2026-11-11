#include <stdio.h>
#include <stdlib.h>

typedef struct Date{
    int year, month ,day;
    /* 👉 函数指针 = C 版的"成员函数"。注意第一个参数必须自己传 self！ */
    void (*print)(const struct Date *self);// 这就是被藏起来的 this
    void (*addDays)(struct Date *self ,int n);
    int (*toOrdinal)(const struct Date *self);
}Date;

static void d_print(const Date *self){
    printf("%04d-%02d-%02d\n",self->year,self->month,self->day);
}
static int d_ordinal(const Date *self) {          // 简化：算年内第几天
    static const int md[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    int s=0; for (int m=1;m<self->month;m++) s+=md[m];
    return s + self->day;
}

static void d_addDays(Date *self,int n){// 简版：只加日，不进位月（周三再完善）
    self->day+=n;
    printf("[addDays]-> %04d-%02d-%02d\n",self->year,self->month,self->day);
}

/* 👉 这就是"构造函数"：分配 + 初始化 + 缝上函数指针，一步都不能忘 */
Date* date_new(int y, int m, int d) {
    Date *p = (Date*)malloc(sizeof(Date));
    if (!p) { perror("malloc"); exit(1); }       // 第 2 周：malloc 必判 NULL
    p->year=y; p->month=m; p->day=d;
    p->print=d_print; p->addDays=d_addDays; p->toOrdinal=d_ordinal;  // ← 手工绑"虚函数表"
    printf("[构造] %p\n", (void*)p);
    return p;
}
/* 👉 "析构函数"：C 里必须显式调，忘了就泄漏 */
void date_delete(Date *p) {
    if (!p) return;
    printf("[析构] %p\n", (void*)p);
    free(p);                                      // 只 free 一次！(对比今天的 BadString 双 free)
}

int main(void) {
    Date *a = date_new(2026,3,2);
    a->print(a);                                  // ⚠️ 得把 a 自己再传一遍 ← C++ 替你省的就是这个
    printf("年内第 %d 天\n", a->toOrdinal(a));
    a->addDays(a, 5);

    /* 💀 C 的两大坑，今天和 BadString 正好对上： */
    Date stack_copy = *a;                          // 浅拷贝：结构体比特复制，函数指针也一起抄（这个无害）
    printf("栈副本: "); stack_copy.print(&stack_copy);

    Date *b = a;                                   // ⚠️ 指针别名：两个指针指同一块
    date_delete(a);
    date_delete(b);                             // 💥 取消注释 = 今天的 BadString 双 free，同一个坑
    b = NULL;                                      // C 的土办法：手动置空防野指针（第 2 周周二你写过）

    /* 忘了 date_new 直接用的下场：函数指针是垃圾 → 调用即崩 */
    Date zombie;                                   // 未初始化
    zombie.print(&zombie);                      // 💥 取消注释试试：Segmentation fault
    return 0;
}