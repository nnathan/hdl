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

bool hole_descriptor_list_init(struct hole_descriptor_list **hdl) {
    *hdl = calloc(1, sizeof(struct hole_descriptor_list));
    (*hdl)->last = INT32_MAX;
    return (*hdl != NULL);
}

bool hole_descriptor_list_complete(struct hole_descriptor_list *hdl) {
    return (hdl->first >= hdl->last);
}

static bool insert_frag(
    struct hole_descriptor_list *hdl,
    int32_t offset,
    int32_t len,
    void *frag
);

static void coalesce(struct hole_descriptor_list *hdl);

bool hole_descriptor_list_add(
    struct hole_descriptor_list *hdl,
    int32_t offset,
    int32_t len,
    bool final,
    void *frag
) {
    ASSERT(offset + len > offset);

    int32_t last = offset + len - 1;

    while (hdl != NULL) {
        if (offset > hdl->last) {
            hdl = hdl->next;
            continue;
        }

        /* although specified in RFC815 this case doesn't
         * seem to make sense since:
         *     last > offset > h->last >= h->first
         * which means last is always greater than h->first
         */
        if (last < hdl->first) {
            hdl = hdl->next;
            continue;
        }

        if (final) {
            hdl->last = offset + len;
        }

        if (offset < hdl->first) {
             /*
             * insert (5,6)
             * (0,10) [10, 14] (15, 5) (20, 10) [30, 30]
             * (0,10) (5,6) [11, 14] (15, 5) (20, 10) [30, 30]
             */
            if (offset + len > hdl->first) hdl->first = offset + len;
            if (!insert_frag(hdl, offset, len, frag)) return false;
            coalesce(hdl);
            return true;
        }

        if (offset == hdl->first) {
            hdl->first = offset + len;
            if (!insert_frag(hdl, offset, len, frag)) return false;
            coalesce(hdl);
            return true;
        }

        ASSERT(offset > hdl->first);
        if (hdl->next) {
            /*
             * insert (15,5)
             * (0,10) [10, 19] (20, 10) [30, 30]
             * (0,10) [10, 14] (15, 5) (20, 10) [30, 30]
             */

            if (hdl->next->first <= offset + len) {
                hdl->last = offset - 1;
                hdl->next->first = offset + len;
                if (!insert_frag(hdl->next, offset, len, frag)) return false;
                coalesce(hdl);
                coalesce(hdl->next);
                return true;
            }


            ASSERT(hdl->next->first > offset + len);
            struct hole_descriptor_list *nh = calloc(1, sizeof(struct hole_descriptor_list));
            if (!nh) return false;
            nh->next = hdl->next;
            hdl->next = nh;
            nh->first = offset + len;
            nh->last = hdl->last;
            hdl->last = offset - 1;
            nh->frag_head = nh->frag_tail = NULL;
            if (!insert_frag(nh, offset, len, frag)) return false;
            coalesce(hdl);
            coalesce(nh);
            return true;
        }

        /*
         * insert (15,5)
         * [0,20]
         * [0,14] (15,5) [20,20]
         */
        hdl->next = calloc(1, sizeof(struct hole_descriptor_list));
        if (!hdl->next) return false;
        hdl->last = offset - 1;
        hdl->next->first = offset + len;
        hdl->next->last = final ? offset + len : INT32_MAX;
        return insert_frag(hdl->next, offset, len, frag);
    }

    return true;
}

static void coalesce(struct hole_descriptor_list *hdl) {
    while (hdl && hdl->next && hdl->first >= hdl->last) {
        hdl->first = (hdl->next->first > hdl->first) ? hdl->next->first : hdl->first;
        hdl->last = hdl->next->last;
        hdl->frag_tail->next = hdl->next->frag_head;
        struct hole_descriptor_list *t = hdl->next;
        hdl->next = hdl->next->next;
        free(t);
        hdl = hdl->next;
    }
}

static bool insert_frag(
    struct hole_descriptor_list *hdl,
    int32_t offset,
    int32_t len,
    void *frag
) {
    struct frag_list *fl = hdl->frag_head;

    if (!fl) {
        hdl->frag_head = hdl->frag_tail = malloc(sizeof(struct frag_list));
        if (!hdl->frag_head) return false;
        hdl->frag_head->next = NULL;
        hdl->frag_head->frag = frag;
        hdl->frag_head->len = len;
        hdl->frag_head->offset = offset;
        return true;
    }

    while (fl) {
        if (offset < fl->offset || (offset == fl->offset && len > fl->len)) {
            struct frag_list *l = malloc(sizeof(struct frag_list));
            if (!l) return false;
            hdl->frag_head = l;
            l->next = fl;
            l->frag = frag;
            l->len = len;
            l->offset = offset;
            break;
        }

        if (fl->next) {
            fl = fl->next;
            continue;
        }

        struct frag_list *l = malloc(sizeof(struct frag_list));
        if (!l) return false;
        fl->next = l;
        hdl->frag_tail = l;
        l->next = NULL;
        l->frag = frag;
        l->len = len;
        l->offset = offset;
        break;
    }

    return true;
}

void print_hole_descriptor_list(struct hole_descriptor_list *hdl) {
    if (!hdl) {
        printf("<empty>\n");
    }

    while (hdl) {
        printf("%10s: %10d\n", "first", hdl->first);
        printf("%10s: %10d\n", "last", hdl->last);

        for (struct frag_list *fl = hdl->frag_head; fl != NULL; fl = fl->next) {
            printf("%10s (%d,%d)\n", "--", fl->offset, fl->len);
        }

        hdl = hdl->next;
    }
}
