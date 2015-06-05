#include "RWLock.h"
#include <iostream>

using namespace std;

RWLock::RWLock() {
	readSwitch = Lightswitch();
	pthread_mutex_init(&roomEmpty, NULL);
	pthread_mutex_init(&turnstile, NULL);
}

void RWLock::rlock() {
	pthread_mutex_lock(&turnstile);
	pthread_mutex_unlock(&turnstile);
	readSwitch.lock(&roomEmpty);
}

void RWLock::wlock() {
	pthread_mutex_lock(&turnstile);
	pthread_mutex_lock(&roomEmpty);
}

void RWLock::runlock() {
	readSwitch.unlock(&roomEmpty);
}

void RWLock::wunlock() {
	pthread_mutex_unlock(&turnstile);
	pthread_mutex_unlock(&roomEmpty);
}
