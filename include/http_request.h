#ifndef _HTTP_REQUEST_INCLUDE_
#define _HTTP_REQUEST_INCLUDE_

#include "common.h"

// const char *request1 = "GET /img/bd_logo1.png HTTP/1.1\r\n
// Host: www.baidu.com\r\n
// User-Agent: curl/7.47.0\r\n
// Range: bytes=0-1024\r\n
// Accept: */*\r\n
// \r\n";

// const char *request2 = "GET /img/bd_logo1.png HTTP/1.1\r\n
// Host: www.baidu.com\r\n
// User-Agent: curl/7.47.0\r\n
// Accept: */*\r\n
// Range: bytes=1025-\r\n
// \r\n";

struct HttpRequest;

typedef void (*PSetRanges)(struct HttpRequest *this, int begin, int end);
typedef const char *(*PToString)(struct HttpRequest *this);
typedef void (*PFreeRequest)(struct HttpRequest *this);

void NewHttpRequest(struct HttpRequest **httpRequest);

void setRanges(struct HttpRequest *this, int begin, int end);
const char *toString(struct HttpRequest *this);
void freeRequest(struct HttpRequest *this);

struct HttpRequestRange
{
    int begin;
    int end;
};

struct HttpRequest
{
    // private field
    struct HttpRequest *_this; // point to ifself
    const char *pre_header;    // genernal header key

    char *request;
    struct HttpRequestRange ranges;

    // func
    PSetRanges setRanges;

    PToString toString;

    PFreeRequest freeRequest;
};

/**
 * response should be free after use
 */
int http_request_do(int socket_fd, const char *request, char **response);

#endif