#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct jobRec {
    char *jobName;
    char *status;
};

int splitString(char *jobs, char *input) {
    char *jobToken;
    int commandLen;
    jobToken = strtok(input, " ");
    for (int i = 0; jobToken != NULL; i++) {
        jobs[i] = jobToken;
        printf("%s\n", jobs[i]);
        jobToken = strtok(NULL, " ");
        commandLen = i;
    }
    return commandLen;
}

char *jobs() {

}


int main() {
    int commandNumber = 0;
    char *jobs[100], *history[100], *path[100], input[100];
    char *builtInFunc[] = {"jobs", "history", "cd", "exit"};
    int stopLoop = 0, status, commandLen;
    pid_t pid;
    while (!stopLoop) {
        int builtIn = 0;
        printf("$ ");
        fflush(stdout);
        //getting command from user
        scanf("%[^\n]s", input);
        //splitting command into tokens and getting back input len
        commandLen = splitString(jobs, input);
        //checking if it is one of the built in functions
        for (int j = 0; j < 4; ++j) {
            if (strcmp(jobs[0], builtInFunc[j]) == 0) {
                builtIn = 1;
                if (j == 3) {
                    stopLoop = 1;
                    break;
                }
            }
        }
        if (builtIn == 0) {
            pid = fork();
            if (pid == 0) {
                //child
                int v = execvp(jobs[0], jobs);
            } else {
                //parent
                if (strcmp(jobs[commandLen], "&") != 0) {
                    int wait = waitpid(pid, &status, WNOHANG);
                }
            }
        }
    }
    return 0;
}

