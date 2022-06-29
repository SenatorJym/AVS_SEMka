//
// Created by Senat on 7. 6. 2022.
//

#ifndef SEMESTRALNA_PRACA_HTTP_H
#define SEMESTRALNA_PRACA_HTTP_H
#include "defines.h"
char * httpResolver(char * message, int length, char * strToSend);
char * httpResponse(int statusCode, char * messageToBeSent);
int getResolver(char * message, int length, int * response);
char * postResolver(char * message, int length, int * response, char * strToSend);
int requestLineVerifier(char * requestLine, int length, int * response);
int uriVerifier(char * uri, int length);
#endif //SEMESTRALNA_PRACA_HTTP_H
