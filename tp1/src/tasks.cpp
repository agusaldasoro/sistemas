#include "tasks.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

void TaskCPU(int pid, vector<int> params) { // params: n
	uso_CPU(pid, params[0]); // Uso el CPU n milisegundos.
}

void TaskIO(int pid, vector<int> params) { // params: ms_pid, ms_io,
	uso_CPU(pid, params[0]); // Uso el CPU ms_pid milisegundos.
	uso_IO(pid, params[1]); // Uso IO ms_io milisegundos.
}

void TaskAlterno(int pid, vector<int> params) { // params: ms_pid, ms_io, ms_pid, ...
	for(int i = 0; i < (int)params.size(); i++) {
		if (i % 2 == 0) uso_CPU(pid, params[i]);
		else uso_IO(pid, params[i]);
	}
}

void TaskConBloqueo(int pid, vector<int> params) {
	int tiempo = params[0];
	int inicioBloq = params[1];
	int finBloq = params[2];

	if(inicioBloq > tiempo){
		uso_CPU(pid, tiempo);
	}else{
		uso_CPU(pid,inicioBloq-2);
		uso_IO(pid,finBloq-inicioBloq+1);
		uso_CPU(pid,tiempo-finBloq);
	}
}

void TaskConsola(int pid, vector<int> params){
	int bloqueantes = params[0];
	int minimo = params[1];
	int maximo = params[2];
	int azar;
	for (int i = 0; i < bloqueantes; ++i){
		azar = rand() % maximo + minimo;
		uso_IO(pid, azar);
	}
}

void TaskBatch(int pid, vector<int> params){
	int total_cpu = params[0];
	int cant_bloqueos = params[1];
	vector<int> azar(cant_bloqueos);
//	int total_aux = total_cpu;
/*	for (int i = 0; i < cant_bloqueos; ++i){
		total_aux-=azar[i];
		azar[i] = rand() % total_aux + azar[i];
	}
*/	int i=0, j=0;
	while(i < cant_bloqueos && j < total_cpu){
		if(azar[i] == j){
			uso_IO(pid, 1);
			i++;
			j++;
		}
		uso_CPU(pid, 1);
		j++;
	}
	while(j<total_cpu){
		uso_CPU(pid, 1);
		j++;
	}
}

void tasks_init(void) {
	/* Todos los tipos de tareas se deben registrar acá para poder ser usadas.
	 * El segundo parámetro indica la cantidad de parámetros que recibe la tarea
	 * como un vector de enteros, o -1 para una cantidad de parámetros variable. */
	register_task(TaskCPU, 1);
	register_task(TaskIO, 2);
	register_task(TaskAlterno, -1);
	register_task(TaskConBloqueo,3);
	register_task(TaskConsola,3);
	register_task(TaskBatch,2);
}
