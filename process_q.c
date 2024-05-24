#include "process_q.h"

/**
 * Function to initialize a queue
 * 
 * Return: queue
*/
Queue* initialize_q() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;
    return q;
}

/**
 * Function to check if the queue is empty
*/
int isEmpty(Queue *q) {
    return (q->front == NULL);
}

/**
 * Function to insert a process to a queue
*/
void enqueue(Queue *q, Process *p) {
    Node *new = (Node *)malloc(sizeof(Node));
    new->process = p;
    new->next = NULL;

    if (isEmpty(q)) {
        q->front = new;
    } else {
        q->rear->next = new;
    }
    q->rear = new;
}

/**
 * Function to return the first process of the queue
*/
Process* dequeue(Queue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return (Process*)NULL;
    }
    Node *temp = q->front;
    Process *p = temp->process;
    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    return p;
}

/**
 * Function to free queue
*/
void free_q(Queue *q) {
    while (!isEmpty(q)) {
        dequeue(q);
    }
    free(q);
}

/**
 * Function to initialize a process.
 * 
 * Input: 
 * process name;
 * arrival time;
 * service time;
 * memory.
 * 
 * Return: a process.
*/
Process* initialize_p(char *name, int arr, int serv, int mem) {
    Process *p = (Process *)malloc(sizeof(Process));
    strcpy(p->pname, name);
    p->arr_time = arr;
    p->serv_time = serv;
    p->rem_time = serv;
    p->mem = mem;
    p->complete_time = -1;
    p->last_used = arr;
    p->isInFrame = 0;
    p->no_pageInFrames = 0;
    p->addr = NULL;
    return p;
}

/**
 * Function to count how many instances in the queue
 * 
 * Return: the size of the queue
*/
int q_size(Queue *q){
    int cnt = 0;
    if (!isEmpty(q)) {
        Node *tmp = q->front;
        while (tmp != NULL) {
            cnt++;
            tmp = tmp->next;
        }
    }
    return cnt;
}

/**
 * Function to return the number of processes have not finished in a process list
*/
int remaining_p(Process **proc_list, int cnt){
    int rem = 0;
    for(int i = 0; i < cnt; i++){
        if(proc_list[i]->rem_time > 0){
            rem++;
        }
    }
    return rem;
}

/**
 * Function to free every process in the process list.
*/
void free_process(Process **proc_list, int cnt){
    for(int i = 0; i < cnt; i++){
        free(proc_list[i]);
    }
    free(proc_list);
}

/*
 * Function to find least recently used process which allocated memories in frame list
 *
 * Return: the least recently used process
*/
Process* find_LRU_proc(Queue *q) {
    Process * lowest_proc;
    if (!isEmpty(q)) {
        Node *tmp = q->front;
        lowest_proc = NULL;
        while (tmp != NULL) {
            if ((lowest_proc == NULL || tmp->process->last_used < lowest_proc->last_used) && tmp->process->isInFrame == 1) {
                lowest_proc = tmp->process;
            }
            tmp = tmp->next;
        }
    }
    return lowest_proc;
}