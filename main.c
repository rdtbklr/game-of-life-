#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "raylib.h"
#include "game_of_life_computing.h"

#define Sizeof_var(variable) printf(# variable "\t%d\n" , sizeof(variable));




void init_block(g_block *block){
    block->current_block_content = (uint64_t*) aligned_alloc(64, sizeof(uint64_t) * Block_side);
    block->next_block_content = (uint64_t*) aligned_alloc(64, sizeof(uint64_t) * Block_side);
    if(block->current_block_content == NULL || block->next_block_content == NULL){
        printf("could not allocate memory\n");
        exit(1);
    }
    memset(block->neighbours_sides, 0, sizeof(struct Game_of_life_block*) * 4);
    memset(block->neighbours_corners, 0, sizeof(struct Game_of_life_block*) * 4);
    block->x = 0;
    block->y = 0;
}

void print_block(g_block *block){
    //char buffer[65];
    //buffer[64] = '\0';
    //printf("----------------------------------------------------------------\n\n\n\n\n\n\n\n");
    BeginDrawing();
    for(uint32_t i = 0; i < 64; i++){
        uint64_t x = block->next_block_content[i];
        for(uint32_t j = 0; j < sizeof(uint64_t) * 8; j++)
        {
            //buffer[63 - j] = ((x >> j) &  1)? '1' : ' ';
            if((x >> j) &  1){
                DrawRectangle((63 - j) * 16,i * 16,16,16, YELLOW);
            }else{

                DrawRectangle((63 - j) * 16,i * 16,16,16, BLACK);
            }

        }
        //printf("%s\n",buffer);
    }
    DrawFPS(0,0);
    EndDrawing();
}
void swap_pointers(g_block *block){
    uint64_t *temp = block->current_block_content;
    block->current_block_content = block->next_block_content;
    block->next_block_content = temp;
}

int main()
{
    g_block *block = (g_block*) calloc(1, sizeof(g_block));
    init_block(block);
    create_random_noise(block);
    //swap_pointers((void**)&block->next_block_content, (void**)&block->current_block_content);
    InitWindow(256 * 4,256 * 4, "Game of life");
    print_block(block);

    for(int i = 0; i <90000; i++){
        compute_block(block);
        print_block(block);
        printf("\n\t%d\n",i);
        swap_pointers(block);

        usleep(1000* 50);
    }
    free(block);
    return 0;
}
