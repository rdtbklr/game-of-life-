#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "game_of_life_computing.h"
#include "memory_management.h"
#include "Structs.h"


void fetch_blocks(Frame *frame,Core *core, size_t size);
void print_frame(Frame *frame);
void print_block_camera(g_block *block,Color alive_cell,Color dead_cell,size_t cell_size);
void print_block(g_block *block,uint64_t start_x,uint64_t start_y,uint8_t cell_size);
Frame init_frame(Color alive_cell_color, Color Dead_cell_color, int cell_size, size_t size, size_t window_size_x, size_t window_size_y);
Camera2D init_camera(float x, float y);
#endif // GRAPHICS_H
