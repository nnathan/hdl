#include "hdl.h"
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char **argv) {
    struct hole_descriptor_list *hdl;

    if (!hole_descriptor_list_init(&hdl)) {
        printf("Failed to create hole descriptor list\n");
        return 1;
    }

    hole_descriptor_list_add(hdl, 0, 100, false, NULL);
    print_hole_descriptor_list(hdl);
    printf("---\n");
    hole_descriptor_list_add(hdl, 20, 100, false, NULL);
    print_hole_descriptor_list(hdl);
    printf("---\n");
    hole_descriptor_list_add(hdl, 180, 20, true, NULL);
    print_hole_descriptor_list(hdl);
    printf("---\n");
    hole_descriptor_list_add(hdl, 150, 40, false, NULL);
    print_hole_descriptor_list(hdl);
    printf("---\n");
    hole_descriptor_list_add(hdl, 10, 170, false, NULL);
    print_hole_descriptor_list(hdl);
    printf("---\n");
    hole_descriptor_list_add(hdl, 50, 10, false, NULL);
    print_hole_descriptor_list(hdl);
    printf("---\n");
    hole_descriptor_list_add(hdl, 0, 205, false, NULL);
    print_hole_descriptor_list(hdl);
    printf("---\n");

    //hole_descriptor_list_add(hdl, 150, 10, false, NULL);
    //print_hole_descriptor_list(hdl);
    //printf("---\n");
    //hole_descriptor_list_add(hdl, 160, 25, false, NULL);
    //print_hole_descriptor_list(hdl);
    //printf("---\n");
    //hole_descriptor_list_add(hdl, 130, 5, false, NULL);
    //print_hole_descriptor_list(hdl);
    //printf("---\n");
    //hole_descriptor_list_add(hdl, 135, 15, false, NULL);
    //print_hole_descriptor_list(hdl);
    //printf("---\n");
    //hole_descriptor_list_add(hdl, 120, 10, false, NULL);
    //print_hole_descriptor_list(hdl);
}
