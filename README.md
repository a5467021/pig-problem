# pig-problem
An interesting problem of Operating System on process scheduling. As homework.

The problem is described as:

> Some pigs are trying to cross a ravine. A single rope traverses the ravine, and pigs can cross hand-over-hand. Up to five pigs can be hanging on the rope at any one time. if there are more than five, then the rope will break and they will all die. Also, if eastward-moving pigs encounter westward moving pigs, all will fall off and die.
> 
> Each pig operates in a separate thread, which executes the code below:
> 
> ```C
> typedef enum {EAST, WEST} Destination ;
> void pig(int id, Destination dest) {
>  WaitUntilSafeToCross(dest);
>  CrossRavine(id, dest);
>  DoneWithCrossing(dest);
> }
> ```
> 
> Variable id holds a unique number identifying that pig. CrossRavine(int pigid, Destination d) is a synchronous call provided to you, and it returns when the calling pig has safely reached its destination. Use semaphores to implement `WaitUntilSafeToCross(Destination d)` and `DoneWithCrossing(Destination d)`.
> 
> (a) For Part (a) You implementation should ensure that:  
i) At most 5 pigs simultaneously execute `CrossRavine()`;  
ii) All pigs executing in `CrossRavine()` are heading in the same direction;  
iii) No pig should wait unnecessarily. (This way, you can maximize the throughput)
> 
> (b) Instead of maximizing the throughput, your solution for Part (b) should ensure there is no starvation (the first two conditions in part (a) still apply). That is, no pig should wait at the ravine forever. You can assume that semaphore's wait queue is FIFO queue.
> 
> \(c) Now instead of pigs, we have students crossing the ravine. Because students are much smarter than pigs, assume that students heading different directions CAN be on the rope at same time. Write a semaphore implementation that ensures the following requirements.  
i) At most M eastward-moving students simultaneously execute `CrossRavine()`;  
ii) At most N westward-moving students simultaneously execute `CrossRavine()`;  
iii) At most K students simultaneously execute `CrossRavine()`;  
iv) you solution should maximize the throughput under above conditions.
