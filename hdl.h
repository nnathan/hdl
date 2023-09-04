#ifndef HDL_H_
#define HDL_H_

#include <stdint.h>
#include <stdbool.h>

struct frag_list {
    struct frag_list *next;
    void *frag;
    int32_t len;
    int32_t offset;
};

struct hole_descriptor_list {
    struct hole_descriptor_list *next;
    int32_t first;
    int32_t last;
    struct frag_list *frag_head;
    struct frag_list *frag_tail;
};

bool hole_descriptor_list_init(struct hole_descriptor_list **hdl);

bool hole_descriptor_list_add(
    struct hole_descriptor_list *hdl,
    int32_t offset,
    int32_t len,
    bool final, /* is this the last fragment in the train? */
    void *frag
);

bool hole_descriptor_list_complete(struct hole_descriptor_list *hdl);

void hole_descriptor_list_walk(
    struct hole_descriptor_list *hdl,
    void (caller)(void *caller_ctx, const void *data, const int32_t offset, const int32_t len),
    void *caller_ctx
);
#endif
