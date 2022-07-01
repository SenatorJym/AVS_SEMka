//
// Created by Senat on 26. 6. 2022.
//

#ifndef SEMESTRALNA_PRACA_JSON_H
#define SEMESTRALNA_PRACA_JSON_H

/*json parameter is the message that is been sent back, so its the whole json body
 *firstCycle is only a checking parameter to determine, if we need comma before the interface, since
 *      there might be multiple interfaces or only 1, we do not know if we need comma or not*/
void convertToJson(char* name, char* protocol, char* address, char* json, int firstCycle);
char * jsonGet(char * json, int * sizeOfMessage);
char* jsonSet(char* jsonMessage);

#endif //SEMESTRALNA_PRACA_JSON_H
