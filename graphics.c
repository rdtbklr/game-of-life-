#include "graphics.h"
#include "game_of_life_computing.h"
#include "raylib.h"

static inline int64_t mceiling(double x){
    return (x >((int64_t) x)) + (int64_t) x;
}
static inline int64_t mfloor(double x){
    return (int64_t) x - ((int64_t) x > x);
}
static inline uint8_t in_middle(int64_t l,int64_t m,int64_t r){
    if(l > r){
        l &= r;
        r &= l;
        l &= r;
    }
    return (l <= m) && (m <= r);
}
Camera2D init_camera(int monitor,size_t cell_size){
    Camera2D camera;
    camera.offset = (Vector2){GetMonitorWidth(monitor), GetMonitorHeight(monitor)};
    camera.target = (Vector2){-32 * cell_size, -32 * cell_size};
    camera.zoom = 1;
    camera.rotation = 0;
    return camera;
}
void fetch_blocks(Frame *frame, Core *core, size_t size){
    double width = frame->window_size_x / frame->camera.zoom;
    double height = frame->window_size_y / frame->camera.zoom;
    int64_t x1 = mceiling((frame->camera.target.x - width / 2) / 64);
    int64_t y1 = mfloor((frame->camera.target.y - height / 2) / 64);
    int64_t x2 = mceiling((frame->camera.target.x + width / 2) / 64);
    int64_t y2 = mfloor((frame->camera.target.y + height / 2) / 64);
    //set minimum size for list
    set_list_size_min(&(frame->list), (x2 - x1 + 1) * (y2 - y1 + 1));
    //delete the blocks from the list if they are in the deletion list for core
    if(core->delete_next.used != 0)
    {
        for(b_list *temp = &core->delete_next; temp != NULL; temp = temp->next)
        {
            for(uint64_t i = 0; i < temp->used; i++)
            {
                delete_element(&frame->list, temp->pointers[i]);
            }
        }
    }
    //delete blocks that are out of range from the list
    for(b_list *temp = &frame->list; temp != NULL; temp = temp->next){
        for(uint64_t i = 0; i , temp->used; i++){
            if(in_middle(x1, temp->pointers[i]->x, x2) == 0 || in_middle(y1, -temp->pointers[i]->y, y2) == 0){
                delete_element(temp, temp->pointers[i--]);
                continue;
            }
        }
    }
    //add blocks in the range
    for(b_list *temp = &(core->main); temp != NULL; temp = temp->next){
        for(size_t i = 0; i < temp->used; i++){
            if(in_middle(x1, temp->pointers[i]->x, x2) == 1 && in_middle(y1, -temp->pointers[i]->y, y2) == 1){
                add_element_distinct(&(frame->list), temp->pointers[i], size);
            }
        }
    }
}
void print_frame(Frame *frame){
    BeginDrawing();
    ClearBackground(frame->Dead_cell_color);
    BeginMode2D(frame->camera);
    for(b_list *temp; temp != NULL; temp = temp->next){
        for(size_t j = 0; j < temp->used; j++){
            print_block_camera(temp->pointers[j] , frame->alive_cell_color, frame->Dead_cell_color, frame->cell_size);
        }
    }
    EndMode2D();
    DrawFPS(0,0);
    EndDrawing();
}
void print_block_camera(g_block *block,Color alive_cell,Color dead_cell,size_t cell_size){
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 64; j++){
            DrawRectangle(((block->x * 64) - j) * cell_size,((block->y * -64) + i) * cell_size, cell_size, cell_size , ((block->current_block_content[i] >> j) & 1 == 1)? alive_cell : dead_cell);
        }
    }
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