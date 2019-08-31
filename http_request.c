#include "http_request.h"

#include <sys/socket.h>

extern int http_response_header_get_value(const char *httpRawData, int key, char **value);

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
    http_response_header_get_value(buf, CONTENT_LENGTH, &value);
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