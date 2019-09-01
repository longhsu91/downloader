#ifndef _TASK_INCLUDE_
#define _TASK_INCLUDE_

#include "common.h"

struct HttpRequest;
struct Task;

typedef void (*PInit)(struct Task *this, void *fp, struct Segment *segment);
typedef void (*PRun)(struct Task *this);


void NewTask(struct Task *task);
void init(struct Task *this, void *fp, struct Segment *segment);
void run(struct Task *this);

struct Task
{
    pthread_t thread;
    void *fp;
    int offset;

    struct Segment *segment;
    struct HttpRequest *request;

    // func
    PInit init;
    PRun run;
};

#endif