#ifndef _USER_SPACE_SLIST_H_
#define _USER_SPACE_SLIST_H_

#include <stddef.h> /* offsetof */

/* container_of: 커널 스타일 그대로 */
#define container_of(ptr, type, member) ({                      \
        const typeof(((type *)0)->member) *__mptr = (ptr);      \
        (type *)((char *)__mptr - offsetof(type, member)); })

/* 노드 정의: 단일 연결 리스트 */
struct slist_node {
    struct slist_node *next;
};

/* 리스트 head 정의 (head + tail) */
struct slist_head {
    struct slist_node *first;
    struct slist_node *last;  /* 꼬리 노드 포인터 */
};

/* 초기화 매크로 */
#define SLIST_HEAD_INIT { .first = NULL, .last = NULL }
#define SLIST_HEAD(name) struct slist_head name = SLIST_HEAD_INIT
#define INIT_SLIST_HEAD(ptr) do { (ptr)->first = NULL; (ptr)->last = NULL; } while (0)
#define INIT_SLIST_NODE(ptr) ((ptr)->next = NULL)

/* 리스트가 비었는지 확인 */
static inline int slist_empty(const struct slist_head *h)
{
    return !h->first;
}

/* 머리 삽입: O(1) */
static inline void slist_add_head(struct slist_node *n, struct slist_head *h)
{
    if (!h->first) {
        h->first = n;
        h->last = n;
        n->next = NULL;
    } else {
        n->next = h->first;
        h->first = n;
    }
}

/* 꼬리 삽입: O(1) */
static inline void slist_add_tail(struct slist_node *n, struct slist_head *h)
{
    n->next = NULL;
    if (!h->first) {
        h->first = n;
        h->last = n;
    } else {
        h->last->next = n;
        h->last = n;
    }
}

/* 머리 pop: O(1) */
static inline struct slist_node *slist_pop_head(struct slist_head *h)
{
    struct slist_node *n = h->first;
    if (!n)
        return NULL;

    h->first = n->next;
    if (!h->first)  /* 리스트가 비었으면 tail도 NULL */
        h->last = NULL;
    n->next = NULL;
    return n;
}

/* 특정 노드 뒤 삽입: O(1) */
static inline void slist_add_after(struct slist_node *pos, struct slist_node *n, struct slist_head *h)
{
    n->next = pos->next;
    pos->next = n;
    if (h->last == pos)  /* pos가 마지막이면 tail 업데이트 */
        h->last = n;
}

/* 삭제: head부터 찾아야 함 (O(n)) */
static inline void slist_del(struct slist_node *target, struct slist_head *h)
{
    struct slist_node **pp = &h->first;
    while (*pp) {
        if (*pp == target) {
            *pp = target->next;
            if (h->last == target)  /* tail 삭제 시 업데이트 */
                h->last = (*pp == NULL) ? NULL : h->last; 
            target->next = NULL;
            return;
        }
        pp = &(*pp)->next;
    }
}

/* 엔트리 매크로 */
#define slist_entry(ptr, type, member) container_of(ptr, type, member)

/* 순회 매크로 */
#define slist_for_each(pos, head) \
    for (pos = (head)->first; pos; pos = pos->next)

#define slist_for_each_safe(pos, n, head) \
    for (pos = (head)->first; pos && ({ n = pos->next; 1; }); pos = n)

#endif /* _USER_SPACE_SLIST_H_ */

