#pragma once

#include <string>
#include <vector>

// #define LOGGING

using namespace std;

enum ProcessType {typ1, typ2};
enum ProcessStatus {created, ready, executing, waiting, terminated};

typedef struct ProcessEntity {
	ProcessType type;
	ProcessStatus status;
	int pid;
	int remainingTime;
} *Process;

typedef struct ResourceSemaphoreEntity {
	char description[20];
	int maximumCount, currentCount;
	int minAllocCount;		// minimum amount of resource allocated in one action
	int *exrtaParams, paramCount;
} *ResourceSemaphore;

void initResources();

Process randomRequest();

int nextJiffle(vector<Process> &procs);

string describeProcess(Process p);
string animateProcess(Process p);
string animateProcesses(Process p1, Process p2);
