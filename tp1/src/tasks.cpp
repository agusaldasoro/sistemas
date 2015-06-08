#include "tasks.h"
#include <iostream>
#include <stdlib.h>
#include <set>

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
//Recibo cantidad de llamadas bloqueantes, tiempo minimo de bloqueo y tiempo maximo
	int bloqueantes = params[0];
	int minimo = params[1];
	int maximo = params[2];
	int azar;
//Por cada bloqueo que tiene que hacer, tomo un numero random
//aplico % (maximo - minimo + 1) (numero entre 0 y maximo-minimo)
//le sumo minimo (numero entre minimo y maximo)
	for (int i = 0; i < bloqueantes; ++i){
		srand((unsigned) time(NULL));
		azar = rand() % (maximo - minimo + 1) + minimo;
		uso_IO(pid, azar);
	}
}

void TaskBatch(int pid, vector<int> params){
//Recibo tiempo total de ejecucion (incluyendo llamada bloqueante) y cantidad de bloqueos a realizar
	int total_cpu = params[0];
	int cant_bloqueos = params[1];
	set<int> azar;
	srand((unsigned) time(NULL));
	while(azar.size() < cant_bloqueos){
		azar.insert(rand() % total_cpu);
	}
//si estoy en el momento de bloquear, bloqueo, sino, hago uso intensivo del cpu
	for (int i = 0; i < total_cpu; ++i) {
		if (azar.count(i) != 0) {
			uso_IO(pid,1);
		} else {
			uso_CPU(pid,1);
		}
	}
}

void tasks_init(void) {
	/* Todos los tipos de tareas se deben registrar acá para poder ser usadas.
	 * El segundo parámetro indica la cantidad de parámetros que recibe la tarea
	 * como un vector de enteros, o -1 para una cantidad de parámetros variable. */
	register_task(TaskCPU, 1);
	register_task(TaskIO, 2);
	register_task(TaskAlterno, -1);
	register_task(TaskConBloqueo, 3);
	register_task(TaskConsola, 3);
	register_task(TaskBatch, 2);
}
