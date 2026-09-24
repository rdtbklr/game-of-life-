#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define Block_side 64


typedef struct Game_of_life_block{
    uint64_t block_content[Block_side/8];
    struct Game_of_life_block *neighbours_side[4]; /*
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
    uint64_t x,y;//starting one is 0,0
}g_block;

typedef struct Node{
    void *data;
    struct Node *prev;
    struct Node *next;
}node;

typedef struct double_linked_list{
    node *head;
    node *tail;
    uint64_t size;
}dbl;



int main()
{
    printf("Hello World!\n");
    return 0;
}
