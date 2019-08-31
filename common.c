#include "common.h"

void local_write_to_disk(FILE *fp, int offset, const char *buf, int length)
{
    int cur = fseek(fp, offset, SEEK_SET);
    fwrite(buf, length, 1, fp);
    fflush(fp);
}

char *d_strstr(const char *buf, const char *needle, int *_pos)
{
    int pos = 0;
    char *p = (char *)buf;

    while (*p != '\0')
    {
        if (memcmp(p, needle, strlen(needle)) == 0)
        {
            *_pos = pos;
            return p;
        }

        p++;
        pos++;
    }

    *_pos = 0;
    return NULL;
}