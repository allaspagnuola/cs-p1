#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "process_q.h"
#include "memory.h" 
#include "frame.h"

void print_performance(Process **proc_list, int cnt, int time_complete);

char* read_command(int argc, char *argv[], char **method, int *quantum);

Process** read_process(int argc, char *argv[], char **method, int *quantum, int *p_cnt);

int infinite(Process **proc_list, int p_cnt, int quantum);

int first_fit(Process **proc_list, int p_cnt, int quantum);

int paged(Process **proc_list, int p_cnt, int quantum);

int virtual(Process **proc_list, int p_cnt, int quantum);


int main(int argc, char *argv[]) {
    char *method = NULL;
    int quantum = 0;
    int p_cnt = 0;
    Process **proc_list;  // List of processes

    // read processes to process list
    proc_list = read_process(argc, argv, &method, &quantum, &p_cnt);

    int time_stamp;
    if (strcmp(method, "infinite") == 0) { 
        time_stamp = infinite(proc_list, p_cnt, quantum);
    }else if (strcmp(method, "first-fit") == 0){
        time_stamp = first_fit(proc_list, p_cnt, quantum);
    }else if (strcmp(method, "paged") == 0){
        time_stamp = paged(proc_list, p_cnt, quantum);
    } else {
        time_stamp = virtual(proc_list, p_cnt, quantum);
    }

    print_performance(proc_list, p_cnt, time_stamp);

    free_process(proc_list, p_cnt);

    return EXIT_SUCCESS;
}

/**
 * Function to calculate and print turnaround time, time overhead and makespan.
 *
 * Input: process list which contains all processes;
 * cnt to count the number of processses;
 * time_complete is the time stamp when all processses are finished.
*/
void print_performance(Process **proc_list, int cnt, int time_complete){
    int total= 0;
    double total_over = 0;
    double max_over = 0;
    for(int i = 0; i < cnt; i++){
        total += proc_list[i]->complete_time - proc_list[i]->arr_time;
        double over = ((double)(proc_list[i]->complete_time - proc_list[i]->arr_time))/(double)proc_list[i]->serv_time;
        total_over += over;
        if(over > max_over) max_over = over;
    }
    printf("Turnaround time %.f\n", ceil((double)total/(double)cnt));
    printf("Time overhead %.2f %.2f\n", max_over, ((int)(total_over/cnt * 100 + 0.5)) / 100.0);
    printf("Makespan %d\n", time_complete);
}

/**
 * Function to read command line, load method and quantum according to command line
 * 
 * Return: file name
*/
char* read_command(int argc, char *argv[], char **method, int *quantum) {

    char *filename = NULL;
    if (argc != 7) {  // Expecting 6 arguments plus the program name
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-f") == 0) {
            filename = argv[i + 1];
        } else if (strcmp(argv[i], "-m") == 0) {
            *method = argv[i + 1];
            if (strcmp(*method, "infinite") != 0 && strcmp(*method, "first-fit") != 0 &&
                strcmp(*method, "paged") != 0 && strcmp(*method, "virtual") != 0) {
                fprintf(stderr, "Invalid memory allocation method: %s\n", *method);
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "-q") == 0) {
            *quantum = atoi(argv[i + 1]);
            if (*quantum < 1 || *quantum > 3) {
                fprintf(stderr, "Invalid quality value: %d. Must be 1, 2, or 3.\n", *quantum);
                exit(EXIT_FAILURE);
            }
        }
    }

    if (!filename || !*method) {
        fprintf(stderr, "Missing required arguments.\n");
        exit(EXIT_FAILURE);
    }
    return filename;
}

/**
 * Function to read processes, change p_cnt as process counter. 
 * 
 * Return: a process list.
*/
Process** read_process(int argc, char *argv[], char **method, int *quantum, int *p_cnt) {

    // read file name, method and quantum from command line
    char *filename = read_command(argc, argv, method, quantum);

    // open file
    FILE *f = fopen(filename, "r");

    char att[MAX_DIGIT];
    int att_cnt = 0, i = 0;
    int t_arr, t_serv, mem;
    char pname[MAX_NAME_LENGTH]; 

    Process **proc_list = NULL; // Initialize a process list
    *p_cnt = 0; // Initialize the process count

    // read file
    char c;
    while ((c = fgetc(f)) != EOF) {
        if (c == ' ' || c == '\n') {
            // when an attribute is read
            att[i] = '\0';
            switch (att_cnt) {
                case 0: 
                //the first attribute is the arrival time of this process
                t_arr = atoi(att); 
                break;

                case 1: 
                //the second attribute is the name of this process
                strcpy(pname, att); 
                break;

                case 2: 
                //the third attribute is the service time of this process
                t_serv = atoi(att); 
                break;

                case 3: 
                //the fourth attribute is the memory usage of this process
                mem = atoi(att); 
                break;
            }
            i = 0;
            att_cnt++;
            if (c == '\n') {
                // if a process has been read
                Process **temp = realloc(proc_list, sizeof(Process*) * (*p_cnt + 1));
                proc_list = temp;
                att_cnt = 0; 
                proc_list[*p_cnt] = initialize_p(pname, t_arr, t_serv, mem);
                (*p_cnt)++;
            }
        } else {
            att[i++] = c;
        }
    }
    // close file
    fclose(f);
    return proc_list;
}

/**
 * Function to run infinite algorithm, corresponding to task 1.
 * 
 * Return: the time stamp when all processes are finished.
*/
int infinite(Process **proc_list, int p_cnt, int quantum) {

    // initialize a ready queue for processes in ready state
    Queue *ready_q = initialize_q();

    int time_stamp = 0;
    int rem_p = p_cnt;
    Process *running = NULL;

    while(rem_p != 0){
        // if proesses haven't finished
        for(int i = 0; i < p_cnt; i++){
            if(proc_list[i]->arr_time <= time_stamp && proc_list[i]->arr_time > time_stamp - quantum){
                // for processes are arrived at this quantum, add them to queue
                enqueue(ready_q, proc_list[i]);
            }
        }
        if(running && running->rem_time == 0){
            // for processses are finished at the start of this quantum
            printf("%d,FINISHED,process-name=%s,proc-remaining=%d\n", time_stamp, running->pname, q_size(ready_q));
            running->complete_time = time_stamp;
            running = NULL;
        }
        if(!isEmpty(ready_q)){
            // run a process from ready queue
            if(running) enqueue(ready_q, running);
            running = dequeue(ready_q);
            printf("%d,RUNNING,process-name=%s,remaining-time=%d\n", time_stamp, running->pname, running->rem_time);
        }

        time_stamp += quantum;
        if (running) {
            // update remaining time if there is a process running at this time stamp
            running->rem_time = running->rem_time - quantum; }

        if(running && running->rem_time < 0) {
            // if there is a process finished at this time stamp
            running->rem_time = 0;}
        rem_p = remaining_p(proc_list, p_cnt); // check how many processes are not finished
        if(rem_p == 0){
            // if the last process is finished at this timestamp
            printf("%d,FINISHED,process-name=%s,proc-remaining=%d\n", time_stamp, running->pname, q_size(ready_q));
            running->complete_time = time_stamp;
        }
    }
    // free ready_queue
    free_q(ready_q);
    return time_stamp;
}

/**
 * Function to run first fit algorithm, corresponding to task 2
 * 
 * Return time stamp when all processes are finished
*/
int first_fit(Process **proc_list, int p_cnt, int quantum){


    Memory *memory = initialize_memory(MEMORY_SIZE);

    // initialize a ready queue for processes in ready state
    Queue *ready_q = initialize_q(); 
    int time_stamp = 0;
    int rem_p = p_cnt;
    Process *running = NULL;

    while(rem_p != 0){
        // if proesses haven't finished
        for(int i = 0; i < p_cnt; i++){
            if(proc_list[i]->arr_time <= time_stamp && proc_list[i]->arr_time > time_stamp - quantum){
                // for processes are arrived at this quantum, add them to queue
                enqueue(ready_q, proc_list[i]);
            }
        }
        if(running && running->rem_time == 0){
            // for processses are finished at the start of this quantum
            printf("%d,FINISHED,process-name=%s,proc-remaining=%d\n", time_stamp, running->pname, q_size(ready_q));
            running->complete_time = time_stamp;
            free_memory(running, memory); 
            running = NULL;
        }
        if(!isEmpty(ready_q)){
            // run a process from ready queue
            if(running) enqueue(ready_q, running);
            running = dequeue(ready_q);
            
            while(running->addr == NULL){
                if(first_fit_allocate(running, memory) == -1){
                    enqueue(ready_q, running);
                    running = dequeue(ready_q);
                }
            }
            
            printf("%d,RUNNING,process-name=%s,remaining-time=%d,mem-usage=%d%%,allocated-at=%d\n", time_stamp, running->pname, running->rem_time, memory_usage(memory), running->addr->start);
        }

        time_stamp += quantum;
        if(running) running->rem_time = running->rem_time - quantum;
        if(running && running->rem_time < 0) running->rem_time = 0;
        rem_p = remaining_p(proc_list, p_cnt); // check how many processes are not finished
        if(rem_p == 0){
            // if the last process is finished at this timestamp
            printf("%d,FINISHED,process-name=%s,proc-remaining=%d\n", time_stamp, running->pname, q_size(ready_q));
            running->complete_time = time_stamp;
        }
    }

    free_all_memory(memory);
    free_q(ready_q);
    return time_stamp;
}

/**
 * Function to run paged algorithm, corresponding to task 3.
 * 
 * Return: the time stamp when all processes are finished.
*/
int paged(Process **proc_list, int p_cnt, int quantum) {
    Queue *ready_q = initialize_q();

    // create a frames list
    Frame_track* frame_track = initialize_frame_track();

    int time_stamp = 0;
    int rem_p = p_cnt;
    Process *running = NULL;

    while(rem_p != 0){
        // if proesses haven't finished
        for(int i = 0; i < p_cnt; i++){
            if(proc_list[i]->arr_time <= time_stamp && proc_list[i]->arr_time > time_stamp - quantum){
                // for processes are arrived at this quantum, add them to queue
                enqueue(ready_q, proc_list[i]);
            }
        }
        if(running && running->rem_time == 0){
            // for processses are finished at the start of this quantum
            printf("%d,EVICTED,evicted-frames=[", time_stamp);
            evict(running, frame_track, ceil((double)running->mem / PAGE_SIZE), 0);
            printf("%d,FINISHED,process-name=%s,proc-remaining=%d\n", time_stamp, running->pname, q_size(ready_q));
            running->complete_time = time_stamp;
            running = NULL;
        }
        if(!isEmpty(ready_q)){ 
            // run a process from ready queue
            if(running) enqueue(ready_q, running);
            running = dequeue(ready_q);

            // check if running has space in frame_list
            if (running->isInFrame == 0) {
                while(insert(running, frame_track, 0) == -1){
                    // find the LRU processes and evict all pages
                    printf("%d,EVICTED,evicted-frames=[", time_stamp);
                    Process *lru_proc = find_LRU_proc(ready_q);
                    evict(lru_proc, frame_track, ceil((double)lru_proc->mem / PAGE_SIZE), 0); 
                }
            }  

            int mem_usage = (int)ceil((double)(FRAME_NUMBER - frame_track->empty_frames) / FRAME_NUMBER * 100);
            printf("%d,RUNNING,process-name=%s,remaining-time=%d,mem-usage=%d%%,mem-frames=[", time_stamp, running->pname, running->rem_time, mem_usage);

            // print all frames of this running process
            int pages_rem = ceil((double)running->mem / PAGE_SIZE);
            for (int i=0; i<FRAME_NUMBER; i++) {
                if (frame_track->frame_list[i] == running) {
                    printf("%d", i);
                    pages_rem --;
                    if (pages_rem == 0) printf("]\n");
                    else printf(",");
                }
            }
        }
 
        time_stamp += quantum;
        if(running) {running->rem_time = running->rem_time - quantum; running->last_used = time_stamp - quantum;}
        if(running && running->rem_time < 0) running->rem_time = 0;
        rem_p = remaining_p(proc_list, p_cnt); // check how many processes are not finished
        if(rem_p == 0){
            // if the last process is finished at this timestamp
            printf("%d,EVICTED,evicted-frames=[", time_stamp);
            evict(running, frame_track, ceil((double)running->mem / PAGE_SIZE), 0);
            printf("%d,FINISHED,process-name=%s,proc-remaining=%d\n", time_stamp, running->pname, q_size(ready_q));
            running->complete_time = time_stamp;
        }
    }  
    free_q(ready_q);
    free_frame(frame_track);
    return time_stamp;
}

/**
 * Function to run virtual algorithm, corresponding to task 4.
 * 
 * Return: the time stamp when all processes are finished.
*/
int virtual(Process **proc_list, int p_cnt, int quantum) {
    Queue *ready_q = initialize_q();

    Frame_track* frame_track = initialize_frame_track();

    int time_stamp = 0;
    int rem_p = p_cnt;
    Process *running = NULL;

    while(rem_p != 0){
        // if proesses haven't finished
        for(int i = 0; i < p_cnt; i++){
            if(proc_list[i]->arr_time <= time_stamp && proc_list[i]->arr_time > time_stamp - quantum){
                // for processes are arrived at this quantum, add them to queue
                enqueue(ready_q, proc_list[i]);
            }
        }
        if(running && running->rem_time == 0){
            // for processses are finished at the start of this quantum
            printf("%d,EVICTED,evicted-frames=[", time_stamp);
            evict(running, frame_track, running->no_pageInFrames, 1);
            printf("%d,FINISHED,process-name=%s,proc-remaining=%d\n", time_stamp, running->pname, q_size(ready_q));
            running->complete_time = time_stamp;
            running = NULL;
        }
        if(!isEmpty(ready_q)){ 
            // run a process from ready queue
            if(running) enqueue(ready_q, running);
            running = dequeue(ready_q); 
            // check if the process has space in frame_list
            if (running->isInFrame == 0) {
                if (ceil((double)running->mem / PAGE_SIZE) <= MIN_RUNNING_PAGE) {
                    // for processes which have less than or equal to 4 pages
                    // insert all pages
 
                    while(insert(running, frame_track, 0) == -1){
                        // find the LRU processes and evict needed pages
                        printf("%d,EVICTED,evicted-frames=[", time_stamp);
                        Process *lru_proc = find_LRU_proc(ready_q);
                        evict(lru_proc, frame_track, ceil((double)running->mem / PAGE_SIZE) - frame_track->empty_frames, 1); 
                    }
                } else {
                    // for processes having more than 4 pages
                    while (insert(running, frame_track, 1) == -1) {
                        // evict LRU processes' pages if less than min_running_page
                        printf("%d,EVICTED,evicted-frames=[", time_stamp);
                        Process *lru_proc = find_LRU_proc(ready_q);
                        if (MIN_RUNNING_PAGE - frame_track->empty_frames >= lru_proc->no_pageInFrames) {
                            evict(lru_proc, frame_track, lru_proc->no_pageInFrames, 1);
                        } else {
                            evict(lru_proc, frame_track, MIN_RUNNING_PAGE - frame_track->empty_frames, 1);
                        }
                    }
                }
            }

            int mem_usage = ceil((double)(FRAME_NUMBER - frame_track->empty_frames) / FRAME_NUMBER * 100);
            printf("%d,RUNNING,process-name=%s,remaining-time=%d,mem-usage=%d%%,mem-frames=[", time_stamp, running->pname, running->rem_time, mem_usage);
            // print all frames of this running process
            int pages_rem = running->no_pageInFrames; 
            for (int i=0; i<FRAME_NUMBER; i++) {
                if (frame_track->frame_list[i] == running) {
                    printf("%d", i);
                    pages_rem --;
                    if (pages_rem == 0) printf("]\n");
                    else printf(",");
                }
            }
        }
  
        time_stamp += quantum;
        if(running) {running->rem_time = running->rem_time - quantum; running->last_used = time_stamp - quantum;}
        if(running && running->rem_time < 0) running->rem_time = 0;
        rem_p = remaining_p(proc_list, p_cnt); // check how many processes are not finished
        if(rem_p == 0){
            // if the last process is finished at this timestamp
            printf("%d,EVICTED,evicted-frames=[", time_stamp);
            evict(running, frame_track, running->no_pageInFrames, 1);
            printf("%d,FINISHED,process-name=%s,proc-remaining=%d\n", time_stamp, running->pname, q_size(ready_q));
            running->complete_time = time_stamp;
        }
    }
    free_q(ready_q);
    free_frame(frame_track);
    return time_stamp;
}
