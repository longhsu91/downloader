#include "http_request.h"

#include <sys/socket.h>

extern int headerGetValue(const char *httpRawData, int key, char **value);

const char *PRE_HEADER = "GET /img/bd_logo1.png HTTP/1.1\r\n\
Host: www.baidu.com\r\n\
User-Agent: curl/7.47.0\r\n\
Accept: */*\r\n";

void NewHttpRequest(struct HttpRequest **httpRequest)
{
    struct HttpRequest *request = (struct HttpRequest *)malloc(sizeof(struct HttpRequest));

    request->_this = request;
    request->pre_header = PRE_HEADER;

    request->setRanges = setRanges;
    request->toString = toString;
    request->freeRequest = freeRequest;

    *httpRequest = request;
}

void setRanges(struct HttpRequest *this, int begin, int end)
{
    this->ranges.begin = begin;
    this->ranges.end = end;
}

const char *toString(struct HttpRequest *this)
{
    int begin = this->ranges.begin;
    int end = this->ranges.end;
    char ranges_str[128];

    if (end > 0)
    {
        sprintf(ranges_str, "Range: bytes=%d-%d\r\n", begin, end);
    }
    else
    {
        sprintf(ranges_str, "Range: bytes=%d-\r\n", begin);
    }

    //printf("ranges_str is [%s]\n", ranges_str);

    int size = strlen(this->pre_header) + strlen(ranges_str) + strlen("\r\n") + 1;
    char *buf = (char *)malloc(sizeof(char) * size);

    memset(buf, 0, size);
    memcpy(buf, this->pre_header, strlen(this->pre_header));
    memcpy(buf + strlen(this->pre_header), ranges_str, strlen(ranges_str));
    memcpy(buf + strlen(this->pre_header) + strlen(ranges_str), "\r\n", 2);
    buf[size] = '\0';

    printf("buf is [%s]", buf);

    this->request = buf;

    return buf;
}

void freeRequest(struct HttpRequest *this)
{
    if (this != NULL)
    {
        if (this->request != NULL)
        {
            free(this->request);
            this->request = NULL;
        }

        if (this->_this != NULL)
        {
            free(this->_this);
            this->_this = NULL;
        }
        this = NULL;
    }
}

/**
 * response should be free after use
 */
int http_request_do(int socket_fd, const char *request, char **response)
{
    char *buf = (char *)malloc(MAX_HTTP_REQUEST_SIZE * sizeof(char));
    char *find = NULL;
    char *p = buf;

    int n = 0;
    int pos = 0;
    int nread = 0;

    char *value = NULL;
    int length = 0;

    // send request to server
    send(socket_fd, request, strlen(request), 0);

    /**
     * http connnection is set keep-alive by default
     * here we should get total length to git rid of I/O block
     */
    // get header inder to get content_length
    while ((n = recv(socket_fd, p, 1024, 0)) > 0)
    {
        p = p + n;
        nread += n;

        if ((find = d_strstr(buf, "\r\n\r\n", &pos)) != NULL)
        {
            break;
        }
    }

    // find content_length
    headerGetValue(buf, CONTENT_LENGTH, &value);
    length = atoi(value);
    free(value);

    // get left content
    int nleft = length - (nread - (pos + 4));
    while (nleft > 0)
    {
        if ((n = recv(socket_fd, p, 1024, 0)) > 0)
        {
            nleft -= n;
            p += n;
            nread += n;
        }
    }

    //printf("nread is %d, nleft is %d\n", nread, nleft);

    *response = buf;

    return 0;
}