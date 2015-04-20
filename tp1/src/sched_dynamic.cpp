#include "sched_dynamic.h"
#include <iostream>

using namespace std;

SchedDynamic::SchedDynamic(vector<int> argn) {
	primera = true;
}

SchedDynamic::~SchedDynamic() {
}

void SchedDynamic::initialize() {
}

void SchedDynamic::load(int pid) {
//Encolo tareas por prioridad si es la primera, la guardo tambien a parte
	datos tarea;
	tarea.pid = pid;
	tarea.deadline = period(pid);
	pq.push(tarea);
	if(primera){
		actual = tarea;
		primera = false;
	}
}

void SchedDynamic::unblock(int pid) {
}

int SchedDynamic::tick(int cpu, const enum Motivo m) {
//Si no estoy corriendo la IDLE y no termino, la encolo por prioridad
	if(current_pid(cpu) != IDLE_TASK && m!=EXIT){
		datos tarea;
		tarea.pid = actual.pid;
		tarea.deadline = actual.deadline - 1;
		pq.push(tarea);
	}
//Si no tengo nada mas que correr, IDLE
	if (pq.empty()) return IDLE_TASK;
//Si no desencolo para correr
	actual = pq.top();
	pq.pop();
	return actual.pid;
}
