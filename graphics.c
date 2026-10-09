#include "graphics.h"
#include "game_of_life_computing.h"
#include "raylib.h"


void print_block(g_block *block,uint64_t start_x,uint64_t start_y,uint8_t cell_size){
    BeginDrawing();
    for(uint32_t i = 0; i < 64; i++){
        uint64_t x = block->next_block_content[i];
        for(uint32_t j = 0; j < sizeof(uint64_t) * 8; j++)
        {
            //buffer[63 - j] = ((x >> j) &  1)? '1' : ' ';
            if((x >> j) &  1){
                DrawRectangle((63 - j) * cell_size,i * cell_size,cell_size,cell_size, YELLOW);
            }else{

                DrawRectangle((63 - j) * cell_size,i * cell_size,cell_size,cell_size, BLACK);
            }

        }
    }
    DrawFPS(0,0);
    EndDrawing();
}