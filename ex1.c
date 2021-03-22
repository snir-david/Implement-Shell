#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct jobRec {
    char jobName[100];
    char *status;
    int background;
    pid_t pid;
};

int splitString(char **jobs, char *input) {
    char *jobToken;
    int commandLen;
    jobToken = strtok(input, " ");
    for (int i = 0; jobToken != NULL; i++) {
        jobs[i] = jobToken;
//        printf("%s\n", jobs[i]);
        jobToken = strtok(NULL, " ");
        commandLen = i;
    }
    return commandLen;
}

void jobs(struct jobRec history[100], int commandNumber) {
    for (int i = 0; i <= commandNumber; ++i) {
        if (history[i].background && strcmp(history[i].status, "RUNNING") == 0) {
            printf("%s\n", history[i].jobName);
        }
    }
}

void historyComm(struct jobRec history[100], int commandNumber) {
    //printing all jobs and status
    for (int i = 0; i <= commandNumber; ++i) {
        printf("%s %s\n", history[i].jobName, history[i].status);
    }
}

void cd() {}

void exitShell() {
    exit(0);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

int main() {
    //initialize variables
    int commandNumber = 0;
    char *currentJob[100];
    char *builtInFunc[] = {"jobs", "history", "cd", "exit"};
    int status, commandLen;
    pid_t pid;
    struct jobRec history[100];

    //shell loop
    while (1) {
        int builtIn = 0;
        char input[100];
        //printing promp
        printf("$ ");
        fflush(stdout);
        //getting command from user
        scanf(" %[^\n]s", input);
        //splitting command into tokens and getting back input len
        commandLen = splitString(currentJob, input);
        //setting jobRecord struct
        strcpy(history[commandNumber].jobName, input);
        history[commandNumber].status = "RUNNING";
        history[commandNumber].background = 0;
        //insert command into history
        //checking if it is one of the built in functions
        for (int j = 0; j < 4; ++j) {
            if (strcmp(currentJob[0], builtInFunc[j]) == 0) {
                builtIn = 1;
                switch (j) {
                    //jumping to case according to the j that entered to the condition
                    case 0:
                        jobs(history, commandNumber);
                        break;
                    case 1:
                        historyComm(history, commandNumber);
                        break;
                    case 2:
                        cd();
                        break;
                    case 3:
                        exitShell();
                        break;
                    default:
                        break;
                }
            }
        }
        //if it's not built-in function, call execvp
        if (builtIn == 0) {
            pid = fork();
            //checking if fork failed
            if (pid < 0) {
                printf("fork failed\n");
                fflush(stdout);
            }
            if (pid == 0) {
                //getting pid to struct
                history[commandNumber].pid = getpid();
                //child - adding null to args for execvp
                if (strcmp(currentJob[commandLen], "&") == 0) {
                    currentJob[commandLen] = NULL;
                } else {
                    currentJob[commandLen + 1] = NULL;
                }
                int execStat = execvp(currentJob[0], currentJob);
                //checking if execvp failed
                if (execStat == -1) {
                    printf("exec failed\n");
                    fflush(stdout);
                }
            } else {
                //parent
                //checking if & last char in user input - if it isn't, waiting for child process
                if (strcmp(currentJob[commandLen], "&") != 0) {
                    int waited = wait(&status);
                    printf(" waiting\n");
                    fflush(stdout);
                } else {
                    printf("not waiting\n");
                    fflush(stdout);
                    history[commandNumber].background = 1;
                    sleep(1);
                }
            }
        }
        history[commandNumber].status = "DONE";
        commandNumber++;
    }
    return 0;
}

#pragma clang diagnostic pop

