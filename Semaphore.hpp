#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H
#include "Headers.hpp"

// Synchronization Warm up 
class Semaphore {
public:
	Semaphore(){ // Constructs a new semaphore with a counter of 0
        mutex=PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
        pthread_cond_init(&cond,NULL);
	    val=0;
	}
	Semaphore(unsigned val){// Constructs a new semaphore with a counter of
		// val
        mutex=PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
        pthread_cond_init(&cond,NULL);
		this->val=val;
	}

	void up() { // Mark: 1 Thread has left the critical section
		pthread_mutex_lock(&mutex);
	    this->val++;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
	}
	void down() { // Block untill counter >0, and mark - One thread has entered the critical section.
        // therad
        pthread_mutex_lock(&mutex);
        while (!(this->val>0)) { //place task in the wait queue
            pthread_cond_wait(&cond,&mutex);
        }
        this->val--;
        pthread_mutex_unlock(&mutex);
	}
private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
	unsigned val; // state
	// TODO 
};

#endif
