#include <iostream>
#include <iomanip>
#include <malloc.h>
#include <sstream>

#ifdef _MSVC_LANG
#include <windows.h>
#endif

#include "ProcessEmulation.h"

using namespace std;

const int SUCCESS = 0;
const int ERR_NO_RESOURCE = 1;
const int ERR_NO_JOBS = 2;

const int execTime = 6;
const int K = 6, M = 3, N = 4;
static int pidCount = 0;
const ResourceSemaphore resProcess = (ResourceSemaphore)malloc(sizeof(ResourceSemaphoreEntity));
const ResourceSemaphore resProcessTyp1= (ResourceSemaphore)malloc(sizeof(ResourceSemaphoreEntity));
const ResourceSemaphore resProcessTyp2 = (ResourceSemaphore)malloc(sizeof(ResourceSemaphoreEntity));
const ResourceSemaphore resTimeTyp1 = (ResourceSemaphore)malloc(sizeof(ResourceSemaphoreEntity));
const ResourceSemaphore resTimeTyp2 = (ResourceSemaphore)malloc(sizeof(ResourceSemaphoreEntity));

void initResources()
{
	resProcess->maximumCount = K;
	resProcess->currentCount = resProcess->maximumCount;
	resProcess->minAllocCount = 1;
	resProcess->paramCount = 0;
	resProcess->exrtaParams = NULL;
	resProcessTyp1->maximumCount = M;
	resProcessTyp1->currentCount = resProcessTyp1->maximumCount;
	resProcessTyp1->minAllocCount = 1;
	resProcessTyp1->paramCount = 0;
	resProcessTyp1->exrtaParams = NULL;
	resProcessTyp2->maximumCount = N;
	resProcessTyp2->currentCount = resProcessTyp2->maximumCount;
	resProcessTyp2->minAllocCount = 1;
	resProcessTyp2->paramCount = 0;
	resProcessTyp2->exrtaParams = NULL;
	resTimeTyp1->maximumCount = 1;
	resTimeTyp1->currentCount = resTimeTyp1->maximumCount;
	resTimeTyp1->minAllocCount = 1;
	resTimeTyp1->paramCount = 0;
	resTimeTyp1->exrtaParams = NULL;
	resTimeTyp2->maximumCount = 1;
	resTimeTyp2->currentCount = resTimeTyp2->maximumCount;
	resTimeTyp2->minAllocCount = 1;
	resTimeTyp2->paramCount = 0;
	resTimeTyp2->exrtaParams = NULL;
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
	resetTime(resTimeTyp1);
	resetTime(resTimeTyp2);
	int finishedJobsTyp1 = 0, finishedJobsTyp2 = 0;

	// collecting data for visualization
	vector<Process> statusTyp1, statusTyp2;
	for(int a = 0; a < execTime; ++a)
		statusTyp1.push_back(NULL), statusTyp2.push_back(NULL);

	for(vector<Process>::iterator it = procs.begin(); it != procs.end(); it++)
	{
		switch((*it)->status)
		{
			case created:
				(*it)->status = ready;
				break;
			case ready:
				if(acquireThread(resProcess, 1) == SUCCESS)
				{
					if((*it)->type == typ1)
					{
						if(acquireTime(resTimeTyp1, 1) == SUCCESS)
						{
							if(acquireThread(resProcessTyp1, 1) == SUCCESS)
									(*it)->status = executing;
							else
							{
								releaseTime(resTimeTyp1, 1);
								releaseThread(resProcess, 1);
								break;
							}
						}
						else
						{
							releaseThread(resProcess, 1);
							break;
						}
					}
					else if((*it)->type == typ2)
					{
						if(acquireTime(resTimeTyp2, 1) == SUCCESS)
						{
							if(acquireThread(resProcessTyp2, 1) == SUCCESS)
								(*it)->status = executing;
							else
							{
								releaseTime(resTimeTyp2, 1);
								releaseThread(resProcess, 1);
								break;
							}
						}
						else
						{
							releaseThread(resProcess, 1);
							break;
						}
					}
				}
				else break;
#ifdef LOGGING
						cout << "[LAUNCH] " << describeProcess(*it) << " launched" << endl;
#endif
			case executing:
				// collecting data for visualization
				if((*it)->type == typ1)
					statusTyp1[execTime - (*it)->remainingTime] = *it;
				else
					statusTyp2[(*it)->remainingTime - 1] = *it;

				(*it)->remainingTime -= 1;

				if((*it)->remainingTime <= 0)
				{
					(*it)->status = terminated;
#ifdef LOGGING
					cout << "[FINISH] " << describeProcess(*it) << " finished" << endl;
#endif
					switch((*it)->type)
					{
						case typ1: ++finishedJobsTyp1; break;
						case typ2: ++finishedJobsTyp2; break;
					}
				}
				break;
		}
	}

	releaseThread(resProcessTyp1, finishedJobsTyp1);
	releaseThread(resProcessTyp2, finishedJobsTyp2);
	releaseThread(resProcess, finishedJobsTyp1 + finishedJobsTyp2);

	// visualization
	for(vector<Process>::iterator it1 = statusTyp1.begin(), it2 = statusTyp2.begin();
		it1 != statusTyp1.end(); ++it1, ++it2)
		cout << animateProcesses(*it1, *it2);
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

	return finishedJobsTyp1 + finishedJobsTyp2;
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
		strout << setw(7) << "";
	else if(p->type == typ1)
		strout << setiosflags(ios::left) << setw(5) << p->pid << "->";
	else if(p->type == typ2)
		strout << "<-" << setiosflags(ios::right) << setw(5) << p->pid;
	strout << ']';
	return strout.str();
}

string animateProcesses(Process p1, Process p2)
{
	if(p1 == NULL) return animateProcess(p2);
	else if(p2 == NULL) return animateProcess(p1);
	else
	{
		ostringstream strout(ostringstream::ate);
		strout << '[';
		strout << setiosflags(ios::left) << setw(3) << p1->pid;
		strout << '|';
		strout << setiosflags(ios::right) << setw(3) << p2->pid;
		strout << ']';
		return strout.str();
	}
}
