// Snir David Nahari 205686538

#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//defining struct for job Record
struct jobRec {
    char jobName[100];
    char jobFullName[100];
    char *status;
    int background;
    pid_t childPid;
};

//echo function that breaking down for echo command
int echo(char **currentJob, char *input) {
    char *dst[100], *jobToken;
    int i = 0, j = 0, k = 0, commandLen = 0;
    //breaking input into tokens
    jobToken = strtok(input, " ");
    for (; jobToken != NULL; i++) {
        currentJob[i] = jobToken;
        jobToken = strtok(NULL, "\"");
        commandLen = i;
    }
    //checking for whitespaces in tokens
    for (; k < i; k++) {
        dst[j] = currentJob[k];
        //checking if cell is whitespace
        if (strcmp(currentJob[k], " ") != 0) {
            j++;
        }
        commandLen = j - 1;
    }
    //putting input without whitespaces
    for (j = 0; j < i; j++) {
        currentJob[j] = dst[j];
    }
    return commandLen;
}

//getting user input and splitting into tokens using whitespace as a delim
int splitString(char **currentJob, char *input) {
    char *jobToken, inputCopy[100];
    int commandLen, i = 0;
    strcpy(inputCopy, input);
    //getting command name
    jobToken = strtok(inputCopy, " ");
    currentJob[0] = jobToken;
    //checking if it echo command
    if (strcmp(currentJob[0], "echo") == 0) {
        commandLen = echo(currentJob, input);
    } else {
        //regular command breaking down
        jobToken = strtok(input, " ");
        for (; jobToken != NULL; i++) {
            currentJob[i] = jobToken;
            jobToken = strtok(NULL, " ");
            commandLen = i;
        }
    }
    return commandLen;
}

//jobs built-in command implantation
void jobs(struct jobRec history[100], int commandNumber) {
    int i = 0;
    for (; i < commandNumber; ++i) {
        //checking which process is running using pid and printing them
        if (waitpid(history[i].childPid, NULL, WNOHANG) != 0 || history[i].childPid == 0) {
            history[i].status = "DONE";
        } else {
            history[i].status = "RUNNING";
            //checking if this process is in background
            if (history[i].background == 1) {
                printf("%s\n", history[i].jobFullName);
                fflush(stdout);
            }
        }
    }
}

//history built-in command implantation
void historyComm(struct jobRec history[100], int commandNumber) {
    //printing all jobs and status
    int i = 0;
    for (; i < commandNumber; ++i) {
        //checking which process is running using pid and printing them
        if (waitpid(history[i].childPid, NULL, WNOHANG) != 0 || history[i].childPid == 0) {
            history[i].status = "DONE";
        } else {
            history[i].status = "RUNNING";
        }
        printf("%s %s\n", history[i].jobFullName, history[i].status);
        fflush(stdout);
    }
    //printing last job - history
    printf("%s %s\n", history[commandNumber].jobFullName, history[commandNumber].status);
    fflush(stdout);
}

//cd built-in command help function for dealing special cases
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
            strcpy(path, getenv("HOME"));
        } else {
            if (getcwd(currentDir, sizeof(currentDir)) == NULL) {
                printf("An error occurred\n");
                fflush(stdout);
            } else {
                //concreting home dir to path
                strcpy(currentDir, getenv("HOME"));
                strcat(currentDir, ++path);
                strcpy(--path, currentDir);
            }
        }
    }
}

//cd built-in command implantation
void cd(char *path, int commandLen, char prevPath[100]) {
    char currentDir[100];
    if (getcwd(currentDir, sizeof(currentDir)) == NULL) {
        printf("An error occurred\n");
        fflush(stdout);
    } else {
        if(commandLen == 0) {
            int cd = chdir(getenv("HOME"));
            if (cd == -1) {
                printf("chdir failed\n");
                fflush(stdout);
            } else {
                strcpy(prevPath, currentDir);
            }
        } else if (commandLen > 1) {
            printf("Too many arguments\n");
            fflush(stdout);
        } else {
            cdCheck(path, prevPath);
            int cd = chdir(path);
            if (cd == -1) {
                printf("chdir failed\n");
                fflush(stdout);
            } else {
                strcpy(prevPath, currentDir);
            }
        }
    }
}

//exit built-in command implantation
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
        int builtIn = 0, i = 0, j = 0;
        char input[100], copyInput[100], specialInput[100];

        //printing promp
        printf("$ ");
        fflush(stdout);
        //getting command from user
        scanf(" %[^\n]s", input);

        //making copy of input for later uses
        strcpy(copyInput, input);

        strcpy(history[commandNumber].jobFullName, input);
        //splitting command into tokens and getting back input len
        commandLen = splitString(currentJob, input);

        //setting jobRecord struct and insert command into history
        strcpy(history[commandNumber].jobName, input);
        history[commandNumber].status = "RUNNING";
        history[commandNumber].background = 0;
        history[commandNumber].childPid = 0;

        //checking if it is one of the built in functions
        for (; j < 4; ++j) {
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
            //getting childPid to struct
            history[commandNumber].childPid = pid;
            if (pid == 0) {
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
                    exitShell();
                }
            } else {
                //parent
                //checking if & last char in user input - if it isn't, waiting for child process
                if (strcmp(currentJob[commandLen], "&") != 0) {
                    int waited = waitpid(pid, NULL, 0);
                    //if waited failed
                    if (waited == -1) {
                        printf("An error occurred\n");
                        fflush(stdout);
                    }
                } else {
                    //replacing full name of history record without "&" char
                    char fullName[100] = {0};
                    for (i = 0; i < commandLen; i++) {
                        strcat(fullName, currentJob[i]);
                        if ( i < commandLen -1) {
                            strcat(fullName, " ");
                        }
                    }
                    strcpy(history[commandNumber].jobFullName, fullName);
                    history[commandNumber].background = 1;
                }
            }
        }
        commandNumber++;
    }
    return 0;
}

#pragma clang diagnostic pop

