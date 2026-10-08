#include"game_of_life_computing.h"



uint8_t look_up_table[]={0x02, 0x2c, 0x2e, 0xfc, 0x2c, 0xd0, 0xfd, 0xd0, 0x2c, 0xd0, 0xfd, 0xd0, 0xd1, 0x01, 0xd1,
                           0x00, 0x2c, 0xd0, 0xfd, 0xd0, 0xd1, 0x01, 0xd1, 0x00, 0xd1, 0x01, 0xd1, 0x01, 0x00, 0x01,
                           0x00, 0x00, 0x02, 0x2c, 0x2e, 0xfc, 0x2c, 0xd0, 0xfd, 0xd0, 0x2c, 0xd0, 0xfd, 0xd0, 0xd1,
                           0x01, 0xd1, 0x00, 0x2c, 0xd0, 0xfd, 0xd0, 0xd1, 0x01, 0xd1, 0x00, 0xd1, 0x01, 0xd1, 0x01, 0x00, 0x01, 0x00};
void compute_block(g_block *block){
    //compute middle with top and bottom
    for(int8_t i = 0; i < 64; i += 8)
    {
        //load data
        __m512i top,bottom,midle;
        if(i == 0){
            top = _mm512_maskz_loadu_epi64(0xFE,(const void*) (block->current_block_content-1));
            if(block->neighbours_sides[0] != NULL){
                top = _mm512_mask_loadu_epi64(top,0x01,(const void*)(block->neighbours_sides[0]->current_block_content + Block_side - 1));
            }
        }else{
            top = _mm512_loadu_epi64((const void*)(block->current_block_content+i - 1));
        }
        midle = _mm512_load_epi64((void*)(block->current_block_content + i));
        if(i == 56){
            bottom = _mm512_maskz_loadu_epi64(0x7F,(const void*) (block->current_block_content + 57));
            if(block->neighbours_sides[2] != NULL){
                bottom = _mm512_mask_loadu_epi64(bottom,0x80,(const void*)(block->neighbours_sides[2]->current_block_content));
            }
        }else{
            bottom = _mm512_loadu_epi64((const void*)(block->current_block_content + i + 1));
        }
        //bitwise logic
        __m512i ternary_xor = _mm512_ternarylogic_epi64(top,bottom,midle, 0x96);
        __m512i ternary_majority = _mm512_ternarylogic_epi64(top,bottom,midle, 0xE8);

        __m512i left_column_L0 = _mm512_srli_epi64(ternary_xor, 1);
        __m512i left_column_L1 = _mm512_slli_epi64(ternary_majority, 1);
        __m512i right_column_L0 = _mm512_slli_epi64(ternary_xor, 1);
        __m512i right_column_L1 = _mm512_srli_epi64(ternary_majority, 1);
        __m512i middle_L0 = _mm512_xor_epi64(top, bottom);
        __m512i middle_L1 = _mm512_and_epi64(top, bottom);

        __m512i B0 = _mm512_ternarylogic_epi64(left_column_L0, right_column_L0, middle_L0, 0x96);
        __m512i Ba1 = _mm512_ternarylogic_epi64(left_column_L0, right_column_L0, middle_L0, 0xE8);
        __m512i Bb1 = _mm512_ternarylogic_epi64(left_column_L1, right_column_L1, middle_L1, 0x96);
        __m512i B2 = _mm512_ternarylogic_epi64(left_column_L1, right_column_L1, middle_L1, 0xE8);

        __m512i temp = _mm512_ternarylogic_epi64(B0, Ba1, Bb1, 0x60);// A and (B xor C) 0x60
        __m512i alive3 = _mm512_ternarylogic_epi64(temp, temp , B2, 0x50);// A and C! 0x50

        temp = _mm512_ternarylogic_epi64(B0,B2, temp ,0x03); // A nor B 0x03
        __m512i alive2 = _mm512_ternarylogic_epi64(temp, Ba1, Bb1, 0x60);// A and (B xor C) 0x60
        __m512i result = _mm512_ternarylogic_epi64(alive3, alive2, midle, 0xF8);// A or (B and C) 0xF8
        _mm512_store_epi64((void*)(block->next_block_content + i) , result);
    }
    //corners
    /*
    struct Game_of_life_block *neighbours_sides[4];
    0 top
    1 right
    2 bottom
    3 left

    struct Game_of_life_block *neighbours_corners[4];/*
    0 top right
    1 bottom right
    2 bottom left
    3 top left
*/
#define rightmost_2_bit 0b11
    uint8_t index = 0,shifter = 0;
    //top right corner
    if(block->neighbours_sides[0] != NULL){
        index = (block->neighbours_sides[0]->current_block_content[Block_side - 1] & rightmost_2_bit) << 1;
    }
    if(block->neighbours_corners[0] != NULL){
        index |= block->neighbours_corners[0]->current_block_content[Block_side - 1] >> 63;
    }
    index <<= 3;
    index |= (block->current_block_content[0] & rightmost_2_bit) << 1;
    shifter |= (block->current_block_content[1] & rightmost_2_bit) << 1;
    if(block->neighbours_sides[1] != NULL){
        index |= block->neighbours_corners[1]->current_block_content[0] >> 63;
        shifter |= block->neighbours_corners[1]->current_block_content[1] >> 63;
    }
    block->next_block_content[0] = (block->next_block_content[0] & 0xFFFFFFFE) | (access_look_up_table(index,shifter));

    //bottom right corner
    index = ((block->current_block_content[Block_side - 2] & rightmost_2_bit) << 4)
            | ((block->current_block_content[Block_side - 1] & rightmost_2_bit) << 1);
    if(block->neighbours_sides[1] != NULL){
        index |= ((block->neighbours_sides[1]->current_block_content[Block_side-2] >> 63) << 3)
        | (block->neighbours_sides[1]->current_block_content[Block_side-1] >> 63) ;
    }
    shifter = 0;
    if(block->neighbours_sides[2] != NULL){
        shifter = (block->neighbours_sides[2]->current_block_content[0] & rightmost_2_bit) << 1;
    }
    if(block->neighbours_corners[1] != NULL){
        shifter |= block->neighbours_corners[1]->current_block_content[0] >> 63;
    }
    block->next_block_content[Block_side - 1] = (block->next_block_content[Block_side - 1] & 0xFFFFFFFE) | (access_look_up_table(index,shifter));

    //bottom left corner
    index = ((block->current_block_content[Block_side - 2] >> 62) << 3) | (block->current_block_content[Block_side - 1] >> 62);
    if(block->neighbours_sides[3] != NULL){
        index |= ((block->neighbours_sides[3]->current_block_content[Block_side - 2] & 0b1) << 5)
        | ((block->neighbours_sides[3]->current_block_content[Block_side - 1] & 0b1) << 2);
    }
    shifter = 0;
    if(block->neighbours_corners[2] != NULL){
        shifter = (block->neighbours_corners[2]->current_block_content[0] & 1) << 2;
    }
    if(block->neighbours_sides[2] != NULL){
        shifter |= block->neighbours_sides[2]->current_block_content[0] >> 62;
    }
    block->next_block_content[Block_side - 1] = (block->next_block_content[Block_side - 1] & 0x7FFFFFFF) | (access_look_up_table(index,shifter) << 63);

    //top left corner

}

void create_random_noise(g_block *block){
    srand(time(NULL));
    for(int i = 0; i < 64; i++){
        block->current_block_content[i] = (((uint64_t) rand()) << 32) + ((uint64_t) rand());
        block->next_block_content[i] = (((uint64_t) rand()) << 32) + ((uint64_t) rand());
    }
}
