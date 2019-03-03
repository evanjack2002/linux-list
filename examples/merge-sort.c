#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#include "common.h"

static uint16_t values[256];

static void list_merge_sort(struct list_head *head, size_t merge_size)
{
    struct list_head list_left, list_right;
    struct listitem *item1 = NULL, *item2 = NULL;
    struct listitem *is1 = NULL, *is2 = NULL;
    size_t i;

    if (list_empty(head) || list_is_singular(head))
        return;

    if (merge_size == 1)
        return;

    INIT_LIST_HEAD(&list_left);
    INIT_LIST_HEAD(&list_right);
    assert(list_empty(&list_left));
    assert(list_empty(&list_right));

    list_splice_init(head, &list_right);
    assert(!list_empty(&list_right));

    i = 1;
    list_for_each_entry (item1, &list_right, list) {
        if (i == (merge_size / 2))
            break;
        i++;
    }
    list_cut_position(&list_left, &list_right, &item1->list);
    assert(!list_empty(&list_left));

    list_merge_sort(&list_left, i);
    list_merge_sort(&list_right, merge_size - i);

    assert(list_empty(head));
    assert(!list_empty(&list_left));
    assert(!list_empty(&list_right));

    if ((list_is_singular(&list_left)) && (list_is_singular(&list_right))) {
        item1 = list_first_entry(&list_left, struct listitem, list);
        item2 = list_first_entry(&list_right, struct listitem, list);

        if (cmpint(&item1->i, &item2->i) < 0)
            list_move(&item1->list, &list_right);
        else
            list_move_tail(&item1->list, &list_right);
        list_splice_init(&list_right, head);
    } else if (list_is_singular(&list_left)) {
        int found = 0;
        item1 = list_first_entry(&list_left, struct listitem, list);
        list_for_each_entry_safe (item2, is1, &list_right, list) {
            if (cmpint(&item1->i, &item2->i) < 0) {
                list_move(&item1->list, &item2->list);
                found = 1;
                break;
            }
        }
        if (!found)
            list_move_tail(&item1->list, &list_right);
        list_splice_init(&list_right, head);
    } else if (list_is_singular(&list_right)) {
        int found = 0;
        item1 = list_first_entry(&list_right, struct listitem, list);
        list_for_each_entry_safe (item2, is1, &list_left, list) {
            if (cmpint(&item1->i, &item2->i) < 0) {
                list_move(&item1->list, &item2->list);
                found = 1;
                break;
            }
        }
        if (!found)
            list_move_tail(&item1->list, &list_left);
        list_splice_init(&list_left, head);
    } else {
        list_for_each_entry_safe (item1, is1, &list_left, list) {
            list_for_each_entry_safe (item2, is2, &list_right, list) {
                if (cmpint(&item2->i, &item1->i) < 0) {
                    list_move_tail(&item2->list, head);
                } else {
                    break;
                }
            }
            list_move_tail(&item1->list, head);
        }
        list_splice_tail_init(&list_right, head);
    }

    assert(list_empty(&list_left));
    assert(list_empty(&list_right));
    assert(!list_empty(head));
    return;
}

int main(void)
{
    struct list_head testlist;
    struct listitem *item, *is = NULL;
    size_t i;
    size_t merge_size;

    random_shuffle_array(values, (uint16_t) ARRAY_SIZE(values));

    INIT_LIST_HEAD(&testlist);
    assert(list_empty(&testlist));

    for (i = 0; i < ARRAY_SIZE(values); i++) {
        item = (struct listitem *) malloc(sizeof(*item));
        assert(item);
        item->i = values[i];
        list_add_tail(&item->list, &testlist);
    }

    assert(!list_empty(&testlist));

    qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);

    merge_size = ARRAY_SIZE(values);
    list_merge_sort(&testlist, merge_size);

    i = 0;
    list_for_each_entry_safe (item, is, &testlist, list) {
        assert(item->i == values[i]);
        list_del(&item->list);
        free(item);
        i++;
    }

    assert(i == ARRAY_SIZE(values));
    assert(list_empty(&testlist));

    return 0;
}
