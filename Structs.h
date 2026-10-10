#ifndef STRUCTS_H
#define STRUCTS_H
#include<stdint.h>
#include <stdlib.h>

typedef struct Game_of_life_block{
    uint64_t *current_block_content;
    uint64_t *next_block_content;

    struct Game_of_life_block *neighbours_sides[4]; /*
    0 top
    1 right
    2 bottom
    3 left
*/
    struct Game_of_life_block *neighbours_corners[4];/*
    0 top right
    1 bottom right
    2 bottom left
    3 top left
*/
    uint64_t x,y;
}g_block;

typedef struct Block_List{
    struct Game_of_life_block **pointers;
    struct Block_List *next;
    size_t size;
    size_t used;
}b_list;

typedef struct Core_list{
    struct Block_List main;
    struct Block_List delete_next;
    struct Block_List create_next;
    uint64_t size;
}Core;


struct Blocks_in_frame{
    b_list list;
    double x,y;
};

#endif // STRUCTS_H
