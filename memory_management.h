#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H
#include <stdint.h>
#include "game_of_life_computing.h"
#include <stdlib.h>
#include "Structs.h"


inline b_list init_list(size_t size);
inline void expand_list(b_list *lt, size_t size);
inline void add_element(b_list *lt, struct Game_of_life_block *block,size_t size);
inline void delete_list(b_list *lt);
inline void delete_element(b_list *lt, struct Game_of_life_block *block);

#endif // MEMORY_MANAGEMENT_H
