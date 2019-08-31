#include "http_response.h"

const char *http_response_header_key_to_str(int key)
{
    const char *str = NULL;

    switch (key)
    {
    case ACCEPT_RANGES:
    {
        str = HTTP_HEADER_ACCEPT_RANGES;
        break;
    }
    case CONTENT_LENGTH:
    {
        str = HTTP_HEADER_CONTENT_LENGTH;
        break;
    }
    default:
    {
        printf("no %d value find.\n", key);
    }
    }

    //printf("%d to str is %s.\n", key, str);

    return str;
}

int http_response_header_get_value(const char *httpRawData, int key, char **value)
{
    char *find = NULL;
    char *buf = NULL;
    int begin = 0;
    int length = 0;

    const char *key_str = http_response_header_key_to_str(key);

    find = d_strstr(httpRawData, key_str, &begin);
    find = d_strstr(find, HTTP_HEADER_BLANK, &length);
    begin = begin + length;

    length = 0;
    for (int i = 0; find[i] != '\r'; i++)
    {
        if (find[i] == ' ')
        {
            begin++;
            continue;
        }

        length++;
    }

    buf = (char *)malloc(sizeof(char) * (length + 1));
    memcpy(buf, httpRawData + begin, length);
    buf[length] = '\0';

    *value = buf;

    //printf("%s: %s.\n", key_str, buf);

    return 0;
}

int http_response_dump(struct HttpResponse *httpResponse)
{
    printf("content_length: %d\n", httpResponse->content_length);
    printf("accept_ranges: %s\n", httpResponse->accept_ranges == true ? "true" : "false");

    return 0;
}

int http_response_parse(const char *httpRawData, struct HttpResponse *httpResponse)
{
    int i = 0;
    int j = 0;

    char *value = NULL;

    // get content_length
    http_response_header_get_value(httpRawData, CONTENT_LENGTH, &value);
    httpResponse->content_length = atoi(value);
    free(value);

    // if support ranges
    http_response_header_get_value(httpRawData, ACCEPT_RANGES, &value);
    if (strcmp(value, "bytes") == 0)
    {
        httpResponse->accept_ranges = true;
    }
    else
    {
        httpResponse->accept_ranges = false;
    }
    free(value);

    // get content
    char *content = NULL;
    content = strstr(httpRawData, HTTP_BODY_CONTENT);
    content += 4;

    httpResponse->content = content;

    return 0;
}