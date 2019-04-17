//
// Created by kalesh on 4/16/19.
//

#include "lock.h"

int r_count;                     // Readers count
int r_wait_count;                // Readers waiting count
int msg_avail;                   // Boolean 0/1
pthread_cond_t read_phase;       // Read phase condition variable
pthread_cond_t write_phase;      // Write phase condition variable
pthread_mutex_t mutex;           // Mutex that protects the counter

/*
 * Initialize the reader-writer lock
 */
void init_rw_lock() {
    r_count = 0;
    r_wait_count = 0;
    pthread_cond_init(&read_phase, NULL);
    pthread_cond_init(&write_phase, NULL);
    msg_avail = 0;
    // Initialize the mutex
    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        fprintf(stderr, "Mutex init has failed\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * Acquire the writer lock.
 */
void acquire_exclusive() {
    pthread_mutex_lock(&mutex);
    while (r_count != 0) {  // Wait until no reader or writer
        pthread_cond_wait(&write_phase, &mutex);
    }
    r_count = -1;               // Writer is currently in its critical section
    pthread_mutex_unlock(&mutex);

}

/*
 * Release the write lock.
 */
void release_exclusive() {
    pthread_mutex_lock(&mutex);
    r_count = 0;                // Writer has left its critical section
    msg_avail = 1;              // Set msg_avail to true
    if (r_wait_count > 0) {     // Readers have priority over writers
        pthread_cond_broadcast(&read_phase);
    } else {
        pthread_cond_signal(&write_phase);
    }
    pthread_mutex_unlock(&mutex);
}

/*
 * Acquire the reader lock
 */
void acquire_shared() {
    pthread_mutex_lock(&mutex);
    r_wait_count++;             // Increment the readers waiting
    while(r_count == -1 || msg_avail == 0) {      // Wait whilst there is a writer or no msg_avail
        pthread_cond_wait(&read_phase, &mutex);
    }
    r_wait_count--;             // Decrement the readers waiting
    r_count++;                  // Increment the active readers count
    pthread_mutex_unlock(&mutex);
}

/*
 * Release the reader lock
 */
void release_shared() {
    pthread_mutex_lock(&mutex);
    r_count--;                  // Decrement the active readers count
    if (r_count == 0 && r_wait_count == 0) {
        msg_avail = 0;          // Reset msg_avail to false
        pthread_cond_signal(&write_phase);
    }
    pthread_mutex_unlock(&mutex);
}

