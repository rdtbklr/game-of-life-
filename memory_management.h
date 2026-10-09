#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H
#include <stdint.h>
#include "game_of_life_computing.h"
#include <stdlib.h>
typedef enum Variable_types  {
    GAME_OF_LIFE_BLOCK = sizeof(g_block)
} var_type;
typedef struct Block_List{
    g_block **pointers;
    struct Block_List *next;
    size_t size;
    size_t used;
    var_type type;
}b_list;

inline b_list init_list(size_t size);
inline void expand_list(b_list *lt, size_t size);
inline void add_element(b_list *lt, g_block *block,size_t size);
inline void delete_list(b_list *lt);
inline void delete_element(b_list *lt, g_block *block);

#endif // MEMORY_MANAGEMENT_H
