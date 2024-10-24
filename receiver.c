#include "receiver.h"
mqd_t mq;
void receive(message_t* message_ptr, mailbox_t* mailbox_ptr){
    /*  TODO: 
        1. Use flag to determine the communication method
        2. According to the communication method, receive the message
    */
    if(mailbox_ptr->flag == 1){
        // Message Passing
        struct mq_attr attr;
        if (mq_getattr(mq, &attr) == -1) {
            perror("mq_getattr");
            return;
        }

        char *buffer = malloc(attr.mq_msgsize);
        if (buffer == NULL) {
            perror("malloc");
            return;
        }
        ssize_t bytes_read = mq_receive(mq, buffer, attr.mq_msgsize, NULL);
        if(bytes_read == -1){
            perror("mq_receive");
            return;
        }
        if(strcmp(buffer, "EOF") == 0){
            printf("Sender exit!\n");
        }else{
            printf("Receiving message: %s\n", buffer);
        }
    }else if(mailbox_ptr->flag == 2){
        // Shared Memory
        char *name = "OS";
        int SIZE = 1024;
        int shm_fd = shm_open(name, O_RDONLY, 0666);
        char* ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
        if(strcmp((char *)ptr, "EOF") == 0){
            printf("Sender exit!\n");
        }else{
            printf("Receiving message: %s\n", (char *)ptr);
        }
        
        shm_unlink(name);
    }
}

int main(int argc, char *argv[]){
    /*  TODO: 
        1) Call receive(&message, &mailbox) according to the flow in slide 4
        2) Measure the total receiving time
        3) Get the mechanism from command line arguments
            • e.g. ./receiver 1
        4) Print information on the console according to the output format
        5) If the exit message is received, print the total receiving time and terminate the receiver.c
    */
    sem_t *sem_send = sem_open("sender", 0);
    sem_t *sem_recv = sem_open("receiver", 0);
    struct timespec start, end;
    double time_taken = 0;
    message_t message;
    mailbox_t mailbox;
    mailbox.flag = atoi(argv[1]);
    if(mailbox.flag == 1){
        printf("Message Passing\n");
        mq = mq_open("/msgQ", O_CREAT | O_RDONLY, 0666, NULL);
        if(mq == -1){
            perror("mq fail");
            return 1;
        }
    }else if(mailbox.flag == 2){
        printf("Shared Memory\n");
    }
    while(1){
        sem_wait(sem_send);
        clock_gettime(CLOCK_MONOTONIC, &start);
        receive(&message, &mailbox);
        clock_gettime(CLOCK_MONOTONIC, &end);
        sem_post(sem_recv);
        time_taken += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
    }

    sem_close(sem_send);
    sem_close(sem_recv);



}