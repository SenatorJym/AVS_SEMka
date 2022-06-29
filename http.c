//
// Created by Senat on 7. 6. 2022.
//

#include "http.h"
#include "json.h"


char * httpResolver(char * message, int length, char * strToSend) {
    /*
    char testovaci[100] = "Texticek Skuskovy";
    StrtoSend = malloc(strlen(testovaci) + 1);
    memset(StrtoSend, 0, strlen(testovaci) + 1);
    strncpy(StrtoSend, testovaci, strlen(testovaci) + 1);
     */
    printf("zaciatok:>%s<\n", message);

    char tempMessage[length];
    int response = 0;
    strncpy(tempMessage, message, length);
    char * method = strtok(tempMessage, " ");     //extract method from message
    if(strncmp(method, GET, 3) == 0) {
        printf("GET\n");
        getResolver(message, length, &response);

        int messageSize = 0;
        strToSend = jsonGet(strToSend, &messageSize);
        strToSend = httpResponse(response, strToSend);

        printf("SKuska: >%s<\n", strToSend);
        //free(StrtoSend);
        return strToSend;
    }
    if(strncmp(method, POST, 4) == 0) {
        printf("POST");
        strToSend = postResolver(message, length, &response, strToSend);
        strToSend = httpResponse(response, strToSend);
        return strToSend;
    }
    httpResponse(405, strToSend);
    //free(StrtoSend);
    return strToSend;
}

char * httpResponse(int statusCode, char * messageToBeSent) {
    int bodySize = 0;
    if(messageToBeSent == NULL) {
        bodySize = 0;
    } else {
        bodySize = (int)(strlen(messageToBeSent) + 1);
    }
    char tempMessage[bodySize];
    if(bodySize > 0) {
        strncpy(tempMessage, messageToBeSent, bodySize);
    }

    free(messageToBeSent);
    messageToBeSent = NULL;

    int messageSize = 0;
    messageSize += strlen(SUPPORTED_HTTP); //excluding \0      "HTTP/1.1"
    messageSize += 1;  // size of space                        " "
    switch(statusCode) {
        case 200:
            messageSize += strlen(HTTP_200);
            break;
        case 404:
            messageSize += strlen(HTTP_404);
            break;
        case 405:
            messageSize += strlen(HTTP_405);
            break;
        case 505:
            messageSize += strlen(HTTP_505);
            break;
        default:
            messageSize += strlen(HTTP_400);
            break;
    }
    messageSize += 2;  //\r\n   //end of start line

    messageSize += strlen(CONTENT_TYPE);
    messageSize += strlen(TEXT_JSON);
    messageSize += 2; //\r\n end of content type

    messageSize += strlen(CONTENT_LENGTH);
    char temp[100];
    sprintf(temp, "%d", bodySize - 1);
    messageSize += (int)strlen(temp);
    messageSize += 2;    //\r\n
    messageSize += 2;  //\r\n      blank line
    messageSize += bodySize;
    messageToBeSent = malloc(messageSize);
    memset(messageToBeSent, 0, messageSize);
    //messageToBeSent = calloc(0, messageSize);
    strncpy(messageToBeSent, SUPPORTED_HTTP, strlen(SUPPORTED_HTTP) + 1);
    strcat(messageToBeSent, " ");
    switch(statusCode) {
        case 200:
            strcat(messageToBeSent, HTTP_200);
            break;
        case 404:
            strcat(messageToBeSent, HTTP_404);
            break;
        case 405:
            strcat(messageToBeSent, HTTP_405);
            break;
        case 505:
            strcat(messageToBeSent, HTTP_505);
            break;
        default:
            strcat(messageToBeSent, HTTP_400);
            break;
    }
    strcat(messageToBeSent, "\r\n");    //end if start line

    strcat(messageToBeSent, CONTENT_TYPE);
    strcat(messageToBeSent, TEXT_JSON);
    strcat(messageToBeSent, "\r\n");    //end of content type

    strcat(messageToBeSent, CONTENT_LENGTH);
    strcat(messageToBeSent, temp);
    strcat(messageToBeSent, "\r\n\r\n");    //end of content length + blank line

    strcat(messageToBeSent, tempMessage);

    //printf("messageSize: %d\n", messageSize);
    printf("Dobry vypis:\n>%s<\n", messageToBeSent);
    return messageToBeSent;
}

int getResolver(char * message, int length, int * response) {
    char tempMessage[length];
    strncpy(tempMessage, message, length);
    //printf("tempMessage>%s<\n", tempMessage);
    char * delimiterPointer = strstr(tempMessage, HTTP_DELIMITER);
    if(delimiterPointer == NULL) {
        *response = 400;
        printf("Delimiter pointer");
        return ERROR;
    }
    *delimiterPointer = '\0';     //extract request line from request
    int rqLineLength = (int)strnlen(tempMessage, length);
    if(requestLineVerifier(tempMessage, rqLineLength, response) == 1) {
        printf("not valid request line\n");
    }
    return SUCCESS;
}

char * postResolver(char * message, int length, int * response, char * strToSend) {
    char tempMessage[length + 1];
    strncpy(tempMessage, message, length);
    tempMessage[length] = '\0';
    //printf("tempMessage>%s<\n", tempMessage);
    char * delimiterPointer = strstr(tempMessage, HTTP_DELIMITER);
    if(delimiterPointer == NULL) {
        *response = 400;
        printf("Delimiter pointer\n");
        return NULL;
    }
    *delimiterPointer = '\0';     //extract request line from request
    int rqLineLength = (int)strnlen(tempMessage, length);
    if(requestLineVerifier(tempMessage, rqLineLength, response) == 1) {
        printf("not valid request line\n");
    }
    strncpy(tempMessage, message, length);
    char * jsonContentType = strstr(tempMessage, TEXT_JSON);
    if(jsonContentType == NULL) {
        *response = 400;
        printf("Wrong content type\n");
        return NULL;
    }
    char * lengthDelimiter = strstr(tempMessage, CONTENT_LENGTH);
    if(lengthDelimiter == NULL) {
        *response = 400;
        printf("No content length\n");
        return NULL;
    }
    lengthDelimiter += strlen(CONTENT_LENGTH);
    int bodyLength = 0;
    char bodyLengthStr[15] = {};
    while(isdigit(*lengthDelimiter) != 0) {
        bodyLengthStr[bodyLength] = *lengthDelimiter;
        bodyLength++;
        lengthDelimiter++;
    }
    if(bodyLength > 14) {
        *response = 400;
        printf("Content length not right\n");
        return NULL;
    }
    bodyLengthStr[bodyLength] = '\0';
    bodyLength = 0;
    bodyLength = atoi(bodyLengthStr);
    printf("POST body length: %d\n", bodyLength);

    char * bodyFounder = strstr(lengthDelimiter, "\r\n\r\n");
    bodyFounder += 4;//posun o 4 znaky na zaciatok tela spravy

    strToSend = malloc(strlen(bodyFounder) + 1);
    strncpy(strToSend, bodyFounder, strlen(bodyFounder) + 1);
    if((strToSend = jsonSet(strToSend)) == NULL) {
        printf("Wrong content.\n");
        *response = 400;
        return NULL;
    }
    return strToSend;
}

int requestLineVerifier(char * requestLine, int length, int * response) {
    char tmpString[length];
    strncpy(tmpString, requestLine, length);
    char * token = NULL;
    token = strtok(tmpString, " ");
    token = strtok(NULL, " ");
    if(uriVerifier(token, (int)strlen(token)) == ERROR) {
        printf("wrong_uri\n");
        *response = 404;
        return ERROR;
    }
    token = strtok(NULL, " ");
    if(strncmp(token, SUPPORTED_HTTP, strlen(SUPPORTED_HTTP)) != SUCCESS) {
        printf("http not supported");
        *response = 505;
        return ERROR;
    }
    printf("Request line: >%s<\n", requestLine);
    *response = 200;
    return SUCCESS;
}

int uriVerifier(char * uri, int length) {
    if(strncmp(uri, VALID_URI, strlen(VALID_URI)) == 0) {
        return SUCCESS;
    }
    return ERROR;
}
