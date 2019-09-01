#ifndef _HTTP_RESPONSE_INCLUDE_
#define _HTTP_RESPONSE_INCLUDE_

#include "common.h"

struct HttpResponse;

typedef void (*PSetRaw)(struct HttpResponse *this, const char *httpRawData);
typedef void (*PSetAcceptRanges)(struct HttpResponse *this, const char *acceptRanges);
typedef bool (*PGetAcceptRanges)(struct HttpResponse *this);
typedef void (*PSetContentLength)(struct HttpResponse *this, const char *contentLength);
typedef int (*PGetContentLength)(struct HttpResponse *this);

typedef void (*PSetContent)(struct HttpResponse *this, const char *content);
typedef const char *(*PGetContent)(struct HttpResponse *this);

typedef int (*PDump)(struct HttpResponse *this);
typedef int (*PParse)(struct HttpResponse *this);
typedef void (*PFreeResponse)(struct HttpResponse *this);

void NewHttpResponse(struct HttpResponse **httpResponse);

void setRaw(struct HttpResponse *this, const char *httpRawData);
void setAcceptRanges(struct HttpResponse *this, const char *acceptRanges);
bool getAcceptRanges(struct HttpResponse *this);
void setContentLength(struct HttpResponse *this, const char *contentLength);
int getContentLength(struct HttpResponse *this);

void setContent(struct HttpResponse *this, const char *content);
const char *getContent(struct HttpResponse *this);

int dump(struct HttpResponse *this);
int parse(struct HttpResponse *this);
void freeResponse(struct HttpResponse *this);

struct HttpResponse
{
    // private field
    struct HttpResponse *_this; // point to itself
    const char *raw;            // point to http response raw data

    bool accept_ranges;
    int content_length;

    const char *content;

    // func
    PSetRaw setRaw;
    PSetAcceptRanges setAcceptRanges;
    PGetAcceptRanges getAcceptRanges;
    PSetContentLength setContentLength;
    PGetContentLength getContentLength;
    PGetContent getContent;
    PSetContent setContent;

    PDump dump;
    PParse parse;
    PFreeResponse freeResponse;
};

// header
const char *headerKeyToStr(int key);

/**
 * value should be free after use
 */
int headerGetValue(const char *httpRawData, int key, char **value);

#endif