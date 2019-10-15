# Solution for Part B
This part implements the problem B. Compared to part A, the difference is the policy for process launching. In part A, processes are required to launch as early as possible. This will increate the throughput while at the cost of letting some processes waiting indefinitely. To avoid this in this part, I decided to lower the throughput and use a queue to ensure that all processes are executed with a predictable maximum latency. Of course, this original method will not be the optimal one but surely will work.

Under new requirements, new processes will not be directly added to execution list. Instead, they will be pushed into a queue first. Every tick, the processing list and the waiting queue will be checked to ensure "the *should* is executing, and the *shouldn't* is waiting".

As in this situation executing process won't turn back to the status of ready, the policy can be set to: if the process at the queue front is executing, release it and check whether next process can be executed. The logic would look like this pseudo-C++ expression:

```Pseudo-C++
queue_of_processes waiting_process;

...

if(waiting_process.front().status != created
    && waiting_process.front().status != ready)
{
    waiting_proces.pop();
    if(!waiting_proces.empty())
        dispatch_process(waiting_process.front());
}
```

This will ensure that all processes will still be excuted as early as possible, but under a prime rule that all processes should be launched sequentially in general.

Using the same seed as in part A, it looks like:

![](example-output.png)

It is clear that process (PID 5) that is executed after process (PID 2) in part A now has to wait for process (PID 3, 4) to finish.
