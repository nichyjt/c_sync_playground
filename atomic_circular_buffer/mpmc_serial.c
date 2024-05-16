#include "mpmc_serial.h"

MQ_t* MQ_init(uint8_t len){
    assert(len > 1); // this can only work if len >= 2
    payload_t* data = malloc(len * sizeof(payload_t));
    MQ_t* mq = (MQ_t*)malloc(sizeof(MQ_t));
    mq->buf = data;
    mq->len = len;
    mq->read = 0;
    mq->write = 0;
    atomic_flag_clear(&mq->mtx);
    return mq;  
}

// Place a message into the MQ
// If the MQ is full, wait until it has space
void MQ_send(MQ_t* mq, payload_t data){
    // acquire the 'mutex' via a spinlock
    bool ok = 0;
    while(!ok){
        while(!atomic_flag_test_and_set(&mq->mtx)){
            // busy wait
            usleep(1); // can do exponential backoff if you're fancy
        }
        // critical section start
        if(!(mq->read == (mq->write + 1) % mq->len)){
            // perform the write iff it is valid
            mq->buf[mq->write] = data;
            mq->write = (mq->write + 1) % mq->len;
            ok = 1;
        }
        // critical section end, release the lock
        atomic_flag_clear(&mq->mtx);
    }
}

// Get a message from the MQ
// If the MQ is empty, it returns 0 and does not block
payload_t MQ_recv(MQ_t* mq){
    payload_t result;
    bool ok = 0;
    while(!ok){
        while(!atomic_flag_test_and_set(&mq->mtx)){
            usleep(1); // can do exponential backoff if you're fancy
        }
        // critical section start
        if(!(mq->read == mq->write)){
            // perform the write iff it is valid
            result = mq->buf[mq->read];
            mq->read = (mq->read + 1) % mq->len;
            ok = 1;
        }
        // critical section end, release the lock
        atomic_flag_clear(&mq->mtx);
    }
    return result;
}

