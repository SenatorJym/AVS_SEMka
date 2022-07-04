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
        response = getResolver(message, length);

        int messageSize = 0;
        strToSend = jsonGet(strToSend, &messageSize);
        strToSend = httpResponse(response, strToSend);

        printf("SKuska: >%s<\n", strToSend);
        //free(StrtoSend);
        return strToSend;
    }
    if(strncmp(method, POST, 4) == 0) {
        printf("POST");
        if((strToSend = postResolver(message, length, &response, strToSend)) == NULL) {
            strToSend = httpResponse(response, strToSend);
            return strToSend;
        }
        if(strncmp(strToSend, API_OFF, strlen(API_OFF)) == 0) {
            strncpy(message, API_OFF, strlen(API_OFF) + 1);
        }
        strToSend = httpResponse(response, strToSend);

        return strToSend;
    }
    strToSend = httpResponse(405, strToSend);
    //free(StrtoSend);
    return strToSend;
}

char * httpResponse(int statusCode, char * messageToBeSent) {
    int bodySize;
    if(messageToBeSent == NULL) {
        bodySize = 0;
    } else {
        bodySize = (int)(strlen(messageToBeSent) + 1);
    }
    char * tempMessage = malloc(bodySize);
    if(bodySize > 0) {
        strncpy(tempMessage, messageToBeSent, bodySize);
    }

    free(messageToBeSent);

    int messageSize = 0;
    messageSize += strlen(SUPPORTED_HTTP); //excluding \0      "HTTP/1.1"
    messageSize += 1;  // size of space                        " "
    switch(statusCode) {
        case 200:
            messageSize += strlen(HTTP_200);
            messageSize += strlen(TEXT_JSON);
            break;
        case 404:
            messageSize += strlen(HTTP_404);
            messageSize += strlen(TEXT_PLAIN);
            bodySize = strlen(HTTP_404) + 1;
            tempMessage = realloc(tempMessage, bodySize);
            strncpy(tempMessage, HTTP_404, strlen(HTTP_404) +1);
            break;
        case 405:
            messageSize += strlen(HTTP_405);
            messageSize += strlen(TEXT_PLAIN);
            bodySize = strlen(HTTP_405) + 1;
            tempMessage = realloc(tempMessage, bodySize);
            strncpy(tempMessage, HTTP_405, strlen(HTTP_405) + 1);
            break;
        case 505:
            messageSize += strlen(HTTP_505);
            messageSize += strlen(TEXT_PLAIN);
            bodySize = strlen(HTTP_505) + 1;
            tempMessage = realloc(tempMessage, bodySize);
            strncpy(tempMessage, HTTP_505, strlen(HTTP_505) + 1);
            break;
        default:
            messageSize += strlen(HTTP_400);
            messageSize += strlen(TEXT_PLAIN);
            bodySize = strlen(HTTP_400) + 1;
            tempMessage = realloc(tempMessage, bodySize);
            strncpy(tempMessage, HTTP_400, strlen(HTTP_400) + 1);
            break;
    }
    messageSize += 2;  //\r\n   //end of start line

    char temp[100];

    messageSize += strlen(CONTENT_TYPE);
    messageSize += 2; //\r\n end of content type

    messageSize += strlen(CONTENT_LENGTH);
    sprintf(temp, "%d", bodySize - 1);
    messageSize += (int) strlen(temp);
    messageSize += 2;    //\r\n
    messageSize += bodySize;
    messageSize += 2;  //\r\n      blank line
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
    if(statusCode == 200) {
        strcat(messageToBeSent, TEXT_JSON);
    } else {
        strcat(messageToBeSent, TEXT_PLAIN);
    }
    strcat(messageToBeSent, "\r\n");    //end of content type

    strcat(messageToBeSent, CONTENT_LENGTH);
    strcat(messageToBeSent, temp);
    strcat(messageToBeSent, "\r\n\r\n");    //end of content length + blank line
    strcat(messageToBeSent, tempMessage);
    //printf("messageSize: %d\n", messageSize);
    printf("Dobry vypis:\n>%s<\n", messageToBeSent);
    free(tempMessage);
    tempMessage = NULL;
    return messageToBeSent;
}

int getResolver(char * message, int length) {
    char tempMessage[length];
    strncpy(tempMessage, message, length);
    //printf("tempMessage>%s<\n", tempMessage);
    char * delimiterPointer = strstr(tempMessage, HTTP_DELIMITER);
    if(delimiterPointer == NULL) {
        printf("Delimiter pointer");
        return 400;
    }
    *delimiterPointer = '\0';     //extract request line from request
    int rqLineLength = (int)strnlen(tempMessage, length);
    if(requestLineVerifier(tempMessage, rqLineLength) != 200) {
        printf("not valid request line\n");
        return 400;
    }
    return 200;
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
    if((*response = requestLineVerifier(tempMessage, rqLineLength)) != 200) {
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
    strToSend = jsonSet(strToSend);

    if((strToSend) == NULL) {
        printf("Wrong content.\n");
        *response = 400;
        return NULL;
    } else if(*strToSend == '\0') {
        printf("Turning off\n");
        strToSend = realloc(strToSend, strlen(API_OFF) + 1);
        strncpy(strToSend, API_OFF, strlen(API_OFF) + 1);
    }
    return strToSend;
}

int requestLineVerifier(char * requestLine, int length) {
    char tmpString[length];
    strncpy(tmpString, requestLine, length);
    char * token = NULL;
    token = strtok(tmpString, " ");
    token = strtok(NULL, " ");
    if(uriVerifier(token) == ERROR) {
        printf("wrong_uri\n");
        return 404;
    }
    token = strtok(NULL, " ");
    if(strncmp(token, SUPPORTED_HTTP, strlen(SUPPORTED_HTTP)) != SUCCESS) {
        printf("http not supported");
        return 505;
    }
    printf("Request line: >%s<\n", requestLine);
    return 200;
}

int uriVerifier(char * uri) {
    if(strncmp(uri, VALID_URI, strlen(VALID_URI)) == 0) {
        return SUCCESS;
    }
    return ERROR;
}
