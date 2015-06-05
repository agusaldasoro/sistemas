#ifndef RWLock_h
#define RWLock_h
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <list>


class Lightswitch{
private:
	unsigned int counter;
	pthread_mutex_t mut;
public:
	Lightswitch(){
		counter = 0;
		pthread_mutex_init(&mut, NULL);
	}
	void lock(pthread_mutex_t* m){
		pthread_mutex_lock(&mut);
		counter++;
		if(counter == 1)
			pthread_mutex_lock(m);
		pthread_mutex_unlock(&mut);
	}
	void unlock(pthread_mutex_t* m){
		pthread_mutex_lock(&mut);
		counter--;
		if(counter == 0)
			pthread_mutex_unlock(m);
		pthread_mutex_unlock(&mut);
	}
};

class RWLock {
    public:
        RWLock();
        void rlock();
        void wlock();
        void runlock();
        void wunlock();

    private:
//        pthread_rwlock_t rwlock;
        Lightswitch readSwitch;
        pthread_mutex_t roomEmpty;
        pthread_mutex_t turnstile;
};

#endif
