#include "hdl.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

bool hole_descriptor_list_init(struct hole_descriptor_list **hdl) {
    *hdl = calloc(1, sizeof(struct hole_descriptor_list));
    (*hdl)->last = INT32_MAX;
    return (*hdl != NULL);
}

bool hole_descriptor_list_complete(struct hole_descriptor_list *hdl) {
    return (!hdl->next && hdl->first >= hdl->last);
}

static bool insert_frag(
    struct hole_descriptor_list *hdl,
    int32_t offset,
    int32_t len,
    void *frag
);

static void coalesce(struct hole_descriptor_list *hdl, bool final);

bool hole_descriptor_list_add(
    struct hole_descriptor_list *hdl,
    int32_t offset,
    int32_t len,
    bool final,
    void *frag
) {
    assert(offset + len > offset);

    int32_t last = offset + len - 1;

    while (hdl != NULL) {
        if (offset > hdl->last) {
            hdl = hdl->next;
            continue;
        }

        /*
         * suppress already seen or duplicate fragments
         */
        if (last < hdl->first) {
            hdl = hdl->next;
            continue;
        }

        if (final) {
            hdl->last = offset + len;
        }

        if (offset < hdl->first) {
            if (offset + len > hdl->first) hdl->first = offset + len;
            if (!insert_frag(hdl, offset, len, frag)) return false;
            coalesce(hdl, final);
            return true;
        }

        if (offset == hdl->first) {
            hdl->first = offset + len;
            if (!insert_frag(hdl, offset, len, frag)) return false;
            coalesce(hdl, final);
            return true;
        }

        assert(offset > hdl->first);
        if (hdl->next) {
            if (hdl->next->first <= offset + len) {
                hdl->last = offset - 1;
                hdl->next->first = offset + len;
                hdl->next->last = final ? offset + len : hdl->next->last;
                if (!insert_frag(hdl->next, offset, len, frag)) return false;
                coalesce(hdl->next, final);
                return true;
            }


            assert(hdl->next->first > offset + len);
            struct hole_descriptor_list *nh = calloc(1, sizeof(struct hole_descriptor_list));
            if (!nh) return false;
            nh->next = hdl->next;
            hdl->next = nh;
            nh->first = offset + len;
            nh->last = hdl->last;
            hdl->last = offset - 1;
            nh->frag_head = nh->frag_tail = NULL;
            if (!insert_frag(nh, offset, len, frag)) return false;
            coalesce(hdl, false);
            coalesce(nh, final);
            return true;
        }

        hdl->next = calloc(1, sizeof(struct hole_descriptor_list));
        if (!hdl->next) return false;
        hdl->last = offset - 1;
        hdl->next->first = offset + len;
        hdl->next->last = final ? offset + len : INT32_MAX;
        return insert_frag(hdl->next, offset, len, frag);
    }

    return true;
}

static void coalesce(struct hole_descriptor_list *hdl, bool final) {
    while (hdl->next && (final ? hdl->first >= hdl->last : hdl->first > hdl->last)) {
        hdl->first = (hdl->next->first > hdl->first) ? hdl->next->first : hdl->first;
        if (!final) hdl->last = (hdl->next->last > hdl->last) ? hdl->next->last : hdl->last;
        hdl->frag_tail->next = hdl->next->frag_head;
        hdl->frag_tail = hdl->next->frag_tail;
        struct hole_descriptor_list *t = hdl->next;
        hdl->next = hdl->next->next;
        free(t);
    }
}

#define SET_FRAG_LIST(fl, next_, frag, len, off) \
  do {                                           \
    fl->next = next_;                            \
    fl->frag = frag;                             \
    fl->len = len;                               \
    fl->off = off;                               \
  } while(0)

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
        SET_FRAG_LIST(hdl->frag_head, NULL, frag, len, offset);
        return true;
    }

    struct frag_list *prev = NULL;
    while (fl) {
        if (offset < fl->offset || (offset == fl->offset && len > fl->len)) {
            struct frag_list *l = malloc(sizeof(struct frag_list));
            if (!l) return false;
            if (!prev) {
                hdl->frag_head = l;
            } else {
                prev->next = l;
            }
            SET_FRAG_LIST(l, fl, frag, len, offset);
            break;
        }

        if (fl->next) {
            prev = fl;
            fl = fl->next;
            continue;
        }

        struct frag_list *l = malloc(sizeof(struct frag_list));
        if (!l) return false;
        fl->next = l;
        hdl->frag_tail = l;
        SET_FRAG_LIST(l, NULL, frag, len, offset);
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
