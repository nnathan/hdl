#include "hdl.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Source: skeeto
static uint8_t rand_byte(uint64_t *s)
{
    *s = *s * 0x3243f6a8885a308d + 1;
    return *s >> 56;
}

uint64_t gen_seed() {
    union {
        uint8_t bytes[8];
        uint64_t u64;
    } v;

    for (int i=0; i<8; i++) {
        v.bytes[i] = rand() % 256;
    }

    return v.u64;
}

// from https://stackoverflow.com/questions/322938/recommended-way-to-initialize-srand
unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

struct fragment {
    int32_t offset;
    int32_t len;
    bool final;
};

void shuffle(struct fragment *l, int n, uint64_t *seed) {
    for (int i = n-1; i > 0; i--) {
        int j = rand_byte(seed) % (i+1);
        struct fragment t = l[i];
        l[i] = l[j];
        l[j] = t;
    }
}

struct fragment random_fragment(int32_t max_len, uint64_t *seed) {
    union {
        uint8_t bytes[4];
        uint32_t u32;
    } v;

    for (int i=0; i < 4; i++) v.bytes[i] = rand_byte(seed);

    struct fragment f = {0};
    f.offset = v.u32 % max_len;
    for (int i=0; i < 4; i++) v.bytes[i] = rand_byte(seed);
    // probably be better to select from range (offset,max_len)
    f.len = v.u32 % (max_len - f.offset) + 1;
    if (f.offset + f.len >= max_len) {
        f.len = max_len - f.offset;
        f.final = true;
    }

    return f;
}

int main(int argc, char **argv) {
    uint64_t s;
    srand(mix(clock(), getpid(), time(NULL)));

    //{
    //    struct fragment *l = calloc(10, sizeof(struct fragment));
    //    for (int i=0; i < 10; i++) {
    //        l[i].offset = 1 * i;
    //        l[i].len = 10 - i;
    //        l[i].final = true;
    //    }
    //    l[9].final = true;

    //    s = gen_seed();

    //    struct hole_descriptor_list *hdl;
    //    hole_descriptor_list_init(&hdl);
    //    if (!hdl) return 1;
    //    shuffle(l, 10, &s);
    //    for(int i=0; i<10; i++) {
    //        printf("--- inserting (%d,%d)%s\n", l[i].offset, l[i].len, l[i].final ? "F" : "");
    //        hole_descriptor_list_add(hdl, l[i].offset, l[i].len, l[i].final, NULL);
    //        print_hole_descriptor_list(hdl);
    //    }
    //}

    //{
    //    struct fragment *l = calloc(10, sizeof(struct fragment));
    //    for (int i=0; i < 10; i++) {
    //        l[i].offset = 10 * i;
    //        l[i].len = 10;
    //    }
    //    l[9].final = true;

    //    s = gen_seed();

    //    struct hole_descriptor_list *hdl;
    //    hole_descriptor_list_init(&hdl);
    //    if (!hdl) return 1;
    //    shuffle(l, 10, &s);
    //    for(int i=0; i<10; i++) {
    //        printf("--- inserting (%d,%d)%s\n", l[i].offset, l[i].len, l[i].final ? "F" : "");
    //        hole_descriptor_list_add(hdl, l[i].offset, l[i].len, l[i].final, NULL);
    //        print_hole_descriptor_list(hdl);
    //    }
    //}

    {
        struct fragment *l = calloc(50, sizeof(struct fragment));
        for (int i=0; i < 10; i++) {
            l[i].offset = 10 * i;
            l[i].len = 10;
        }
        l[9].final = true;

        s = gen_seed();

        for (int i=10; i < 50; i++) {
            l[i] = random_fragment(100, &s);
        }

        struct hole_descriptor_list *hdl;
        hole_descriptor_list_init(&hdl);
        if (!hdl) return 1;
        shuffle(l, 50, &s);
        for(int i=0; i<50; i++) {
            printf("--- inserting (%d,%d)%s\n", l[i].offset, l[i].len, l[i].final ? "F" : "");
            hole_descriptor_list_add(hdl, l[i].offset, l[i].len, l[i].final, NULL);
            print_hole_descriptor_list(hdl);
        }
    }

    return 0;
}
