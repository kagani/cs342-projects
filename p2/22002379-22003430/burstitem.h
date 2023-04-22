#ifndef BURSTITEM_H
#define BURSTITEM_H

struct BurstItem {
    int pid; // starts w,th 1 and increases sequentially
    int burstLength;
    int arrivalTime; // timestamp before adding to the queue
    int remainingTime;
    int finishTime;
    int turnaroundTime;
    int processorId;
};

#endif