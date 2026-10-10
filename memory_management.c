#include "memory_management.h"
#include <stdlib.h>
#include <stdio.h>


inline b_list init_list(size_t size){
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
inline void expand_list(b_list *lt, size_t size){
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
inline void add_element(b_list *lt, g_block *block, size_t size){
    for(; lt->next != NULL; lt = lt->next){}
    if(lt->size == lt->used){
        expand_list(lt, size);
        if(lt->next != NULL){
            lt = lt->next;
        }
    }
    lt->pointers[lt->used] = block;
    lt->used++;
}
inline void delete_list(b_list *lt){
    free(lt->pointers);
    free(lt);
}
inline void delete_element(b_list *lt, g_block *block){
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
        delete_list(temp);
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
}
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

        }
    }
}