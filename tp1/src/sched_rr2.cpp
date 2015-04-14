#include <vector>
#include <queue>
#include "sched_rr2.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR2::SchedRR2(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	cores = argn[0];
	for (int i = 1; i < (int)argn.size(); ++i){
		quantums.push_back(argn[i]);
		quantums_timer.push_back(argn[i]+1);
		actuales.push_back(IDLE_TASK);
		queue<int, deque<int> >* q = new queue<int, deque<int> >();
		colas.push_back(make_pair(0, q));
	}
}

SchedRR2::~SchedRR2() {
	for (int i = 0; i < cores; ++i){
		delete colas[i].second;
	}
}


void SchedRR2::load(int pid) {
	if(siguiente < cores && actuales[siguiente] == IDLE_TASK){
		actuales[siguiente] = pid;
		colas[siguiente].first++;
		siguiente++;
	}
	else{
		siguiente = cores;
		int a_usar = 0, min_proc = colas[0].first;
		for (int i = 1; i < cores; ++i){
			if(min_proc > colas[i].first)
				a_usar = i;
		}
		colas[a_usar].first++;
		colas[a_usar].second->push(pid);
	}
}

void SchedRR2::unblock(int pid) {
}

int SchedRR2::tick(int cpu, const enum Motivo m) {
	if(actuales[cpu] != IDLE_TASK)
		quantums_timer[cpu]--;
	if(quantums_timer[cpu] <= 0 || m == EXIT || m == BLOCK)
		next(cpu, m);
	return actuales[cpu];
}

int SchedRR2::next(int cpu, const enum Motivo m) {
	if(colas[cpu].second->empty() && m == EXIT){
		colas[cpu].first--;
		actuales[cpu] = IDLE_TASK;
		return IDLE_TASK;
	}
	if(m != EXIT || m == BLOCK)
		colas[cpu].second->push(actuales[cpu]);
	actuales[cpu] = colas[cpu].second->front();
	colas[cpu].second->pop();
	quantums_timer[cpu] = quantums[cpu];
	return actuales[cpu];
}
