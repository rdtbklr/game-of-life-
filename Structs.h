#ifndef STRUCTS_H
#define STRUCTS_H
#include<stdint.h>
#include <stdlib.h>
#include "raylib.h"
#include <time.h>
enum NEIGHBOUR_SIDES{
    TOP = 0,
    RIGHT = 1,
    BOTTOM = 2,
    LEFT = 3
};
enum NEIGHBOUR_CORNERS{
    TOP_RIGHT= 0,
    BOTTOM_RIGHT = 1,
    BOTTOM_LEFT = 2,
    TOP_LEFT = 3
};

enum G_BLOCK_FLAGS{
    BLOCK_EMPTY =   0b00000001,
    BLOCK_CHANGED = 0b00000010
};

typedef struct Texture_Image_duo{
    Image img;
    Texture txt;
    uint64_t *last_render;
} ti_duo;

typedef struct Game_of_life_block{
    struct Texture_Image_duo *render_block;
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
    int64_t x,y;
    uint8_t flags;/*
    mask 00000001 if the block is empty
*/
}g_block;

typedef struct Block_List{
    struct Game_of_life_block **pointers;
    struct Block_List *next;
    size_t size;
    size_t used;
}b_list;

typedef struct Memory_Buffer_Stack{
    void **pointers;
    uint64_t size;
    uint64_t used;
}memb;

typedef struct Core_list{
    struct Block_List main;
    struct Block_List delete_next;
    struct Block_List create_next;
    struct Memory_Buffer_Stack block_buffer;
    size_t size;
}Core;


typedef struct Blocks_in_frame{
    b_list list;
    struct Memory_Buffer_Stack texture_buffer;
    Camera2D camera;
    Color alive_cell_color, Dead_cell_color;
    uint64_t cell_size;
    uint64_t size;
    uint64_t window_size_x,window_size_y;
}Frame;

typedef struct UPS_Timer{
    uint64_t *queue;
    double update_interval;
    struct timespec update_interval_t,next_update,delay;
    uint64_t head,size;
}U_t;


#endif // STRUCTS_H
