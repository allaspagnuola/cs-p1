#ifndef PROCESS_Q_H
#define PROCESS_Q_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 8 // the maximum length of a process name
#define MAX_DIGIT 10 // the upper bound of an integer is 2^32, which has 10 digits

typedef struct Block Block;
typedef struct Memory Memory;

typedef struct Process{
    char pname[MAX_NAME_LENGTH]; // process name
    int arr_time; // arrival time
    int serv_time; // service time
    int rem_time; // remaining time
    int mem; // memory
    int complete_time; // time stamp when the process is completed
    int last_used; // the last time this process has runned
    int isInFrame; // 0 if the process is not in frames, 1 if the process is in frames
    int no_pageInFrames; // number of pages that stored in frames
    Block *addr; // the block this process is allocated at
} Process;

typedef struct Node {
    Process *process;
    struct Node *next;
} Node;

typedef struct {
    Node *front;
    Node *rear;
} Queue;

Queue* initialize_q();

int isEmpty(Queue *q);

void enqueue(Queue *q, Process *p);

Process* dequeue(Queue *q);

void free_q(Queue *q);

int q_size(Queue *q);

int remaining_p(Process **proc_list, int cnt);

Process* initialize_p(char *name, int arr, int serv, int mem);

void free_process(Process **proc_list, int cnt);

Process* find_LRU_proc(Queue *q);

#endif