#include "hdl.h"
#include <stdio.h>
#include <stdlib.h>

// Source: skeeto/handmade hero
#ifdef DEBUG
#define ASSERT(expr)             \
    if (!(expr)) {               \
        *(volatile int *) 0 = 0; \
    }
#else
#define ASSERT(expr)
#endif

bool hole_descriptor_list_init(struct hole_descriptor_list *hdl) {
    hdl->size = INT32_MAX;
    hdl->head = malloc(sizeof(struct hole));
    if (!hdl->head) return false;
    hdl->head->first = 0;
    hdl->head->last = INT32_MAX;
    hdl->head->data_head = hdl->head->data_tail = NULL;
    hdl->head->next = NULL;
    return true;
}

bool hole_descriptor_list_complete(struct hole_descriptor_list *hdl) {
    return (hdl->head->first >= hdl->head->last);
}

static bool insert_hole_data(struct hole *h, int32_t offset, int32_t len, void *frag);
static void coalesce(struct hole *h);

bool hole_descriptor_list_add(
    struct hole_descriptor_list *hdl,
    int32_t offset,
    int32_t len,
    bool final,
    void *frag
) {
    ASSERT(offset + len > offset);

    struct hole *h = hdl->head;

    int32_t last = offset + len - 1;

    while (h != NULL) {
        if (offset > h->last) {
            h = h->next;
            continue;
        }

        /* although specified in RFC815 this case doesn't
         * seem to make sense since:
         *     last > offset > h->last >= h->first
         * which means last is always greater than h->first
         */
        if (last < h->first) {
            h = h->next;
            continue;
        }

        if (final) {
            h->last = offset + len;
        }

        if (offset < h->first) {
             /*
             * insert (5,6)
             * (0,10) [10, 14] (15, 5) (20, 10) [30, 30]
             * (0,10) (5,6) [11, 14] (15, 5) (20, 10) [30, 30]
             */
            if (offset + len > h->first) h->first = offset + len;
            if (!insert_hole_data(h, offset, len, frag)) return false;
            coalesce(h);
            return true;
        }

        if (offset == h->first) {
            h->first = offset + len;
            if (!insert_hole_data(h, offset, len, frag)) return false;
            coalesce(h);
            return true;
        }

        ASSERT(offset > h->first);
        if (h->next) {
            /*
             * insert (15,5)
             * (0,10) [10, 19] (20, 10) [30, 30]
             * (0,10) [10, 14] (15, 5) (20, 10) [30, 30]
             */

            if (h->next->first <= offset + len) {
                h->last = offset - 1;
                h->next->first = offset + len;
                if (!insert_hole_data(h->next, offset, len, frag)) return false;
                coalesce(h);
                coalesce(h->next);
                return true;
            }


            ASSERT(h->next->first > offset + len);
            struct hole *nh = malloc(sizeof(struct hole));
            if (!nh) return false;
            nh->next = h->next;
            h->next = nh;
            nh->first = offset + len;
            nh->last = h->last;
            h->last = offset - 1;
            nh->data_head = nh->data_tail = NULL;
            if (!insert_hole_data(nh, offset, len, frag)) return false;
            coalesce(h);
            coalesce(nh);
            return true;
        }

        /*
         * insert (15,5)
         * [0,20]
         * [0,14] (15,5) [20,20]
         */
        h->next = malloc(sizeof(struct hole));
        if (!h->next) return false;
        h->last = offset - 1;
        h->next->first = offset + len;
        h->next->last = final ? offset + len : INT32_MAX;
        return insert_hole_data(h->next, offset, len, frag);
    }

    return true;
}

static void coalesce(struct hole *h) {
    while (h && h->next && h->first >= h->last) {
        h->first = (h->next->first > h->first) ? h->next->first : h->first;
        h->last = h->next->last;
        h->data_tail->next = h->next->data_head;
        struct hole *t = h->next;
        h->next = h->next->next;
        free(t);
        h = h->next;
    }
}

static bool insert_hole_data(struct hole *h, int32_t offset, int32_t len, void *frag) {
    struct hole_data *hd = h->data_head;

    if (!hd) {
        h->data_head = h->data_tail = malloc(sizeof(struct hole_data));
        if (!h->data_head) return false;
        h->data_head->next = NULL;
        h->data_head->frag = frag;
        h->data_head->len = len;
        h->data_head->offset = offset;
        return true;
    }

    while (hd) {
        if (offset < hd->offset || (offset == hd->offset && len > hd->len)) {
            struct hole_data *d = malloc(sizeof(struct hole_data));
            if (!d) return false;
            h->data_head = d;
            d->next = hd;
            d->frag = frag;
            d->len = len;
            d->offset = offset;
            break;
        }

        if (hd->next) {
            hd = hd->next;
            continue;
        }

        struct hole_data *d = malloc(sizeof(struct hole_data));
        if (!d) return false;
        hd->next = d;
        h->data_tail = d;
        d->next = NULL;
        d->frag = frag;
        d->len = len;
        d->offset = offset;
        break;
    }

    return true;
}

void print_hole_descriptor_list(struct hole_descriptor_list *hdl) {
    struct hole *h = hdl->head;

    if (!h) {
        printf("<empty>\n");
    }

    while (h) {
        printf("%10s: %10d\n", "first", h->first);
        printf("%10s: %10d\n", "last", h->last);

        for (struct hole_data *hd = h->data_head; hd != NULL; hd = hd->next) {
            printf("%10s (%d,%d)\n", "--", hd->offset, hd->len);
        }

        h = h->next;
    }
}
