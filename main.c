#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define Block_side 64
#define Sizeof_var(variable) printf(# variable "\t%d\n" , sizeof(variable));


typedef struct Node{
    void *data;
    struct Node *prev;
    struct Node *next;
    uint64_t xplusy;
}node;

typedef struct double_linked_list{
    node *head;
    node *tail;
    uint64_t size;
    uint64_t min_xplusy;
    uint64_t max_xplusy;
}dll;

typedef struct Game_of_life_block{
    uint64_t current_block_content[Block_side];
    uint64_t next_block_content[Block_side];

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
    uint64_t x,y;//starting one is 0,0
    node *dll_node;
}g_block;

void add_node(dll *list, void* data)
{
    node *nd = (node*) malloc(sizeof(node));
    nd->data = data;
    nd->xplusy = ((g_block*)data)->x + ((g_block*)data)->y;
    ((g_block*)data)->dll_node = nd;
    if(list->size == 0)
    {
        list->head = nd;
        list->tail = nd;
        list->size = 1;
        list->min_xplusy = nd->xplusy;
        list->max_xplusy = nd->xplusy;
        nd->prev = nd;
        nd->next = nd;
        return;
    }
    if(nd->xplusy <= list->min_xplusy)
    {
        nd->next = list->head;
        nd->prev = list->tail;
        list->head->prev = nd;
        list->tail->next = nd;
        list->head = nd;
        list->size++;
        list->min_xplusy = nd->xplusy;
        return;
    }
    if(nd->xplusy > list->max_xplusy)
    {
        nd->next = list->head;
        nd->prev = list->tail;
        list->head->prev = nd;
        list->tail->next = nd;
        list->tail = nd;
        list->size++;
        list->max_xplusy = nd->xplusy;
        return;
    }if(list->size == 1){
        nd->next = list->head;
        nd->prev = list->tail;
        list->head->prev = nd;
        list->tail->next = nd;
        list->head = nd;
        list->size++;
    }
    node *temp = list->head;
    if(nd->xplusy == temp->xplusy){
        nd->next =temp;
        nd->prev = temp->prev;
        temp->prev = nd;
        nd->prev->next = nd;
        list->size++;
        list->head = nd;

    }
    for( ; temp->next != list->head; temp= temp->next)
    {
        if(nd->xplusy == temp->xplusy){
            nd->next =temp;
            nd->prev = temp->prev;
            temp->prev = nd;
            nd->prev->next = nd;
            list->size++;
            return;
        }
    }
    printf("An error occured while adding node to linked list on add_node function");
    exit(1);
}
void init_block(g_block *block){
    memset(block->current_block_content, 0, sizeof(uint64_t) * Block_side);
    memset(block->next_block_content, 0, sizeof(uint64_t) * Block_side);

    memset(block->neighbours_sides, 0, sizeof(struct Game_of_life_block*) * 4);
    memset(block->neighbours_corners, 0, sizeof(struct Game_of_life_block*) * 4);
    block->x = 0;
    block->y = 0;
}
void printb(uint16_t c, uint16_t result){
    char buffer[16];
    buffer[3] = '\n';
    buffer[9] = '\n';
    buffer[13] = '\n';
    buffer[7] = '\t';
    buffer[14] = '\n';
    buffer[15] = '\0';
    buffer[8] = (result == 1)? '1' : '0';
    buffer[0] = (((c >> 8) & 1) == 1)? '1' : '0';
    buffer[1] = (((c >> 7) & 1) == 1)? '1' : '0';
    buffer[2] = (((c >> 6) & 1) == 1)? '1' : '0';

    buffer[4] = (((c >> 5) & 1) == 1)? '1' : '0';
    buffer[5] = (((c >> 4) & 1) == 1)? '1' : '0';
    buffer[6] = (((c >> 3) & 1) == 1)? '1' : '0';

    buffer[10] = (((c >> 2) & 1) == 1)? '1' : '0';
    buffer[11] = (((c >> 1) & 1) == 1)? '1' : '0';
    buffer[12] = (((c >> 0) & 1) == 1)? '1' : '0';
    printf(buffer);
}
int main()
{
    Sizeof_var(g_block)
    Sizeof_var(node)
    Sizeof_var(dll)

    for(uint16_t i = 0; i < 512; i++){
        int8_t count = 0,result;
        for(int8_t j = 0;j < 3; j++){
            count += ((i >> (6 + j)) & 1) + ((j != 1)? (((i >> (3 + j))) & 1) : 0) + ((i >> j) & 1);
        }
        //printf("%d\t%d\t", i, count);
        if((i & 0b10000)){
            if(count < 2 || count > 3){
                result = 0;
                //printf("0\n");
            }else{
                result = 1;
                //printf("1\n");
            }
        }else if(count == 3){
            result = 1;
            //printf("1\n");
        }else{
            result = 0;
        }
        printb(i,result);
    }


    return 0;
}
