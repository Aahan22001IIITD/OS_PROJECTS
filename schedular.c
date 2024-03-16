    
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

int demon_id;
int fd11;
void* shm11;
size_t shm_size11;
char shm_name11[100];
sem_t mutex11;

//struct of queue storing the processes
struct MyQueue
{
    struct pdt_node *front;
    struct pdt_node *rear;
};

//A node for a process
struct pdt_node
{
    int pid;
    char cmd[100];
    char start_time[9];
    char end_time[9];
    double start_milli;
    double waiting_time;
    int priority;
    double duration;
    int number_of_times_got_cpu;
    int state; // 1 ready , 2 running , 3 completed
    struct pdt_node *next_process;
};

//dequeue method for a process queue
struct pdt_node *dequeue(struct MyQueue *queue)
{
    if (queue->front == NULL)
    {
        return NULL; // Queue is empty
    }
    struct pdt_node *temp = queue->front;
    queue->front = queue->front->next_process;
    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }
    return temp;
}

//used for shared memory
typedef struct shm_t{
    struct MyQueue_CPU* q_cpu;
    struct MyQueue* q;
    struct MyQueue_CPU* q_running_processes;
    sem_t mutex;
    int nprocs;
    int NCPU;
    int n;
    char arr[10][100];
    int arr1[10];
    struct pdt_node* process_;
    int demon_id;
    int parent_pid;
    int keeping_count;
    bool flag;
}shm_t;



//for the cleanup of the shared memory
void clean_up(int fd,void* shm,size_t shm_size,char* shm_name) {
    if (munmap(shm,shm_size)==-1) {
        printf("munmap failed\n");
        exit(1);
    }

    if (close(fd)==-1) {
        printf("error in closing\n");
        exit(1);
    }

    if (shm_unlink(shm_name)==-1) {
        printf("error in shm_unlink\n");
        exit(1);
    }


}

// Function to handle the SIGINT (Ctrl+C) signal
void signal_handler(int signal)
{
    // Check if the signal is SIGINT
    if (signal == SIGINT)
    {
        printf("Got the CTRL+C command, exiting from the shell\n");
        // Open history.txt for reading
        FILE *file = fopen("history.txt", "r");
        if (file == NULL)
        {
            printf("Error in opening the file history.txt\n");
            exit(1);
        }
        char str[1000];
        // Read and print the content of history.txt
        while (fgets(str, sizeof(str), file))
        {
            printf("%s\n", str);
        }
        // Check for errors during file reading
        if (ferror(file))
        {
            printf("Error in reading from the file history.txt\n");
            exit(1);
        }
        // Close history.txt
        if (fclose(file) != 0)
        {
            printf("Error in closing the file history.txt\n");
            exit(1);
        }
        // Remove the history.txt file
        kill(demon_id,SIGTERM);
        clean_up(fd11,shm11,shm_size11,shm_name11);
        sem_destroy(&mutex11);
        int r = remove("history.txt");
        if (r != 0)
        {
            // printf("Error in removing the file history.txt\n");
            // exit(1);
        }
    } 
    // Exit the program
    exit(0);
    //return true;
}

// Function to execute a block of commands
int work(char *command[][100], int commands, bool background)
{
    // Store the background flag
    bool b = background;
    int ssid = 0;
    // Fork a new process
    int rc = fork();
    if (rc < 0)
    {
        printf("error in fork in the work function\n");
        exit(1);
    }
    else if (rc == 0)
    {
        if (b)
        {
            // Set the child process as a session leader
            ssid = setsid();
        }
        int fd[2];
        int children[commands - 1];
        int check = 0;
        int i = 0;
        // Loop through the commands
        for (; i < commands - 1; i++)
        {
            if (pipe(fd) == -1)
            {
                printf("error in pipe\n");
                exit(1);
            }
            children[i] = fork();
            if (children[i] == 0)
            {
                if (i != 0)
                {
                    // Redirect input for commands other than the first
                    dup2(check, 0);
                    close(check);
                }
                // Redirect output to the pipe
                if (dup2(fd[1], 1) == -1)
                {
                    printf("Error in dup2\n");
                    exit(1);
                }
                close(fd[1]);
                // Execute the command
                execvp(command[i][0], command[i]);
                printf("Error in execvp inside if(children[i]!=0)\n");
                exit(1);
            }
            else if (children[i] < 0)
            {
                printf("Error in fork\n");
                exit(1);
            }
            close(check);
            close(fd[1]);
            check = fd[0];
        }
        if (check != 0)
        {
            // Redirect input for the last command
            if (dup2(check, 0) == -1)
            {
                printf("Error in dup2\n");
                exit(1);
            }
            close(check);
        }
        // Execute the last command
        execvp(command[i][0], command[i]);
        // printf("Please give a valid command.\n");
        exit(1);
    }
    else
    {
        if (!b)
        {
            int wc;
            do
            {
                // Wait for the child process to finish
                wc = waitpid(rc, NULL, 0);
            } while (wc == -1 && errno == EINTR);
            if (wc == -1)
            {
                printf("Error in wait\n");
                exit(1);
            }
            return rc;
        }
        else
        {
            background = false;
            return ssid;
        }
    }
    // Register the SIGINT handler
    signal(SIGINT, signal_handler);
}
// Function to launch command execution
int launch(char *command[][100], int commands, bool background)
{
    signal(SIGINT, signal_handler);
    int status;
    status = work(command, commands, background);
    return status;
}
// Function to execute a block of commands and record execution history
int execution_block(char *arr[][100], int commands, int flag, bool background)
{
    int status;
    if (flag == 1)
    {
        goto execution;
    }
    time_t start;
    struct tm *t1;
    char start_time[9];
    time(&start);
    t1 = localtime(&start);
    strftime(start_time, sizeof(start_time), "%T", t1);
    time_t end;
    struct tm *t2;
    char end_time[9];
    struct timeval st;
    struct timeval en;
    gettimeofday(&st, NULL);
    double start_milli = (double)st.tv_sec * 1000 + (double)st.tv_usec / 1000;

execution:
    status = launch(arr, commands, background);
    if (flag == 1)
    {
        goto end;
    }
    time(&end);
    t2 = localtime(&end);
    strftime(end_time, sizeof(end_time), "%T", t2);
    gettimeofday(&en, NULL);
    double end_milli = (double)en.tv_sec * 1000 + (double)en.tv_usec / 1000;
    double duration = end_milli - start_milli;

    FILE *file = fopen("history.txt", "a");
    if (file == NULL)
    {
        printf("Error in opening the file history.txt\n");
        exit(1);
    }
    fprintf(file, "Pid:%d  ", status);
    fprintf(file, "Start time is: %s  ", start_time);
    fprintf(file, "end time is: %s  ", end_time);
    fprintf(file, "duration %f milliseconds  ", duration);
    fprintf(file, "The command is: ");
    for (int k = 0; k < commands; k++)
    {
        for (int i = 0; arr[k][i] != NULL; i++)
        {
            fprintf(file, "%s ", arr[k][i]);
        }
        if (k < commands - 1)
        {
            fprintf(file, "|");
        }
    }
    if (background)
    {
        fprintf(file, "&");
    }
    fprintf(file, "\n");
    if (fclose(file) != 0)
    {
        printf("Error in closing the history.txt file\n");
        exit(1);
    }
    // Register the SIGINT handler
    signal(SIGINT, signal_handler);
end:
    return status;
}
void executing_script(char *str)
{
    FILE *file1 = fopen(str, "r");
    if (file1 == NULL)
    {
        printf("Error in opening the file\n");
        exit(1);
    }
    char lines[256];
    int commands = 0;
    int status;

    time_t start;
    struct tm *t1;
    char start_time[9];
    time(&start);
    t1 = localtime(&start);
    strftime(start_time, sizeof(start_time), "%T", t1);

    bool background = false;
    struct timeval st;
    gettimeofday(&st, NULL);
    double start_milli = (double)st.tv_sec * 1000 + (double)st.tv_usec / 1000;

    while (fgets(lines, sizeof(lines), file1))
    {
        commands = 0;
        char *arr[100][100];
        char *str = strtok(lines, " \n");
        int i = 0;
        int j = 0;
        while (str != NULL)
        {
            if (*str == '|')
            {
                arr[i][j] = NULL;
                commands++;
                i++;
                j = 0;
                str = strtok(NULL, " \n");
                continue;
            }
            if (*str == '&')
            {
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

    if (ferror(file1))
    {
        printf("Error in reading from the file\n");
        exit(1);
    }

    if (fclose(file1) != 0)
    {
        printf("Error in closing the file\n");
        exit(1);
    };
    time_t end;
    struct tm *t2;
    char end_time[9];
    struct timeval en;

    time(&end);
    t2 = localtime(&end);
    strftime(end_time, sizeof(end_time), "%T", t2);

    gettimeofday(&en, NULL);
    double end_milli = (double)en.tv_sec * 1000 + (double)en.tv_usec / 1000;

    double duration = end_milli - start_milli;

    FILE *file = fopen("history.txt", "a");
    if (file == NULL)
    {
        printf("Error in opening the file\n");
        exit(1);
    }
    fprintf(file, "Pid:%d  ", status);
    fprintf(file, "Start time is: %s  ", start_time);
    fprintf(file, "end time is: %s  ", end_time);
    fprintf(file, "duration %f milliseconds  ", duration);
    fprintf(file, "The command is: ");
    fprintf(file, "1 %s", str);
    fprintf(file, "\n");
    if (fclose(file) != 0)
    {
        printf("Error in closing the file\n");
        exit(1);
    }
    // Register the SIGINT handler
    signal(SIGINT, signal_handler);
}


struct CPU_NODE
{
    struct pdt_node *process_running;
    struct CPU_NODE *next_CPU;
};
struct MyQueue_CPU
{
    struct CPU_NODE *front;
    struct CPU_NODE *rear;
};
struct MyQueue *createQueue()
{
    struct MyQueue *queue = (struct MyQueue *)malloc(sizeof(struct MyQueue));
    if (!queue)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    queue->front = queue->rear = NULL;
    return queue;
}
struct MyQueue_CPU *createQueue_CPU()
{
    struct MyQueue_CPU *queue = (struct MyQueue_CPU *)malloc(sizeof(struct MyQueue_CPU));
    if (!queue)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    queue->front = queue->rear = NULL;
    return queue;
}

int NCPU;                  // globally set number of cpu's
int TSLICE;                // global set time slice

struct CPU_NODE *dequeue_cpu(struct MyQueue_CPU *queue)
{
    if (queue->front == NULL)
    {
        return NULL; // Queue is empty
    }
    struct CPU_NODE *temp = queue->front;
    queue->front = queue->front->next_CPU;
    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }
    return temp;
}
int isEmpty(struct MyQueue *queue)
{
    return (queue->front == NULL);
}
void printPDTNode(struct pdt_node *node)
{
    printf("PRINTING A PDT NODE\n");
    printf("______________________________\n");
    if (node != NULL)
    {
        printf("\nPID: %d\n", node->pid);
        printf("Command: %s\n", node->cmd);
        //printf("Start Time: %d\n", node->start_time);
        //printf("Wait Time: %d\n", node->elapsed_time);
        printf("Priority: %d\n", node->priority);

        const char *state_str = "Unknown";
        switch (node->state)
        {
        case 1:
            state_str = "Ready";
            break;
        case 2:
            state_str = "Running";
            break;
        case 3:
            state_str = "Completed";
            break;
        }
        printf("State: %s\n", state_str);
    }
    else
    {
        printf("Node is NULL\n");
    }
    printf("______________________________\n");
}
void printQueue(struct MyQueue *queue)
{
    printf("printQueue\n");
    struct pdt_node *current = queue->front;
    while (current != NULL)
    {
        printPDTNode(current);
        current = current->next_process;
    }
}

struct MyQueue *makeq()
{
    struct MyQueue *q = (struct MyQueue *)malloc(sizeof(struct MyQueue));
    if (!q)
    {
        printf("Memory Allocation failed");
        exit(EXIT_FAILURE);
    }
    q->rear = q->front = NULL;
    return q;
}



void enqueue_1(struct MyQueue* q,struct pdt_node *process)
{
    process->next_process = NULL;
    if (q->rear == NULL)
    {
        q->front = q->rear = process;
        return;
    }
    else
    {
        q->rear->next_process = process;
        q->rear = process;
    }
}



void enqueue_CPU_1(struct CPU_NODE *CPU,struct MyQueue_CPU* q_cpu)
{
    CPU->next_CPU = NULL;
    if (q_cpu->rear == NULL)
    {
        q_cpu->front = q_cpu->rear = CPU;
        return;
    }
    else
    {
        q_cpu->rear->next_CPU = CPU;
        q_cpu->rear = CPU;
    }
}
struct CPU_NODE *createCPU_NODE()
{

    struct CPU_NODE *newNode = (struct CPU_NODE *)malloc(sizeof(struct CPU_NODE));
    if (newNode == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    // one cpu will be created by this segment.
    newNode->process_running = NULL;
    newNode->next_CPU = NULL;
    return newNode;
}
void multipleCPUS_creation(shm_t* shm)
{
    for (int i = 0; i < NCPU; i++)
    {
        struct CPU_NODE *temp = createCPU_NODE();
        enqueue_CPU_1(temp,shm->q_cpu);
    }
}


void print_CPU_List_1(struct MyQueue_CPU* q) {
    printf("print_CPU_List_1 \n");
    struct CPU_NODE *current = q->front;
    int i = 0;
    while (current != NULL)
    {
        printPDTNode(current->process_running);
        printf("%d", i);
        current = current->next_CPU;
        i++;
    }
}


int isempty(struct MyQueue* q) {
    if (q->front==NULL) {
        //return true;
        return 1;
    }
    //return false;
    return 0;
}

int isempty_cpu(struct MyQueue_CPU* q) {
    if (q->front==NULL) {
        //return true;
        return 1;
    }
    //return false;
    return 0;
}







void sortQueueByPriority(shm_t* shm)
{
    // Count the number of processes
    struct pdt_node* temp[10];
    int count1=0;
    struct pdt_node* temp1=shm->q->front;
    while(temp1!=NULL) {
        struct pdt_node* temp2=temp1->next_process;
        struct pdt_node* ptr1=dequeue(shm->q);
        ptr1->next_process=NULL;
        temp[count1]=ptr1;
        count1++;
        temp1=temp2;
    }
    
    shm->q->front=NULL;
    shm->q->rear=NULL;

    for (int i=count1-1;i>=0;i--) {
        for (int j=0;j<i;j++) {
            if (temp[j]->priority>temp[j+1]->priority) {
                int pid=temp[j]->pid;
                temp[j]->next_process=NULL;
                temp[j+1]->next_process=NULL;
                char cmd[100];
                strcpy(cmd,temp[j]->cmd);
                char start_time[9];
                strcpy(start_time,temp[j]->start_time);
                char end_time[9];
                strcpy(end_time,temp[j]->end_time);
                double start_milli=temp[j]->start_milli;
                double waiting_time=temp[j]->waiting_time;
                int priority=temp[j]->priority;
                double duration=temp[j]->duration;
                int state=temp[j]->state;


                temp[j]->pid=temp[j+1]->pid;
                strcpy(temp[j]->cmd,temp[j+1]->cmd);
                strcpy(temp[j]->start_time,temp[j+1]->start_time);
                strcpy(temp[j]->end_time,temp[j+1]->end_time);
                temp[j]->start_milli=temp[j+1]->start_milli;
                temp[j]->waiting_time=temp[j+1]->waiting_time;
                temp[j]->priority=temp[j+1]->priority;
                temp[j]->duration=temp[j+1]->duration;
                temp[j]->state=temp[j+1]->state;


                temp[j+1]->pid=pid;
                strcpy(temp[j+1]->cmd,cmd);
                strcpy(temp[j+1]->start_time,start_time);
                strcpy(temp[j+1]->end_time,end_time);
                temp[j+1]->start_milli=start_milli;
                temp[j+1]->waiting_time=waiting_time;
                temp[j+1]->priority=priority;
                temp[j+1]->duration=duration;
                temp[j+1]->state=state;

            }
        }
    }

    for (int i=0;i<count1;i++) {
        enqueue_1(shm->q,temp[i]);
    }
}


int RUNNING = 0; // 1 for running else 0
struct pdt_node *prc_itr;
struct CPU_NODE *cpu_itr;
int nprocs;

void do_execution(shm_t* shm,int *arr,int count) {
    


    sem_t semaphore1;
    int arr1[count];
    for (int i=0;i<count;i++) {
        arr1[i]=0;
    }
    sem_init(&semaphore1,1,1);
    struct CPU_NODE* ptr=shm->q_running_processes->front;
    for (int i=0;i<count;i++) {
        ptr->process_running->state=2;
       if (ptr->process_running->pid!=0) {
            arr1[i]=ptr->process_running->pid;
            ptr=ptr->next_CPU;
            continue;
        }
        arr1[i]=fork();
        time_t start;
        struct tm* t1;
        char start_time[9];
        time(&start);
        t1 = localtime(&start);
        strftime(start_time, sizeof(start_time), "%T", t1);
        strcpy(ptr->process_running->start_time,start_time);
        struct timeval st;
        gettimeofday(&st, NULL);
        double start_milli1 = (double)st.tv_sec * 1000 + (double)st.tv_usec / 1000;
        ptr->process_running->start_milli=start_milli1;
        if(arr1[i]<0) {
            printf("Error in fork\n");
            exit(1);
        } else if (arr1[i]==0) {
            sem_wait(&semaphore1);
            char* arr2[2];
            arr2[0]=ptr->process_running->cmd;
            arr2[1]=NULL;
            execvp(arr2[0],arr2);
            printf("error in execvp\n");
            fflush(stdout);
            exit(1);
        } else {
            sem_post(&semaphore1);
            kill(arr1[i],SIGSTOP);
            
        }
        ptr=ptr->next_CPU;
    }

    for (int i=0;i<count;i++) {
        kill(arr1[i],SIGCONT);
    }


    usleep(TSLICE*1000000);


    ptr=shm->q_running_processes->front;
    shm->keeping_count++;
    for (int i=0;i<count;i++) {
        int status;
        int result=waitpid(arr1[i],&status,WNOHANG);
        ptr->process_running->number_of_times_got_cpu++;
        if (result==0) {
            kill(arr1[i],SIGSTOP);
            ptr->process_running->pid=arr1[i];
            arr[i]=arr1[i];
            enqueue_1(shm->q,ptr->process_running);
            ptr->process_running->state=1;
        } else {
            ptr->process_running->pid=arr1[i];
            time_t end;
            struct tm* t2;
            char end_time[9];
            time(&end);
            t2 = localtime(&end);
            strftime(end_time, sizeof(end_time), "%T", t2);
            strcpy(ptr->process_running->end_time,end_time);
            struct timeval en;
            gettimeofday(&en, NULL);
            double end_milli1= (double)en.tv_sec * 1000 + (double)en.tv_usec / 1000;
            double duration = end_milli1 - ptr->process_running->start_milli;
            ptr->process_running->duration+=(TSLICE*1000)*((ptr->process_running->number_of_times_got_cpu));
            ptr->process_running->waiting_time+=((TSLICE*1000)*(shm->keeping_count-ptr->process_running->number_of_times_got_cpu));
            ptr->process_running->state=3;
            FILE *file = fopen("history.txt", "a");
            if (file == NULL)
            {
                printf("Error in opening the file\n");
                exit(1);
            }
            fprintf(file, "Pid:%d  ", ptr->process_running->pid);
            fprintf(file, "Start time is: %s  ", ptr->process_running->start_time);
            fprintf(file, "end time is: %s  ", end_time);
            fprintf(file,"waiting time is: %f  ",ptr->process_running->waiting_time);
            fprintf(file, "duration %f milliseconds  ", ptr->process_running->duration);
            fprintf(file, "The command is: ");
            fprintf(file, " %s", ptr->process_running->cmd);
            fprintf(file, "\n");
            if (fclose(file) != 0)
            {
                printf("Error in closing the file\n");
                exit(1);
            }
            
        }
        ptr=ptr->next_CPU;
    }

    sem_destroy(&semaphore1);
}

//handling a single process
void PROCESS_HANDLER( shm_t* shm)
{

    int count=0;
    while(isempty(shm->q)!=1) {
        count=0;
        sortQueueByPriority(shm);
        while(isempty_cpu(shm->q_cpu)!=1 && isempty(shm->q)!=1) {
            //printf("again\n");
            struct CPU_NODE* temp1=dequeue_cpu(shm->q_cpu);
            struct pdt_node* temp2=dequeue(shm->q);
            temp2->state=1;
            temp1->process_running=temp2;
            enqueue_CPU_1(temp1,shm->q_running_processes);
            count++;
        }
        int arr[count];
        for (int i=0;i<count;i++) {
            arr[i]=0;
        }
        do_execution(shm,arr,count);
        struct CPU_NODE* ptr=shm->q_running_processes->front;
        for (int i=0;i<count;i++) {
            ptr->process_running=NULL;
            struct CPU_NODE* temp1=dequeue_cpu(shm->q_running_processes);
            enqueue_CPU_1(temp1,shm->q_cpu);
            ptr=shm->q_running_processes->front;
        }        
    }
}


void delete_queue(shm_t* shm) {
    //printf("dequeue karne aaya to hu\n");
    struct pdt_node* ptr=shm->q->front;
    while(ptr!=NULL) {
        struct pdt_node* temp=ptr->next_process;
        dequeue(shm->q);
        ptr=temp;
    }
}


int main(int argc, char *argv[])
{

    
    char shm_name[100]="/kartikey";
    size_t shm_size=sizeof(shm_t);
    int fd=shm_open(shm_name,O_CREAT | O_RDWR,S_IRWXU);
    if (fd==-1) {
        printf("error in shm_opend\n");
        exit(1);
    }

    if (ftruncate(fd,shm_size)==-1) {
        printf("Error in ftruncate\n");
        exit(1);
    }

    void* shm1=mmap(NULL,shm_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(shm1==MAP_FAILED)
    {
        perror ("main: mmap failed:");
    }
    shm_t* shm=(shm_t*)shm1;

    fd11=fd;
    shm11=shm1;
    shm_size11=shm_size;
    strcpy(shm_name11,shm_name);
    mutex11=shm->mutex;

    sem_init(&shm->mutex,1,0);

    NCPU = atoi(argv[1]);
    TSLICE = atoi(argv[2]);
    shm->q=malloc(sizeof(struct MyQueue));
    shm->q_cpu=malloc(sizeof(struct MyQueue_CPU*));
    shm->q_running_processes=malloc(sizeof(struct MyQueue_CPU*));
    
    shm->q = createQueue();
    shm->q_cpu = createQueue_CPU();
    shm->q_running_processes=createQueue_CPU();
    multipleCPUS_creation(shm);
    shm->n=0;
    shm->keeping_count=0;
                    
    for (int i=0;i<10;i++) {
        shm->arr[i][0]='c';
    }
    printf("The total number of CPU resources you assigned are: %d\n", NCPU);
    printf("The time quantam: %d\n", TSLICE);
    char arr1[100];
    char *arr[100][100];
    FILE *file = fopen("history.txt", "w");
    if (file == NULL)
    {
        printf("Error in opening the file history.txt\n");
    }
    if (fclose(file) != 0)
    {
        printf("Error in closing the file history.txt\n");
    };


    int rc=fork();
    if (rc<0) {
        printf("errro in fork\n");
        exit(1);
    } else if (rc==0) {
        if (setsid()==-1) {
            printf("Error in setsid\n");
            exit(1);
        }

        while(1) {
            //Scheduler While loop
            sleep(30);
            sem_wait(&shm->mutex);
            int rcc=fork();
            if (rcc!=0) {
                shm->parent_pid=rcc;
            }
            if (rcc<0) {
                exit(1); 
            } else if (rcc==0) {
                int flag=0;
                
                for (int i=0;i<shm->n;i++) {
                    if (shm->arr[i][0]=='c') {
                        continue;
                    }
                    flag=1;
                    struct pdt_node* process_ =(struct pdt_node*)malloc(sizeof(struct pdt_node));
                    int j=0;
                    for (;j<strlen(shm->arr[i]);j++) {
                        process_->cmd[j]=shm->arr[i][j];
                    }
                    process_->cmd[j]='\0';
                    process_->priority = shm->arr1[i];
                    process_->state = 1;
                    process_->start_milli=0;
                    process_->waiting_time=0;
                    process_->number_of_times_got_cpu=0;
                    process_->duration=0;
                    process_->pid = 0;
                    enqueue_1(shm->q,process_);
                    shm->arr[i][0]='c';
                }
                if (flag==1) {
                    
                    PROCESS_HANDLER(shm);
                    shm->keeping_count=0;
                    shm->flag=true;
                    shm->n=0;
                }
                exit(0);
            } else {
                int wc=wait(NULL);
            }
        }
        
    } else {
        shm->demon_id=rc;
        demon_id=rc;
        while (1)
        {
            signal(SIGINT, signal_handler);
            signal(SIGUSR1,signal_handler);
            if (shm->flag==true) {
                delete_queue(shm);
                shm->flag=false;
            }
            printf("Aahan@Aahan:~$ ");
            bool background = false;
            int commands = 0;
            char* temp = fgets(arr1, sizeof(arr1), stdin);
            if (strncmp(temp, "submit ", 7) == 0)
            {
                if (shm->flag==true) {
                    delete_queue(shm);
                    shm->flag=false;
                }
                printf("TESTER\n");
                char temp1[10];
                strncpy(temp1,temp+7,strlen(temp)-10);
                char* prog=strdup(temp1);
                char* priority=temp+strlen(temp)-2;
                char* check=temp+strlen(temp)-3;
                char* prog1=temp+7;
                char *newline = strchr(priority, '\n');
                if (newline != NULL)
                {
                    *newline = '\0';
                }
                printf("Changing the process details\n");
                struct pdt_node* process_ =(struct pdt_node*)malloc(sizeof(struct pdt_node));
                if ((int)(*priority)-48<=4 && (int)(*priority)-48>=1) {
                    if (*check!=' ') {
                        strcpy(shm->arr[shm->n],prog1);
                        shm->arr1[shm->n]=1;
                        strcpy(process_->cmd,prog1);
                    } else {
                        strcpy(shm->arr[shm->n],prog);
                        shm->arr1[shm->n]=(int)(*priority)-48;
                        strcpy(process_->cmd,prog);
                    }
                }  else {
                    strcpy(shm->arr[shm->n],prog1);
                    shm->arr1[shm->n]=1;
                    strcpy(process_->cmd,prog1);
                }
                shm->n++;
                
                process_->priority = shm->arr1[shm->n-1];
                process_->state = 1;
                process_->duration=0;
                process_->pid = 0;
                shm->process_=process_;
                enqueue_1(shm->q,process_);
                printf("Process updated successfully ,Adding the process in the queue...\n");
                
                printf("Now we will giving each cpu a process and updating the process detail side by side\n");
                nprocs = shm->n; // creating this nprocs var for every child as this can be different // this is the var storing no of process in the ready queue at a time.
                printf("Now the scheduler is ready with the queues.\n");
                printf("Now enter RUN to start the scheduler.(I.e putting up the progs in q_cpu and start running )\n");
                printf("Porcess added successfully\n");
                printf("The no of programs in the prog queue %d\n", nprocs);
                sem_post(&shm->mutex);
                sortQueueByPriority(shm);
                printQueue(shm->q);
            }
            else if (strncmp(temp, "history", 7)==0 ) 
            {
                char lines[256];
                FILE* file=fopen("history.txt","r");
                while(fgets(lines,sizeof(lines),file)) {
                    printf("%s\n",lines);
                }
                fclose(file);
            }
            else if (temp != NULL)
            {
                if (arr1[0] == '\n')
                {
                    continue;
                }
                if (arr1[0] == '1')
                {
                    char *str = strtok(arr1, " \n");
                    executing_script(str);
                    continue;
                }
                char *str = strtok(arr1, " \n");
                int i = 0;
                int j = 0;
                while (str != NULL)
                {
                    if (*str == '|')
                    {
                        arr[i][j] = NULL;
                        commands++;
                        i++;
                        j = 0;
                        str = strtok(NULL, " \n");
                        continue;
                    }
                    if (*str == '&')
                    {
                        background = true;
                        break;
                    }
                    arr[i][j] = strdup(str);
                    str = strtok(NULL, " \n");
                    j++;
                }
                commands++;
                arr[i][j] = NULL;
                int flag = 0;
                execution_block(arr, commands, flag, background);
            }

            else
            {
                printf("Error in input from stdin\n");
                continue;
            }
            signal(SIGINT, signal_handler);
            signal(SIGUSR1,signal_handler);
        }
        
        
    }
    signal(SIGINT, signal_handler);
    return 0;
}
