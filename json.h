//
// Created by Senat on 26. 6. 2022.
//

#ifndef SEMESTRALNA_PRACA_JSON_H
#define SEMESTRALNA_PRACA_JSON_H

void convertToJson(char* name, char* protocol, char* address, char* json, int firstCycle);
char * jsonGet(char * json, int * sizeOfMessage);
char* jsonSet(char* jsonMessage);

#endif //SEMESTRALNA_PRACA_JSON_H
