/* pid_list.c

 Nicola Fletcher - 09/26/2020


*/

#include "pid_list.h"


void add_end(PList *L, pid_t new_pid, char *file_name){

    Node* new_node = (Node*) malloc(sizeof(Node));
    if(new_node == NULL){
        perror("Malloc failed");
        return;
    }
    new_node->next = NULL;
    new_node->pid = new_pid;
    new_node->file = file_name;

    if (L->head == NULL){
        L->head = new_node;
    }else{
        Node* curr = L->head;
        while(curr->next != NULL){
            curr = curr->next;
        }

        curr->next = new_node;
    }

}


//delete specified node from the provided list
void delete_node(PList* L, pid_t pid){
    Node* curr = L->head;

    //node to be deleted is head node
    if(curr->pid == pid){
        //if its the only node in the list
        if(curr->next == NULL){
            L->head = NULL;
            free(curr);
        }
        else{
            L->head = curr->next;
            curr->next = NULL;
            free(curr);
        }

    }

    else{
        //find previous node
        while(curr->next != NULL && curr->next->pid != pid){
            curr = curr->next;
        }
        //node not found
        if(curr->next == NULL){
            printf("The node to be deleted is already not in the list");
            return;
        }

        curr->next = curr->next->next;
        free(curr);
    }


}
void delete_list(PList* L){

    Node* curr = L->head;
    Node* del_node = curr;

    while(curr != NULL){

        Node* del_node = curr;
        curr = curr->next;
        free(del_node);
    }

}

int contains(PList* L, pid_t pid){
    Node* curr = L->head;

    while(curr != NULL){

        if(curr->pid == pid){
            return 1;
        }
        curr = curr->next;
    }
    return 0;

}

void print_list(PList* L){

    Node* curr = L->head;
    while(curr != NULL){
        printf("%d %s\n", curr->pid, curr->file);
        curr = curr->next;
    }
    printf("\n");
}



