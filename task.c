#include "common.h"
#include "http_response.h"
#include "http_request.h"
#include "manager.h"
#include "task.h"

#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <strings.h>

void NewTask(struct Task *task)
{
    task->init = init;
    task->run = run;
}

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
    struct HttpRequest *http_request = task->request;
    FILE *fp = task->fp;
    int offset = task->offset;

    char *http_response_raw_data = NULL;
    http_request_do(socket_fd, http_request->toString(http_request), &http_response_raw_data);
    http_request->freeRequest(http_request);

    struct HttpResponse *http_response = NULL;
    NewHttpResponse(&http_response);
    http_response->setRaw(http_response, http_response_raw_data);
    http_response->parse(http_response);
    http_response->dump(http_response);

    local_write_to_disk(fp, offset, http_response->getContent(http_response), http_response->getContentLength(http_response));

    // free
    http_response->freeResponse(http_response);
    free(http_response_raw_data);

    return NULL;
}

void init(struct Task *this, void *fp, struct Segment *segment)
{
    this->segment = segment;

    struct HttpRequest *request = NULL;
    NewHttpRequest(&request);
    request->setRanges(request, segment->begin, segment->end);
    this->fp = fp;
    this->request = request;
    this->offset = segment->begin;
}
void run(struct Task *this)
{
    void *exit_status;
    pthread_create(&this->thread, NULL, task_thread, (void *)this);
    pthread_join(this->thread, &exit_status);
}