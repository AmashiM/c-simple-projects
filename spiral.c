/**
 * Description: Iterates over an array of any size in a spiral
 * 
 */


#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>

#define SIZE 20

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef NULL
#define NULL (void*)0x0
#endif

static uint8_t** array;

void array_init(){
    array = (uint8_t**)malloc(sizeof(uint8_t*) * SIZE);

    for(int i = 0; i < SIZE; i++){
        array[i] = (uint8_t*)malloc(sizeof(uint8_t) * SIZE);
    }
}

void array_free(){
    for(int i = 0; i < SIZE; i++){
        free(array[i]);
        array[i] = NULL;
    }
    free(array);
    array = NULL;
}

void array_print(){
    uint8_t* row;
    for(int i = 0; i < SIZE; i++){
        row = array[i];
        printf("[");
        for(int q = 0; q < SIZE; q++){
            printf("%d", row[q]);
            if(q + 1 < SIZE){
                printf(",");
            }
        }
        printf("]\n");
    }
}

void array_fill(uint8_t value){
    uint8_t* row;
    for(int i = 0; i < SIZE; i++){
        row = array[i];
        for(int q = 0; q < SIZE; q++){
            row[q] = value;   
        }
    }
}

typedef void (*array_spiral_callback)(uint8_t*);

void array_spiral_for1(array_spiral_callback cb){
    int x, y;
    int min_x, max_x;
    int min_y, max_y;
    x = -1;
    y = 0;

    max_x = SIZE;
    max_y = SIZE;
    min_x = 0;
    min_y = 0;

    uint8_t* value;

    // uint8_t running = 1;

    // left, down, right, up

    int width;
    int height;

    width = max_x - min_x;
    height = max_y - min_y;

    // iterations should be half the size, but for testing we're doing half -1
    for(int iterations = 0; iterations < ((SIZE / 2))-1; iterations++){
        // the 4 directions
        for(int direction = 0; direction < 4; direction++){
            switch(direction){
                default:
                case 0:
                    // left
                    width = max_x - min_x;
                    for(int i = 0; i < width; i++){
                        x++;
                        value = &array[y][x];
                        cb(value);
                    }
                    min_y++; // top row cleared
                    break;
                case 1:
                    // down
                    height = max_y - min_y;
                    for(int i = 0; i < height; i++){
                        y++;
                        value = &array[y][x];
                        cb(value);
                    }
                    max_x--;
                    break;
                case 2:
                    // right
                    width = max_x - min_x;
                    for(int i = 0; i < width; i++){
                        x--;
                        value = &array[y][x];
                        cb(value);
                    }
                    max_y--;
                    break;
                case 3:
                    // up
                    height = max_y - min_y;
                    for(int i = 0; i < height; i++){
                        y--;
                        value = &array[y][x];
                        cb(value);
                    }
                    min_x ++;
                    break;
            }

        }

        // debug
        array_print();
    }

    // while(running){

    //     choose_direction:
    //         switch(direction){
    //             case 0:
    //                 goto move_left;
    //                 break;
    //             case 1:
    //                 goto move_down;
    //                 break;
    //             case 2:
    //                 goto move_right;
    //                 break;
    //             case 3:
    //                 goto move_up;
    //                 break;
    //         }
    //         goto after_movement;

    //     switch_direction:
    //         direction++;
    //         if(direction > 3) {
    //             direction = 0; // reset
    //         }
    //         goto local_next;

    //     // left to right
    //     move_left:
    //         min_y++;
    //         for(int i = min_x; i < max_x; i++){
    //             x++;
    //             value = array[y][x];
    //             cb(&value);
    //         }
    //         goto after_movement;
    //     // right to left
    //     move_right:
    //         for(int i = max_x; i > min_x; i--){
    //             x--;
    //             value = array[y][x];
    //             cb(&value);
    //         }
    //         goto after_movement;
    //     // up from bottom
    //     move_up:
    //         for(int i = min_y; i < max_y; i++){
    //             y++;
    //             value = array[y][x];
    //             cb(&value);
    //         }
    //         goto after_movement;
    //     // down from top
    //     move_down:
    //         for(int i = max_y; i > min_y; i--){
    //             y--;
    //             value = array[y][x];
    //             cb(&value);
    //         }
    //         goto after_movement;

    //     after_movement:
    //         goto switch_direction;

    //     local_next:
    //         if((max_x - min_x) <= 1 && (max_y - min_y) <= 1){
    //             running = 0;
    //             break;
    //         }
    //         continue;
    // }

    
}


void array_spiral_for2(array_spiral_callback cb){
    int x, y;
    int min_x, max_x;
    int min_y, max_y;
    x = -1;
    y = 0;

    max_x = SIZE;
    max_y = SIZE;
    min_x = 0;
    min_y = 0;

    uint8_t* value;

    // moves in this direction: left, down, right, up
    /**
     * 0: 0000 | w | x | ++
     * 1: 0001 | h | y | ++
     * 2: 0010 | w | x | --
     * 3: 0011 | h | y | --
     */

    // iterations should be half the size, but for testing we're doing half -1
    for(int iterations = 0; iterations < (SIZE / 2); iterations++){
        int c; // change in movement speed
        // the 4 directions
        for(uint8_t direction = 0; direction < 4; direction++){
            int orientation; // 1 == horizontal, 0 == vertical
            int length; // length can be either width or height

            if(direction < 2) {
                c = 1;
            } else {
                c = -1;
            }

            orientation = direction & 0b000000001;

            if(orientation == 0){
                length = max_x - min_x; // width
            } else {
                length = max_y - min_y; // height
            }

            // process movement for length
            for(int i = 0; i < length; i++){

                if(orientation == 0){
                    x = x + c;
                } else {
                    y = y + c;
                }

                value = &array[y][x];
                cb(value);
            }

            switch(direction){
                default: printf("something unexpected happened"); break;
                case 0: min_y++; break;
                case 1: max_x--; break;
                case 2: max_y--; break;
                case 3: min_x++; break;
            }
        }

        // debug
        array_print();
    }
}


void array_spiral_for(array_spiral_callback cb){
    /**md
     * `r/s(x|y)/` = min
     * `r/b(x|y)/` = max
     */
    int x, y, sx, bx, sy, by;
    x = -1;

    bx = by = SIZE;
    y = sx = sy = 0;


    // moves in this direction: left, down, right, up
    // if you want to change directions, you need to change the code but main thing is that I do use unsigned numbers here so you'll need to adjust for signed values or use logic.
    // iterations should be half the size
    for(int i = 0; i < (SIZE / 2); i++){
        int c; // change in movement speed
        // the 4 directions
        for(uint8_t direction = 0; direction < 4; direction++){
            int o; // 1 == horizontal, 0 == vertical
            int l; // length(l) can be either width or height

            if(direction < 2) {
                c = 1;
            } else {
                c = -1;
            }

            o = direction & 0b000000001;

            if(o == 0){
                l = bx - sx; // width
            } else {
                l = by - sy; // height
            }

            // process movement for length(l) where m is the steps taken
            for(unsigned int m = 0; m < l; m++){

                if(o == 0){
                    x = x + c;
                } else {
                    y = y + c;
                }

                // get value and pass to callback
                cb(&array[y][x]);
            }

            switch(direction){
                default: printf("something unexpected happened"); break;
                case 0: sy++; break;
                case 1: bx--; break;
                case 2: by--; break;
                case 3: sx++; break;
            }
        }
    }
}

static int a = 0;

void on_new_pos(uint8_t* value){
    a++;
    *value = *value + a;
}

int main(void){
    array_init();

    array_fill(1);

    array_spiral_for(&on_new_pos);

    printf("done\n");

    array_print();

    array_free();
    return 0;
}