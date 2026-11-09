#include <stdlib.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

struct ListNode* reverseList(struct ListNode* head) {
    struct ListNode *prev = NULL;   // 前驱指针，初始为 NULL
    struct ListNode *cur = head;    // 当前遍历指针

    while (cur != NULL) {
        struct ListNode *nxt = cur->next;  // 暂存后继节点，防止断链
        cur->next = prev;                  // 当前节点指向前驱（反转方向）
        prev = cur;                        // 前驱后移
        cur = nxt;                         // 当前指针后移
    }
    return prev;  // 遍历结束后 prev 指向原链表的尾节点（新头）
}

/* --- 本地自测用（LeetCode 不需要交这段）--- */
#include <stdio.h>   // printf

/**
 * 根据数组创建链表
 * @param vals 整型数组（元素值）
 * @param n    数组长度
 * @return 链表头指针
 *
 * 注意：这里使用了 C99 的复合字面量创建哑结点，
 * 若编译器不支持，可改为 struct ListNode dummy; dummy.next = NULL;
 */
struct ListNode* mk(int vals[], int n) {
    // 使用复合字面量创建哑结点（栈上临时对象，生命周期仅限本函数）
    struct ListNode *dummy = &(struct ListNode){0, NULL};
    struct ListNode *tail = dummy;   // 尾指针，初始指向哑结点

    for (int i = 0; i < n; i++) {
        struct ListNode *node = malloc(sizeof(struct ListNode)); // 分配新节点
        node->val = vals[i];
        node->next = NULL;
        tail->next = node;   // 尾节点链接新节点
        tail = node;         // 更新尾指针
    }
    return dummy->next;  // 返回第一个实际节点的指针（跳过哑结点）
}

/**
 * 打印链表
 * @param h 链表头指针（const 修饰表示不修改链表）
 */
void printList(struct ListNode *h) {
    for (; h != NULL; h = h->next) {
        printf("%d->", h->val);
    }
    printf("null\n");
}

/**
 * 释放整个链表（防止内存泄漏）
 * @param h 链表头指针
 */
void freeList(struct ListNode *h) {
    while (h != NULL) {
        struct ListNode *temp = h->next;  // 暂存下一个节点
        free(h);                          // 释放当前节点
        h = temp;                         // 移动到下一个节点
    }
}

/**
 * 主函数：本地测试
 */
int main(void) {
    int v[] = {1, 2, 3, 4, 5};
    struct ListNode *head = mk(v, 5);   // 创建链表 1->2->3->4->5
    printList(head);                     // 打印原链表

    head = reverseList(head);            // 反转链表
    printList(head);                     // 打印反转后结果，期望 5->4->3->2->1->null

    freeList(head);                      // 释放链表内存
    return 0;
}