#include "hdl.h"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

void print_hole_descriptor_list(struct hole_descriptor_list *hdl)
{
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

void print_frag(void *caller_ctx, const void *frag, const int32_t offset, const int32_t len)
{
    printf("* (%d,%d,%p)\n", offset, len, frag);
}

#define HDL_ADD(hdl, off, len, final, verbose)                       \
  do {                                                               \
    printf("--- inserting (%d,%d)%s\n", off, len, final ? "F" : ""); \
    assert(hole_descriptor_list_add(hdl, off, len, final, NULL));    \
    if (verbose) print_hole_descriptor_list(hdl);                    \
  } while(0)

int main(int argc, char **argv)
{
    struct hole_descriptor_list *hdl;

    if (!hole_descriptor_list_init(&hdl)) {
        printf("Failed to create hole descriptor list\n");
        return 1;
    }

    HDL_ADD(hdl, 69, 22, false, false);
    HDL_ADD(hdl, 75, 16, false, false);
    HDL_ADD(hdl, 80, 10, false, false);
    HDL_ADD(hdl, 81, 12, false, false);
    HDL_ADD(hdl, 59, 23, false, false);
    HDL_ADD(hdl, 30, 10, false, false);
    HDL_ADD(hdl, 63, 34, false, false);
    HDL_ADD(hdl, 87, 9, false, false);
    HDL_ADD(hdl, 13, 32, false, false);
    HDL_ADD(hdl, 44, 56, true, true);
    HDL_ADD(hdl, 0, 13, false, true);
    assert(hole_descriptor_list_complete(hdl));
    hole_descriptor_list_walk(hdl, print_frag, NULL);
    hole_descriptor_list_destroy(hdl);

  return 0;
}
