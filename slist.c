#include <stdio.h>
#include <stdlib.h>
#include "slist.h"

struct item {
    int key;
    struct slist_node node;
};

static struct item *item_new(int key)
{
    struct item *it = malloc(sizeof(*it));
    if (!it) return NULL;
    it->key = key;
    INIT_SLIST_NODE(&it->node);
    return it;
}

static void item_free(struct item *it)
{
    free(it);
}

static void print_list(const struct slist_head *head, const char *tag)
{
    const struct slist_node *pos;
    printf("%s: [", tag);
    slist_for_each(pos, head) {
        const struct item *it = slist_entry(pos, struct item, node);
        printf("%d", it->key);
        if (pos->next) printf(", ");
    }
    printf("]\n");
}

int main(void)
{
    SLIST_HEAD(head);
    struct item *it;
    struct slist_node *pos, *n;

    /* 머리에 0~2 삽입 */
    for (int i = 0; i < 3; ++i) {
        it = item_new(i);
        slist_add_head(&it->node, &head);
    }
    print_list(&head, "after add_head 0..2");

    /* 꼬리에 3~5 삽입 */
    for (int i = 3; i < 6; ++i) {
        it = item_new(i);
        slist_add_tail(&it->node, &head);
    }
    print_list(&head, "after add_tail 3..5");

    /* pop head */
    pos = slist_pop_head(&head);
    if (pos) {
        it = slist_entry(pos, struct item, node);
        printf("popped: %d\n", it->key);
        item_free(it);
    }
    print_list(&head, "after pop_head");

    /* 짝수 삭제 */
    slist_for_each_safe(pos, n, &head) {
        it = slist_entry(pos, struct item, node);
        if (it->key % 2 == 0) {
            slist_del(&it->node, &head);
            item_free(it);
        }
    }
    print_list(&head, "after remove evens");

    return 0;
}

