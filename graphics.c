#include "graphics.h"
#include "game_of_life_computing.h"
#include "raylib.h"
#include <string.h>
#include <immintrin.h>

static inline int64_t mceiling(double x){
    return (x >((int64_t) x)) + (int64_t) x;
}
static inline int64_t mfloor(double x){
    return (int64_t) x - ((int64_t) x > x);
}
static inline uint8_t in_middle(int64_t l,int64_t m,int64_t r){
    if(l > r){
        l ^= r;
        r ^= l;
        l ^= r;
    }
    return (l <= m) && (m <= r);
}
Camera2D init_camera(float x, float y){
    Camera2D camera;
    camera.offset.x = x/2;
    camera.offset.y = y/2;
    camera.target.x = 0;//-32 * cell_size;
    camera.target.y = 0;//-32 * cell_size;
    camera.zoom = 1;
    camera.rotation = 0;
    return camera;
}
Frame init_frame(Color alive_cell_color, Color Dead_cell_color, int cell_size, size_t size, size_t window_size_x, size_t window_size_y){
    Frame frame;
    frame.texture_buffer = init_Memory_Buffer_Stack(size);
    frame.alive_cell_color = alive_cell_color;
    frame.Dead_cell_color = Dead_cell_color;
    frame.camera = init_camera(window_size_x,window_size_y);
    frame.cell_size = cell_size;
    frame.size = size;
    frame.list = init_list(frame.size);
    frame.window_size_x = window_size_x;
    frame.window_size_y = window_size_y;
    return frame;
}

ti_duo* create_Texture_Image_duo(){
    ti_duo *duo = malloc(sizeof(ti_duo));
    duo->img.width = 64;
    duo->img.height = 64;
    duo->img.format = PIXELFORMAT_UNCOMPRESSED_R5G5B5A1;
    duo->img.mipmaps = 1;
    duo->img.data = aligned_alloc(64, sizeof(int16_t) * 64 * 64);
    if(duo->img.data == NULL){
        printf("could not allocate image data\n");
        exit(1);
    }
    memset(duo->img.data, 0, sizeof(int16_t) * 64 * 64);
    //printf("This image is %s\n",(IsImageValid(duo->img)) ? "valid" : "not valid");
    duo->txt = LoadTextureFromImage(duo->img);
    return duo;
}
ti_duo* get_Texture_Image_duo(memb *buffer){
    ti_duo *duo = pop_from_memb(buffer);
    if(duo == NULL){
        return create_Texture_Image_duo();
    }
    return duo;
}
void delete_Texture_Image_duo(ti_duo *duo){
    free(duo->img.data);
    UnloadTexture(duo->txt);
    free(duo);
}

void fetch_blocks(Frame *frame, Core *core, size_t size){
    double width = frame->window_size_x / frame->camera.zoom;
    double height = frame->window_size_y / frame->camera.zoom;
    int64_t x1 = mceiling((frame->camera.target.x - width / 2) / (64 * frame->cell_size));
    int64_t y1 = mfloor((frame->camera.target.y - height / 2) / (64 * frame->cell_size));
    int64_t x2 = mceiling((frame->camera.target.x + width / 2) / (64 * frame->cell_size));
    int64_t y2 = mfloor((frame->camera.target.y + height / 2) / (64 * frame->cell_size));
    //printf("(%d,%d)\t(%d,%d)\n",x1,y1,x2,y2);
    //set minimum size for list
    set_list_size_min(&(frame->list), (x2 - x1 + 1) * (y2 - y1 + 1));


    //delete blocks that are out of range from the list
    for(b_list *temp = &frame->list; temp != NULL; temp = temp->next){
        for(uint64_t i = 0; i < temp->used; i++){
            if(in_middle(x1, temp->pointers[i]->x, x2) == 0 || in_middle(y1, -temp->pointers[i]->y, y2) == 0){
                if(add_to_memb(&frame->texture_buffer, temp->pointers[i]->render_block) != NULL){
                    delete_Texture_Image_duo(temp->pointers[i]->render_block);
                }
                temp->pointers[i]->render_block = NULL;
                delete_element(temp, temp->pointers[i--]);
                continue;
            }
        }
    }

    //delete the blocks from the list if they are in the deletion list for core
    if(core->delete_next.used != 0)
    {
        for(b_list *temp = &core->delete_next; temp != NULL; temp = temp->next)
        {
            for(uint64_t i = 0; i < temp->used; i++)
            {
                if(add_to_memb(&frame->texture_buffer, temp->pointers[i]->render_block) != NULL){
                    delete_Texture_Image_duo(temp->pointers[i]->render_block);
                }
                temp->pointers[i]->render_block = NULL;
                delete_element(&frame->list, temp->pointers[i]);
            }
        }
    }

    //add blocks in the range
    for(b_list *temp = &(core->main); temp != NULL; temp = temp->next){
        for(size_t i = 0; i < temp->used; i++){
            if(in_middle(x1, temp->pointers[i]->x, x2) == 1 && in_middle(y1, -temp->pointers[i]->y, y2) == 1){
                add_element_distinct(&(frame->list), temp->pointers[i], size);
                if(temp->pointers[i]->render_block == NULL){
                    temp->pointers[i]->render_block = get_Texture_Image_duo(&frame->texture_buffer);
                    update_block_texture(temp->pointers[i],frame->alive_cell_color,frame->Dead_cell_color);
                    SetTextureFilter(temp->pointers[i]->render_block->txt, TEXTURE_FILTER_POINT);
                }
            }
        }
    }

}
#define convert32_to_16(color) ((((color.r >> 3) & 0b11111) << 11) | (((color.g >> 3) & 0b11111) << 6) | (((color.b >> 3) & 0b11111) << 1) | ( color.a > 0xF))
void update_block_texture(g_block *block,Color alive_cell,Color dead_cell){
    uint16_t calive_cell,cdead_cell;
    calive_cell = convert32_to_16(alive_cell);
    cdead_cell = convert32_to_16(dead_cell);
    __m512i alive = _mm512_set1_epi16( calive_cell);
    __m512i dead = _mm512_set1_epi16( cdead_cell);
    __m512i result1,result2;
    __m512i *data = block->render_block->img.data;
    __m512i permutexm = _mm512_set_epi16(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31);
    for(uint8_t i = 0; i < 64; i++){
        uint32_t first_half = block->current_block_content[i] & 0xFFFFFFFF;
        uint32_t second_half = block->current_block_content[i] >> 32;

        result1 = _mm512_mask_blend_epi16(first_half, dead, alive);
        result2 = _mm512_mask_blend_epi16(second_half, dead, alive);

        result1 = _mm512_permutexvar_epi16(permutexm, result1);
        result2 = _mm512_permutexvar_epi16(permutexm, result2);

        _mm512_storeu_si512(data + 2 * i + 1, result1);
        _mm512_storeu_si512(data + 2 * i, result2);
    }
    UpdateTexture(block->render_block->txt, block->render_block->img.data);
}
void print_frame(Frame *frame){
    //BeginDrawing();
    ClearBackground(frame->Dead_cell_color);
    BeginMode2D(frame->camera);
    for(b_list *temp = &frame->list; temp != NULL; temp = temp->next){
        for(size_t j = 0; j < temp->used; j++){
            Vector2 v;
            v.x = temp->pointers[j]->x * 64*16;
            v.y = temp->pointers[j]->y * (-64)*16;
            if((temp->pointers[j]->flags & BLOCK_CHANGED) != 0){
                temp->pointers[j]->flags = temp->pointers[j]->flags & (~BLOCK_CHANGED);
                update_block_texture(temp->pointers[j], frame->alive_cell_color, frame->Dead_cell_color);
            }
            DrawTextureEx(temp->pointers[j]->render_block->txt,v, 0.0, 16, WHITE);
            //printf("printing (%d,%d)\n",temp->pointers[j]->x, temp->pointers[j]->y);

            //print_block_camera(temp->pointers[j] , frame->alive_cell_color, frame->cell_size);


        }
    }
    EndMode2D();
    DrawFPS(0,0);
    //EndDrawing();
}
void print_block_camera(g_block *block,Color alive_cell,size_t cell_size){
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 64; j++){
            if(((block->current_block_content[i] >> j) & 1) == 1){
                DrawRectangle(((block->x * 64) - j) * cell_size,((block->y * -64) + i) * cell_size, cell_size, cell_size , alive_cell);
            }
        }
    }
/*
    DrawRectangleLines((block->x - 1) * 64*cell_size + cell_size,(block->y) * -64*cell_size, cell_size * 64, cell_size * 64, GREEN );
    uint64_t middlex = block->x  * 64*cell_size,middley = (block->y) * -64*cell_size;
    middlex -= 63 * cell_size;
    middley += 64 * cell_size / 2;

    DrawText(TextFormat("block coordinates (%d,%d)", block->x,block->y), middlex, middley, 40, WHITE);
    middley += 40;

    char *asd[] = {"top\0","right\0","bottom\0","left\0"};
    for(int i = 0; i < 4; i++){
        if(block->neighbours_sides[i] != NULL){
            DrawText(TextFormat("%s neighbor side coordinates (%d,%d)",asd[i], block->neighbours_sides[i]->x,block->neighbours_sides[i]->y), middlex, middley, 40, WHITE);
        }
        middley += 40;
        if(block->neighbours_corners[i] != NULL){

            DrawText(TextFormat("%s %s neighbor corner coordinates (%d,%d)",asd[((i ^ (i >> 1)) == 1)? 2: 0],asd[(i < 2)? 1 : 3], block->neighbours_corners[i]->x,block->neighbours_corners[i]->y), middlex, middley, 40, WHITE);
        }
        middley += 40;
    }
    if((block->flags & 1) == 1){
        DrawText(TextFormat("empty"), middlex, middley, 40, RED);
    }*/


}

void print_block(g_block *block,uint64_t start_x,uint64_t start_y,uint8_t cell_size){
    for(uint32_t i = 0; i < 64; i++){
        uint64_t x = block->current_block_content[i];
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
}