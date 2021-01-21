// Nicola Fletcher - 09/20/20

#include <unistd.h>     // fork(), execvp()
#include <stdio.h>      // printf(), scanf(), setbuf(), perror()
#include <string.h>
#include <stdlib.h>   // malloc()
#include <sys/types.h> // pid_t
#include <sys/wait.h>   // waitpid()
#include <signal.h>     // kill(), SIGTERM, SIGKILL, SIGSTOP, SIGCONT
#include <errno.h>      // errno
#include <readline/readline.h>
#include "pid_list.h"

void bg_cmd(char **args, PList *list);
void bglist_cmd(PList *list);
void check_zombieProcess(PList *list);
void bg_sig(PList *list, char *string_pid, char *sig);
void pstat_cmd(PList *list, char *string_pid);

int main(int argc, char *argv[]){

    int loop = 1;

    //list to store current background processes
    PList *list = (PList*) malloc(sizeof(PList));
    list->head = NULL;

    //continuously prompts user for input until user types 'exit'
    while(loop != 0){


        char *input = NULL;
        char *prompt = "PMan:  > ";

        input = readline(prompt);

        char **args = malloc(sizeof(char*) * (1024 + 1));

        if(args == NULL){
            perror("Malloc failed");
            exit(EXIT_FAILURE);
        }

        char *token = strtok(input, " ");

        //prevent segfault from empty input
        if(token == NULL){
            printf("Please provide input\n");
            continue;
        }

        char *cmd = token;

        int i = 0;

        while(token != NULL){

            args[i] = token;

            token = strtok(NULL, " ");
            i++;
        }
        //add null terminator to args for use in execvp
        args[i] = NULL;
        //exclude cmd from array
        args++;

        //stop loop, end session
        if(strcmp("exit", cmd) == 0){
            loop = 0;
            printf("session ended\n");
            continue;

        }

        else if (strcmp("bg", cmd) == 0) {
            if(args[0] == NULL) {
                printf("Please provide a file\n");
                continue;
            }
            bg_cmd(args, list);
        }
        else if (strcmp("bglist", cmd) == 0) {
            bglist_cmd(list);
        }
        else if (strcmp("bgkill", cmd) == 0) {
            if(args[0] == NULL){
                printf("Please provide a pid\n");
                continue;
            }
            bg_sig(list, args[0], "bgkill");
        }
        else if (strcmp("bgstop", cmd) == 0) {
            if(args[0] == NULL) {
                printf("Please provide a pid\n");
                continue;
            }
            bg_sig(list, args[0], "bgstop");
        }
        else if (strcmp("bgstart", cmd) == 0) {
            if(args[0] == NULL) {
                printf("Please provide a pid\n");
                continue;
            }
            bg_sig(list, args[0], "bgstart");
        }
        else if (strcmp("pstat", cmd) == 0) {
            if(args[0] == NULL) {
                printf("Please provide a pid\n");
                continue;
            }
            pstat_cmd(list, args[0]);
        }
            //invalid command
        else {
            printf("PMan:  > %s: command not found\n", cmd);
        }

        check_zombieProcess(list);

    }

    delete_list(list);
    free(list);

    return 0;
}

//starts a specified background process and adds an entry (with the file name and pid) to the linked list
void bg_cmd(char **args, PList *list) {

    char *file_name = args[0];

    pid_t pid;
    pid = fork();

    //child process
    if(pid == 0) {
        if (execvp(args[0], args) < 0) {
            perror("Error on execvp");
            exit(-1);
        }
        exit(EXIT_SUCCESS);

    }
        //parent process
    else if(pid > 0) {

            add_end(list, pid, file_name);

    }
    else{
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

}

//prints out a list of all the processes currently running in the background
void bglist_cmd(PList *list) {

    check_zombieProcess(list);

    int num = 0;
    Node *curr = list->head;

    printf("PROCESS LIST\n");


    while (curr != NULL) {
        num++;
        printf("%d:   %s\n", curr->pid, curr->file);
        curr = curr->next;
    }
    printf("Total background jobs:  %d\n", num);
}

//sends a signal to change the state of a background process. The signal options are bgkill, bgstop, and bgstart
void bg_sig(PList *list, char *string_pid, char *sig){

    pid_t pid = atoi(string_pid);

    int retVal;

    if(!contains(list, pid)){
        printf("Error: process with the pid %d could not be found\n", pid);
        return;
    }

    if(strcmp("bgkill", sig) == 0){
        retVal = kill(pid, SIGTERM );
        if(retVal == 0){
            printf("User has terminated process with pid %d\n", pid);
        }
        else{
            printf("Failed to terminate process with pid %d\n", pid);
        }
    }
    else if(strcmp("bgstop", sig) == 0){
        retVal = kill(pid, SIGSTOP);
        if(retVal == 0){
            printf("User has temporarily stopped process with pid %d\n", pid);
        }
        else{
            printf("Failed to temporarily stop process with pid %d\n", pid);
        }

    }
    else if(strcmp("bgstart", sig) == 0){
        retVal = kill(pid, SIGCONT);
        if(retVal == 0){
            printf("User has restarted process with pid %d\n", pid);
        }
        else{
            printf("Failed to restart process with pid %d\n", pid);
        }

    }

}
//parses the stat and status files for a specified process, and collects and displays the comm, state,
//utime, stime, rss, voluntary_ctxt_switches, and nonvoluntary_ctxt_switches
void pstat_cmd(PList *list, char *string_pid){

    pid_t pid = atoi(string_pid);

    if(!contains(list, pid)){
        printf("Error: Process with pid %s does not exist\n", string_pid);
        return;
    }

    //information to be collected from the files and displayed
    char comm[256];
    char state;
    float utime;
    float stime;
    long rss;
    char voluntary_ctxt_switches[256];
    char nonvoluntary_ctxt_switches[256];

    //STAT FILE
    char stat_path[256];
    strcpy(stat_path, "/proc/");
    strcat(stat_path, string_pid);
    strcat(stat_path, "/stat");

    FILE * stat_file = fopen(stat_path, "r");
    if(stat_file == NULL){
        printf("Error: could not open stat file for process with pid %s\n", string_pid);
        return;
    }

    if(fscanf(stat_file, "%*d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %f %f %*d %*d %*d %*d %*d %*d %*u %*u %ld"
            , comm, &state, &utime, &stime, &rss) == EOF){

        printf("Error: could not read stat file for process with pid %s\n", string_pid);
    }

    fclose(stat_file);

    //convert utime and stime from clock ticks to seconds
    utime = utime / sysconf(_SC_CLK_TCK);
    stime = stime / sysconf(_SC_CLK_TCK);


    //STATUS FILE
    char status_path[256];
    strcpy(status_path, "/proc/");
    strcat(status_path, string_pid);
    strcat(status_path, "/status");

    FILE * status_file = fopen(status_path, "r");
    if(status_file == NULL){
        printf("Error: could not open status file for process with pid %s\n", string_pid);
        return;
    }

    int count = 1;
    char line[256];

    //parse status file and only retrieve the two ctxt_switch lines
    while(fgets(line, sizeof(line), status_file) != NULL){
        if(count == 54){
            strcpy(voluntary_ctxt_switches, line);
        }
        if(count == 55){
            strcpy(nonvoluntary_ctxt_switches, line);
            break;
        }
        count++;
    }

    printf("STATUS OF PROCESS %s\n", string_pid);

    printf("comm:   %s\n", comm);
    printf("state:   %c\n", state);
    printf("utime:   %.2f seconds\n", utime);
    printf("stime:   %.2f seconds\n", stime);
    printf("rss:   %ld\n", rss);
    printf("%s", voluntary_ctxt_switches);
    printf("%s\n", nonvoluntary_ctxt_switches);


}

//clears the zombie processes from the linked list (used at the end of every loop + at the beginning of bglist)
void check_zombieProcess(PList *list){
    int status;
    int retVal = 0;

    while(1){

        if(list->head == NULL){
            return ;
        }
        retVal = waitpid(-1, &status, WNOHANG);
        if(retVal > 0) {

            printf("Process with pid %d has terminated\n", retVal);

            if(contains(list,retVal)){
                delete_node(list, retVal);
            }

        }
        else if(retVal == 0){
            break;
        }
        else{
            perror("waitpid failed");
            exit(EXIT_FAILURE);
        }
    }
}
