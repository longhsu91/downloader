#include "common.h"
#include "manager.h"

int main(int argc, char **argv)
{
    FILE *fp;
    fp = fopen("/home/longx/logo.png", "w+");

    struct Manager *manager = (struct Manager *)malloc(sizeof(struct Manager));
    calSegment(manager, 7877);
    manager->fp = fp;

    initTask(manager);
    startTask(manager);

    freeManager(manager);
    free(manager);

    fclose(fp);

    return 0;
}