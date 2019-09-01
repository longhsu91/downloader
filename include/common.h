#ifndef _COMMON_H_INCLUDED_
#define _COMMON_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bool int
#define true 1
#define false 0

#define MAX_HTTP_REQUEST_SIZE (1024 * 1024)

#define HTTP_HEADER_BLANK " "
#define HTTP_HEADER_ACCEPT_RANGES "Accept-Ranges"
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length"

#define HTTP_BODY_CONTENT "\r\n\r\n"

#define SEGMENT_SIZE 1000

enum HttpHeaderKey
{
    ACCEPT_RANGES,
    CONTENT_LENGTH,
};

struct Segment
{
    int index;
    int begin;
    int end;
};

void local_write_to_disk(FILE *fp, int offset, const char *buf, int length);
char *d_strstr(const char *buf, const char *needle, int *_pos);

#endif