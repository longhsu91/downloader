#include "manager.h"
#include "task.h"

void calSegment(struct Manager *manager, int total_length)
{
    if (total_length <= 0)
    {
        return;
    }

    int n = total_length / SEGMENT_SIZE;
    int r = total_length % SEGMENT_SIZE;
    if (r != 0)
    {
        n += 1;
    }

    struct Segment *segment = (struct Segment *)malloc(sizeof(struct Segment) * n);

    int i = 0;
    for (i = 0; i < n - 1; i++)
    {
        segment[i].index = i;
        segment[i].begin = i * SEGMENT_SIZE;
        segment[i].end = i * SEGMENT_SIZE + SEGMENT_SIZE - 1;

        //printf("segment %d: [%d, %d]\n", segment[i].index, segment[i].begin, segment[i].end);
    }

    segment[i].index = i;
    segment[i].begin = i * SEGMENT_SIZE;
    segment[i].end = i * SEGMENT_SIZE + r - 1;
    //printf("segment %d: [%d, %d]\n", segment[i].index, segment[i].begin, segment[i].end);

    manager->segment = segment;
    manager->segment_count = n;
}

void initTask(struct Manager *this)
{
    struct Task *tasklist = (struct Task *)malloc(sizeof(struct Task) * (this->segment_count));

    for (int i = 0; i < this->segment_count; i++)
    {
        NewTask(&tasklist[i]);
        tasklist[i].init(&tasklist[i], this->fp, &(this->segment[i]));
    }

    this->tasklist = tasklist;
}

void startTask(struct Manager *this)
{
    for (int i = 0; i < this->segment_count; i++)
    {
        this->tasklist[i].run(&(this->tasklist[i]));
    }
}

void freeManager(struct Manager *this)
{
    if (this != NULL)
    {
        if (this->tasklist != NULL)
        {
            free(this->tasklist);
            this->tasklist = NULL;
        }

        if (this->segment != NULL)
        {
            free(this->segment);
            this->segment = NULL;
        }
    }
}