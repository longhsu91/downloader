#include "common.h"
#include "http_response.h"
#include "http_request.h"

#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <strings.h>

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

void *task_thread(void *arg)
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

    http_request_do(socket_fd, http_request, &http_response_raw_data);

    http_response_parse(http_response_raw_data, &http_response);

    http_response_dump(&http_response);

    local_write_to_disk(fp, offset, http_response.content, http_response.content_length);

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

    pthread_create(&thread[0], NULL, task_thread, (void *)&task[0]);
    pthread_create(&thread[1], NULL, task_thread, (void *)&task[1]);

    pthread_join(thread[0], &exit_status);
    pthread_join(thread[1], &exit_status);

    fclose(fp);

    return 0;
}