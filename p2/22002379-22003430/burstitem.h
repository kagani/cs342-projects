#ifndef BURSTITEM_H
#define BURSTITEM_H

typedef struct
{
    int pid;         // [1, INF)
    int burstLength; // process length
    int arrivalTime; // Timestamp of process arrival
    int remainingTime;
    int finishTime;
    int turnaroundTime;
    int processorId;
} BurstItem;

#endif