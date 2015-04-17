#include "sched_fixed.h"
#include <iostream>

using namespace std;

SchedFixed::SchedFixed(vector<int> argn) {
}

SchedFixed::~SchedFixed() {
}

void SchedFixed::initialize() {
	for (int i = 0; i < total_tasks(); ++i){
		periodos.push_back(period(i));
	}
}

void SchedFixed::load(int pid) {
		datos tarea;
		tarea.pid = pid;
		tarea.periodo = periodos[pid];
		pq.push(tarea);
}

void SchedFixed::unblock(int pid) {
}

int SchedFixed::tick(int cpu, const enum Motivo m) {
	if(current_pid(cpu) != IDLE_TASK && m!=EXIT){
		datos tarea;
		tarea.pid = current_pid(cpu);
		tarea.periodo = periodos[tarea.pid];
		pq.push(tarea);
	}
	if (pq.empty()) return IDLE_TASK;
	int sig = pq.top().pid; pq.pop();
	return sig;
}
