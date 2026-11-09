#include <iostream>
#include <vector>   // 用于方便构造测试数据
using namespace std;

// ---------- 链表结点定义（带三个构造函数） ----------
struct ListNode
{
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

// ---------- 辅助函数 ----------

/**
 * 从 vector 创建链表（尾插法）
 * @param arr  输入数组
 * @return     链表头指针
 */
ListNode* createLinkedList(const vector<int>& arr) {
    if (arr.empty()) return nullptr;
    ListNode *head = new ListNode(arr[0]);
    ListNode *tail = head;
    for (size_t i = 1; i < arr.size(); ++i) {
        tail->next = new ListNode(arr[i]);
        tail = tail->next;
    }
    return head;
}

/**
 * 打印链表（格式：1 -> 2 -> 3 -> nullptr）
 * @param head  链表头指针
 */
void printLinkedList(ListNode* head) {
    while (head != nullptr) {
        cout << head->val << " -> ";
        head = head->next;
    }
    cout << "nullptr" << endl;
}

/**
 * 释放整个链表的内存（防止内存泄漏）
 * @param head  链表头指针
 */
void deleteLinkedList(ListNode* head) {
    while (head != nullptr) {
        ListNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ---------- 解决方案 ----------
class Solution {
public:
    /**
     * ✅ 主推：迭代法反转链表
     * 时间复杂度 O(n)，空间复杂度 O(1)
     *
     * 思路：用三个指针 prev/cur/nxt 原地翻转
     * ① 保存 cur->next（救命绳）
     * ② cur->next 指向 prev
     * ③ prev 前移
     * ④ cur 前移
     */
    ListNode* reverseList(ListNode* head) {
        ListNode *prev = nullptr, *cur = head;
        while (cur != nullptr) {
            ListNode *nxt = cur->next;   // ① 先存后继（救命绳）
            cur->next = prev;            // ② 掉头
            prev = cur;                  // ③ prev 前移
            cur = nxt;                   // ④ cur 前移
        }
        return prev;                     // cur 空了，prev 落在原尾 = 新头
    }

    /**
     * 📌 加餐：递归法反转链表
     * 时间复杂度 O(n)，空间复杂度 O(n)（递归栈深度）
     *
     * 思路：假设后 N-1 个结点已经反转好了，
     * 只需把当前结点的 next 指向自己，再把自己的 next 置空。
     */
    ListNode* reverseListRec(ListNode* head) {
        if (head == nullptr || head->next == nullptr) return head;
        ListNode *newHead = reverseListRec(head->next); // 反转后半段，返回新头
        head->next->next = head;   // 让"下一个"指回自己
        head->next = nullptr;      // 自己的 next 断开（否则成环！）
        return newHead;
    }
};

// ---------- 主函数（测试） ----------
int main() {
    Solution sol;

    // 测试用例 1：普通链表
    cout << "=== Test 1: 1->2->3->4->5 ===" << endl;
    vector<int> arr1 = {1, 2, 3, 4, 5};
    ListNode* list1 = createLinkedList(arr1);
    cout << "原始链表: ";
    printLinkedList(list1);

    ListNode* rev1_iter = sol.reverseList(list1);
    cout << "迭代反转: ";
    printLinkedList(rev1_iter);
    // 注意：此时 rev1_iter 是新的头，原来的 list1 已被破坏，不能再使用
    deleteLinkedList(rev1_iter);   // 释放内存

    // 测试用例 2：空链表
    cout << "\n=== Test 2: empty list ===" << endl;
    ListNode* list2 = nullptr;
    ListNode* rev2 = sol.reverseList(list2);
    cout << "反转空链表: ";
    printLinkedList(rev2);          // 输出 nullptr

    // 测试用例 3：单节点
    cout << "\n=== Test 3: single node ===" << endl;
    ListNode* list3 = new ListNode(42);
    cout << "原始链表: ";
    printLinkedList(list3);
    ListNode* rev3 = sol.reverseList(list3);
    cout << "迭代反转: ";
    printLinkedList(rev3);
    delete rev3;                    // 只有一个结点，直接 delete

    // 测试用例 4：递归法
    cout << "\n=== Test 4: recursion on 1->2->3->4->5 ===" << endl;
    vector<int> arr4 = {1, 2, 3, 4, 5};
    ListNode* list4 = createLinkedList(arr4);
    cout << "原始链表: ";
    printLinkedList(list4);
    ListNode* rev4_rec = sol.reverseListRec(list4);
    cout << "递归反转: ";
    printLinkedList(rev4_rec);
    deleteLinkedList(rev4_rec);

    return 0;
}