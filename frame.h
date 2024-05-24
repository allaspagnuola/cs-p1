#ifndef FRAME_H
#define FRAME_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "process_q.h"

#define FRAME_NUMBER 512 // maximum frame number
#define PAGE_SIZE 4 // fixed page and frame size
#define MIN_RUNNING_PAGE 4  // the minimum pages in frames that a process allowed to run

typedef struct Frame_track{
    Process* frame_list[FRAME_NUMBER]; // a frame list to store frames for process
    int empty_frames; // number of empty frames in frame list
} Frame_track;

Frame_track* initialize_frame_track();

int insert(Process *p, Frame_track *track, int virtual);

void evict(Process *p, Frame_track *track, int pages_cnt, int virtual);

void free_frame(Frame_track *track);

#endif