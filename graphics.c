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
}
void print_frame(Frame *frame){
    //BeginDrawing();
    ClearBackground(frame->Dead_cell_color);
    BeginMode2D(frame->camera);
    for(b_list *temp = &frame->list; temp != NULL; temp = temp->next){
        for(size_t j = 0; j < temp->used; j++){
            //printf("printing (%d,%d)\n",temp->pointers[j]->x, temp->pointers[j]->y);
            print_block_camera(temp->pointers[j] , frame->alive_cell_color, frame->Dead_cell_color, frame->cell_size);
        }
    }
    EndMode2D();
    DrawFPS(0,0);
    //EndDrawing();
}
void print_block_camera(g_block *block,Color alive_cell,Color dead_cell,size_t cell_size){
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 64; j++){
            if(((block->current_block_content[i] >> j) & 1) == 1){
                DrawRectangle(((block->x * 64) - j) * cell_size,((block->y * -64) + i) * cell_size, cell_size, cell_size , alive_cell);

            }else{
                DrawRectangle(((block->x * 64) - j) * cell_size,((block->y * -64) + i) * cell_size, cell_size, cell_size ,dead_cell);
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
    }
*/

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