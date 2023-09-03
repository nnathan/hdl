#ifndef HDL_H_
#define HDL_H_

#include <stdint.h>
#include <stdbool.h>

struct hole_data {
    struct hole_data *next;
    void *frag;
    int32_t len;
    int32_t offset;
};

struct hole {
    struct hole *next;
    int32_t first;
    int32_t last;
    struct hole_data *data_head;
    struct hole_data *data_tail;
};

struct hole_descriptor_list {
    struct hole *head;
    int32_t size;
};

bool hole_descriptor_list_init(struct hole_descriptor_list *hdl);

bool hole_descriptor_list_add(
    struct hole_descriptor_list *hdl,
    int32_t offset,
    int32_t len,
    bool final, /* is this the last fragment in the train? */
    void *frag
);

bool hole_descriptor_list_complete(struct hole_descriptor_list *hdl);

void print_hole_descriptor_list(struct hole_descriptor_list *hdl);
#endif
