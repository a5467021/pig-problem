#include <iostream>
#include <queue>
#include <vector>

#ifdef _MSVC_LANG
#include <windows.h>
#endif

#include "ProcessEmulation.h"

using namespace std;

int main()
{
	unsigned int seed = 0;
	srand(seed);

	initResources();
	Process newProcess = NULL;
	vector<Process> procs;  // processes that are being executed as soon as possible
	queue<Process> waitingProcs;  // processes that don't meet the requirements for running
	int ticks = 0, yieldingTicks = 600;

	do {
		if(ticks < yieldingTicks)
		{
			newProcess = randomRequest();
			if(newProcess != NULL)
			{
				// if(!waitingProcs.empty()) cout << describeProcess(waitingProcs.front()) << endl;
				
				waitingProcs.push(newProcess);
				
#ifdef LOGGING
				cout << "[CREATE] " << describeProcess(newProcess)
					<<" created @ tick " << ticks << endl;
#endif
			}
		}

		// block the succeeding jobs from running if the first-coming job isn't launched
		if(!waitingProcs.empty() && waitingProcs.front()->status == executing)
		{
			waitingProcs.pop();
			if(!waitingProcs.empty())
				procs.push_back(waitingProcs.front());
		}
		// launch the first-coming job
		if(procs.empty())
			if(!waitingProcs.empty())
				procs.push_back(waitingProcs.front());

		int programsFinished = nextJiffle(procs);
#ifdef LOGGING
		if(programsFinished)
			cout << "[ INFO ] " << programsFinished
			<< " processes finished @ tick " << ticks << endl;
#endif

#ifdef _MSVC_LANG
		Sleep(50);
#endif
	} while(ticks++ < yieldingTicks || !procs.empty());

	nextJiffle(procs);
	
	return 0;
}