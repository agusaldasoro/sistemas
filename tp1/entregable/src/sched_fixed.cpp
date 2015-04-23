#include "sched_fixed.h"
#include <iostream>

using namespace std;

SchedFixed::SchedFixed(vector<int> argn) {
}

SchedFixed::~SchedFixed() {
}

void SchedFixed::initialize() {
//Calculo prioridades de cada tarea
	for(int i = 0; i < total_tasks(); ++i){
		periodos.push_back(period(i));
	}
}

void SchedFixed::load(int pid) {
//Encolo tareas por prioridad
	datos tarea;
	tarea.pid = pid;
	tarea.periodo = periodos[pid];
	pq.push(tarea);
}

void SchedFixed::unblock(int pid) {
}

int SchedFixed::tick(int cpu, const enum Motivo m) {
//Si no estoy corriendo la IDLE y no termino, la encolo por prioridad
	if(current_pid(cpu) != IDLE_TASK && m!=EXIT){
		datos tarea;
		tarea.pid = current_pid(cpu);
		tarea.periodo = periodos[tarea.pid];
		pq.push(tarea);
	}
//Si no tengo nada mas que correr, IDLE
	if (pq.empty()) return IDLE_TASK;
//Si no desencolo para correr
	int sig = pq.top().pid; pq.pop();
	return sig;
}
