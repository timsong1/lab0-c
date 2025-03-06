#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*  struct list_head {
 *    struct list_head *prev;
 *    struct list_head *next;
 *  }
 */

/*  typedef struct {
 *    char *value;
 *    struct list_head list;
 *  } element_t;
 */
/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (head) {
        head->prev = head;
        head->next = head;
    }
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        q_release_element(list_entry(node, element_t, list));
    }
    free(head);
}
/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *element = (element_t *) malloc(sizeof(element_t));
    if (!element)
        return false;
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }
    list_add(&element->list, head);
    return true;
}
/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *element = (element_t *) malloc(sizeof(element_t));
    if (!element)
        return false;
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }
    list_add_tail(&element->list, head);
    return true;
}
/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *curr = container_of(head->next, element_t, list);
    list_del_init(head->next);
    if (sp && bufsize > 0 && curr->value) {
        strncpy(sp, curr->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return curr;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *curr = container_of(head->prev, element_t, list);
    list_del_init(head->prev);
    if (sp && bufsize > 0 && curr->value) {
        strncpy(sp, curr->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return curr;
}
/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list
    if (!head || list_empty(head))
        return false;
    struct list_head *left, *right;
    left = head->prev;
    right = head->next;
    while (left != right && left->prev != right) {
        left = left->prev;
        right = right->next;
    }
    list_del(left);
    q_release_element(list_entry(left, element_t, list));
    return true;
}
void delete_linked_list(struct list_head *head,
                        struct list_head *from,
                        struct list_head *to)
{
    struct list_head *node, *safe;
    bool flag = false;
    list_for_each_safe (node, safe, head) {
        if (node == to)
            return;
        if (node == from)
            flag = true;
        if (flag) {
            list_del(node);
            q_release_element(list_entry(node, element_t, list));
        }
    }
}
/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    if (head->next == head->prev)
        return true;
    struct list_head *curr = head->next, *next = curr->next;
    while (curr != head && next != head) {
        const element_t *element = list_entry(curr, element_t, list);
        if (strcmp(element->value, list_entry(next, element_t, list)->value) ==
            0) {
            while (next != head) {
                if (strcmp(list_entry(next, element_t, list)->value,
                           element->value) != 0)
                    break;
                next = next->next;
            }
            delete_linked_list(head, curr, next);
            curr = next;
            next = next->next;
            continue;
        }
        curr = curr->next;
        next = curr->next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || head->next == head->prev) {
        return;
    }
    struct list_head *curr, *next;
    curr = head->next;
    next = curr->next;
    while (curr != head && next != head) {
        curr->prev->next = next;
        next->next->prev = curr;
        curr->next = next->next;
        next->prev = curr->prev;
        curr->prev = next;
        next->next = curr;
        curr = curr->next;
        next = curr->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || head->next == head->prev)
        return;
    struct list_head *node = NULL, *safe;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    // Assume k is a positive integer and is less than or equal to the length of
    // the linked list.
    if (!head || head->prev == head->next)
        return;
    int len = 0;
    struct list_head *li = head->next, *start = head, *stop;
    while (li != head) {
        len++;
        if (len == k) {
            stop = li->next;
            struct list_head *curr = start->next, *next = curr->next,
                             *next_start = stop;
            while (curr != stop) {
                while (next != stop) {
                    curr->prev->next = next;
                    next->next->prev = curr;
                    curr->next = next->next;
                    next->prev = curr->prev;
                    curr->prev = next;
                    next->next = curr;
                    next = curr->next;
                }
                curr = start->next;
                next = curr->next;
                stop = stop->prev;
            }
            len = 0;
            li = start = next_start->prev;
        }
        li = li->next;
    }
}
struct list_head *merge_sort(struct list_head *head, bool descend)
{
    if (!head || !head->next)
        return head;
    struct list_head *slow, *fast, *left, *right;
    slow = fast = left = head;
    while (fast && fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    right = slow->next;
    slow->next = NULL;
    right->prev = NULL;
    left = merge_sort(left, descend);
    right = merge_sort(right, descend);
    struct list_head **ptr = &head;
    for (;;) {
        if (!left) {
            *ptr = right;
            break;
        }
        if (!right) {
            *ptr = left;
            break;
        }
        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) > 0) {
            if (descend) {
                *ptr = left;
                ptr = &left->next;
                left = left->next;
            } else {
                *ptr = right;
                ptr = &right->next;
                right = right->next;
            }
        } else {
            if (descend) {
                *ptr = right;
                ptr = &right->next;
                right = right->next;
            } else {
                *ptr = left;
                ptr = &left->next;
                left = left->next;
            }
        }
    }
    return head;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || head->next == head->prev)
        return;
    head->prev->next = NULL;
    head->next = merge_sort(head->next, descend);
    // re-link prev pointer
    struct list_head *li = head->next, *prev = head;
    while (li) {
        li->prev = prev;
        li = li->next;
        prev = prev->next;
    }
    prev->next = head;
    head->prev = prev;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    int count = 1;
    if (!head || head->prev == head->next)
        return 0;
    struct list_head *curr = head->prev, *min = head->prev, *prev = curr->prev;
    while (curr->prev != head) {
        element_t *previous;
        const element_t *min_element;
        previous = list_entry(prev, element_t, list);
        min_element = list_entry(min, element_t, list);
        if (strcmp(previous->value, min_element->value) > 0) {
            list_del(prev);
            q_release_element(previous);
            prev = curr->prev;
        } else {
            min = curr = prev;
            count++;
            prev = curr->prev;
        }
    }
    return count;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    int count = 1;
    if (!head || head->prev == head->next)
        return 0;
    struct list_head *curr = head->prev, *max = head->prev, *prev = curr->prev;
    while (curr->prev != head) {
        element_t *previous;
        const element_t *max_element;
        previous = list_entry(prev, element_t, list);
        max_element = list_entry(max, element_t, list);
        if (strcmp(previous->value, max_element->value) < 0) {
            list_del(prev);
            q_release_element(previous);
            prev = curr->prev;
        } else {
            max = curr = prev;
            count++;
            prev = curr->prev;
        }
    }
    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *context = list_entry(head->next, queue_contex_t, chain);
    struct list_head *first_queue;
    context->q->prev->next = NULL;
    struct list_head *li = context->chain.next;
    while (li != head) {
        first_queue = context->q->next;
        queue_contex_t *li_context = list_entry(li, queue_contex_t, chain);
        li_context->q->prev->next = NULL;
        struct list_head *curr = li_context->q->next;
        struct list_head **ptr = &context->q->next;
        for (;;) {
            if (!curr) {
                *ptr = first_queue;
                break;
            }
            if (!first_queue) {
                *ptr = curr;
                break;
            }
            if (strcmp(list_entry(first_queue, element_t, list)->value,
                       list_entry(curr, element_t, list)->value) > 0) {
                if (descend) {
                    *ptr = first_queue;
                    ptr = &first_queue->next;
                    first_queue = first_queue->next;
                } else {
                    *ptr = curr;
                    ptr = &curr->next;
                    curr = curr->next;
                }
            } else {
                if (descend) {
                    *ptr = curr;
                    ptr = &curr->next;
                    curr = curr->next;
                } else {
                    *ptr = first_queue;
                    ptr = &first_queue->next;
                    first_queue = first_queue->next;
                }
            }
        }
        context->size += li_context->size;
        INIT_LIST_HEAD(li_context->q);
        li_context->size = 0;
        li = li->next;
    }
    struct list_head *index = context->q, *next = index->next;
    while (next) {
        next->prev = index;
        index = index->next;
        next = index->next;
    }
    context->q->prev = index;
    index->next = context->q;
    return context->size;
}
