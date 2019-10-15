#include <iostream>
#include <iomanip>
#include <malloc.h>
#include <sstream>

#include "ProcessEmulation.h"

using namespace std;

const int SUCCESS = 0;
const int ERR_NO_RESOURCE = 1;
const int ERR_NO_JOBS = 2;

const int execTime = 6;
static int pidCount = 0;
const ResourceSemaphore resThread = (ResourceSemaphore)malloc(sizeof(ResourceSemaphoreEntity));
const ResourceSemaphore resTime = (ResourceSemaphore)malloc(sizeof(ResourceSemaphoreEntity));

void initResources()
{
	resThread->maximumCount = 5;
	resThread->currentCount = 5;
	resThread->minAllocCount = 1;
	resThread->paramCount = 0;
	resThread->exrtaParams = NULL;
	resTime->maximumCount = 1;
	resTime->currentCount = 1;
	resTime->minAllocCount = 1;
	resTime->paramCount = 0;
	resTime->exrtaParams = NULL;
}

int genPid()
{
	pidCount += 1;
	return pidCount - 1;
}

int acquireThread(const ResourceSemaphore &S, int requiredResCount)
{
	// ensure resources are allocated in "atoms"
	if(requiredResCount % S->minAllocCount != 0)
		requiredResCount = (requiredResCount / S->minAllocCount + 1) * S->minAllocCount;

	if(S->currentCount < requiredResCount) return ERR_NO_RESOURCE;
	else
	{
		S->currentCount -= requiredResCount;
		return SUCCESS;
	}
}

int releaseThread(const ResourceSemaphore &S, int acquiredResCount)
{
	S->currentCount += acquiredResCount;
	return SUCCESS;
}

int acquireTime(const ResourceSemaphore &S, int requiredResCount)
{
	// ensure resources are allocated in "atoms"
	if(requiredResCount % S->minAllocCount != 0)
		requiredResCount = (requiredResCount / S->minAllocCount + 1) * S->minAllocCount;

	if(S->currentCount < requiredResCount) return ERR_NO_RESOURCE;
	else
	{
		S->currentCount -= requiredResCount;
		return SUCCESS;
	}
}

int releaseTime(const ResourceSemaphore &S, int resCount)
{
	S->currentCount += resCount;
	return SUCCESS;
}

int resetTime(const ResourceSemaphore &S)
{
	S->currentCount = S->maximumCount;
	return SUCCESS;
}

Process randomRequest()
{
	Process p = (Process)malloc(sizeof(ProcessEntity));
	if(p == NULL) return NULL;

	int selection = rand() % 100;

	if(selection < 30)
		p->type = typ1;
	else if(selection > 70)
		p->type = typ2;
	else
	{
		free(p);
		return NULL;
	}

	p->pid = genPid();
	p->remainingTime = execTime;
	p->status = ready;

	return p;
}

int nextJiffle(vector<Process> &procs)
{
	resetTime(resTime);
	int finishedJobs = 0;

	// collecting data for visualization
	vector<Process> statusBar;
	for(int a = 0; a < execTime; ++a) statusBar.push_back(NULL);

	for(vector<Process>::iterator it = procs.begin(); it != procs.end(); it++)
	{
		switch((*it)->status)
		{
			case created:
				(*it)->status = ready;
				break;
			case ready:
				if(acquireTime(resTime, 1) == SUCCESS)
				{
					if(acquireThread(resThread, 1) == SUCCESS)
					{
						(*it)->status = executing;
#ifdef LOGGING
						cout << "[LAUNCH] " << describeProcess(*it) << " launched" << endl;
#endif
					}
					else
					{
						releaseTime(resTime, 1);
						break;
					}
				}
				else break;
			case executing:
				// collecting data for visualization
				if((*it)->type == typ1)
					statusBar[execTime - (*it)->remainingTime] = *it;
				else
					statusBar[(*it)->remainingTime - 1] = *it;

				(*it)->remainingTime -= 1;

				if((*it)->remainingTime <= 0)
				{
					(*it)->status = terminated;
#ifdef LOGGING
					cout << "[FINISH] " << describeProcess(*it) << " finished" << endl;
#endif
					++finishedJobs;
				}
				break;
		}
	}

	releaseThread(resThread, finishedJobs);

	// visualization
	for(vector<Process>::iterator it = statusBar.begin(); it != statusBar.end(); ++it)
		cout << animateProcess(*it);
	cout << endl;

	// garbage collecting
	{
		vector<Process>::iterator it = procs.begin();
		while(it != procs.end())
		{
			while(it != procs.end() && (*it)->status == terminated)
			{
				free(*it);
				it = procs.erase(it);
			}
			if(it != procs.end()) ++it;
		}
	}

	return finishedJobs;
}

string describeProcess(Process p)
{
	ostringstream strout(ostringstream::ate);
	strout.str("Process (");
	strout << "PID " << p->pid << ", ";
	strout << "type " << p->type + 1;
	strout << ")";
	return strout.str();
}

string animateProcess(Process p)
{
	ostringstream strout(ostringstream::ate);

	strout << '[';
	if(p == NULL)
		strout << setw(6) << "";
	else if(p->type == typ1)
		strout << setiosflags(ios::left) << setw(4) << p->pid << "->";
	else if(p->type == typ2)
		strout << "<-" << setiosflags(ios::right) << setw(4) << p->pid;
	strout << ']';
	return strout.str();
}
