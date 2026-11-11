# 📅 第 3 周 · 周二任务指导：构造函数与析构函数

> 昨天你砌好了封装那道墙，写出第一个 `Date` 类，并亲眼看到 `[构造]` 自己跳出来打印了一次。
> 今天把对象的**一生**看全：**出生**（默认/有参/拷贝构造）→ **活着** → **死亡**（析构），重点盯住两个东西：
> **拷贝构造什么时候被偷偷调用**，以及**不写它会出什么人命**。

今天是计划第二天，共 3 块任务，建议总时长 **2~3 小时**：

1. C++ 主线：四种特殊成员函数 + 生命周期 + **浅拷贝双 free 崩溃**现场复现（约 90 min）
2. C 语言穿插：`struct` + **函数指针**模拟成员函数（20 min）
3. 算法副线：LeetCode 21 合并两个有序链表（迭代 + 递归），C++ 版 + C 版各一遍（约 40 min）

写代码仍推荐 OnlineGDB 快速验证，正式产出落进 `week3/`。

------

## 第一步：构造 / 拷贝构造 / 析构（C++ 主线）

### 1.1 站在昨天的肩膀上

昨天你只写了**一个**构造函数（还顺手用默认实参兼作了默认构造），并在睡前加了一行 `~Date()` 观察顺序。
今天先说清编译器眼里**对象的一生**有几道关口：

| 关口               | 函数长相                  | 谁调用 | 什么时候                      |
| :----------------- | :------------------------ | :----- | :---------------------------- |
| 出生（空手）       | `Date()`                  | 编译器 | `Date a;`                     |
| 出生（带料）       | `Date(int,int,int)`       | 编译器 | `Date a(2026,3,2);`           |
| **克隆**           | `Date(const Date &other)` | 编译器 | `Date b = a;` / 传值 / 返回值 |
| 搬家（今天只眼熟） | `Date(Date &&other)`      | 编译器 | C++11 移动，第 5 周细讲       |
| **死亡**           | `~Date()`                 | 编译器 | 出作用域 / `delete`           |

> ⚠️ **"拷贝构造"是你不写编译器也会替你生成一个**的那个。它的默认行为是**逐成员比特拷贝（浅拷贝）**——今天的核心坑就埋在这里。

### 1.2 四种"看起来像赋值，其实是拷贝构造"的场景（必背）

```cpp
Date a(2026, 3, 2);
Date b = a;          // ✅ ① 拷贝构造（不是赋值！虽然有 = ）
foo(a);              // ✅ ② 传值进函数 → 拷贝一份实参
Date c = foo2();     // ✅ ③ 函数按值返回 → 拷贝（C++17 后常被 RVO 优化掉）
Date arr[3] = {a};   // ✅ ④ a 拷贝给 arr[0]，其余走默认构造

b = a;               // ❌ 这才是"拷贝赋值运算符 operator="（两个对象都已存在）
```

**分辨口诀**：**等号左边是不是一个"还没出生的新对象"？** 是 → 拷贝构造；否（两边都活着）→ 拷贝赋值。

### 本质区别一句话 

- **拷贝构造**：创建一个**全新的对象**，用另一个已存在的对象来初始化它。 
- **拷贝赋值**：两个对象**都已经存在**，把一个的值赋给另一个。 

### 逐个拆解 

#### ① `Date b = a;` 

- 等号左边 `b` 是**正在被创建的变量**（还没出生）。 
- 虽然用了 `=`，但它等价于 `Date b(a);` —— 调用拷贝构造函数。 
- **口诀验证**：等号左边是一个“还没出生的新对象”→ 拷贝构造。 

#### ② `foo(a);`（传值进函数） 

```cpp
void foo(Date d) { ... }
foo(a);
```

- 形参 `d` 是函数调用时才在栈上创建的局部变量，它是**新对象**。 
- 实参 `a` 的值被复制给 `d`，这个过程调用拷贝构造。 
- **为什么不是引用传递？** 因为你写的是 `Date d` 而不是 `Date &d`。传值必然产生副本。 

#### ③ `Date c = foo2();`（函数按值返回） 

```cpp
Date foo2() {
    Date temp(2026, 3, 2);
    return temp;  // 这里发生了什么？
}
Date c = foo2();
```

- 理论上，`return temp;` 会把 `temp` 拷贝构造给一个临时对象（返回值），然后这个临时对象再拷贝构造给 `c`。 
- **C++17 强制 RVO（返回值优化）**：现代编译器会直接在 `c` 的位置构造 `temp`，跳过两次拷贝。但**语义上**它仍然是拷贝构造（只是被优化掉了）。 
- **口诀验证**：`c` 是新对象，用函数的返回值初始化 → 拷贝构造。 

#### ④ `Date arr[3] = {a};` 

- 数组元素 `arr[0]` 是**新对象**，用 `a` 拷贝构造。 

- `arr[1]` 和 `arr[2]` 没有提供初始值，走**默认构造**（不是拷贝构造也不是赋值）。 

- 整个语句等于： 

  ```cpp
  Date arr[3];           // 先默认构造三个对象
  arr[0] = a;            // ❌ 你以为是这样？错！
  ```

  实际上数组初始化列表是直接初始化，不是先默认构造再赋值。所以： 

  ```cpp
  Date arr[3] = {a};     // arr[0] 拷贝构造自 a，arr[1] 和 arr[2] 默认构造
  ```

### 对比：`b = a;`（拷贝赋值） 

- `b` 和 `a` 都已经存在（之前已经构造好了）。 
- 用 `=` 时，调用的是 `operator=(const Date&)`，即拷贝赋值运算符。 
- **口诀验证**：等号两边都是活着的对象 → 拷贝赋值。

### 1.3 析构：LIFO，和构造完全相反

## 析构函数（Destructor） 

### 是什么 

析构函数是**对象生命周期的终点**。当对象即将被销毁时，编译器自动调用它，用来做**清理工作**（比如释放动态内存、关闭文件、归还锁等）。

### 长什么样 

```cpp
~Date();  // 波浪号 + 类名，无参数、无返回值、不能重载
```

### 什么时候调用 

- **栈对象**离开作用域时（`}` 处） 
- **堆对象**被 `delete` 时 
- 程序结束时（全局/静态对象） 

### 默认析构函数 

如果你不写，编译器会生成一个**什么都不做的析构函数**。对于你的 `Date` 类（成员都是内置类型 `int`），默认析构足够了，因为不需要手动释放任何资源。

### 为什么需要显式写析构？ 

当你的类**管理着动态内存、文件句柄、网络连接等资源**时，必须自己写析构来释放它们，否则会造成内存泄漏或资源泄漏。

### 在你的 `Date` 类中演示 

我们可以加一个析构函数，让它打印一条消息，方便观察生命周期：

```cpp
~Date() {
    cout << "[析构] " << year_ << "-" << month_ << "-" << day_ << endl;
}
```

运行昨天的 `main()`，你会看到每个对象在退出作用域时依次打印析构信息，并且顺序与构造相反（LIFO）。

---

```cpp
{                       // 作用域开始
    Date a(2026, 3, 2); // 构造 a
    Date b(2026, 3, 3); // 构造 b
    { Date c(2026,3,4); } // 构造 c → 立刻出作用域 → 析构 c   ← 最先死
}                       // 析构 b → 析构 a                    ← 逆序！
```

**为什么逆序？** 因为 `b` 可能在构造时用到了 `a` 的东西（比如持有 `a` 的引用/指针），先死 `a` 会让 `b` 拿着悬空引用。**后进先出**是唯一安全的销毁顺序——和栈一模一样，也正好呼应本周算法线的"栈"。

### 为什么析构顺序和构造完全相反？ 

```cpp
{                       // 作用域开始
    Date a(2026, 3, 2); // 构造 a（第一个构造）
    Date b(2026, 3, 3); // 构造 b（第二个构造）
    { Date c(2026,3,4); } // 构造 c（第三个构造）→ 出内层作用域 → 析构 c（第一个析构）
}                       // 出外层作用域 → 析构 b（第二个析构）→ 析构 a（第三个析构）
```

**构造顺序**：a → b → c

**析构顺序**：c → b → a （完全相反）

### 为什么要逆序？ 

因为 C++ 的设计原则是：**后构造的对象可能依赖于先构造的对象**。

举个例子：

```cpp
class Logger {
    ofstream file;
public:
    Logger() { file.open("log.txt"); }
    ~Logger() { file.close(); }
};

class Worker {
    Logger &log;
public:
    Worker(Logger &l) : log(l) { log.file << "Worker started\n"; }
    ~Worker() { log.file << "Worker ended\n"; }
};

int main() {
    Logger logger;       // 先构造 logger
    Worker worker(logger); // 后构造 worker，它持有 logger 的引用
    // ...
    // 析构时：必须先析构 worker（它还要写日志），再析构 logger（关闭文件）
}
```

如果先析构 `logger`，文件就被关了，`worker` 析构时试图写日志就会崩溃。**后进先出**保证了依赖关系安全。

### 和栈的关系 

- 局部对象的生命周期就是**栈**：后入栈的先出栈。 
- 你本周算法线正好学到“栈”数据结构，这就是天然的对应：**构造 = push，析构 = pop**。 

### 特殊情况 

- **静态存储期对象**（全局变量、`static` 局部变量）：析构顺序与构造顺序相反，但发生在 `main()` 结束后。 
- **动态分配的对象**（`new`/`delete`）：析构顺序由你手动控制，不存在自动 LIFO 规则。

### 1.4 💀 今天的重头戏：浅拷贝双 free 崩溃

`Date` 全是 `int`，浅拷贝无害。所以我们**故意造一个会炸的类**，亲手把坑踩一遍——这一脚踩下去，你一辈子都会记得写拷贝构造。

新建 `week3/tue_shallow_bomb.cpp`：

```cpp
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

```

编译运行：`g++ -std=c++17 -g tue_shallow_bomb.cpp -o bomb && ./bomb`

**你会看到**：两个对象的 `data_=` 打印出**完全相同**的地址；然后析构两次同一个地址，程序以 `double free or corruption` 崩掉（或静默 UB）。

```
[构造] hello (data_=0x1b611639ae0)                                                           
--- 到此为止看着都正常 ---                                                                   
内容: hello                                                                                  
内容: hello                                                                                  
--- 即将出作用域 ---                                                                         
[析构] hello 释放 0x1b611639ae0                                                              
[析构] c� 释放 0x1b611639ae0    
```



这个输出清晰地展示了**浅拷贝（shallow copy）** 带来的经典问题——**双重释放（double free）**。

### 逐步解读 

1. **[构造] hello (data_=0x1b611639ae0)** 
   `BadString x("hello")` 调用构造函数，在堆上分配了一块内存（地址 `0x1b611639ae0`），存入 `"hello"`。`x.data_` 指向这块内存。 
2. **--- 到此为止看着都正常 ---** 
   随后执行 `BadString y = x;` —— 这里触发了**编译器生成的默认拷贝构造函数**，它只是**逐字节复制**成员变量，所以 `y.data_` 也被赋值为同一个地址 `0x1b611639ae0`。此时两个对象的 `data_` 指针指向**同一块堆内存**。 
3. **两次 内容: hello** 
   `x.print()` 和 `y.print()` 都输出 `"hello"`，因为内存还没被释放，内容完好，看起来一切正常。 
4. **--- 即将出作用域 ---** 
   离开 `main` 函数时，局部对象按**逆序**析构：先析构 `y`，再析构 `x`。 
5. **第一次析构：[析构] hello 释放 0x1b611639ae0** 
   `y` 的析构函数执行 `delete[] data_;`，成功释放了地址 `0x1b611639ae0` 处的内存。此时 `x.data_` 变成了**悬垂指针**（dangling pointer），指向一块已被归还给操作系统的内存。 
6. 第二次析构：`[析构] c� 释放 0x1b611639ae0``x` 的析构函数尝试再次 `delete[]` 同一个地址。这是未定义行为（UB），具体表现因环境而异： 
   - 这里打印 `data_` 时，那块内存的内容已经被破坏（可能被操作系统标记为无效或被其他数据覆盖），所以显示为乱码 `"c�"`。 
   - 第二次 `delete[]` 可能导致程序崩溃（Segmentation fault），但某些环境下可能侥幸通过（如本例），但这绝不安全。 

**补充解读：**

### 1. `strcpy` 函数是干什么的？ 

`strcpy(destination, source)` 是 C 标准库 `<cstring>` 提供的字符串复制函数。它将 `source` 所指向的字符串（包括结尾的空字符 `'\0'`）**逐个字符复制**到 `destination` 所指向的内存区域。

在你的代码中：

```cpp
data_ = new char[strlen(s) + 1];   // 分配足够容纳 s 的内存（+1 给 '\0'）
strcpy(data_, s);                  // 把 s 的内容复制到 data_
```

这一步确保了 `data_` 拥有一份独立的、与传入参数 `s` 内容相同的副本。

### 2. `(void *)data_` 是什么意思？ 

`data_` 的类型是 `char*`。直接 `cout << data_` 时，C++ 会把它当作 C 风格字符串，**输出其指向的字符串内容**，而不是指针的地址。

为了**输出指针本身的数值（即内存地址）**，需要将 `char*` 强制转换为 `void*`。转换后 `cout` 会输出地址的十六进制表示（如 `0x1b611639ae0`）。

所以 `(void *)data_` 的作用就是：**以地址形式打印指针的值**，方便观察两个对象是否指向同一块内存。

### 3. 析构函数的三元表达式为什么没有输出 `"?"`？ 

析构函数中的代码：

```cpp
cout << "[析构] " << (data_ ? data_ : "?") << " 释放 " << (void *)data_ << endl;
```

- `data_ ? data_ : "?"` 的含义是：如果 `data_` 不是空指针（`nullptr`），就输出 `data_` 指向的字符串；否则输出 `"?"`。 

在你的例子中，`y` 和 `x` 的 `data_` 都指向同一个堆地址 `0x1b611639ae0`，**都不是空指针**。因此：

- 第一次析构（`y`）：`data_` 有效，输出 `"hello"`。 
- 第二次析构（`x`）：虽然那块内存已经被释放，但 `data_` 仍然保存着原来的地址（悬垂指针），**不是 nullptr**，所以三元表达式判定为真，试图输出 `data_` 指向的内容。然而内存已被破坏，内容不可读，因此输出了乱码 `"c�"`。 

**结论**：只有当 `data_` 被显式设为 `nullptr` 时，才会输出 `"?"`。这里没有设置，所以永远不会输出 `"?"`。这也暴露了浅拷贝的另一个隐患：**析构后指针没有被置空**，成为野指针，后续访问会导致未定义行为。

### 本质问题 

- **没有遵守“三/五法则”**：类管理了动态内存（`data_`），就必须显式定义**拷贝构造函数**、**拷贝赋值运算符**和**析构函数**（C++11 起还包括移动构造/赋值）。 
- **默认拷贝是逐成员复制**：对于指针成员，只复制指针值（地址），不复制所指向的内存内容，导致多个对象共享同一块堆内存，析构时重复释放。

#### ✅ 正确写法：Rule of Three（三法则）

> **只要你需要自己写「析构 / 拷贝构造 / 拷贝赋值」中的任意一个，三个都得写。**
> （C++11 后升级为 Rule of Five，再加移动构造 + 移动赋值，第 5 周补）

```cpp
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
        if (this != &other) {                     // 自赋值检查：a = a
            char *tmp = new char[strlen(other.data_)+1];  // 先申请成功…
            strcpy(tmp, other.data_);
            delete[] data_;                       // …再释放旧的（中间抛异常也不炸）
            data_ = tmp;
        }
        return *this;                             // 支持 a = b = c 链式
    }
    // ③ 析构：唯一负责释放
    ~GoodString() { delete[] data_; }

    void print() const { cout << data_ << " @" << (void*)data_ << endl; }
};

int main() {
    GoodString x("hello");
    GoodString y = x;      // 深拷贝：地址不同 ✅
    x.print(); y.print();
    GoodString z("zzz");
    z = x = y;             // 链式赋值 ✅
    y.print();
    return 0;              // 三块独立内存，各 free 各的，干净退出 ✅
}
```

**把 `BadString` 换成 `GoodString` 再跑一次**：地址不同了，退出时三个析构依次打印，无崩溃。**这一前一后两次运行，就是今天最有价值的 10 分钟。**

### 1.5 回到 Date：补齐"四大件"（今日必做产出） ✍️

打开昨天的 `week3/mon_date.cpp`，另存为 `week3/tue_date_lifecycle.cpp`，补上这些：

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

class Date {
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
        cout<<"[构造]   "<<*this<<"  存活="<<alive_<<"\n";   // ← 明天 operator<< 今天先用 print 代替
    }
    Date(const Date &o) : year_(o.year_), month_(o.month_), day_(o.day_) {  // ✅ 显式写拷贝构造
        ++alive_; cout<<"[拷贝构造] 来自 "<<o.year_<<"-"<<o.month_<<"-"<<o.day_<<"  存活="<<alive_<<"\n";
    }
    ~Date() { --alive_; cout<<"[析构]   "<<year_<<"-"<<month_<<"-"<<day_<<"  存活="<<alive_<<"\n"; }

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
    return 0;                  // 析构 d → c → b → a（严格逆序）
}
```

**输出：**

```powershell
=== 1. 三种出生 ===
[构造]   2026-03-02  存活=1
[构造]   1970-01-01  存活=2
[拷贝构造] 来自 2026-3-2  存活=3

=== 2. 传参方式对比 ===
[拷贝构造] 来自 2026-3-2  存活=4
  byValue  内 2026-03-02
[析构]   2026-3-2  存活=3
  byRef    内 2026-03-02  (零拷贝)

=== 3. 返回值 ===
[构造]   2000-06-01  存活=4

=== 4. 堆上对象：delete 才析构 ===
[构造]   1999-12-31  存活=5
[析构]   1999-12-31  存活=4
--- p 已死，main 即将结束 ---
当前存活 = 4
[析构]   2000-6-1  存活=3
[析构]   2026-3-2  存活=2
[析构]   1970-1-1  存活=1
[析构]   2026-3-2  存活=0
```

**必须亲眼确认的 5 件事（写进笔记）：**

|  #   | 现象                                                    | 说明                                                         |
| :--: | :------------------------------------------------------ | :----------------------------------------------------------- |
|  1   | `Date c = a;` 打印的是 **`[拷贝构造]`** 而不是 `[构造]` | 有 `=` 不代表赋值，看左边是不是新对象                        |
|  2   | `byValue(a)` 一进一出：先拷贝构造、函数返回时立刻析构   | **按值传对象 = 白拷一次**，这就是为什么第 2 周教你用 `const T&` |
|  3   | `byRef(a)` 全程零打印                                   | `const Date&` 不产生对象，纯别名                             |
|  4   | `delete p` 那行**立刻**打印析构，不等 main 结束         | 栈对象出作用域死；堆对象 `delete` 时才死（忘了 = 内存泄漏）  |
|  5   | 最后四条析构顺序：`d → c → b → a`                       | **严格逆构造序（LIFO）**，`p` 已经在第 4 步先走了            |

**关于`this`的辨析：**

`this`是一个隐含的指针，存在于每个非静态成员函数（构造、析构、普通成员函数）内部。它指向调用该函数的那个对象本身。

### 具体来说 

当你写下：

```cpp
Date x(2025, 12, 25);
x.print();
```

在 `print()` 函数内部，`this` 就是 `&x`（`x` 的地址）。所以 `this->year_` 等价于 `x.year_`。

### 在你刚刚加入的拷贝赋值运算符中 

```cpp
Date& operator=(const Date& rhs) {
    if (this == &rhs) { ... }   // ①
    ...
    return *this;                // ②
}
```

- **① this == &rhs**：检查当前对象（等号左边的对象）是否和右边的对象是**同一个对象**。例如 `x = x` 时，`this` 是 `&x`，`&rhs` 也是 `&x`，两者相等，于是跳过赋值操作，防止自我赋值带来的问题。 
- **② return \*this**：`*this` 解引用得到当前对象本身，返回它的引用，从而支持链式赋值如 `a = b = c`。 

### 什么时候能省略 `this->` 

大多数情况下可以省略，编译器会自动在成员名前加上 `this->`。例如：

```cpp
void print() const {
    cout << year_ << ...;   // 编译器自动理解为 this->year_
}
```

但当参数名和成员名冲突时必须显式使用 `this->`：

```cpp
void setYear(int year_) {
    this->year_ = year_;    // 不加 this-> 的话，year_ 指参数自己
}
```

### 一句话总结 

**this 就是“我自己”的地址**，在成员函数里用它来访问自己的成员，或者把自己作为一个整体传给其他函数。

**完成前自测 4 题（写进笔记）：**

1. `Date c = a;` 和 `c = a;`（假设 `c` 已存在）分别调用什么函数？怎么一眼区分？

   - **Date c = a;**：调用**拷贝构造函数**。因为 `c` 正在被**定义**（左边有类型名 `Date`），这是一个新对象的初始化。 
   - **c = a;**：调用**拷贝赋值运算符**（`operator=`）。因为 `c` 已经存在（之前已定义），这是对一个已存在对象的赋值。 

   **一眼区分的方法**：看等号左边有没有写类型名。

   - 有类型名 → 构造（可能是默认、有参、拷贝） 
   - 没有类型名 → 赋值

2. 编译器自动生成的拷贝构造干了什么？为什么 `Date` 没事、`BadString` 直接崩？

   编译器自动生成的拷贝构造函数执行**逐成员拷贝（shallow copy）**：将源对象的每个非静态成员按字节复制到目标对象。

   - **Date 类**：成员是 `int year_, month_, day_`。整数之间互相复制，各自拥有独立的值，没有任何共享资源，所以安全。 
   - **BadString 类**：成员是 `char* data_`。浅拷贝只复制了**指针的值**（即地址），导致两个对象的 `data_` 指向**同一块堆内存**。当第一个对象析构时 `delete[] data_` 释放了这块内存；第二个对象析构时再次 `delete[]` 同一地址 → **double free**（未定义行为，输出乱码或崩溃）。

3. **Rule of Three** 说的是哪三个函数？为什么只写一个不够？

   **Rule of Three** 指的是：如果一个类需要显式定义以下三个函数中的**任何一个**，那么通常也需要显式定义另外两个：

   1. **析构函数**（`~ClassName()`） 
   2. **拷贝构造函数**（`ClassName(const ClassName&)`） 
   3. **拷贝赋值运算符**（`ClassName& operator=(const ClassName&)`） 

   **为什么只写一个不够？**

   这三个函数共同管理类的资源（如动态内存、文件句柄等）。

   - 如果你写了析构函数来释放资源，但没写拷贝构造/赋值，编译器生成的浅拷贝会让多个对象共享同一资源，导致 double free 或内存泄漏。 
   - 如果你写了拷贝构造/赋值来实现深拷贝，但没写析构，资源永远不会被释放（内存泄漏）。 

   三者是一个整体，缺一不可。

4. `byValue(Date d)` 改成 `byValue(const Date& d)` 省掉了什么？这和第 2 周周三的哪个知识点是同一件事？

   - **省掉了**：一次拷贝构造 + 一次析构（即整个对象的复制和销毁过程）。 
   - **与第 2 周周三的知识点关联**：当时学的是 **const 对象与 const 成员函数**，其中提到 **const 引用可以绑定到临时对象，避免拷贝**。这里 `const Date& d` 就是一个 const 引用，它不产生新对象，只是原对象的别名，因此零拷贝。这和“用 const 引用接收函数参数以避免拷贝”是同一个原理。

**现在对这段Cpp代码进行结构分析：**

### 一、总体布局 

程序由三部分组成：**Date 类定义**、**全局辅助函数**、**main 函数**。目的是演示 C++ 对象的构造、拷贝、传参、析构以及生命周期的完整流程。

------

### 二、`Date` 类（核心） 

| 部分                 | 内容                                 | 作用                                                         |
| -------------------- | ------------------------------------ | ------------------------------------------------------------ |
| **私有成员**         | `int year_, month_, day_`            | 存储年月日                                                   |
| **私有静态成员**     | `static int alive_`                  | 统计当前存活的 `Date` 对象个数                               |
| **私有成员函数**     | `isValid()` `sync()`                 | 校验日期合法性，非法时回退到 1970-01-01                      |
| **公有构造函数**     | `Date(int y=1970,int m=1,int d=1)`   | 有参/默认构造，调用 `sync` 校验，递增 `alive_`，打印构造信息 |
| **公有拷贝构造函数** | `Date(const Date &o)`                | 逐成员拷贝，递增 `alive_`，打印拷贝来源                      |
| **公有析构函数**     | `~Date()`                            | 递减 `alive_`，打印析构信息                                  |
| **公有成员函数**     | `print()` `year()` `month()` `day()` | 格式化输出、获取各分量                                       |
| **公有静态成员函数** | `static int alive()`                 | 返回 `alive_` 的值                                           |
| **类外定义**         | `int Date::alive_ = 0;`              | 静态成员必须在类外单独定义并初始化                           |

------

### 三、全局辅助函数 

| 函数       | 签名                        | 行为                               | 教学目的                         |
| ---------- | --------------------------- | ---------------------------------- | -------------------------------- |
| `byValue`  | `void byValue(Date d)`      | 按值传参，函数内部调用 `d.print()` | 展示按值传参会触发拷贝构造和析构 |
| `byRef`    | `void byRef(const Date &d)` | 常量引用传参，内部调用 `d.print()` | 展示引用传参零拷贝               |
| `makeDate` | `Date makeDate(int y)`      | 返回一个临时 `Date` 对象           | 展示返回值优化（RVO）的效果      |

------

### 四、`main` 函数（执行流程） 

`main` 分为四个小节，每节聚焦一个生命周期现象：

```powershell
=== 1. 三种出生 ===
  a: 有参构造 → b: 默认构造 → c: 拷贝构造（c = a 是初始化，不是赋值）

=== 2. 传参方式对比 ===
  byValue(a):  进入函数前拷贝一次，退出时析构一次
  byRef(a):    全程无拷贝

=== 3. 返回值 ===
  d = makeDate(2000): 构造临时对象，然后被 RVO 直接构造到 d（无多余拷贝）

=== 4. 堆上对象 ===
  new Date(...) → 构造 → delete → 立即析构
  最后输出存活数，然后 main 结束，自动析构 d、c、b、a（严格逆序）
```

------

### 五、关键设计亮点 

1. **静态成员 alive_**：跨所有对象共享计数器，用于实时追踪对象数量。 
2. **sync() + isValid()**：将校验逻辑封装，避免构造函数体内重复代码。 
3. **拷贝构造显式写出**：虽然对 `Date` 来说默认的逐成员拷贝已经够用，但显式写出是为了打印日志，便于观察拷贝行为。 
4. **全局函数分离**：将测试逻辑（传参、返回值）写成独立函数，让 `main` 更清晰，同时体现函数间对象的传递方式。 

------

### 六、可改进之处（笔记可选） 

- 拷贝构造的打印格式与 `print()` 不一致（没有前导零），可以统一。 

- 缺少拷贝赋值运算符（`operator=`），目前没用到，但遵循 Rule of Three 应一并提供。

   要在现有程序中加入拷贝赋值运算符的学习实验，建议按以下步骤进行：

  ------

  ### 1. 在 `Date` 类中添加拷贝赋值运算符声明与实现 

  在 `public:` 区域内（析构函数后面）添加：

  ```cpp
  Date& operator=(const Date& rhs) {
      if (this == &rhs) {                     // ① 自赋值检查
          cout << "[自赋值跳过]\n";
          return *this;
      }
      // ② 拷贝数据（对于 Date 类，只有 int，无需释放旧资源）
      year_ = rhs.year_;
      month_ = rhs.month_;
      day_  = rhs.day_;
      // ③ 注意：alive_ 不增减，因为赋值不创建也不销毁对象
      cout << "[拷贝赋值] " << rhs.year_ << "-" << rhs.month_ << "-" << rhs.day_
           << "  存活=" << alive_ << "\n";
      return *this;                           // ④ 返回 *this 支持链式赋值
  }
  ```

  **说明：**

  - ① 自赋值检查：防止 `a = a` 时误操作（对于 `Date` 无害，但养成好习惯）。 
  - ② 逐成员拷贝，与拷贝构造相同。 
  - ③ 赋值不改变对象数量，所以不操作 `alive_`。 
  - ④ 返回 `*this` 使得可以连续赋值如 `a = b = c`。 

  ------

  ### 2. 在 `main` 函数中添加测试代码 

  在 `=== 4. 堆上对象 ===` 后面、`return 0` 之前插入一段：

  ```cpp
  cout << "\n=== 5. 拷贝赋值 vs 拷贝构造 ===\n";
  Date x(2025, 12, 25);
  Date y(2026, 1, 1);
  cout << "赋值前：x = "; x.print(); cout << ", y = "; y.print(); cout << "\n";
  x = y;                      // 调用 operator=
  cout << "赋值后：x = "; x.print(); cout << "\n";
  
  cout << "\n自赋值测试：\n";
  x = x;                      // 应当触发自赋值检查
  ```

  ------

  ### 3. 预期输出示例（新增部分） 

  ```
  === 5. 拷贝赋值 vs 拷贝构造 ===
  [构造]   2025-12-25  存活=6
  [构造]   2026-01-01  存活=7
  赋值前：x = 2025-12-25, y = 2026-01-01
  [拷贝赋值] 2026-1-1  存活=7
  赋值后：x = 2026-01-01
  
  自赋值测试：
  [自赋值跳过]
  ```

  注意：赋值不会改变 `alive_` 计数，而拷贝构造会增加。

  ------

  ### 4. 配套讲解要点（供你记录笔记） 

  | 对比项                        | 拷贝构造 `Date c = a`          | 拷贝赋值 `c = a`（c已存在）          |
  | ----------------------------- | ------------------------------ | ------------------------------------ |
  | 调用时机                      | 新对象初始化                   | 已有对象赋值                         |
  | 是否改变对象数量              | 是（alive_++）                 | 否                                   |
  | 是否需要自赋值检查            | 不需要（新对象不可能等于自身） | 需要（防止自我赋值）                 |
  | 返回值                        | 无                             | `*this`（支持链式）                  |
  | 若类管理资源（如`BadString`） | 需深拷贝，否则 double free     | 需先释放旧资源再深拷贝，且自赋值安全 |

  ------

  ### 5. 提醒：Rule of Three 完整性 

  至此，`Date` 类已经显式定义了：

  - 析构函数（虽然无事可做，但已写出） 
  - 拷贝构造函数 
  - 拷贝赋值运算符 

  符合 **Rule of Three** 的要求。对于 `Date` 这样的纯整数类，即使不写也安全，但写上更规范，也为将来管理资源的类打下基础。

- `sync()` 在构造函数体中被调用，而初始化列表已经赋初值，存在“先赋值再覆盖”的冗余（不过不影响正确性）。

------

## 第二步：C 语言穿插（限时 20 分钟） ⏱️

**目标：用函数指针把"成员函数"缝到 struct 上，看清 C++ 的 `this` 到底藏在哪。**

新建 `week3/tue_method_c.c`：

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Date {
    int year, month, day;
    /* 👉 函数指针 = C 版的"成员函数"。注意第一个参数必须自己传 self！ */
    void  (*print)(const struct Date *self);      // 这就是被藏起来的 this
    void  (*addDays)(struct Date *self, int n);
    int   (*toOrdinal)(const struct Date *self);
} Date;

static void d_print(const Date *self) {
    printf("%04d-%02d-%02d\n", self->year, self->month, self->day);
}
static int d_ordinal(const Date *self) {          // 简化：算年内第几天
    static const int md[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    int s=0; for (int m=1;m<self->month;m++) s+=md[m];
    return s + self->day;
}
static void d_addDays(Date *self, int n) {        // 简版：只加日，不进位月（周三再完善）
    self->day += n;  printf("[addDays] → %04d-%02d-%02d\n", self->year,self->month,self->day);
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
    // date_delete(b);                             // 💥 取消注释 = 今天的 BadString 双 free，同一个坑
    b = NULL;                                      // C 的土办法：手动置空防野指针（第 2 周周二你写过）

    /* 忘了 date_new 直接用的下场：函数指针是垃圾 → 调用即崩 */
    Date zombie;                                   // 未初始化
    // zombie.print(&zombie);                      // 💥 取消注释试试：Segmentation fault
    return 0;
}
```

编译运行：`gcc -std=c11 -Wall tue_method_c.c -o mc && ./mc`

### C vs C++ 对比（接昨天编号，写进差异笔记第 7~12 条）

|  #   | 对比项        | C（今天）                                          | C++（今天）                                          |
| :--: | :------------ | :------------------------------------------------- | :--------------------------------------------------- |
|  7   | 成员函数      | 结构体里塞**函数指针**，手动赋值                   | 类里直接声明，`this` 隐式传入                        |
|  8   | `this` 从哪来 | 你得**亲手把 `self` 当第一个参数传** `a->print(a)` | 编译器自动塞，写 `a.print()` 即可                    |
|  9   | 构造          | `date_new()`，**忘调=垃圾值/野指针**               | 构造函数**不可忘**，创建即调用                       |
|  10  | 析构          | `date_delete()`，**忘调=泄漏；调两次=双 free**     | 出作用域自动调，**且只调一次**                       |
|  11  | 拷贝控制      | 结构体直接 `=`，永远是浅拷贝，无从干预             | 可自定义拷贝构造/赋值 → 深拷贝，还能 `= delete` 禁掉 |
|  12  | "虚函数表"    | 手工绑函数指针，每个对象多 3 个指针的开销          | 编译器实现 vptr（第 5 周多态细讲），机制神似         |

> **一句话结论**：C++ 的成员函数 ≈ **藏在参数表第一位的 `self`** + 编译器帮你自动传参；
> C++ 的构造/析构 ≈ **强制你不会忘的 `date_new` / `date_delete`**。
> 今天两边各写一版，你就再也不会觉得 `this` 是玄学了。

**C代码写法辨析：**

```c
void (*print)(const struct Date *self);// 这就是被藏起来的 this
void (*addDays)(struct Date *self ,int n);
int (*toOrdinal)(const struct Date *self);
```

这种写法是 **C 语言模拟面向对象编程**的经典模式。你看到的 `void (*print)(const struct Date *self)` 实际上就是在**手动模拟 C++ 编译器对成员函数的处理方式**。

### 本质：把隐式的 `this` 变成显式的参数 

在 C++ 中，当你写：

```cpp
class Date {
    void print() const;   // 有一个隐藏的 const Date* this 参数
};
```

编译器会把 `print()` 翻译成类似：

```cpp
void Date_print(const Date* this);   // 名字修饰后，第一个参数就是 this
```

而 C 语言没有类，也没有隐式的 `this`，所以想实现“对象调用方法”的效果，就必须**自己把对象指针作为第一个参数传进去**。上面的代码正是这样做的：

- `struct Date` 是一个结构体，里面存放数据（年、月、日）。 
- `void (*print)(const struct Date *self);` 是一个函数指针，它指向的函数接收一个 `const struct Date*` 参数（即 `self`，相当于 C++ 的 `this`）。 
- 同理，`void (*addDays)(struct Date *self, int n);` 模拟了一个修改对象状态的成员函数（非 const），`int (*toOrdinal)(const struct Date *self);` 模拟了一个 const 成员函数。 

### 实际使用时 

通常会这样定义一个“类”：

```c
struct Date {
    int year, month, day;
    void (*print)(const struct Date *self);
    void (*addDays)(struct Date *self, int n);
};

void Date_print(const struct Date *self) {
    printf("%04d-%02d-%02d\n", self->year, self->month, self->day);
}

void Date_addDays(struct Date *self, int n) {
    // 修改 self->day 等...
}

// 初始化一个对象，绑定函数指针
struct Date d = {2026, 3, 2, Date_print, Date_addDays};
d.print(&d);   // 显式传入 &d，相当于 C++ 的 d.print()
```

可以看到，C语言还需要一步将具体函数功能与结构体中的函数指针绑定：

初始化列表来绑定函数指针：

```c
struct Date d = {2026, 3, 2, Date_print, Date_addDays};
```

### 与你学的 C++ 的联系 

- C++ 的 `this` 就是 C 语言中显式传递的那个 `self` 指针，只不过 C++ 帮你自动传了。 
- C++ 成员函数末尾的 `const`（如 `void print() const`）对应 C 语言中参数为 `const struct Date*` 的函数——都承诺不修改对象。 

### 一句话总结 

**这种写法就是把 C++ 编译器隐藏的 this 指针“扒开”给你看，让你明白成员函数本质上就是第一个参数为对象指针的普通函数。** 很多早期的 C++ 实现（cfront）就是这样先把 C++ 代码翻译成带 `this` 参数的 C 代码，再编译成机器码的。

---

**对于“函数指针”的理解**

**普通的指针（如 int\*）存放的是一个地址，该地址处存放着数据（比如一个整数）。**

**函数指针**也是一样的道理——它存放的也是一个地址，只不过那个地址处存放的不是数据，而是一段**可执行的机器指令（即函数体）**。换句话说，函数指针指向的是函数的入口，你可以通过它来调用那个函数。

### 为什么函数也有地址？ 

你写的每一个函数（比如 `void foo() { ... }`），在编译后都会被放到内存的**代码区**（text segment），CPU 可以从那里开始执行指令。这个起始位置就是函数的地址。在 C/C++ 中，**函数名本身就代表这个地址**，就像数组名代表数组首地址一样。

例如：

```cpp
void hello() {
    cout << "Hello\n";
}
cout << (void*)hello;  // 打印 hello 函数的地址（一个十六进制数）
```

### 函数指针的声明、赋值、调用 

声明函数指针时，必须指明它指向的函数的**返回类型**和**参数列表**，这样才能知道如何正确地调用它。

#### 1. 声明 

```cpp
// 声明一个函数指针 pf，它可以指向“返回值为 void、参数为 int”的函数
void (*pf)(int);
```

解读：`pf` 是一个指针，它指向一个函数，该函数接受一个 `int` 参数，返回 `void`。

**注意括号**：`(*pf)` 表示 pf 是指针，如果没有括号就会变成 `void* pf(int)`（返回指针的函数）。

#### 2. 赋值 

```cpp
void myFunc(int x) { cout << x; }
pf = myFunc;        // 直接把函数名赋给指针（函数名就是地址）
// 也可以写成 pf = &myFunc; 效果一样，& 是可选的
```

#### 3. 调用 

```cpp
pf(42);             // 通过函数指针调用函数，等价于 myFunc(42)
(*pf)(42);          // 另一种写法，效果相同
```

### 回到你看到的 C 模拟 OOP 代码 

```c
struct Date {
    int year, month, day;
    void (*print)(const struct Date *self);  // 声明一个函数指针成员
};
```

这里 `print` 是 `struct Date` 的一个成员，它的类型是 `void (*)(const struct Date*)`，可以指向任何*返回 void、接受一个 const struct Date 参数**的函数。

当你写：

```c
void Date_print(const struct Date *self) { ... }
struct Date d;
d.print = Date_print;    // 将函数地址存入成员
d.print(&d);             // 通过成员调用
```

整个过程就是：

1. 把 `Date_print` 函数的地址存到 `d.print` 这个变量里。 
2. 通过 `d.print(&d)` 取出这个地址，跳转到 `Date_print` 的代码去执行，并把 `&d` 作为参数传给它。 

### 一句话总结 

**函数指针就是指向函数入口的指针，它跟数据指针一样存的是地址，区别在于那个地址处的“内容”是可执行的代码而不是数据。** 你可以像使用普通指针一样赋值、传参，然后用它来调用函数。

---

**对象指针**

从**“存放地址”**这个层面上说，**对象指针和函数指针是完全一样的道理**——它们都是一个变量，里面存了一个内存地址。

区别只在于这个地址所指向的内存区域里装的是什么：

- **普通数据指针（包括对象指针）**：指向的区域存放的是**数据**（整数、浮点数、结构体的各个字段等）。你可以通过指针读写这些数据。 
- **函数指针**：指向的区域存放的是**可执行的机器指令**（即函数体）。你不能通过函数指针去“读取指令内容”，只能通过它来**调用**那个函数（跳转过去执行）。 

### 对象指针具体是什么样的？ 

假设你有：

```cpp
Date a(2026, 3, 2);
Date *p = &a;   // p 是一个对象指针
```

那么：

- `p` 里存的是 `a` 在内存中的首地址。 
- 通过 `p` 你可以访问 `a` 的成员：`p->year_`、`p->print()`。 
- `p` 本身是一个变量，它也有自己的地址（`&p`），里面放的是另一个地址（`&a`）。 

这和 `int *q = &x` 完全一样——`q` 里存的是 `x` 的地址，通过 `q` 读写 `x` 的值。

### 那对象指针和函数指针有什么交叉点吗？ 

有。当你通过对象指针调用成员函数时，例如 `p->print()`，编译器实际上做了两件事：

1. 把 `p` 作为 `this` 指针传给 `print` 函数（隐式参数）。 
2. 找到 `print` 函数的地址（非虚函数在编译期确定，虚函数通过虚表查找），然后跳转过去执行。 

也就是说，**对象指针提供了数据（对象本身），而成员函数的地址则由编译器在背后通过函数指针机制（或直接硬编码）来定位**。这两者分工明确。

### 一句话总结 

**对象指针和函数指针都是“存地址的变量”，区别在于那个地址里装的是数据还是代码。** 你之前理解的“指针就是一个地址，地址里存的是数据”并没有错，函数指针只是把这个概念扩展到了代码区而已。

---

**C版本构造/析构体会：**

### 从 C 的角度看 C++ 构造函数的本质 

你上面那段 C 代码里的 `date_new` 函数，其实就是 C++ 构造函数在底层的真实写照：

1. **分配内存**：`malloc(sizeof(Date))` —— C++ 的 `new` 运算符内部也会调用 `operator new` 来申请内存。 
2. **初始化数据成员**：`p->year=y; p->month=m; p->day=d;` —— 对应 C++ 构造函数的初始化列表或函数体。 
3. **绑定函数指针**：`p->print=d_print;` —— 对应 C++ 中**非虚成员函数**的地址绑定。区别在于 C++ 不在每个对象里存函数指针，而是在编译期根据类型静态决定（非虚）或通过虚函数表（虚函数）在运行时查找。C 的这种“每个对象自己背函数指针”的做法，更像 C++ 的**虚函数表指针（vptr）**，只不过 C++ 把多个函数指针集中到一张表里，每个对象只存一个指向表的指针，节省空间。 

### 关键差异：C++ 帮你省了什么 

| 操作          | C 手动做          | C++ 自动做                |
| ------------- | ----------------- | ------------------------- |
| 内存分配      | `malloc` + 判空   | `new` 运算符，失败抛异常  |
| 成员初始化    | 逐字段赋值        | 初始化列表 + 构造函数体   |
| 函数指针绑定  | 逐个赋值          | 编译器静态绑定或 vtable   |
| 析构清理      | `free` + 手动置空 | `delete` 自动调用析构函数 |
| 拷贝控制      | 无（浅拷贝风险）  | 可自定义拷贝构造/赋值     |
| 自调用 `this` | 显式传参          | 隐式 `this` 指针          |

### 最重要的收获 

你现在应该能体会到：**C++ 的类并不是什么魔法，它只是在 C 的结构体 + 函数指针的基础上，加了一层编译器的语法糖和自动化管理。** 你刚才写的 `a->print(a)` 在 C++ 里变成了 `a->print()`，少写一个参数，多了一份安全。

------

## 第三步：算法副线——LeetCode 21 合并两个有序链表

昨天 206 练的是**指针掉头**，今天练**指针拼接**。同一套工具，换个用法。

### 3.1 思路（哨兵节点是今天的灵魂）

两个升序链表，把它们缝成一个升序链表。

**核心技巧：Dummy（哨兵）节点。** 没有它你就得特判"谁是新头"；有了它，第一个真节点也走统一的 `tail->next = ...` 逻辑，**特判归零**。

```
l1: 1→2→4        l2: 1→3→4
dummy→?
 tail 永远指向"结果链的末尾"，谁小就把谁摘过来挂上去

① 1==1 取 l1 → dummy→1    l1:2→4
② 2<3  取 l1 → dummy→1→2  l1:4
③ 4>3  取 l2 → ...→2→3    l2:4
④ 4==4 取 l1 → ...→3→4    l1:null
⑤ l1 空了 → tail->next = l2  （剩下的整段直接接上，不用再比！）
```

### 3.2 C++ 版（迭代 + 递归）

```cpp
class Solution {
public:
    // ✅ 主推：哨兵迭代法  O(n+m) / O(1)
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        ListNode dummy(0);          // 🪄 栈上哨兵，不用 new（明天讲为什么这样最干净）
        ListNode *tail = &dummy;

        while (l1 && l2) {                       // 两条都还有货才比
            if (l1->val <= l2->val) { tail->next = l1; l1 = l1->next; }
            else                    { tail->next = l2; l2 = l2->next; }
            tail = tail->next;
        }
        tail->next = l1 ? l1 : l2;               // 🪄 一边的活儿干完，另一边整段挂上
        return dummy.next;                       // 哨兵本身丢弃，返回真头
        // dummy 出作用域自动析构 ← 今天上午学的，没有 new 就没有 delete，零泄漏
    }

    // 📌 加餐：递归法  O(n+m) / O(n+m) 栈
    ListNode* mergeRec(ListNode* l1, ListNode* l2) {
        if (!l1) return l2;                      // 基准：一边空了，返回另一边
        if (!l2) return l1;
        if (l1->val <= l2->val) { l1->next = mergeRec(l1->next, l2); return l1; }
        else                    { l2->next = mergeRec(l2->next, l1); return l2; }
    }
};
```

> 💡 **和昨天 206 递归对照着看**：206 递归是"把指向关系**倒过来**"，21 递归是"把指向关系**接起来**"。两道题放一起，链表的递归就通了。

### 3.3 C 版（纯指针 + 哨兵 + 完整自测）

```c
#include <stdio.h>
#include <stdlib.h>

struct ListNode { int val; struct ListNode *next; };

struct ListNode* mergeTwoLists(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode dummy = {0, NULL};   /* 栈上哨兵，C 也能用这招 */
    struct ListNode *tail = &dummy;

    while (l1 && l2) {
        if (l1->val <= l2->val) { tail->next = l1; l1 = l1->next; }
        else                    { tail->next = l2; l2 = l2->next; }
        tail = tail->next;
    }
    tail->next = l1 ? l1 : l2;
    return dummy.next;
}

/* ---------- 本地自测（LeetCode 不用交） ---------- */
static struct ListNode* mk(const int v[], int n) {
    struct ListNode dummy = {0,NULL}, *t = &dummy;
    for (int i=0;i<n;i++){
        struct ListNode *nd = malloc(sizeof(*nd));   // 第 2 周周二：malloc + 判空
        if(!nd) { perror("malloc"); exit(1); }
        nd->val=v[i]; nd->next=NULL; t->next=nd; t=nd;
    }
    return dummy.next;
}
static void printL(const struct ListNode *h){        // const 指针：第 2 周周三
    for(;h;h=h->next) printf("%d->",h->val); printf("null\n");
}
static void freeL(struct ListNode *h){               // 先存后继再 free（和 206 同套路）
    while(h){ struct ListNode *t=h->next; free(h); h=t; }
}
int main(void){
    int a[]={1,2,4}, b[]={1,3,4};
    struct ListNode *l1=mk(a,3), *l2=mk(b,3);
    printL(l1); printL(l2);
    struct ListNode *m = mergeTwoLists(l1,l2);
    printL(m);            /* 期望 1->1->2->3->4->4->null */
    freeL(m);             /* ⚠️ 只 free 一次！l1/l2 的节点已被并入 m，再 freeL(l1) 就是今天上午的双 free */
    return 0;
}

```

> ⚠️ **C 版独有的坑（和上午 BadString 完美呼应）**：合并后 `l1`、`l2` 的指针已经**焊进**结果链了，
> 谁再 `freeL(l1)` 一次，就是**第二次释放同一批节点** → `double free`。
> C++ 版我们用栈上 `dummy` + 不 `new` 节点，从根上避开了这个坑——**这就是 RAII 的第一次甜头**。

### 3.4 五个易错点（提交前自查）

|  #   | 坑                                                           | 后果                   | 解法                                     |
| :--: | :----------------------------------------------------------- | :--------------------- | :--------------------------------------- |
|  1   | 不用 dummy，手写 `if (l1->val < l2->val) head = ...`         | 一堆特判，必漏一种     | **哨兵一把梭**，最后 `return dummy.next` |
|  2   | 忘了 `tail = tail->next;`                                    | 死循环 / 只挂一个节点  | 挂完**必须**推进 tail                    |
|  3   | 循环里 `l1 = l1->next` 写在 `tail->next = l1` **之前**       | 挂上去的是旧节点，链断 | 顺序：**先挂，后推**                     |
|  4   | 结尾写 `while(l1) {tail->next=l1;tail=tail->next;l1=l1->next;}` | 能过，但多余           | 直接 `tail->next = l1?l1:l2;` 整段接     |
|  5   | 递归版漏基准 `if(!l1) return l2;`                            | 空指针解引用崩         | 先写基准，再写递归                       |

### 3.5 手动走一遍

```
l1: 1→2→4 , l2: 1→3→4
dummy(0), tail=&dummy
比 1 vs 1 → 取 l1 → dummy.1,  l1→2,  tail→节点1
比 2 vs 1 → 取 l2(第一个1) …… 注意是 l2 的 1 先走完
→ dummy.1→1→2→3→4,  l1 剩 [4], l2 空
tail->next = l1(4)  ⇒ 1→1→2→3→4→4→null ✅

```

------

## 第四步：今日收尾 ✅

对照检查，全部打勾后 commit：

- [ ] `BadString` 崩溃版跑出过 `double free`（亲眼看到两个相同的地址被析构两次）
- [ ] `GoodString` 深拷贝版跑通，**Rule of Three** 三个函数都写出来了
- [ ] `tue_date_lifecycle.cpp` 五个现象全部对上（尤其 `byValue` vs `byRef` 的拷贝次数差）
- [ ] 能不看书回答自测 4 题（拷贝构造 vs 赋值怎么分 / Rule of Three / `const T&` 省了什么）
- [ ] C 版函数指针练习跑通，并且**故意取消一次 `date_delete(b)` 的注释**复现双 free
- [ ] 差异笔记新增 ≥ 6 条（编号 7~12 直接接昨天）
- [ ] LC 206（昨天）+ LC 21（今天）C++/C 四份代码都在，`freeL` 只调一次
- [ ] `learning-log` commit 1 次
      建议信息：`week3-tue: ctor/copy-ctor/dtor lifecycle + Rule of Three + LC21 merge lists`

### 为明天做铺垫（1 分钟即可） 👀

明天进入 **运算符重载**：给 `Date` 装上 `+`（加天数）和 `<<`（直接 `cout << d;`）。

今天你已经攒好了两个"痒点"，明天正好一起挠掉：

```cpp
// 痒点 1：今天 print() 只能这么调，丑
cout << "生日: "; a.print(); cout << "\n";

// 痒点 2：今天加天数只能这么调，更丑
Date later = a;  later.addDays(10);      // 明天 → Date later = a + 10;  ✅

// 痒点 3：今天 operator<< 想访问 private 只能靠 getter，明天用 friend 一刀切进去（周五正式讲）

```

**今晚睡前做两件小事（各 1 分钟）：**

1. 在今天的 `Date` 里试着写这一行，看编译器报什么错（**故意的**，明天解释）：

   ```cpp
   Date x = a + 10;      // ❌ no match for 'operator+'
   
   ```

2. 回忆一下：为什么 `operator<<` **不能**写成 `Date` 的成员函数？
   提示：`cout << d` 左边是谁？你能给 `std::ostream` 类里加函数吗？
   👉 想不通就对了，明天第一节课就解这个谜，答案正是**周五的友元**。

生命周期这道关今天过了，**浅拷贝那颗雷你已经亲手炸过一次**——这是本周里程碑第 2 条（"理解拷贝构造何时被调用"）最扎实的拿法。明天见 💪