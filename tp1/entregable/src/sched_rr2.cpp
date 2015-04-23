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
//Nueva cola para cada core
		queue<int, deque<int> >* q = new queue<int, deque<int> >();
//Agrego el par, cantidad de tareas en core, cola
		colas.push_back(make_pair(0, q));
	}
}

SchedRR2::~SchedRR2() {
//Libero memoria del heap
	for (int i = 0; i < cores; ++i){
		delete colas[i].second;
	}
}


void SchedRR2::load(int pid) {
//Sumo uno a la cantidad de tareas del core usado
//Si todavia no encole nada, tengo al menos un core inactivo
	if(siguiente < cores){
		actuales[siguiente] = pid;
		colas[siguiente].first++;
		siguiente++;
	}
//Sino, busco el core con la minima cantidad de procesos y la agrego a su cola
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
//Si no estoy corriendo la IDLE, estoy corriendo una tarea importante, usa quantum, lo actualizo
	if(actuales[cpu] != IDLE_TASK)
		quantums_timer[cpu]--;
//Si se acabo el quantum o, termino o se bloqueo la tarea, cambio de tarea, si no sigo con la actual
	if(quantums_timer[cpu] <= 0 || m == EXIT || m == BLOCK)
		next(cpu, m);
	return actuales[cpu];
}

int SchedRR2::next(int cpu, const enum Motivo m) {
//Si no tengo nada en la cola y termino la tarea actual, pongo a correr la IDLE y resto en la cantidad de tareas del core
	if(colas[cpu].second->empty() && m == EXIT){
		colas[cpu].first--;
		actuales[cpu] = IDLE_TASK;
		return IDLE_TASK;
	}
//Si no termino o se bloqueo, la encolo
	if(m != EXIT || m == BLOCK)
//notar que si la cola estaba vacia, entonces encolo y desencolo la misma tarea
		colas[cpu].second->push(actuales[cpu]);
//La primera de la cola, entra al procesador y la saco de la cola
	actuales[cpu] = colas[cpu].second->front();
	colas[cpu].second->pop();
//La nueva tarea tiene todo el quantum disponible
	quantums_timer[cpu] = quantums[cpu];
	return actuales[cpu];
}
