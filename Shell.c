#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>

// Function to handle the SIGINT (Ctrl+C) signal
void signal_handler(int signal) {
    // Check if the signal is SIGINT
    if (signal == SIGINT) {
        printf("Got the CTRL+C command, exiting from the shell\n");
        // Open history.txt for reading
        FILE* file = fopen("history.txt", "r");
        if (file == NULL) {
            printf("Error in opening the file history.txt\n");
            exit(1);
        }
        char str[1000];
        // Read and print the content of history.txt
        while (fgets(str, sizeof(str), file)) {
            printf("%s\n", str);
        }
        // Check for errors during file reading
        if (ferror(file)) {
            printf("Error in reading from the file history.txt\n");
            exit(1);
        }
        // Close history.txt
        if (fclose(file) != 0) {
            printf("Error in closing the file history.txt\n");
            exit(1);
        }
        // Remove the history.txt file
        int r = remove("history.txt");
        if (r != 0) {
            printf("Error in removing the file history.txt\n");
            exit(1);
        }
    }
    // Exit the program
    exit(0);
}

// Function to execute a block of commands
int work(char* command[][100], int commands, bool background) {
    // Store the background flag
    bool b = background;
    int ssid = 0;
    // Fork a new process
    int rc = fork();
    if (rc < 0) {
        printf("error in fork in the work function ");
        exit(1);
    } else if (rc == 0) {
        if (b) {
            // Set the child process as a session leader
            ssid = setsid();
        }
        int fd[2];
        int children[commands - 1];
        int check = 0;
        int i = 0;
        // Loop through the commands
        for (; i < commands - 1; i++) {
            if (pipe(fd) == -1) {
                printf("error in pipe");
                exit(1);
            }
            children[i] = fork();
            if (children[i] == 0) {
                if (i != 0) {
                    // Redirect input for commands other than the first
                    dup2(check, 0);
                    close(check);
                }
                // Redirect output to the pipe
                if (dup2(fd[1], 1) == -1) {
                    printf("Error in dup2\n");
                    exit(1);
                }
                close(fd[1]);
                // Execute the command
                execvp(command[i][0], command[i]);
                printf("Error in execvp inside if(children[i]!=0)");
                exit(1);
            } else if (children[i] < 0) {
                printf("Error in fork");
                exit(1);
            }
            close(check);
            close(fd[1]);
            check = fd[0];
        }
        if (check != 0) {
            // Redirect input for the last command
            if (dup2(check, 0) == -1) {
                printf("Error in dup2\n");
                exit(1);
            }
            close(check);
        }
        // Execute the last command
        execvp(command[i][0], command[i]);
        printf("Error in execvp outside the loop");
        exit(1);
    } else {
        if (!b) {
            int wc;
            do {
                // Wait for the child process to finish
                wc = waitpid(rc, NULL, 0);
            } while (wc == -1 && errno == EINTR);
            if (wc == -1) {
                printf("Error in wait\n");
                exit(1);
            }
            return rc;
        } else {
            background = false;
            return ssid;
        }
    }
    // Register the SIGINT handler
    signal(SIGINT, signal_handler);
}

// Function to launch command execution
int launch(char* command[][100], int commands, bool background) {
    signal(SIGINT, signal_handler);
    int status;
    status = work(command, commands, background);
    return status;
}
// Function to execute a block of commands and record execution history
int execution_block(char* arr[][100], int commands, int flag, bool background) {
    int status;
    if (flag == 1) {
        goto execution;
    }
    time_t start;
    struct tm* t1;
    char start_time[9];
    time(&start);
    t1 = localtime(&start);
    strftime(start_time, sizeof(start_time), "%T", t1);
    time_t end;
    struct tm* t2;
    char end_time[9];
    struct timeval st;
    struct timeval en;
    gettimeofday(&st, NULL);
    double start_milli = (double)st.tv_sec * 1000 + (double)st.tv_usec / 1000;

execution:
    status = launch(arr, commands, background);
    if (flag == 1) {
        goto end;
    }
    time(&end);
    t2 = localtime(&end);
    strftime(end_time, sizeof(end_time), "%T", t2);
    gettimeofday(&en, NULL);
    double end_milli = (double)en.tv_sec * 1000 + (double)en.tv_usec / 1000;
    double duration = end_milli - start_milli;

    FILE* file = fopen("history.txt", "a");
    if (file == NULL) {
        printf("Error in opening the file history.txt");
        exit(1);
    }
    fprintf(file, "Pid:%d  ", status);
    fprintf(file, "Start time is: %s  ", start_time);
    fprintf(file, "end time is: %s  ", end_time);
    fprintf(file, "duration %f milliseconds  ", duration);
    fprintf(file, "The command is: ");
    for (int k = 0; k < commands; k++) {
        for (int i = 0; arr[k][i] != NULL; i++) {
            fprintf(file, "%s ", arr[k][i]);
        }
        if (k < commands - 1) {
            fprintf(file, "|");
        }
    }
    if (background) {
        fprintf(file, "&");
    }
    fprintf(file, "\n");
    if (fclose(file) != 0) {
        printf("Error in closing the history.txt file\n");
        exit(1);
    }
    // Register the SIGINT handler
    signal(SIGINT, signal_handler);
end:
    return status;
}

// Function to execute a script
void executing_script(char* str) {
    FILE* file1 = fopen(str, "r");
    if (file1 == NULL) {
        printf("Error in opening the file");
        exit(1);
    }
    char lines[256];
    int commands = 0;
    int status;

    time_t start;
    struct tm* t1;
    char start_time[9];
    time(&start);
    t1 = localtime(&start);
    strftime(start_time, sizeof(start_time), "%T", t1);

    bool background = false;
    struct timeval st;
    gettimeofday(&st, NULL);
    double start_milli = (double)st.tv_sec * 1000 + (double)st.tv_usec / 1000;

    while (fgets(lines, sizeof(lines), file1)) {
        commands = 0;
        char* arr[100][100];
        char* str = strtok(lines, " \n");
        int i = 0;
        int j = 0;
        while (str != NULL) {
            if (*str == '|') {
                arr[i][j] = NULL;
                commands++;
                i++;
                j = 0;
                str = strtok(NULL, " \n");
                continue;
            }
            if (*str == '&') {
                background = true;
                break;
            }
            arr[i][j] = strdup(str);
            str = strtok(NULL, " \n");
            j++;
        }
        commands++;
        arr[i][j] = NULL;
        int flag = 1;
        status = execution_block(arr, commands, flag, background);
    }

    if (ferror(file1)) {
        printf("Error in reading from the file");
        exit(1);
    }

    if (fclose(file1) != 0) {
        printf("Error in closing the file\n");
        exit(1);
    };
    time_t end;
    struct tm* t2;
    char end_time[9];
    struct timeval en;

    time(&end);
    t2 = localtime(&end);
    strftime(end_time, sizeof(end_time), "%T", t2);

    gettimeofday(&en, NULL);
    double end_milli = (double)en.tv_sec * 1000 + (double)en.tv_usec / 1000;

    double duration = end_milli - start_milli;

    FILE* file = fopen("history.txt", "a");
    if (file == NULL) {
        printf("Error in opening the file");
        exit(1);
    }
    fprintf(file, "Pid:%d  ", status);
    fprintf(file, "Start time is: %s  ", start_time);
    fprintf(file, "end time is: %s  ", end_time);
    fprintf(file, "duration %f milliseconds  ", duration);
    fprintf(file, "The command is: ");
    fprintf(file, "1 %s", str);
    fprintf(file, "\n");
    if (fclose(file) != 0) {
        printf("Error in closing the file\n");
        exit(1);
    }
    // Register the SIGINT handler
    signal(SIGINT, signal_handler);
}


int main() {
    char arr1[100];
    char* arr[100][100];

    FILE* file = fopen("history.txt", "w");
    if (file == NULL) {
        printf("Error in opening the file history.txt\n");
    }
    if (fclose(file) != 0) {
        printf("Error in closing the file history.txt\n");
    };

    do {
        signal(SIGINT, signal_handler);
        printf("Aahan@Aahan:~$ ");
        bool background = false;
        int commands = 0;
        if (fgets(arr1, sizeof(arr1), stdin) != NULL) {
            if (arr1[0] == '\n') {
                continue;
            }
            if (arr1[0] == '1') {
                char* str = strtok(arr1, " \n");
                str = strtok(NULL, " \n");
                printf(str);
                printf("\n");
                executing_script(str);
                continue;
            }
            char* str = strtok(arr1, " \n");
            int i = 0;
            int j = 0;
            while (str != NULL) {
                if (*str == '|') {
                    arr[i][j] = NULL;
                    commands++;
                    i++;
                    j = 0;
                    str = strtok(NULL, " \n");
                    continue;
                }
                if (*str == '&') {
                    background = true;
                    break;
                }
                arr[i][j] = strdup(str);
                str = strtok(NULL, " \n");
                j++;
            }
            commands++;
            arr[i][j] = NULL;
        } else {
            printf("Error in input from stdin");
            continue;
        }
        int flag = 0;
        execution_block(arr, commands, flag, background);
        signal(SIGINT, signal_handler);
    } while (1);
    signal(SIGINT, signal_handler);
    return 0;
}




