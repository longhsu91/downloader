#ifndef _MANAGER_INCLUDE_
#define _MANAGER_INCLUDE_

#include "common.h"

struct Manager
{
    void *fp;         // file descriptor to write
    int total_length; // the length of download task

    int segment_count; // the number of segments
    struct Segment *segment;

    struct Task *tasklist;
};

void calSegment(struct Manager *this, int total_length);

void initTask(struct Manager *this);
void startTask(struct Manager *this);

void freeManager(struct Manager *this);

#endif