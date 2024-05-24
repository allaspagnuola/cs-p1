#include "memory.h"

/**
 * Function to create and initialise blocks
 * 
 * return: Block*
*/
Block* create_block(int start, int size, Process* p){
    Block *b = (Block*)malloc(sizeof(Block));
    b->start = start;
    b->size = size;
    b->p = p;
    b->next = NULL;
    b->prev = NULL;
    return b;
}

/**
 * Function to initialize memory 
 * 
 * return: Memory*
*/
Memory* initialize_memory(int size){
    Memory *m = (Memory*)malloc(sizeof(Memory));
    m->size = size;
    m->head = create_block(0, size, NULL);
    return m;
}

/**
 * Function to achieve first fit allocation coresponding to Task 2
 * 
 * Input: 
 * process;
 * memory;
 * 
 * return: 0 for success or -1 for failure
*/
int first_fit_allocate(Process *p, Memory *m){
    Block *curr = m->head;
    while(curr){
        if(p->mem <= curr->size && curr->p == NULL) break;
        curr = curr->next;
    }
    if(curr && curr->size > p->mem){
        Block *prev = curr->prev;
        Block *next = curr->next;

        p->addr = create_block(curr->start, p->mem, p);
        p->addr->next = create_block(curr->start + p->mem, curr->size - p->mem, NULL);
        p->addr->next->prev = p->addr;

        p->addr->prev = prev;
        if(prev) prev->next = p->addr;

        p->addr->next->next = next;
        if(next) next->prev = p->addr->next;

        
        if(curr->start == 0) m->head = p->addr;
    }else if(curr && curr->size == p->mem){
        curr->p = p;
        p->addr = curr;
    }else{return -1;}
    return 0;
}

/**
 * Function to free the memory of a certain process and handle situations where the memory
 * blocks should be merged
 * 
 * Input:
 * process;
 * memory;
*/
void free_memory(Process *p, Memory *m){
    p->addr->p = NULL;
    Block *prev = p->addr->prev;
    Block *next = p->addr->next;

    if(prev && prev->p == NULL && next && next->p == NULL){
        // if the prev and next are both vacant, merge these 3 blocks together
        if(prev->start == 0){
            // if the prev block is the head of the list
            m->head = create_block(0, prev->size+p->addr->size+next->size, NULL);
            m->head->next = next->next;
            if(next->next) next->next->prev = m->head;
        }else{
            prev->prev->next = create_block(0, prev->size+p->addr->size+next->size, NULL);
            prev->prev->next->next = next->next;
            if(next->next) next->next->prev = prev->prev->next;
        }
        free(p->addr);
    }else if(prev && prev->p == NULL){
        // if the prev is vacant, merge it with the block just freed
        if(prev->start == 0){
            // if the prev block is the head of the list
            m->head = create_block(0, prev->size+p->addr->size, NULL);
            m->head->next = next;
            if(next && next->next) next->next->prev = m->head;
        }else{
            prev->prev->next = create_block(0, prev->size+p->addr->size, NULL);
            prev->prev->next->next = next;
            if(next && next->next) next->next->prev = prev->prev->next;
        }
        free(p->addr);
    }else if(next && next->p == NULL){
        // if the next is vacant, merge it with the block just freed
        if(p->addr->start == 0){
            // if the freed block is the head of the list
            m->head = create_block(0, p->addr->size+next->size, NULL);
            m->head->next = next->next;
            if(next->next) next->next->prev = m->head;
        }else{
            prev->next = create_block(0, p->addr->size+next->size, NULL);
            prev->next->prev = prev;
            prev->next->next = next->next;
            if(next->next) next->next->prev = prev->next;
        }
        free(p->addr);
    }else{
        // both prev and next are occupied
        p->addr->p = NULL;
    }
    
    p->addr = NULL;
}

/**
 * Function to calculate memory usage
 * 
 * Input:
 * memory;
 * 
 * return: memory usage
*/
int memory_usage(Memory *m){
    Block *curr = m->head;
    int used = 0;
    while(curr){
        if(curr->p) used += curr->size;
        curr = curr->next;
    }
    return ceil((double)used*100/m->size);
}

/**
 * Function to free all memory allocation
*/
void free_all_memory(Memory *m){
    Block *curr = m->head;
    while(curr){
        Block *temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(m);
}