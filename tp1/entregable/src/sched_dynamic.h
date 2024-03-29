#ifndef __SCHED_DYNAMIC_
#define __SCHED_DYNAMIC_

#include <vector>
#include <queue>
#include "basesched.h"

using namespace std;

class SchedDynamic : public SchedBase {
	public:
		SchedDynamic(std::vector<int> argn);
        ~SchedDynamic();
		virtual void initialize();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		unsigned int tick_actual;
		struct datos{
			int pid;
			int deadline;
			bool operator< (const datos& otro) const{
				return otro.deadline <= deadline;
			}
		};
		priority_queue<datos> pq;
		datos actual;
		bool primera;
};

#endif
