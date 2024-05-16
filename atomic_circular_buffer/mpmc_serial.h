#include <stdatomic.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t id;
    int data;
} payload_t;

typedef struct {
    uint8_t len;
    uint8_t read;
    uint8_t write;
    atomic_flag mtx; // use atomics as a mutex
    payload_t* buf;        // circular buffer
} MQ_t;

MQ_t* MQ_init(uint8_t len);

// Place a message into the MQ
// If the MQ is full, wait until it has space
void MQ_send(MQ_t* mq, payload_t data);

// Get a message from the MQ
// If the MQ is empty, it returns 0 and does not block
payload_t MQ_recv(MQ_t* mq);

