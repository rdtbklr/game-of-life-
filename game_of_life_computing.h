#ifndef GAME_OF_LIFE_COMPUTING_H
#define GAME_OF_LIFE_COMPUTING_H
#include <stdint.h>


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

void compute_block(g_block *block);
void create_random_noise(g_block *block);
void init_block(g_block *block);

#endif // GAME_OF_LIFE_COMPUTING_H
