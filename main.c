#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "raylib.h"
#include "game_of_life_computing.h"
#include "graphics.h"

#define Sizeof_var(variable) printf(# variable "\t%d\n" , sizeof(variable));





#define cell_size 16

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
    InitWindow(64 * cell_size,64 * cell_size, "Game of life");
    print_block(block,0 ,0 , cell_size);

    for(int i = 0; WindowShouldClose() == 0; i++){
        compute_block(block);
        print_block(block,0 ,0 , cell_size);
        printf("\n\t%d\n",i);
        swap_pointers(block);

        usleep(1000 * 5);
    }
    free(block);
    return 0;
}
