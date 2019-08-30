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

struct Manager
{
    FILE *fp;
};

struct Task
{
    FILE *fp;
    int offset;
    const char *request;
};

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

static void do_request(FILE *fp, int offset, int socket_fd, const char *request)
{
    char buf[1024 * 1024];
    char *find = NULL;

    send(socket_fd, request, strlen(request), 0);

    int n = 0;
    char *p = buf;
    int pos = 0;
    int nread = 0;
    while ((n = recv(socket_fd, p, 1024, 0)) > 0)
    {
        p = p + n;
        nread += n;

        if ((find = dl_strstr(buf, "\r\n\r\n", &pos)) != NULL)
        {
            break;
        }
    }
    printf("pos is %d\n", pos);

    int pos1 = 0;
    find = dl_strstr(buf, "Content-Length", &pos1);
    printf("pos1 is %d\n", pos1);
    find = strstr(find, " ");
    char buf_length[10];
    int i = 0;
    while (find[i] != '\r')
    {
        buf_length[i] = find[i];
        i++;
    }
    buf_length[i] = '\0';

    int content_length = atoi(buf_length);
    printf("content_length is %d\n", content_length);

    int nleft = content_length - (nread - pos + 4);

    while (nleft > 0)
    {
        if ((n = recv(socket_fd, p, 1024, 0)) > 0)
        {
            printf("n  = %d\n", n);
            nleft -= n;
            p += n;
            nread += n;
        }
    }

    printf("tolel length is %d\n", nread);
    // get body
    find = strstr(buf, "\r\n\r\n");
    find = find + 4;

    write_to_disk(fp, offset, find, content_length);
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
    const char *request = task->request;
    FILE *fp = task->fp;
    int offset = task->offset;

    printf("offset is %d\n", offset);
    do_request(fp, offset, socket_fd, request);

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
    task[0].request = request1;
    task[0].offset = 0;

    task[1].fp = fp;
    task[1].request = request2;
    task[1].offset = 1025;

    pthread_create(&thread[0], NULL, func, (void *)&task[0]);
    pthread_create(&thread[1], NULL, func, (void *)&task[1]);

    pthread_join(thread[0], &exit_status);
    pthread_join(thread[1], &exit_status);

    printf("close fp\n");

    fclose(fp);

    return 0;
}