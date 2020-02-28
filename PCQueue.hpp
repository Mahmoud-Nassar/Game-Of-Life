#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
// Single Producer - Multiple Consumer queue
template <typename T>class PCQueue {

public:

	/** constructs a new default PCQueue */
	PCQueue() {
		queue1 = queue<T>();
		/******************* init mutex ***********/
		pthread_mutexattr_t Attr1;
		pthread_mutexattr_init(&Attr1);
		pthread_mutexattr_settype(&Attr1, PTHREAD_MUTEX_ERRORCHECK);
		pthread_mutex_init(&mutex, &Attr1);
		pthread_cond_init(&cond, NULL);
		/*********************************************/
		producers_waiting=0;
	}

	// Blocks while queue is empty. When queue holds items, allows for a single
	// thread to enter and remove an item from the front of the queue and return it.
	// Assumes multiple consumers.
	T pop() {
		pthread_mutex_lock(&mutex);
		while (!queue1.size()||producers_waiting>0) {
			pthread_cond_wait(&cond, &mutex);
		}
		T temp = queue1.front();
		queue1.pop();
		pthread_mutex_unlock(&mutex);
		return temp;
	}

	// Allows for producer to enter with *minimal delay* and push items to back of the queue.
	// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.  
	// Assumes single producer 
	void push(const T &item) {
		producers_waiting++;
		pthread_mutex_lock(&mutex);
		queue1.push(item);
		producers_waiting--;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}

	~PCQueue() {
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);
	}

	uint size() {
		return (uint) queue1.size();
	}

private:
	std::queue<T> queue1;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int producers_waiting;
};
// Recommendation: Use the implementation of the std::queue for this exercise
#endif