#include "mpmc_serial.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LEN 10

void* producer(void* arg){
    void** args = (void**) arg;
    MQ_t* mq = (MQ_t*) args[0];
    int id = (int) (intptr_t) args[1];
    payload_t pl;
    for(int i = 0; i < 5; ++i) {
        printf("P%d send %d\n", id, i);
        pl.data = i;
        pl.id = id;
        MQ_send(mq, pl);
        printf("P%d send %d ok!\n", id, i);
        usleep(100000); // Sleep for 100 ms to simulate work
    }
    return NULL;
}

void* consumer(void* arg) {
    void** args = (void**) arg;
    MQ_t* mq = (MQ_t*) args[0];
    int id = (int) (intptr_t) args[1];
    while (1) {
        payload_t data = MQ_recv(mq);
        printf("C%d get P%d-%d\n", id, data.id, data.data);
        usleep(300000); // Sleep for 300 ms
    }
    return NULL;
}

int main() {
    pthread_t producers[5], consumers[5];
    MQ_t* mq = MQ_init(LEN);
    const int num_producers = 3;

    void* producer_args[num_producers][2];
    void* consumer_args[num_producers][2];
    for (int i = 0; i < num_producers; ++i) {
        producer_args[i][0] = (void*) mq;
        producer_args[i][1] = (void*) (intptr_t) i;
        if (pthread_create(&producers[i], NULL, producer, producer_args[i]) != 0) {
            perror("Failed to create producer thread");
            return 1;
        }

        consumer_args[i][0] = (void*) mq;
        consumer_args[i][1] = (void*) (intptr_t) i;
        if (pthread_create(&consumers[i], NULL, consumer, consumer_args[i]) != 0) {
            perror("Failed to create consumer thread");
            return 1;
        }
    }

    for (int i = 0; i < num_producers; ++i) {
        pthread_join(producers[i], NULL);
    }
    pthread_join(consumers[0], NULL); //blocks forever
    // Cleanup
    free(mq);
    return 0;
}