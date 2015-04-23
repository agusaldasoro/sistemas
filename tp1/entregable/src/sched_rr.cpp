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
//primer core a utilizar
	siguiente = 0;
}

SchedRR::~SchedRR() {
}

void SchedRR::load(int pid) {
//Si todavia no encole nada, tengo al menos un core inactivo
//Sino encolo en la cola global
	if(siguiente < cores){
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
//Si no estoy corriendo la IDLE, estoy corriendo una tarea importante, usa quantum, lo actualizo
	if(actuales[cpu] != IDLE_TASK)
		quantums_timer[cpu]--;
//Si se acabo el quantum o, termino o se bloqueo la tarea, cambio de tarea, si no sigo con la actual
	if(quantums_timer[cpu] <= 0 || m == EXIT || m == BLOCK)
		next(cpu, m);
	return actuales[cpu];
}

int SchedRR::next(int cpu, const enum Motivo m) {
//Si no tengo nada en la cola y termino la tarea actual, pongo a correr la IDLE
	if(cola.empty() && m == EXIT){
		actuales[cpu] = IDLE_TASK;
		return IDLE_TASK;
	}
//Si no termino o se bloqueo, la encolo
	if(m != EXIT || m == BLOCK)
//notar que si la cola estaba vacia, entonces encolo y desencolo la misma tarea
		cola.push(actuales[cpu]);
//La primera de la cola, entra al procesador y la saco de la cola
	actuales[cpu] = cola.front();
	cola.pop();
//La nueva tarea tiene todo el quantum disponible
	quantums_timer[cpu] = quantums[cpu];
	return actuales[cpu];
}
