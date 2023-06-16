/*
 * PMan.c
 * 
 * Text file parsing in pstat was re-used from Seng265 assignment 1 in Winter 2021 at the University of Victoria, taught by Hausi Muller and completed by Paul Garewal. 
 * 
 * 
 * 
 */



#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "emalloc.h"

#define MAX_LINE_LEN 2048
#define MAX_PROC 100

// node struct to store our pid information
typedef struct node_t {
    pid_t            pid;
    char*             path;
    struct node_t  *next;
} node_t;


node_t *new_node(pid_t pid, char* path); // create new pid node with path
node_t *add_end(node_t * node, node_t *);// add process to linked list
node_t *deleteNode(node_t *, pid_t pid);// delete process in linked list

node_t* process_list = NULL; // establishment of head of linked list of nodes.

int size = 0; // keep track of size of process list.

/**
 * @brief Function: checkProcess
 * 
 *        Description: Determines if a process id exists in the stored linked list (process_list).
 *                       
 * 
 * @param pid_t pid - pid that we want to check if it exists.
 */

int checkProcess(pid_t pid){

    if(process_list == NULL){
      printf("Process list is currently empty, no processes recorded as running.\n");
      return false;
    }
    
    else{

      while(process_list != NULL){

        if(process_list->pid == pid){

          return true;
        }
        process_list = process_list->next;
      }
    }
    return false;

}
/**
 * @brief Function: printList
 * 
 *        Description: Prints out a linked list of nodes.
 *                       
 * 
 * @param *node - a linked list of nodes
 */

void printList(node_t *node){

	while(node != NULL){

		printf("%d: %s\n", node->pid, node->path);

		node = node->next;

	}
  printf("Total background jobs : %d\n", size);
}
/**
 * @brief Function: BG
 * 
 *        Description: Starts a process in the background, calling fork().
 *                       
 * 
 * @param char** command - command prompt of exectable to start in the background. Must be a C executable file. 
 */
void func_BG(char **cmd){
  	//Your code here;
    
    pid_t pid = fork();
    int status;

    printf("pid = %ld\n", (long) pid);

    // we have a child process
    if(pid == 0){
        printf("...\n");

        char* cmd1 = cmd[1];
        
        execvp(cmd1, &cmd[1]);

        // we won't reach here unless something goes wrong
        printf("Could not find and run file requested\n");
        exit(1);
    }
    //parent process
    else if(pid > 0){

        //int opts = WNOHANG | __W_CONTINUED | WUNTRACED;

        char* cmd1 = cmd[1];

        int retVal;
       
        char buffer[500];

        char *path = realpath(cmd1, buffer);

        node_t* new = new_node(pid, path);

        process_list = add_end(process_list, new);

        ++size;

        retVal = waitpid(pid, &status, WNOHANG);
        
        if(retVal == -1){
          perror("Fail at waitpid");
          exit(EXIT_FAILURE);
        }
        
        sleep(1);
    }
    else{

      //failure to create process
      perror("\nFailed to create a new process.\n");
    }
    
}

/**
 * @brief Function: BGlist
 * 
 *        Description: Displays a list of all the programs currently executing in the background.
 *                       
 * 
 * @param node_t - linked list of processes to be displayed.
 */
    
void func_BGlist(char **cmd){
	//Your code here;
  
  //if our stored list is not empty, print it out
  if(process_list != NULL){

    printf("...\n");

    printList(process_list);

    sleep(1);

  }
    // if null, we know there are no processes in list, therefore non running or paused
    else{

      printf("There are no processes currently running.\n");

    }

}

/**
 * @brief Function: BGkill
 * 
 *        Description: Send the TERM signal to terminate the pid process.
 *                       
 * 
 * @param str_pid - string argument passed of the pid to kill.
 */

void func_BGkill(char *str_pid){
	

  pid_t pid = strtol(str_pid, NULL, 10);
  
  if(checkProcess(pid) == false){

    printf("Process does not exist\n");

    return;
  }

  else{

    int killer = kill(pid, SIGTERM);

    if(killer == 0){

      printf("Process: %d was terminated.\n", pid);

      deleteNode(process_list, pid);
      --size;

      sleep(1);

    }
    else{

      printf("FAILURE TO KILL %d\n", pid);

    }
  }

}
/**
 * @brief Function: BGstop
 * 
 *        Description: Send the STOP signal to temporarily stop the pid process.
 *                       
 * 
 * @param str_pid - string argument passed of the pid to stop
 */

void func_BGstop(char *str_pid){
	//Your code here

  pid_t pid = strtol(str_pid, NULL, 10);

  // if process pid doesn't exist we exit
  if(checkProcess(pid) == false){

    printf("Process does not exist\n");

    return;
  }

  int stop = kill(pid, SIGSTOP); // stop the process

  // if successful print what we have done
  if(stop == 0){

    printf("Process: %d was stopped.\n", pid);

    sleep(1);

  }
}
/**
 * @brief Function: BGstart
 * 
 *        Description: Send the CONT signal to re-start the pid process (Which has been previously stopped).
 *                       
 * 
 * @param str_pid - string argument passed of the pid to start.
 */

void func_BGstart(char *str_pid){
	

  pid_t pid = strtol(str_pid, NULL, 10); // get pid into pid_t form

  // if checkprocess returns false, pid is not running.
  if(checkProcess(pid) == false){

    printf("Process does not exist\n");

    return;
  }

  int cont = kill(pid, SIGCONT); // start the pid

  // if cont == 0 out kill(continue) function was a success
  if(cont == 0){

    printf("Process: %d was started.\n", pid);

    sleep(1);
  }else{
    printf("SIGCONT failed in kill() for process.");
  }

}
/**
 * @brief Function: pstat
 * 
 *        Description: Prints information related to the process pid, such as name of executable, state, utime, stime, rss and context switches.
 *                       
 *        Help sourced from: https://www.geeksforgeeks.org/c-program-to-read-contents-of-whole-file/  
                      and https://cboard.cprogramming.com/c-programming/137188-obtain-space-separated-values-text-file.htmlto parse txt file and from Seng265 Assignment 1 in Winter 2021.

 * @param str_pid - string argument passed in of the pid
 */

void func_pstat(char * str_pid){
	
  pid_t pid = strtol(str_pid, NULL, 10); // get pid into pid_t form

  if(checkProcess(pid) == false){
    printf("Error: Process %d does not exist", pid);
  }
  
  else{

        char stat[MAX_LINE_LEN]; // to hold the proc file path
        char status[MAX_LINE_LEN]; // to hold the proc file path

        sprintf(stat, "/proc/%d/stat", pid); // copy file path into stat so we can open
        sprintf(status, "/proc/%d/status", pid); // copy file path into status so we can open

        char *statInfo[MAX_LINE_LEN]; // to hold the information we will obtain from stat

        FILE* statFile = fopen(stat, "r"); // open /proc/[pid]/stat file

        /* If file is null, we return an error message and a 0 int to stop the file.*/
        if(statFile == NULL){ 

            printf("Proc stat file is unreadable.");
            return;

        }else{
          
         int i = 0; // counter for our fgets file

         char singleLine[MAX_LINE_LEN]; /* initialize singleLine to hold one line in the file */

         while(fgets(singleLine, MAX_LINE_LEN, statFile) != NULL){

                char* state;

                state = strtok(singleLine, " ");

                statInfo[i] = state;

                i++;
                
                while(state != NULL){

                  statInfo[i] = state;

                  state = strtok(NULL, " ");
                  
                  ++i;
                } 
        }
        fclose(statFile);

      }
            // turn the time strings and rss into long unsigned ints
            float utime = strtol(statInfo[13], NULL, 30) / sysconf(_SC_CLK_TCK);
            float stime = strtol(statInfo[14], NULL, 30) / sysconf(_SC_CLK_TCK);

            long int rss = strtol(statInfo[24], NULL, 10);// get the rss in long digit format

            //print what we have found
            printf("...\n");
            printf("comm:\t%s\n", statInfo[2]);
            printf("state:\t%s\n", statInfo[3]);
            printf("utime:\t%f\n", utime);
            printf("stime:\t%f\n", stime);
            printf("rss:\t%ld\n", rss);

            // get status line
            FILE* statusFile = fopen(status, "r");

            // if null we get out
            if(statusFile == NULL){
              printf("Proc status file is unreadable\n");
              return;

            }else{
              
                char line[256];

                while(fgets(line, 256, statusFile)){
                  
                  // compare lines, if we find the context switches, print them out
                if (strncmp(line, "voluntary_ctxt_switches", 22) == 0){
                printf("%s", line );
                }
                else if(strncmp(line, "nonvoluntary_ctxt_switches", 25) == 0){
                  printf("%s", line);
                }
          }


      }fclose(statusFile);


            sleep(1);
        
      }

  }
 
int main(){

    char user_input_str[50];

    for(;;) {
      
      printf("PMan: > ");

      fgets(user_input_str, 50, stdin);

      char * ptr = strtok(user_input_str, " \n");

      if(ptr == NULL){
        continue;
      }

      char * lst[50];
      int index = 0;
      lst[index] = ptr;
      index++;

      while(ptr != NULL){
        ptr = strtok(NULL, " \n");
        lst[index]=ptr;
        index++;
      }
      
      if (strcmp("bg",lst[0]) == 0){
        func_BG(lst);

      } else if (strcmp("bglist",lst[0]) == 0) {
        func_BGlist(lst);

      } else if (strcmp("bgkill",lst[0]) == 0) {
        func_BGkill(lst[1]);

      } else if (strcmp("bgstop",lst[0]) == 0) {
        func_BGstop(lst[1]);

      } else if (strcmp("bgstart",lst[0]) == 0) {
        func_BGstart(lst[1]);

      } else if (strcmp("pstat",lst[0]) == 0) {
        func_pstat(lst[1]);

      } else if (strcmp("q",lst[0]) == 0) {

        printf("Bye Bye \n");
        exit(0);

      } else {
        printf("Invalid input\n");
        printf("Inputs must be a bg type command or pstat followed by pid\n");
      }
    }

  return 0;
}

