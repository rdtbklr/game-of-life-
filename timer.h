#ifndef TIMER_H
#define TIMER_H
#include "Structs.h"
#include <stdio.h>
#include <time.h>
#include <stdint.h>

#define get_next_update(U_t) U_t.next_update

U_t init_timer(double update_interval, uint64_t size);
void add_number(U_t *t, uint64_t number);
float fetch_n(U_t t, uint64_t n);
void next_update_absolute(U_t *t);
void next_update_relative(U_t *t, struct timespec start_time, struct timespec end_time);
void change_update_interval(U_t *t, double update_interval);
void free_U_t(U_t t);
uint8_t x_bigger_equal_y(struct timespec x, struct timespec y);
uint8_t spin_wait(struct timespec current, struct timespec *next, struct timespec min_wait);


#endif // TIMER_H
