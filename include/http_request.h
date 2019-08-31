#ifndef _HTTP_REQUEST_INCLUDE_
#define _HTTP_REQUEST_INCLUDE_

#include "common.h"

/**
 * response should be free after use
 */
int http_request_do(int socket_fd, const char *request, char **response);

#endif