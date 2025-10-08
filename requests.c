#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL && cookies_count > 0) {
        strcpy(line, "Cookie: ");
        for (int i = 0; i < cookies_count; i++) {
            strcat(line, cookies[i]);
            if (i < cookies_count - 1)
                strcat(line, "; ");
        }
        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
    int body_data_fields_count, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(BUFLEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3: add content-type
    if (content_type != NULL) {
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    }

    // Step 4: compute body content
    if (body_data != NULL && body_data_fields_count > 0) {
    for (int i = 0; i < body_data_fields_count; i++) {
    strcat(body_data_buffer, body_data[i]);
    }
    }

    // Step 5: content-length
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);

    // Step 6 (optional): add cookies
    if (cookies != NULL && cookies_count > 0) {
    strcpy(line, "Cookie: ");
    for (int i = 0; i < cookies_count; i++) {
    strcat(line, cookies[i]);
    if (i < cookies_count - 1)
    strcat(line, "; ");
    }
    compute_message(message, line);
    }

    // Step 7: end of headers
    compute_message(message, "");

    // Step 8: body
    compute_message(message, body_data_buffer);

    // Free resources
    free(line);
    free(body_data_buffer);

    return message;
}
