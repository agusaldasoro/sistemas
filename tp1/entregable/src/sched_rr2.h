#ifndef __SCHED_RR2__
#define __SCHED_RR2__

#include <vector>
#include <queue>
#include "basesched.h"

using namespace std;

class SchedRR2 : public SchedBase {
	public:
		SchedRR2(std::vector<int> argn);
        ~SchedRR2();
		virtual void initialize() {};
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		int cores;
		vector<int> quantums;
		vector<int> quantums_timer;
		vector<int> actuales;
		int siguiente;
		vector<pair<int, queue<int, deque<int> >* > > colas;
		int next(int cpu, const enum Motivo m);
};

#endif
