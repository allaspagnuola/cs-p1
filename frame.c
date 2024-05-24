#include "frame.h"

/**
 * Function to initialize frame track for task 3 and task 4
 *
 * Return: frame track
*/
Frame_track* initialize_frame_track() {
    Frame_track* frame_track = (Frame_track*)malloc(sizeof(Frame_track));
    frame_track->empty_frames = FRAME_NUMBER;
    for (int i=0; i<FRAME_NUMBER; i++) {
        frame_track->frame_list[i] = NULL;
    }
    return frame_track;
}

/**
 * Function to allocate process to free frames
 * Input: 
 * virtual = 0 if process needs to load all frames to run;
 * virtual = 1 if process needs to load 4 frames to run.
 * 
 * 
 * Return: -1 (if there are no enough free frames in frame list)
 * Return: number of pages of this process have been inserted to the frame (if free frames are allocated to process)
*/
int insert(Process *p, Frame_track *track, int virtual){
    int pages_cnt = 0; 
    if(virtual == 0){
        // process needs to load all frames to run
        if (track->empty_frames < ceil((double)p->mem / PAGE_SIZE)) return -1;
    } else {
        // process needs to load 4 frames to run
        if (track->empty_frames < MIN_RUNNING_PAGE) return -1;
    }
    
    for(int i = 0; i < FRAME_NUMBER; i++){
        // allocate process to free frames
        if(track->frame_list[i] == NULL){
            track->frame_list[i] = p;
            track->empty_frames = track->empty_frames - 1;
            p->no_pageInFrames++;
            pages_cnt++;
        }
        if(pages_cnt == ceil((double)p->mem / PAGE_SIZE)) break;
    }
    p->isInFrame = 1;
    return pages_cnt;
}

/**
 * Function to evict page frames allcoated for LRU process.
 *  
 * Input: 
 * pages_cnt: number of pages need to be evicted.
 * virtual = 1 if this is used for task 4
 * virtual = 0 if this is used for other tasks
*/
void evict(Process *p, Frame_track *track, int pages_cnt, int virtual){
    for(int i = 0; i < FRAME_NUMBER; i++){
        if(track->frame_list[i] == p){
            track->frame_list[i] = NULL;
            track->empty_frames = track->empty_frames + 1;
            p->no_pageInFrames--;
            pages_cnt--;
            printf("%d", i);
            if(pages_cnt != 0) printf(",");
            else {printf("]\n");break;}
        }
    }
    if ((virtual == 1 && p->no_pageInFrames < MIN_RUNNING_PAGE) || virtual == 0) {
        p->isInFrame = 0;
    }
}

/**
 * Function to free frame list
*/
void free_frame(Frame_track *track){
    free(track);
}
