#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H
#include <stdint.h>
#include "game_of_life_computing.h"
#include <stdlib.h>
#include <stdio.h>
#include "Structs.h"

static inline b_list init_list(size_t size){
    b_list lt;
    lt.pointers = (g_block **) malloc (size * sizeof(g_block));
    if(lt.pointers == NULL){
        perror("init_list failed\nmalloc failed");
        exit(1);
    }
    lt.size = size;
    lt.used = 0;
    lt.next = NULL;
    return lt;
}
static inline g_block* find_element(b_list *lt, int64_t x, int64_t y){
    for(;lt != NULL; lt = lt->next){
        for(uint64_t i = 0; i <= lt->used; i++){
            if(lt->pointers[i]->x == x && lt->pointers[i]->y == y){
                return lt->pointers[i];
            }
        }
    }
    return NULL;
}
static inline void set_list_size_min(b_list *lt, size_t size){
    size_t lt_size = 0;
    b_list *last;
    for(b_list *temp = lt; temp != NULL; temp = temp->next)
    {
        lt_size += temp->size;
        last = temp;
    }
    if(lt_size < size)
    {
        lt_size = size - lt_size;
        g_block **temp = (g_block **) realloc(last->pointers, sizeof(g_block) * (last->size + lt_size));
        if(temp != NULL)
        {
            last->pointers = temp;
            return;
        }
        last->next = (b_list*) malloc(sizeof(b_list));
        if(last->next == NULL)
        {
            perror("expand_list failed\nmalloc failed");
            exit(1);
        }
        *(last->next) = init_list(lt_size);

    }
}
static inline void expand_list(b_list *lt, size_t size){
    for(; lt->next != NULL; lt = lt->next){}
    g_block **ptr = (g_block **) realloc(lt->pointers, sizeof(g_block) * (lt->size + size));
    if(ptr != NULL){
        lt->pointers = ptr;
        lt->size += size;
        return;
    }
    b_list *new_list = malloc(sizeof(b_list));
    if(new_list == NULL){
        perror("expand_list failed\nmalloc failed");
        exit(1);
    }
    *new_list = init_list(size);
    lt->next = new_list;
}
static inline void add_element(b_list *lt, g_block *block, size_t size){
    while(1){
        if(lt->used < lt->size){
            lt->pointers[lt->used] = block;
            lt->used++;
            return;
        }
        if(lt->next == NULL)
        {
            break;
        }
    }
    expand_list(lt, size);
    lt = lt->next;
    lt->pointers[lt->used] = block;
    lt->used++;
}
static inline g_block* add_element_distinct(b_list *lt, g_block *block, size_t size){
    b_list *last;
    for(b_list *temp = lt; temp != NULL; temp = temp->next){
        for(size_t i = 0; i <= temp->used; i++){
            if(temp->pointers[i]->x == block->x && temp->pointers[i]->y == block->y){
                return block;
            }
        }
        last = temp;
    }
    if(last->used == last->size){
        expand_list(last,size);
    }
    last->pointers[last->used] = block;
    last->used++;
    return NULL;
}
static inline void delete_list_instance(b_list *lt){
    free(lt->pointers);
    free(lt);
}
static inline void free_list(b_list *lt){
    b_list *temp = lt;
    while(1){
        free(temp->pointers);
        if(temp->next != NULL){
            lt = temp->next;
            free(temp);
            temp = lt;
        }else{
            return;
        }
    }
}
static inline void delete_element(b_list *lt, g_block *block){
    b_list *temp = lt,*lt_block = NULL;
    size_t index;
    while(1)
    {
        if(lt_block == NULL){
            for(size_t i = 0;i < temp->used; i++){
                if(temp->pointers[i] == block){
                    lt_block = temp;
                    index = i;
                }
            }
        }
        if(temp->next == NULL)
        {
            break;
        }
        temp = temp->next;
    }
    lt_block->pointers[index] = temp->pointers[temp->used];
    if(temp->used == 0){
        delete_list_instance(temp);
        while(1){
            if(lt->next == temp){
                lt->next = NULL;
                break;
            }
            lt = lt->next;
        }
        return;
    }
    temp->used--;
}/*
inline void add_by_index(b_list *lt, size_t index, g_block *block){
    b_list *temp = lt;
    size_t size = 0;
    while(1){
        if(size + temp->size> index){
            temp->pointers[index - size] = block;
            return;
        }else{
            size += temp->size;
        }
        if(temp->next == NULL){
            break;
        }
    }
}
inline void get_by_index(b_list *lt, size_t index){
    while(lt != NULL){
        if
    }
}*/

#endif // MEMORY_MANAGEMENT_H
