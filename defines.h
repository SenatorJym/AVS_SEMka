//
// Created by Senat on 7. 6. 2022.
//

#ifndef SEMESTRALNA_PRACA_DEFINES_H
#define SEMESTRALNA_PRACA_DEFINES_H
#define GET "GET"
#define POST "POST"
#define HTTP_DELIMITER "\r\n"
#define SUPPORTED_HTTP "HTTP/1.1"
#define VALID_URI "/api/mamvsetkyryzeny/interfaces"
#define SUCCESS 0
#define ERROR 1
#define HTTP_200 "200 OK"
#define HTTP_404 "404 Not Found"
#define HTTP_405 "405 Method Not Allowed"
#define HTTP_505 "505 HTTP Version Not Supported"
#define HTTP_400 "400 Bad Request"
#define CONTENT_TYPE "Content-Type: "
#define TEXT_PLAIN "text/plain"
#define TEXT_JSON "application/json"
#define TEXT_HTML "text/html"
#define CONTENT_LENGTH "Content-Length: "
#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <unistd.h>
#include <string.h>/* read, write, close */
#include <stdio.h>

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <ctype.h>
#include <net/if.h>
#include <sys/ioctl.h>


#endif //SEMESTRALNA_PRACA_DEFINES_H
