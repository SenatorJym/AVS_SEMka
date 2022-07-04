
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
 * Will be called if the request method is GET. Will determine if the incoming request is valid.
 * @param message Received http request
 * @param length Size of request
 * @return Returns corresponding HTTP status code
 * */
int getResolver(char * message, int length);
/**
 * Checks if the POST request is valid, then passes the body of the request to change IP parameters. Will set response
 * accordingly.
 * @param message Received HTTP request
 * @param length Length of HTTP request
 * @param response Will set HTTP status code
 * @param strToSend Place for the body that will be generated
 * @return Returns pointer to the allocated memory containing body for the response
 * */
char * postResolver(char * message, int length, int * response, char * strToSend);
/**
 * Checks if the request line in HTTP request is valid. Returns corresponding HTTP status code
 * @param requestLine String with request line
 * @param length Length of the request line
 * @return HTTP status code
 * */
int requestLineVerifier(char * requestLine, int length);
/**
 * Verifies the uri.
 * @param uri Uri to be verified
 * @return SUCCESS if correct ERROR if not
 * */
int uriVerifier(char * uri);
#endif //SEMESTRALNA_PRACA_HTTP_H
