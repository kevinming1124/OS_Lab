#include "sender.h"
mqd_t mq;
void send(message_t message, mailbox_t* mailbox_ptr){
    /*  TODO: 
        1. Use flag to determine the communication method
        2. According to the communication method, send the message
    */
    char *name = "OS";
    int SIZE = 1024;
    if(mailbox_ptr->flag == 1){
        // Message Passing
        
        mq_send(mq, message.message, strlen(message.message)+1, 0);
        
    }else if(mailbox_ptr->flag == 2){
        // Shared Memory
        int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
        ftruncate(shm_fd, SIZE);
        char *ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
        sprintf(ptr, "%s", message.message);
        ptr += strlen(message.message);
    }
}



int main(int argc, char *argv[]){
    /*  TODO: 
        1) Call send(message, &mailbox) according to the flow in slide 4
        2) Measure the total sending time
        3) Get the mechanism and the input file from command line arguments
            • e.g. ./sender 1 input.txt
                    (1 for Message Passing, 2 for Shared Memory)
        4) Get the messages to be sent from the input file
        5) Print information on the console according to the output format
        6) If the message form the input file is EOF, send an exit message to the receiver.c
        7) Print the total sending time and terminate the sender.c
    */
    sem_t *sem_send = sem_open("sender", O_CREAT, 0666, 0);
    sem_t *sem_recv = sem_open("receiver", O_CREAT, 0666, 1);
    if (sem_send == SEM_FAILED || sem_recv == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    struct timespec start, end;
    double time_taken = 0;
    message_t message;
    mailbox_t mailbox;
    mailbox.flag = atoi(argv[1]);
    FILE *file = fopen(argv[2], "r");
    if(file == NULL){
        printf("File not found\n");
        return 0;
    }
    if(mailbox.flag == 1){
        printf("Message Passing\n");
        // struct mq_attr attr;
        // attr.mq_flags = 0;
        // attr.mq_maxmsg = 10;
        // attr.mq_msgsize = 1024;
        // attr.mq_curmsgs = 0;
        mq = mq_open("/msgQ", O_CREAT | O_WRONLY, 0666, NULL);
        if(mq == -1){
            perror("mq fail");
            return 1;
        }
    }else if(mailbox.flag == 2){
        printf("Shared Memory\n");
    }
    while(fgets(message.message, 100, file) != NULL){
        if(strcmp(message.message, "EOF") != 0)
            message.message[strlen(message.message) - 1] = '\0';
        sem_wait(sem_recv);
        printf("1");
        clock_gettime(CLOCK_MONOTONIC, &start);
        send(message, &mailbox);
        printf("3");
        clock_gettime(CLOCK_MONOTONIC, &end);
        sem_post(sem_send);
        time_taken += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
        if(strcmp(message.message, "EOF") != 0){
            printf("Sending message: %s\n", message.message);
        }else{
            printf("End of input file! exit!\n");
        }
        

    }
    printf("Total sending time: %lf s\n", time_taken);


    sem_close(sem_send);
    sem_close(sem_recv);

}