#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//defining struct for job Record
struct jobRec {
    char jobName[100];
    char *status;
    int background;
    pid_t pid;
};

//getting user input and splitting into tokens using whitespace as a delim
int splitString(char **currentJob, char *input) {
    char *jobToken;
    int commandLen;
    jobToken = strtok(input, " ");
    for (int i = 0; jobToken != NULL; i++) {
        currentJob[i] = jobToken;
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

void cdCheck(char *path, char prevPath[100]) {
    char currentDir[100];
    //checking special chars in cd command (~ or -)
    if (strcmp(path, "-") == 0) {
        //setting previous path as path for chdir command
        strcpy(path, prevPath);
    } else if (strstr(path, "~")) { //checking if string contain ~
        //checking if this the only char
        if (strcmp(path, "~") == 0) {
            //changing path to home dir
            strcpy(path,getenv("HOME"));
        } else {
            if (getcwd(currentDir, sizeof(currentDir)) == NULL) {
                printf("An error occurred\n");
            } else {
                //concreting home dir to path
                strcpy(currentDir,getenv("HOME"));
                strcat(currentDir, ++path);
                strcpy(--path, currentDir);
//                printf("%s\n", path);
            }
        }
    }
}

void cd(char *path, int commandLen, char prevPath[100]) {
    char currentDir[100];
    if (getcwd(currentDir, sizeof(currentDir)) == NULL) {
        printf("An error occurred\n");
    } else {
        if (commandLen > 1) {
            printf("Too many arguments\n");
        } else {
            cdCheck(path, prevPath);
            int cd = chdir(path);
            if (cd == -1) {
                printf("chdir failed\n");
            } else {
                strcpy(prevPath, currentDir);
//                printf("%s\n", prevPath);
            }
        }
    }
}


void exitShell() {
    exit(0);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

int main() {
    //initialize variables
    int commandNumber = 0, status, commandLen;
    char *currentJob[100], prevPath[100], *builtInFunc[] = {"jobs", "history", "cd", "exit"};
    struct jobRec history[100];
    pid_t pid;

    //shell loop
    while (1) {
        int builtIn = 0;
        char input[100];
        //printing promp
        printf("$ ");
        fflush(stdout);
        //getting command from user
        //TODO - dealing with "empty" command
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
                        cd(currentJob[1], commandLen, prevPath);
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
//                    printf(" waiting\n");
//                    fflush(stdout);
                } else {
//                    printf("not waiting\n");
//                    fflush(stdout);
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

