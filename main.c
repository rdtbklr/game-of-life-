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
    double Update_rate = 20.0,Frame_rate = 240.0;
    g_block *block = init_block(0,0);
    create_random_noise(block);
    //swap_pointers((void**)&block->next_block_content, (void**)&block->current_block_content);
    InitWindow(64 * cell_size,64 * cell_size, "Game of life ");
    print_block(block,0 ,0 , cell_size);
    U_t Updater,Renderer,UPS_tracker;
    Updater = init_timer(1/ Update_rate, 0);
    Renderer = init_timer(1/ Frame_rate, 0);
    UPS_tracker = init_timer(0.1, 101);
    printf("Updater %9.ld %9.ld\tRenderer %9.ld %9.ld\tUPS_tracker %9.ld %9.ld\n",
           Updater.update_interval_t.tv_sec,Updater.update_interval_t.tv_nsec,
           Renderer.update_interval_t.tv_sec,Renderer.update_interval_t.tv_nsec,
           UPS_tracker.update_interval_t.tv_sec,UPS_tracker.update_interval_t.tv_nsec);
    struct timespec temp_t,current;

    clock_gettime(CLOCK_MONOTONIC, &Updater.next_update);
    clock_gettime(CLOCK_MONOTONIC, &Renderer.next_update);
    clock_gettime(CLOCK_MONOTONIC, &UPS_tracker.next_update);
    for(uint64_t i = 0; WindowShouldClose() != 1; ){
        clock_gettime(CLOCK_MONOTONIC, &current);
        if(x_bigger_equal_y(current, Updater.next_update)){
            compute_block_avx512(block);
            swap_pointers(block);
            next_update_absolute(&Updater);
            i++;
        }
        if(x_bigger_equal_y(current, Renderer.next_update)){
            BeginDrawing();
            print_block(block ,0,0,cell_size);
            // Draw TextFormat("string", variables), X, Y, Font Size, Color
            DrawText(TextFormat("0.1s %.2f\n1s %.2f\n10s %.2f", fetch_n(UPS_tracker, 1), fetch_n(UPS_tracker, 10), fetch_n(UPS_tracker, 100)), 120, 0, 20, LIME);
            EndDrawing();

            clock_gettime(CLOCK_MONOTONIC, &temp_t);
            next_update_relative(&Renderer,current,temp_t);
            current = temp_t;
        }
        if(x_bigger_equal_y(current, UPS_tracker.next_update)){
            add_number(&UPS_tracker,i);

            next_update_absolute(&UPS_tracker);
        }
        /*printf("current: %9.ld %9.ld\tupdater: %9.ld %9.ld\trenderer: %9.ld %9.ld\tups tracker: %9.ld %9.ld\n",current.tv_sec,current.tv_nsec,
               Updater.next_update.tv_sec, Updater.next_update.tv_nsec,
               Renderer.next_update.tv_sec, Renderer.next_update.tv_nsec,
               UPS_tracker.next_update.tv_sec, UPS_tracker.next_update.tv_nsec   );*/
        temp_t = *smallest(&Updater.next_update, &Renderer.next_update, &UPS_tracker.next_update);
        if(spin_wait(current, &temp_t, minimum_wait)){

            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &temp_t, 0);
        }
    }
    free(block);
    return 0;
}
