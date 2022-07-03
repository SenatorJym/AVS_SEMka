//
// Created by Senat on 26. 6. 2022.
//

#ifndef SEMESTRALNA_PRACA_JSON_H
#define SEMESTRALNA_PRACA_JSON_H

/** Creates a single string(interface) in json format. Automatically maps it to the whole json body, that`s
 * why this method should only be called internally.
 * @param name determines the name of an interface that has been found in a device.
 * @param protocol determines what version protocol is used, IPv4 or IPv6.
 * @param address is exact address of the device on corresponding interface.
 * @param json is the whole json body response, in which is each interface added gradually.
 * @param firstCycle checks, if the added interface into the json body is the first interface.
 */
void convertToJson(char* name, char* protocol, char* address, char* json, int firstCycle);

/**
 * It is the GET method to retrieve all of the interfaces that could be found on the corresponding device.
 * It returns json format, and can found IP address, protocol version and name of an interface.
 * @param json is the json body that is being typed into.
 * @param sizeOfMessage represents size of the message.
 * @return value is json and it is a pointer on the first character.
 */
char * jsonGet(char * json, int * sizeOfMessage);

/**
 * Sets the value that is being send. It is required that the message is in json format, and it gives IP address,
 * mask and the name of desired interface. If the mask or IP address is in wrong format, method returns NULL. Or
 * if everything is Ok, it returns the changed IP address and mask.
 * @param jsonMessage represents json formatted IP address, mask and name of desired interface.
 * @return value is NULL if any problems occurred, or pointer on first character of changed IP address and mask
 * in json format.
 */
char* jsonSet(char* jsonMessage);

#endif //SEMESTRALNA_PRACA_JSON_H
