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

//VIEJA VERSION
//	siguiente = 0;
}

SchedRR::~SchedRR() {
}

void SchedRR::load(int pid) {
	//llegan tareas, las encolo
	cola.push(pid);

//VIEJA VERSION
//	if(siguiente < cores){
//		actuales[siguiente] = pid;
//		siguiente++;
//	}
//	else{
//		siguiente = cores;
//		cola.push(pid);
//	}
}

void SchedRR::unblock(int pid) {
	//cuando se desbloquea vuelve a la cola
	cola.push(pid);
}

int SchedRR::tick(int cpu, const enum Motivo m) {
	//si corria la IDLE, termino o se bloqueo la tarea actual, veo si hay otra disponible
	if(actuales[cpu] == IDLE_TASK || m == EXIT || m == BLOCK)
		return next(cpu);
	//sino disminuyo el timer
	quantums_timer[cpu]--;
	//si se le acabo, como no termino, la pusheo y pido la siguiente
	if(quantums_timer[cpu] <= 0){
		cola.push(actuales[cpu]);
		return next(cpu);
	}
	//si no retorno la actual
	return actuales[cpu];

//VIEJA VERSION
//	if(actuales[cpu] != IDLE_TASK)
//		quantums_timer[cpu]--;
//	if(quantums_timer[cpu] <= 0 || m == EXIT || m == BLOCK)
//		next(cpu, m);
//	return actuales[cpu];
}

int SchedRR::next(int cpu) {
	//si llamo a next, voy a cambiar la tarea, restauro los ticks
	quantums_timer[cpu] = quantums[cpu];
	//si no hay nada en la cola, sigue la IDLE
	if(cola.empty())
		actuales[cpu] = IDLE_TASK;
	//sino, avanza la proxima en la cola
	else{
		actuales[cpu] = cola.front();
		cola.pop();
	}
	return actuales[cpu];

//VIEJA VERSION
//	if(cola.empty() && m == EXIT){
//		actuales[cpu] = IDLE_TASK;
//		return IDLE_TASK;
//	}
//	if(m != EXIT || m != BLOCK)
//		cola.push(actuales[cpu]);
//	actuales[cpu] = cola.front();
//	cola.pop();
//	quantums_timer[cpu] = quantums[cpu];
//	return actuales[cpu];
}
