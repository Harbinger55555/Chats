//
// Created by kalesh on 4/16/19.
//

#ifndef CHATS_LOCK_H
#define CHATS_LOCK_H



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
