# Solution for Part A
This part implements the problem A. From the description we can extract these information:

1. There is one method of executing the programs, and only once in a time.
2. Every executing program can run simultaneously, and the concurrency of running processes are limited.
3. There are two types of processes, and processes of different kinds cannot be executed at the same time.

Info 1 means there is a constraint that only one program can be executed at a minimal time unit. This can be considered as a critical resource, and I name it `time`.

Additionally, info 2 indicates that all process require a same critical resource, and I name it `process` here. A program that does'n turn into a process cannot be executed.

Info 3 implies that the category of processes allowed to execute is also a critical resource, and I call it `type`.

Therefore, I designed three *semaphores* in this problem, namingly `time`, `process` and `type`. The amount of `time` and `type` is set to 1 according to the situation as illustrated above, and the amount of `process` is determined by the situation. In this problem, it is 5. The mapping of variables in the description and my program can be listed as follows:

|in Problem Description|in My Program|
|:--------------------:|:-----------:|
|pig|process|
|rope|processor|
|function `pig`|the whole program|
|enum `Destination`|enum `ProcessType`|
|`WaitUntilSafeToCross()`|all `acquire...` functions related to semaphore|
|`CrossRavine()`|`NextJiffle()`|
|`DoneWithCrossing()`|all `release...` functions related to semaphore|

The `acquire...` and `release...` functions mentioned here are my implementations of `wait()` and `signal()`. I do not use multithreading to *emulate* the process; rather, I *simulate* with virtual time ticks.

In order to achieve maximum throughput, process launching policy is set to "lauch any program if it is available". To implement this, I employed a simple list, adding all launching requests to it, iterating over the list in every time tick and launch all available programs.

My definition of a semaphore is a C struct:

```C
typedef struct ResourceSemaphoreEntity {
	char description[20];
	int maximumCount, currentCount;
	int minAllocCount;		// minimum amount of resource allocated in one action
	int *exrtaParams, paramCount;
} *ResourceSemaphore;
```

Where `maximumCount`, `currentCount` and `minAllocCount` are necessary properties of a semaphore. Semaphore manipulations are written as sepatared functions prefixed with `acquire`, `release` and so on.

The general routine of execution is like this:

```C
void WaitUntilSafeToCross()
{
	waitDirection();	// acquireType()
	waitWeight();		// acquireProcess()
	waitEntry();		// acquireTime()
}

void CrassRavine()
{
	// just cross the ravine
	...
}

void DoneWithCrossing()
{
	if(no_pigs_on_rope) signalDirection();	// releaseType()
	signalWeight();							// releaseProcess()
	signalEntry();							// releaseTime()
}
```

The program automatically generates jobs with `rand()` function and any desired seed to simulate all possible situations. The output will show the current "running pigs" like this:

![](example-output.png)

Changeable parameters:

|Name|Usage|Location|
|:--:|:---:|:------:|
|`seed`|seed for rand()|main.cpp|
|`yieldingTicks`|in how many ticks should processes be generated|main.cpp|
|`execTime`|how long will a process execute|ProcessEmulation.cpp|
|`resProcess->maximumCount`|maximum processes that can be executed together|ProcessEmulation.cpp|
