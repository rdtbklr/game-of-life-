#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "game_of_life_computing.h"

struct Blocks_in_frame{
    g_block *blocks;
};

void print_block(g_block *block,uint64_t start_x,uint64_t start_y,uint8_t cell_size);
#endif // GRAPHICS_H
