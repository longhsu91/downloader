#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

const char *request1 = "GET /img/bd_logo1.png HTTP/1.1\r\n\
Host: www.baidu.com\r\n\
User-Agent: curl/7.47.0\r\n\
Range: bytes=0-1024\r\n\
Accept: */*\r\n\
\r\n";

const char *request2 = "GET /img/bd_logo1.png HTTP/1.1\r\n\
Host: www.baidu.com\r\n\
User-Agent: curl/7.47.0\r\n\
Accept: */*\r\n\
Range: bytes=1025-\r\n\
\r\n";

static void write_to_disk(FILE *fp, int offset, const char *buf, int length)
{
    int cur = fseek(fp, offset, SEEK_SET);
    fwrite(buf, length, 1, fp);
    fflush(fp);
}

char *dl_strstr(const char *buf, const char *needle, int *_pos)
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

/**
 * response should be free after use
 */
static int http_do_request(int socket_fd, const char *request, char **response)
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

        if ((find = dl_strstr(buf, "\r\n\r\n", &pos)) != NULL)
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

    printf("nread is %d, nleft is %d\n", nread, nleft);

    *response = buf;

    return 0;
}

static int http_parse_response(const char *httpRawData, struct HttpResponse *httpResponse)
{
    char *find = NULL;
    char length[10];
    int i = 0;
    int j = 0;

    // get content_length
    find = strstr(httpRawData, HTTP_HEADER_CONTENT_LENGTH);
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

    // get content
    find = strstr(httpRawData, HTTP_HEADER_DOUBLE_NEW_LINES);
    find += 4;

    httpResponse->content_length = atoi(length);
    httpResponse->content = find;

    return 0;
}

void *func(void *arg)
{
    struct sockaddr_in server_addr;

    int socket_fd = -1;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        printf("socket create failed: %s\n", strerror(errno));
    }

    // set server address
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    if (inet_pton(AF_INET, "119.63.197.151", &server_addr.sin_addr.s_addr) < 0)
    {
        printf("inet_pton error: %s\n", strerror(errno));
    }

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("connect error: %s\n", strerror(errno));
    }

    struct Task *task = (struct Task *)arg;
    const char *http_request = task->request.request;
    FILE *fp = task->fp;
    int offset = task->offset;

    struct HttpResponse http_response;
    char *http_response_raw_data = NULL;
    char *find = NULL;

    http_do_request(socket_fd, http_request, &http_response_raw_data);

    http_parse_response(http_response_raw_data, &http_response);

    write_to_disk(fp, offset, http_response.content, http_response.content_length);

    free(http_response_raw_data);

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t thread[2];
    void *exit_status;

    FILE *fp;
    fp = fopen("/home/longx/logo.png", "w+");

    struct Manager manager;
    manager.fp = fp;

    struct Task task[2];
    task[0].fp = fp;
    task[0].request.request = request1;
    task[0].offset = 0;

    task[1].fp = fp;
    task[1].request.request = request2;
    task[1].offset = 1025;

    pthread_create(&thread[0], NULL, func, (void *)&task[0]);
    pthread_create(&thread[1], NULL, func, (void *)&task[1]);

    pthread_join(thread[0], &exit_status);
    pthread_join(thread[1], &exit_status);

    fclose(fp);

    return 0;
}