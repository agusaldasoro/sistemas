#ifndef __SCHED_FIXED_
#define __SCHED_FIXED_

#include <vector>
#include <queue>
#include <vector>
#include "basesched.h"

using namespace std;

class SchedFixed : public SchedBase {
	public:
		SchedFixed(std::vector<int> argn);
        ~SchedFixed();
		virtual void initialize();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		struct datos{
			int pid;
			int periodo;
			bool operator< (const datos& otro) const{
				return otro.periodo <= periodo;
			}
		};
		vector<int> periodos;
		priority_queue<datos> pq;
};

#endif
