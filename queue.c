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

    element_t *tmp = list_first_entry(head, element_t, list);
    list_del(&tmp->list);

    if (sp && bufsize) {
        strncpy(sp, tmp->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    return tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *tmp = list_last_entry(head, element_t, list);
    list_del(&tmp->list);

    if (sp && bufsize) {
        strncpy(sp, tmp->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
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

    struct list_head *node, *save, *tmp;
    list_for_each_safe (node, save, head) {
        for (tmp = save; tmp != head; tmp = tmp->next)
            if (!strcmp(list_entry(node, element_t, list)->value,
                        list_entry(tmp, element_t, list)->value)) {
                list_del(node);
                q_release_element(list_entry(node, element_t, list));
                break;
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
    for (struct list_head **l = &head->next; *l != head && (*l)->next != head;
         l = &(*l)->next) {
        struct list_head *first = head, *second = head->next;
        first->prev = second;
        first->next = second->next;
        second->prev = first->prev;
        second->next = first;
        *l = first;
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
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
