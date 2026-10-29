#ifndef GAME_OF_LIFE_COMPUTING_H
#define GAME_OF_LIFE_COMPUTING_H
#include <stdint.h>
#include "memory_management.h"
#include "Structs.h"




uint8_t compute_block_avx512(g_block *block);
void create_random_noise(g_block *block);
g_block* init_block(int64_t x, int64_t y);
void free_block(g_block *block);
void main_compute(Core *core);
void empty_create_next(Core *core);
void check_for_deletion(Core *core);
void delete_from_Core(Core *core);
Core init_core(uint64_t size);
void add_block(Core *core, g_block *block_n);
void swap_pointers_core(Core *core);
#endif // GAME_OF_LIFE_COMPUTING_H
