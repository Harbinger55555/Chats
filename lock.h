//
// Created by kalesh on 4/16/19.
//

#ifndef CHATS_LOCK_H
#define CHATS_LOCK_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

extern int r_count;                     // Readers count
extern int r_wait_count;                // Readers waiting count
extern int msg_avail;                   // Boolean 0/1
extern pthread_cond_t read_phase;       // Read phase condition variable
extern pthread_cond_t write_phase;      // Write phase condition variable
extern pthread_mutex_t mutex;           // Mutex that protects the counter

/*
 * Initialize the reader-writer lock
 */
void init_rw_lock();

/*
 * Acquire the writer lock.
 */
void acquire_exclusive();

/*
 * Release the write lock.
 */
void release_exclusive();

/*
 * Acquire the reader lock
 */
void acquire_shared();

/*
 * Release the reader lock
 */
void release_shared();

#endif //CHATS_LOCK_H
