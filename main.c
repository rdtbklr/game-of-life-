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
#include "timer.h"
#include <math.h>

#define Sizeof_var(variable) printf(# variable "\t%d\n" , sizeof(variable));


struct timespec minimum_wait = {0, 500000};
#define cell_size 16

void swap_pointers(g_block *block){
    uint64_t *temp = block->current_block_content;
    block->current_block_content = block->next_block_content;
    block->next_block_content = temp;
}
struct timespec* smallest(struct timespec *x, struct timespec *y, struct timespec *z){
    if(x_bigger_equal_y(*x, *y))
    {
        if(x_bigger_equal_y(*y,*z))
        {
            return z;
        }else
        {
            return y;
        }
    }else{
        if(x_bigger_equal_y(*x,*z))
        {
            return z;
        }else
        {
            return x;
        }
    }
}

int main()
{

    srand(time(NULL));
    // initialising core
    Core core = init_core(32);
    for(int i = -5; i < 5; i++){
        for(int j = -5; j < 5; j++){
            g_block *temp_g = init_block(i,j);
            //create_random_noise(temp_g);
            for(int ii = 0; ii < 64; ii++)
            {
                temp_g->current_block_content[ii] = 0xFFFFFFFFFFFFFFFF;
            }
            add_block(&core, temp_g);
        }
    }

    //init frame

    Frame frame;
    Color y = YELLOW, b = BLACK;


    frame = init_frame( y, b , 16,32, 1024, 1024);
    frame.camera.target.x = 0;
    frame.camera.target.y = 0;
    frame.camera.zoom = 0.080;
    uint8_t frame_size_change =0 ;
    //printf("size of frame %d\t camera position x: %.2f y: %.2f\n", frame.list.used, frame.camera.target.x,frame.camera.target.y);
    //sleep(5);
    InitWindow(64 * cell_size,64 * cell_size, "Game of life ");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowState(FLAG_WINDOW_MAXIMIZED);
    frame.window_size_x = GetScreenWidth();
    frame.window_size_y = GetScreenHeight();
    frame.camera.offset.x = frame.window_size_x / 2.0;
    frame.camera.offset.y = frame.window_size_y / 2.0;
    fetch_blocks(&frame,&core, 32);



    // timing things
    double Update_rate = 10.0,Frame_rate = 60.0;
    U_t Updater,Renderer,UPS_tracker;
    Updater = init_timer(1/ Update_rate, 0);
    Renderer = init_timer(1/ Frame_rate, 0);
    UPS_tracker = init_timer(0.1, 101);
    struct timespec temp_t,current;
    clock_gettime(CLOCK_MONOTONIC, &Updater.next_update);
    clock_gettime(CLOCK_MONOTONIC, &Renderer.next_update);
    clock_gettime(CLOCK_MONOTONIC, &UPS_tracker.next_update);
    uint8_t pressed=0,released=0,upd = 0;
    //main loop
    for(uint64_t i = 1; WindowShouldClose() != 1; ){
        clock_gettime(CLOCK_MONOTONIC, &current);
        //drawing _____________________________________________________
        if(x_bigger_equal_y(current, Renderer.next_update)){
            if(IsWindowResized()){
                frame.window_size_x = GetScreenWidth();
                frame.window_size_y = GetScreenHeight();
                frame.camera.offset.x = frame.window_size_x / 2.0;
                frame.camera.offset.y = frame.window_size_y / 2.0;

                fetch_blocks(&frame, &core, frame.size);
            }

            BeginDrawing();
            print_frame(&frame);

            DrawText(TextFormat("%d\n0.1s %.2f\n1s %.2f\n10s %.2f",core.main.used, fetch_n(UPS_tracker, 1), fetch_n(UPS_tracker, 10), fetch_n(UPS_tracker, 100)), 120, 0, 20, LIME);
            EndDrawing();

            clock_gettime(CLOCK_MONOTONIC, &temp_t);
            next_update_relative(&Renderer,current,temp_t);
            current = temp_t;
        }
        // computing _____________________________________________________
        if(x_bigger_equal_y(current, Updater.next_update)){

            main_compute(&core);//gets the next state of the simulation and creates new blocks but does not empty the create next
            swap_pointers_core(&core);
            //check for deletion
            if((i % 192) == 0){
                check_for_deletion(&core);//removes from the main list and strips them from their neighbours
            }

            if(core.create_next.used != 0 || core.delete_next.used != 0){
                fetch_blocks(&frame, &core, frame.size);
            }

            empty_create_next(&core);//sets the size for zero
            delete_from_Core(&core);//frees from the memory and sets the size to zero


            //time stuff
            next_update_absolute(&Updater);
            i++;
        }
        // calculating ups _____________________________________________________
        if(x_bigger_equal_y(current, UPS_tracker.next_update)){
            add_number(&UPS_tracker,i);

            next_update_absolute(&UPS_tracker);
        }

        // sleeping _____________________________________________________
        temp_t = *smallest(&Updater.next_update/*&Renderer.next_update*/, &Renderer.next_update, &UPS_tracker.next_update);
        if(spin_wait(current, &temp_t, minimum_wait)){
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &temp_t, 0);
        }
    }
    printf("size of core %d\n", core.main.used);
    free_core(&core);
    return 0;
}
