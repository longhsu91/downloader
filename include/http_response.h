#ifndef _HTTP_RESPONSE_INCLUDE_
#define _HTTP_RESPONSE_INCLUDE_

#include "common.h"

// header
const char *http_response_header_key_to_str(int key);

/**
 * value should be free after use
 */
int http_response_header_get_value(const char *httpRawData, int key, char **value);

int http_response_dump(struct HttpResponse *httpResponse);

int http_response_parse(const char *httpRawData, struct HttpResponse *httpResponse);

#endif