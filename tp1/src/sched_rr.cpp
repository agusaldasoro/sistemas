#include <vector>
#include <queue>
#include "sched_rr.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR::SchedRR(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	cores = argn[0];
	for (int i = 1; i < (int)argn.size(); ++i){
		quantums.push_back(argn[i]);
		quantums_timer.push_back(argn[i]+1);
		actuales.push_back(IDLE_TASK);
	}
	siguiente = 0;
}

SchedRR::~SchedRR() {
}

void SchedRR::load(int pid) {
	if(siguiente < cores && actuales[siguiente] == IDLE_TASK){
		actuales[siguiente] = pid;
		siguiente++;
	}
	else{
		siguiente = cores;
		cola.push(pid);
	}
}

void SchedRR::unblock(int pid) {
}

int SchedRR::tick(int cpu, const enum Motivo m) {
	if(actuales[cpu] != IDLE_TASK)
		quantums_timer[cpu]--;
	if(quantums_timer[cpu] <= 0 || m == EXIT || m == BLOCK)
		next(cpu, m);
	return actuales[cpu];
}

int SchedRR::next(int cpu, const enum Motivo m) {
	if(cola.empty() && m == EXIT){
		actuales[cpu] = IDLE_TASK;
		return IDLE_TASK;
	}
	if(m != EXIT || m == BLOCK)
		cola.push(actuales[cpu]);
	actuales[cpu] = cola.front();
	cola.pop();
	quantums_timer[cpu] = quantums[cpu];
	return actuales[cpu];
}
