#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "process_q.h"

#define MEMORY_SIZE 2048

typedef struct Process Process;

typedef struct Block{
    int start;  // index in memory where this block starts from
    int size; // size of this block
    Process *p; // the process allocated at this block, otherwise NULL
    Block *next; // the block after this block
    Block *prev; // the block before this block
} Block;

typedef struct Memory{
    int size;
    Block *head;
} Memory;

Block* create_block(int start, int size, Process* p);

Memory* initialize_memory(int size);

void free_memory(Process *p, Memory *m);

int first_fit_allocate(Process *p, Memory *m);

int memory_usage(Memory *m);

void free_all_memory(Memory *m);

#endif