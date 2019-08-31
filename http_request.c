#include "http_request.h"

#include <sys/socket.h>

/**
 * response should be free after use
 */
int http_request_do(int socket_fd, const char *request, char **response)
{
    char *buf = (char *)malloc(MAX_HTTP_REQUEST_SIZE * sizeof(char));
    char *find = NULL;
    char *p = buf;

    int i = 0;
    int j = 0;
    char length[10];

    int n = 0;
    int pos = 0;
    int nread = 0;

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
    find = strstr(buf, HTTP_HEADER_CONTENT_LENGTH);
    find = strstr(find, HTTP_HEADER_BLANK);

    while (find[i] != '\r')
    {
        if (find[i] == ' ')
        {
            i++;
            continue;
        }

        length[j++] = find[i++];
    }
    length[j] = '\0';

    // get left content
    int nleft = atoi(length) - (nread - (pos + 4));
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