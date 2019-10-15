#include <iostream>
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
	vector<Process> procs;
	int ticks = 0, yieldingTicks = 600;

	do {
		if(ticks < yieldingTicks)
		{
			Process newProcess = randomRequest();
			if(newProcess != NULL)
			{
				procs.push_back(newProcess);
#ifdef LOGGING
				cout << "[CREATE] " << describeProcess(newProcess)
					<<" created @ tick " << ticks << endl;
#endif
			}
		}

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