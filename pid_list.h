#ifndef PID_LIST_H

#define PID_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


typedef struct Node{
    pid_t pid;
    char *file;
    struct Node* next;
} Node;

typedef struct{
    Node* head;
} PList;

void add_end(PList *L, pid_t new_pid, char *file_name);
void delete_node(PList* L, pid_t pid);
void print_list(PList* L);
void delete_list(PList* L);
int contains(PList* L, pid_t pid);


#endif //"PID_LIST_H"