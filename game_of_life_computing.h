#ifndef GAME_OF_LIFE_COMPUTING_H
#define GAME_OF_LIFE_COMPUTING_H
#include <stdint.h>
#include "memory_management.h"
#include "Structs.h"




uint8_t compute_block_avx512(g_block *block);
void create_random_noise(g_block *block);
g_block* init_block(int64_t x, int64_t y);
void free_block(g_block *block);
void main_compute(Core *core, size_t size);

#endif // GAME_OF_LIFE_COMPUTING_H
