#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        free(head);
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    if (list_empty(l)) {
        free(l);
        return;
    }
    struct list_head *node, *save;
    list_for_each_safe (node, save, l)
        q_release_element(container_of(node, element_t, list));
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_ele = malloc(sizeof(element_t));
    if (!new_ele) {
        free(new_ele);
        return false;
    }
    size_t len = strlen(s) + 1;
    char *new_str = malloc(len * sizeof(char));
    if (!new_str) {
        free(new_str);
        free(new_ele);
        return false;
    }
    strncpy(new_str, s, len);
    new_ele->value = new_str;
    list_add(&new_ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_ele = malloc(sizeof(element_t));
    if (!new_ele) {
        free(new_ele);
        return false;
    }
    size_t len = strlen(s) + 1;
    char *new_str = malloc(len * sizeof(char));
    if (!new_str) {
        free(new_str);
        free(new_ele);
        return false;
    }
    strncpy(new_str, s, len);
    new_ele->value = new_str;
    list_add_tail(&new_ele->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *tmp = container_of(head->next, element_t, list);

    if (sp && bufsize) {
        strncpy(sp, tmp->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    list_del(head->next);
    return tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *tmp = container_of(head->prev, element_t, list);

    if (sp && bufsize) {
        strncpy(sp, tmp->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    list_del(head->prev);
    return tmp;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int size = 0;
    struct list_head *count = NULL;
    list_for_each (count, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *fast = head->next, *slow = head->next;
    while (fast != head && fast->next != head)
        slow = slow->next, fast = fast->next->next;
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    struct list_head *node, *save;
    bool is_dup = false;
    list_for_each_safe (node, save, head) {
        if (node->next != head &&
            !strcmp(list_entry(node, element_t, list)->value,
                    list_entry(save, element_t, list)->value)) {
            is_dup = true;
            list_del(node);
            q_release_element(list_entry(node, element_t, list));
        } else if (is_dup) {
            is_dup = false;
            list_del(node);
            q_release_element(list_entry(node, element_t, list));
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    struct list_head *node, *save;
    for (node = head->next, save = node->next; node != head && save != head;
         node = node->next, save = node->next) {
        list_move(node, save);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node, *save;
    list_for_each_safe (node, save, head)
        list_move(node, head);
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k <= 1)
        return;
    int i = k;
    struct list_head *node, *save, *cut = head;
    list_for_each_safe (node, save, head) {
        if (--i)
            continue;
        i = k;
        LIST_HEAD(tmp);
        list_cut_position(&tmp, cut, node);
        q_reverse(&tmp);
        list_splice(&tmp, cut);
        cut = save->prev;
    }
}

static inline void merge_list(struct list_head *head,
                              struct list_head *left,
                              struct list_head *right)
{
    while (!list_empty(left) && !list_empty(right)) {
        if (strcmp(list_entry(left->next, element_t, list)->value,
                   list_entry(right->next, element_t, list)->value) < 0) {
            list_move_tail(left->next, head);
        } else {
            list_move_tail(right->next, head);
        }
    }

    if (!list_empty(left)) {
        list_splice_tail_init(left, head);
    } else {
        list_splice_tail_init(right, head);
    }
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    /* Using merge sort */
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    /* Find the middle point */
    struct list_head *fast, *slow;
    fast = head, slow = head;
    do {
        fast = fast->next->next;
        slow = slow->next;
    } while (fast != head && fast->next != head);

    /* Divide into left and right parts */
    LIST_HEAD(left);
    LIST_HEAD(right);
    list_cut_position(&left, head, slow);
    list_cut_position(&right, head, head->prev);

    /* Recursion */
    q_sort(&left);
    q_sort(&right);
    merge_list(head, &left, &right);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *elem = list_entry(head->prev, element_t, list),
              *save = list_entry(elem->list.prev, element_t, list);
    char *best = elem->value;
    while (&elem->list != head) {
        if (strcmp(elem->value, best) < 0) {
            list_del(&elem->list);
            q_release_element(elem);
        } else {
            best = elem->value;
        }

        elem = save;
        save = list_entry(elem->list.prev, element_t, list);
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    int n = q_size(head);
    while (n > 1) {
        struct list_head *fast = head->next, *slow = head->next;
        for (int i = 0; i < n / 2; ++i) {
            LIST_HEAD(temp);
            merge_list(&temp, container_of(fast, queue_contex_t, chain)->q,
                       container_of(fast->next, queue_contex_t, chain)->q);

            list_splice_tail(&temp,
                             container_of(slow, queue_contex_t, chain)->q);

            fast = fast->next->next;
            slow = slow->next;
        }
        if (n & 1)
            list_splice_tail_init(container_of(fast, queue_contex_t, chain)->q,
                                  container_of(slow, queue_contex_t, chain)->q);

        n = (n + 1) / 2;
    }

    return q_size(container_of(head->next, queue_contex_t, chain)->q);
}
