//
// Created by Senat on 7. 6. 2022.
//

#ifndef SEMESTRALNA_PRACA_HTTP_H
#define SEMESTRALNA_PRACA_HTTP_H
#include "defines.h"

/**
 * Main resolver for any http relates work. Will take * message that should contain
 * http request. Will create and allocate space for reply, stored in * strToSend.
 * @param message Http request received by socket
 * @param length Length of message
 * @param strToSend Pinter to a memory space where reply will be created
 * @return Returns pointer to a http reply
 */
char * httpResolver(char * message, int length, char * strToSend);
/**
 * Responsible for calculating and allocating required space for reply and for the creation itself.
 * @param statusCode Status code to by sent in the http reply
 * @param messageToBeSent Body of the http reply
 * @return Returns pointer to a http reply
 */
char * httpResponse(int statusCode, char * messageToBeSent);
/**
 * Will be called if the request method is GET. Will determine if the incoming request is valid. If not will set
 * response to 400
 * */
int getResolver(char * message, int length);
char * postResolver(char * message, int length, int * response, char * strToSend);
int requestLineVerifier(char * requestLine, int length);
int uriVerifier(char * uri, int length);
#endif //SEMESTRALNA_PRACA_HTTP_H
