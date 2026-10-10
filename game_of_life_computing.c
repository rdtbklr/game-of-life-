#include"game_of_life_computing.h"
#include<stdlib.h>
#include <time.h>
#include <stdint.h>
#include <immintrin.h>
#include <stdio.h>
#include <string.h>
#define Block_side 64
#define access_look_up_table(index, shifter) ((uint64_t)((look_up_table[(index)] >> (shifter)) & 1))



uint8_t look_up_table[64] = {
    0x80, 0x68, 0xE8, 0x7E, 0x68, 0x16, 0x7E, 0x17,
    0x68, 0x16, 0x7E, 0x17, 0x16, 0x01, 0x17, 0x01,
    0x68, 0x16, 0x7E, 0x17, 0x16, 0x01, 0x17, 0x01,
    0x16, 0x01, 0x17, 0x01, 0x01, 0x00, 0x01, 0x00,
    0x68, 0x16, 0x7E, 0x17, 0x16, 0x01, 0x17, 0x01,
    0x16, 0x01, 0x17, 0x01, 0x01, 0x00, 0x01, 0x00,
    0x16, 0x01, 0x17, 0x01, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
/*
void create_lookup_table(){
    uint8_t values[64];
    for(int i = 0; i < 64; i++){
        values[i] = 0;
    }
    for(uint16_t i = 0,x; i < 512; i++){
        x = 0;
        for(uint8_t j = 0; j < 9; j++)
        {
            x += (i >> j) & 1;
        }
        if((x == 3) || ((x == 4) && ((i & 0b000010000) == 0b000010000))){
            values[i >> 3] |= 1 << (i & 0b111);
        }
    }
    printf("look_up_table[64] = {");
    for(int i = 0; i < 64; i++){
        printf("%c0x%.2X%c",((i & 7) == 0)? '\n' : ' ',values[i], (i != 63)? ',' : '}');
    }
    printf(";\n");
}*/

/*
    10000000 top right
    01000000 bottom right
    00100000 bottom left
    00010000 top left
    00001000 top
    00000100 right
    00000010 bottom
    00000001 left
    uint8_t that this function returns gets and by these and that neighbour will get created next cycle
*/
enum generation_enum{
    TOP_RIGHT =     0b10000000,
    BOTTOM_RIGHT =  0b01000000,
    BOTTOM_LEFT =   0b00100000,
    TOP_LEFT =      0b00010000,
    TOP =           0b00001000,
    RIGHT =         0b00000100,
    BOTTOM =        0b00000010,
    LEFT =          0b00000001
};
uint8_t compute_block_avx512(g_block *block){
    uint8_t new_block_mask = 0;
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
#define left_63_bit_mask (UINT64_C(0xFFFFFFFFFFFFFFFE))
#define right_63_bit_mask (UINT64_C(0x7FFFFFFFFFFFFFFF))
    uint8_t index = 0,shifter = 0;


    //top right corner
    index = (block->current_block_content[0] & 0b11) << 1;
    shifter = (block->current_block_content[1] & 0b11) << 1;
    if(block->neighbours_sides[0] != NULL){
        index |= (block->neighbours_sides[0]->current_block_content[Block_side - 1] & 0b11) << 4;
    }
    if(block->neighbours_corners[0] != NULL){
        index |= (block->neighbours_corners[0]->current_block_content[Block_side - 1] >> 63) << 3;
    }
    if(block->neighbours_sides[1] != NULL){
        index |= block->neighbours_sides[1]->current_block_content[0] >> 63;
        shifter |= block->neighbours_sides[1]->current_block_content[1] >> 63;
    }
    block->next_block_content[0] = (block->next_block_content[0] & left_63_bit_mask) | (access_look_up_table(index,shifter));
    //checking new block generation
    if(access_look_up_table((index >> 3) & 1, index & 0b111) == 1){
        //generate top
        new_block_mask |= TOP;
    }
    if((index & 0b010011) == 0b010011){
        //generate top right
        new_block_mask |= TOP_RIGHT;
    }
    if(access_look_up_table(index & 0b011000, (index & 0b10) | ((shifter >> 1) & 1)) == 1){
        //generate right
        new_block_mask |= RIGHT;
    }

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
    block->next_block_content[Block_side - 1] = (block->next_block_content[Block_side - 1] & left_63_bit_mask) | (access_look_up_table(index,shifter));
    //checking new block generation
    if(access_look_up_table((index >> 1) & 0b1001, shifter & 0b011) == 1){
        //generate right
        new_block_mask |= RIGHT;
    }
    if(((index & 0b11) == 0b11) && ((shifter & 0b10) == 0b10)){
        //generate bottom right
        new_block_mask |= BOTTOM_RIGHT;
    }
    if(access_look_up_table(shifter & 1, index & 0b111) == 1){
        //generate bottom
        new_block_mask |= BOTTOM;
    }

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
    block->next_block_content[Block_side - 1] = (block->next_block_content[Block_side - 1] & right_63_bit_mask) | (access_look_up_table(index,shifter) << 63);
    //checking new block generation
    if(access_look_up_table(shifter & 0b100, index & 0b111) == 1){
        //generate bottom
        new_block_mask |= BOTTOM;
    }
    if(((index & 0b110) == 0b110) && ((shifter & 0b010) == 0b010)){
        //generate bottom left
        new_block_mask |= BOTTOM_LEFT;
    }
    if(access_look_up_table((index >> 1) & 0b1001, shifter & 0b110) == 1){
        //generate bottom
        new_block_mask |= BOTTOM;
    }


    //top left corner
    index = block->current_block_content[0] >> 62;
    shifter = block->current_block_content[1] >> 62;
    if(block->neighbours_corners[3] != NULL){
        index |= (block->neighbours_corners[3]->current_block_content[Block_side - 1] & 1) << 5;
    }
    if(block->neighbours_sides[0] != NULL){
        index |= (block->neighbours_sides[0]->current_block_content[Block_side - 1] >> 62) << 3;
    }
    if(block->neighbours_sides[3] != NULL){
        index |= (block->neighbours_sides[3]->current_block_content[0] & 1) << 2;
        shifter |= (block->neighbours_sides[3]->current_block_content[1] & 1) << 2;
    }
    block->next_block_content[0] = (block->next_block_content[0] & right_63_bit_mask) | (access_look_up_table(index,shifter) << 63);
    //checking new block generation
    if(access_look_up_table((index >> 1) & 0b11001, shifter & 0b010) == 1){
        //generate left
        new_block_mask |= LEFT;
    }
    if((index & 0b010110) == 0b010110){
        //generate top left
        new_block_mask |= TOP_LEFT;
    }
    if(access_look_up_table(index >> 5, index & 0b111) == 1){
        //generate top
        new_block_mask |= TOP;
    }

    //right side without corners
    if(block->neighbours_sides[1] != NULL){
        index = ((block->current_block_content[0] & 0b11) << 1) | (block->neighbours_sides[1]->current_block_content[0] >> 63);
        for(uint16_t i = 1; i < Block_side - 1; i++){
            index = (index & 0b111) << 3;
            index |= ((block->current_block_content[i] & 0b11) << 1) | (block->neighbours_sides[1]->current_block_content[i] >> 63);
            shifter = ((block->current_block_content[i + 1] & 0b11) << 1) | (block->neighbours_sides[1]->current_block_content[i + 1] >> 63);
            block->next_block_content[i] = (block->next_block_content[i] & left_63_bit_mask) | (access_look_up_table(index,shifter));
        }
    }
    else{
        index = (block->current_block_content[0] & 0b11) << 1;
        for(uint16_t i = 1; i < Block_side - 1; i++){
            index = (index & 0b111) << 3;
            index |= (block->current_block_content[i] & 0b11) << 1;
            shifter = (block->current_block_content[i + 1] & 0b11) << 1;
            block->next_block_content[i] = (block->next_block_content[i] & left_63_bit_mask) | (access_look_up_table(index,shifter));

            if(((index & 0b010010) == 0b010010) && ((shifter & 0b010) == 0b010)){
                new_block_mask |= RIGHT;
                //create right neighbour event
            }
        }
    }

    //left side without corners
    if(block->neighbours_sides[3] != NULL){
        index = (block->current_block_content[0] >> 62) | ((block->neighbours_sides[3]->current_block_content[0] & 1) << 2);
        for(uint16_t i = 1; i < Block_side - 1; i++){
            index = (index & 0b111) << 3;
            index |= (block->current_block_content[i] >> 62) | ((block->neighbours_sides[3]->current_block_content[i] & 1) << 2);
            shifter = (block->current_block_content[i + 1] >> 62) | ((block->neighbours_sides[3]->current_block_content[i + 1] & 1) << 2);
            block->next_block_content[i] = (block->next_block_content[i] & right_63_bit_mask) | (access_look_up_table(index,shifter) << 63);
        }
    }
    else{
        index = block->current_block_content[0] >> 62;
        for(uint16_t i = 1; i < Block_side - 1; i++){
            index = (index & 0b111) << 3;
            index |= block->current_block_content[i] >> 62;
            shifter = block->current_block_content[i + 1] >> 62;
            block->next_block_content[i] = (block->next_block_content[i] & right_63_bit_mask) | (access_look_up_table(index,shifter) << 63);

            if(((index & 0b010010) == 0b010010) && ((shifter & 0b010) == 0b010)){
                new_block_mask |= LEFT;
                //create left neighbour event
            }
        }
    }
    //check for top and bottom for new block generation
    for(uint8_t i = 0; i < 61; i++){
        if(((block->current_block_content[0] >> i) & 0b111) == 0b111){
            new_block_mask |= TOP;
            //create top neighbour event
        }
        if(((block->current_block_content[Block_side - 1] >> i) & 0b111) == 0b111){
            new_block_mask |= BOTTOM;
            //create bottom neighbour event
        }
    }
    return new_block_mask;
}

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
void create_random_noise(g_block *block){
    srand(time(NULL));
    for(int i = 0; i < 64; i++){
        block->current_block_content[i] = (((uint64_t) rand()) << 32) + ((uint64_t) rand());
        block->next_block_content[i] = (((uint64_t) rand()) << 32) + ((uint64_t) rand());
    }
}
void free_block(g_block *block){
    for(uint8_t i = 0; i < 4; i++){
        if(block->neighbours_corners[i] != NULL){
            block->neighbours_corners[i]->neighbours_corners[(i + 2) & 0b11] = NULL;
        }
        if(block->neighbours_sides[i] != NULL){
            block->neighbours_sides[i]->neighbours_sides[(i + 2) & 0b11] = NULL;
        }
    }
    free(block->current_block_content);
    free(block->next_block_content);
    free(block);
}
void main_compute(Core core){

}
