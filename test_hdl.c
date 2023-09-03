#include "hdl.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

struct fragment {
    int32_t offset;
    int32_t len;
    bool final;
};

#define PRINT_FRAGS(l,n)                               \
  do {                                                 \
    printf("--- ");                                    \
    for (int i=0; i<(n); i++) {                        \
        printf("(%d,%d) ", (l)[i].offset, (l)[i].len); \
    }                                                  \
    printf("\n");                                      \
  } while (0);



int main(int argc, char **argv) {

    // --- (0,1)
    {
        struct hole_descriptor_list *hdl;
        hole_descriptor_list_init(&hdl);
        assert(hdl != NULL);
        const int n = 1;
        struct fragment *l = calloc(n, sizeof(struct fragment));
        l[0].offset = 0;
        l[0].len = 1;
        l[0].final = true;

        PRINT_FRAGS(l, n);

        for (int i=0; i<n; i++) {
            hole_descriptor_list_add(hdl, l[i].offset, l[i].len, l[i].final, NULL);
        }

        free(l);

        assert(hole_descriptor_list_complete(hdl));

        const int el_n = 1;
        struct fragment *el = calloc(el_n, sizeof(struct fragment));
        el[0].offset = 0;
        el[0].len = 1;

        int i = 0;
        for (struct frag_list *fl = hdl->frag_head; fl; fl = fl->next, i++) {
            assert(fl->offset == el[i].offset);
            assert(fl->len == el[i].len);
        }
        assert(i == el_n);
        free(el);
    }
    // --- (0,5) (1,2) (5,1)
    {
        struct hole_descriptor_list *hdl;
        hole_descriptor_list_init(&hdl);
        assert(hdl != NULL);
        const int n = 3;
        struct fragment *l = calloc(n, sizeof(struct fragment));
        l[0].offset = 0;
        l[0].len = 5;
        l[1].offset = 1;
        l[1].len = 2;
        l[2].offset = 5;
        l[2].len = 1;
        l[2].final = true;

        PRINT_FRAGS(l, n);

        for (int i=0; i<n; i++) {
            hole_descriptor_list_add(hdl, l[i].offset, l[i].len, l[i].final, NULL);
        }

        free(l);

        assert(hole_descriptor_list_complete(hdl));

        const int el_n = 2;
        struct fragment *el = calloc(el_n, sizeof(struct fragment));
        el[0].offset = 0;
        el[0].len = 5;
        el[1].offset = 5;
        el[1].len = 1;

        int i = 0;
        for (struct frag_list *fl = hdl->frag_head; fl; fl = fl->next, i++) {
            assert(fl->offset == el[i].offset);
            assert(fl->len == el[i].len);
        }
        assert(i == el_n);
        free(el);
    }
    // --- (0,1) (1,1) (2,1) (3,1) (4,1)
    {
        struct hole_descriptor_list *hdl;
        hole_descriptor_list_init(&hdl);
        assert(hdl != NULL);
        const int n = 5;
        struct fragment *l = calloc(n, sizeof(struct fragment));
        for (int i=0; i < n; i++) {
            l[i].offset = 1 * i;
            l[i].len = 1;
        }
        l[4].final = true;

        PRINT_FRAGS(l, n);

        for (int i=0; i<n; i++) {
            hole_descriptor_list_add(hdl, l[i].offset, l[i].len, l[i].final, NULL);
        }

        free(l);

        assert(hole_descriptor_list_complete(hdl));

        const int el_n = 5;
        struct fragment *el = calloc(el_n, sizeof(struct fragment));
        for (int i=0; i < n; i++) {
            el[i].offset = 1 * i;
            el[i].len = 1;
        }

        int i = 0;
        for (struct frag_list *fl = hdl->frag_head; fl; fl = fl->next, i++) {
            assert(fl->offset == el[i].offset);
            assert(fl->len == el[i].len);
        }
        assert(i == el_n);
        free(el);
    }
    // --- (1,1) (3,1) (4,1) (2,1) (0,1)
    {
        struct hole_descriptor_list *hdl;
        hole_descriptor_list_init(&hdl);
        assert(hdl != NULL);
        const int n = 5;
        struct fragment *l = calloc(n, sizeof(struct fragment));
        l[0].offset = 1;
        l[0].len = 1;
        l[1].offset = 3;
        l[1].len = 1;
        l[2].offset = 4;
        l[2].len = 1;
        l[2].final = true;
        l[3].offset = 2;
        l[3].len = 1;
        l[4].offset = 0;
        l[4].len = 1;

        PRINT_FRAGS(l, n);

        for (int i=0; i<n; i++) {
            hole_descriptor_list_add(hdl, l[i].offset, l[i].len, l[i].final, NULL);
        }

        free(l);

        const int el_n = 5;
        struct fragment *el = calloc(el_n, sizeof(struct fragment));
        for (int i=0; i < n; i++) {
            el[i].offset = 1 * i;
            el[i].len = 1;
        }

        int i = 0;
        for (struct frag_list *fl = hdl->frag_head; fl; fl = fl->next, i++) {
            assert(fl->offset == el[i].offset);
            assert(fl->len == el[i].len);
        }
        assert(i == el_n);
        free(el);
    }
    // --- (7,3) (4,6) (1,9) (0,10)
    {
        struct hole_descriptor_list *hdl;
        hole_descriptor_list_init(&hdl);
        assert(hdl != NULL);
        const int n = 4;
        struct fragment *l = calloc(n, sizeof(struct fragment));
        l[0].offset = 7;
        l[0].len = 3;
        l[0].final = true;
        l[1].offset = 4;
        l[1].len = 6;
        l[1].final = true;
        l[2].offset = 1;
        l[2].len = 9;
        l[2].final = true;
        l[3].offset = 0;
        l[3].len = 10;
        l[3].final = true;

        PRINT_FRAGS(l, n);

        for (int i=0; i<n; i++) {
            hole_descriptor_list_add(hdl, l[i].offset, l[i].len, l[i].final, NULL);
        }

        free(l);

        const int el_n = 4;
        struct fragment *el = calloc(el_n, sizeof(struct fragment));
        el[0].offset = 0;
        el[0].len = 10;
        el[0].final = true;
        el[1].offset = 1;
        el[1].len = 9;
        el[1].final = true;
        el[2].offset = 4;
        el[2].len = 6;
        el[2].final = true;
        el[3].offset = 7;
        el[3].len = 3;
        el[3].final = true;

        int i = 0;
        for (struct frag_list *fl = hdl->frag_head; fl; fl = fl->next, i++) {
            assert(fl->offset == el[i].offset);
            assert(fl->len == el[i].len);
        }
        assert(i == el_n);
        free(el);
    }

    return 0;
}
