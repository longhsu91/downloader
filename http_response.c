#include "http_response.h"

void NewHttpResponse(struct HttpResponse **httpResponse)
{
    struct HttpResponse *response = (struct HttpResponse *)malloc(sizeof(struct HttpResponse));
    response->_this = response;
    response->raw = NULL;

    response->setRaw = setRaw;

    response->setAcceptRanges = setAcceptRanges;
    response->getAcceptRanges = getAcceptRanges;
    response->setContentLength = setContentLength;
    response->getContentLength = getContentLength;
    response->setContent = setContent;
    response->getContent = getContent;

    response->dump = dump;
    response->parse = parse;
    response->freeResponse = freeResponse;

    *httpResponse = response;
}

void setRaw(struct HttpResponse *this, const char *httpRawData)
{
    this->raw = httpRawData;
}

void setAcceptRanges(struct HttpResponse *this, const char *acceptRanges)
{
    this->accept_ranges = strcmp(acceptRanges, "bytes") == 0 ? true : false;
}
bool getAcceptRanges(struct HttpResponse *this)
{
    return this->accept_ranges;
}

void setContentLength(struct HttpResponse *this, const char *contentLength)
{
    this->content_length = atoi(contentLength);
}

int getContentLength(struct HttpResponse *this)
{
    return this->content_length;
}

void setContent(struct HttpResponse *this, const char *content)
{
    this->content = content;
}
const char *getContent(struct HttpResponse *this)
{
    return this->content;
}

void freeResponse(struct HttpResponse *this)
{
    if (this != NULL)
    {
        if (this->_this != NULL)
        {
            free(this->_this);
            this->_this = NULL;
        }
        this = NULL;
    }
}

const char *headerKeyToStr(int key)
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

int headerGetValue(const char *httpRawData, int key, char **value)
{
    char *find = NULL;
    char *buf = NULL;
    int begin = 0;
    int length = 0;

    const char *key_str = headerKeyToStr(key);

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

int dump(struct HttpResponse *this)
{
    printf("content_length: %d\n", this->getContentLength(this));
    printf("accept_ranges: %d\n", this->getAcceptRanges(this));

    return 0;
}

int parse(struct HttpResponse *this)
{
    char *value = NULL;

    // get content_length
    headerGetValue(this->raw, CONTENT_LENGTH, &value);
    this->setContentLength(this, value);
    free(value);

    // if support ranges
    headerGetValue(this->raw, ACCEPT_RANGES, &value);
    this->setAcceptRanges(this, value);
    free(value);

    // get content
    char *content = NULL;
    content = strstr(this->raw, HTTP_BODY_CONTENT);
    content += 4;
    this->setContent(this, content);

    return 0;
}