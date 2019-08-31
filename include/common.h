#ifndef _COMMON_H_INCLUDED_
#define _COMMON_H_INCLUDED_

#define MAX_HTTP_REQUEST_SIZE (1024 * 1024)

#define HTTP_HEADER_BLANK " "
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length"
#define HTTP_HEADER_DOUBLE_NEW_LINES "\r\n\r\n"

struct Manager
{
    void *fp;
};

struct HttpRequest
{
    const char *request;
};

struct HttpResponse
{
    int content_length;
    const char *content;
};

struct Task
{
    void *fp;
    int offset;
    struct HttpRequest request;
};

#endif