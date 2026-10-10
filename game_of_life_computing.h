#ifndef GAME_OF_LIFE_COMPUTING_H
#define GAME_OF_LIFE_COMPUTING_H
#include <stdint.h>
#include "memory_management.h"
#include "Structs.h"




uint8_t compute_block_avx512(g_block *block);
void create_random_noise(g_block *block);
void init_block(g_block *block);
void free_block(g_block *block);
void main_compute(Core core);

#endif // GAME_OF_LIFE_COMPUTING_H
