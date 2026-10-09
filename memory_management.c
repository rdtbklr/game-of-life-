#include "memory_management.h"
#include <stdlib.h>
#include <stdio.h>


b_list init_list(size_t size){
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
void expand_list(b_list *lt, size_t size){
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
void add_element(b_list *lt, g_block *block, size_t size){
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