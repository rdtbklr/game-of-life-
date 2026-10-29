#include "timer.h"

struct timespec addition(struct timespec x, struct timespec y){
    x.tv_nsec += y.tv_nsec;
    x.tv_sec += y.tv_sec;
    if(x.tv_nsec >= 1000000000L){
        x.tv_nsec -= 1000000000L;
        x.tv_sec++;
    }
    return x;
}
struct timespec subtraction(struct timespec x, struct timespec y){
    if(x.tv_nsec < y.tv_nsec){
        x.tv_nsec = x.tv_nsec + 1000000000l -y.tv_nsec;
        x.tv_sec--;
    }else{
        x.tv_nsec -= y.tv_nsec;
    }
    x.tv_sec -= y.tv_sec;
    return x;
}

struct timespec halve(struct timespec x){
    x.tv_nsec = x.tv_nsec / 2 + (((x.tv_sec & 1) == 1)? 500000000l : 0);
    x.tv_sec /= 2;
    return x;
}
uint8_t x_bigger_equal_y(struct timespec x, struct timespec y){
    if(x.tv_sec > y.tv_sec){
        return 1;
    }else if(x.tv_sec == y.tv_sec && x.tv_nsec >= y.tv_nsec){
        return 1;
    }
    return 0;
}

U_t init_timer(double update_interval, uint64_t size){
    U_t t;
    if(size != 0){
        t.queue = (uint64_t*) calloc(size,sizeof(uint64_t));
        if(t.queue == NULL){
            printf("timer could not be init");
            exit(1);
        }
    }
    t.size = size;
    t.head = 0;
    t.update_interval = update_interval;
    t.update_interval_t.tv_sec = (long int) update_interval;
    t.update_interval_t.tv_nsec = ((long int) (update_interval * 1000000000L)) % 1000000000L;
    t.next_update = (struct timespec){0,0};
    t.delay = (struct timespec){0,0};
    return t;
}

void add_number(U_t *t, uint64_t number){
    t->head = (t->head == 0)? (t->size - 1) : (t->head - 1);
    t->queue[t->head] = number;
}
float fetch_n(U_t t, uint64_t n){
    uint64_t ans = t.queue[t.head] - t.queue[(t.head + n) % t.size];
    return ans / (t.update_interval * n);
}

void next_update_absolute(U_t *t/*, struct timespec start_time, struct timespec end_time*/){
    /*
    t->delay = halve(addition(t->delay, subtraction(end_time , start_time)));
    t->next_update = subtraction(addition(end_time , t->update_interval_t) , t->delay);*/
    t->next_update = addition(t->next_update, t->update_interval_t);
}
void next_update_relative(U_t *t, struct timespec start_time, struct timespec end_time){
    t->delay = halve(addition(t->delay, subtraction(end_time , start_time)));
    t->next_update = subtraction(addition(end_time , t->update_interval_t) , t->delay);
}
void change_update_interval(U_t *t, double update_interval){
    t->update_interval = update_interval;
    t->update_interval_t.tv_sec = (long int) update_interval;
    t->update_interval_t.tv_nsec = ((long int) (update_interval * 1000000000L)) % 1000000000L;
}

uint8_t spin_wait(struct timespec current, struct timespec *next, struct timespec min_wait){
    current = subtraction(*next, current);
    *next = subtraction(*next, min_wait);
    return x_bigger_equal_y(current, min_wait);
}
void free_U_t(U_t t){
    if(t.size != 0){
        free(t.queue);
    }
}