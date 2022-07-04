#include "defines.h"
//34

void convertToJson(char name[], char protocol[], char address[], char json[], int firstCycle) {
    char interface [100] = "";

    if (firstCycle == 1) {
        strcat(interface, "{\"nazov\":\"");
    } else {
        strcat(interface, ",{\"nazov\":\"");
    }
    strcat(interface, name);
    strcat(interface, "\", \"protokol\":\"");

    strcat(interface, protocol);
    strcat(interface, "\", \"adresa\":\"");

    strcat(interface, address);
    strcat(interface, "\"}");
    strcat(json, interface);
    memset(interface, 0, sizeof(interface));
}

/* this method returns whole Json body in which are listed interfaces, IP address, IPv4/6 and names of interfaces */
char * jsonGet(char* json, int * sizeOfMessage) {
    json = malloc(1000 * sizeof(char));
    json[0]=0;
    strcat(json, "{\"rozhrania\":[");
    int firstCycle = 1;
    struct ifaddrs *addresses;
    if (getifaddrs(&addresses) == -1) {
        printf("getifaddrs call failed\n");
        return NULL;
    }

    struct ifaddrs *address = addresses;
    while(address) {
        int family = address->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6) {
            printf("%s\t", address->ifa_name);
            printf("%s\t", family == AF_INET ? "IPv4" : "IPv6");
            char ap[50];
            const int family_size = family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
            getnameinfo(address->ifa_addr, family_size, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
            printf("\t%s\n", ap);

            convertToJson(address->ifa_name, family == AF_INET ? "IPv4" : "IPv6", ap, json, firstCycle);
            firstCycle = 0;
        }
        address = address->ifa_next;
    }
    strcat(json, "]}");
    freeifaddrs(addresses);

    sizeOfMessage = (int*)sizeof(json);
    //sizeOfMessage = (int*)sizeof(json);
    printf("%i",sizeOfMessage);
    return json;
}

/*this method validates the IP address, if the IP address is OK, it returns 1. if there are any mistakes
 * it returns 0*/
int validate_ip(char *ip) {
    int i, num, dots = 0;
    char *ptr;
    if (ip == NULL)
        return 0;
    ptr = strtok(ip, ".");
    if (ptr == NULL)
        return 0;
    while (ptr) {
        num = atoi(ptr);
        if (num >= 0 && num <= 255) {
            ptr = strtok(NULL, ".");
            if (ptr != NULL)
                dots++;
        } else
            return 0;
    }
    if (dots != 3)
        return 0;
    return 1;
}

char* jsonSet(char* jsonMessage) {
    int fd;
    struct ifreq ifr, ifr1;
    struct sockaddr_in* addr;
    struct sockaddr_in* addrMask;
    char* ipCopy = malloc(20*sizeof(char));
    char* ipMaskCopy = malloc(20*sizeof(char));
    char ip_address[20];
    char ip_mask[20];
    char* rozhranie = NULL;
    int count = 0;
    char* separatedText[20];
    char tempString[strlen(jsonMessage) + 1];

    /*copy jsonMessage into temp message */
    strncpy(tempString, jsonMessage, strlen(jsonMessage) + 1);

    /* cuts message into blocks of text via " separator*/
    char* token = strtok(tempString, "\"");
    while(token != NULL) {
        printf("%s\n", token);
        separatedText[count] = token;
        count++;
        token = strtok(NULL, "\"");
    }

    int verifier = 0;

    for(int i=0; i<count; i++) {
        if(strstr(separatedText[i], "APIOFF") != NULL) {
            jsonMessage = realloc(jsonMessage, 1);
            *jsonMessage = '\0';
            free(ipCopy);
            free(ipMaskCopy);
            return jsonMessage;
        }
        /*load name of interface into variables */
        if(strstr(separatedText[i], "ROZHRANIE") != NULL) {
            rozhranie = separatedText[i+2];
            verifier++;
        }
        /*load IP into variables */
        if(strstr(separatedText[i], "IP") != NULL) {
            strcpy(ip_address, separatedText[i+2]);
            strcpy(ipCopy, separatedText[i+2]);
            verifier++;
        }
        /*load IP mask into variables */
        if(strstr(separatedText[i], "MASK") != NULL) {
            strcpy(ip_mask, separatedText[i+2]);
            strcpy(ipMaskCopy, separatedText[i+2]);
            verifier++;
        }
    }
    /* verifier checks, if all of the parameters were filled */
    if(verifier != 3) {
        return NULL;
    }

    printf("%s is ip", ip_address);
    printf("%s is interface", rozhranie);
    printf("%s je maska", ip_mask);

    if((validate_ip(ipCopy) == 0) && (validate_ip(ipMaskCopy) == 0)) {
        return NULL;
    } else {
        printf("Spravny format IPv4.");

        /*AF_INET - to define network interface IPv4*/
        /*Creating soket for it.*/
        fd = socket(AF_INET, SOCK_DGRAM, 0);

        /*AF_INET - to define IPv4 Address type.*/
        ifr1.ifr_addr.sa_family = AF_INET;

        /* define the ifr_name - port name
        where network attached.*/
        memcpy(ifr1.ifr_name, rozhranie, IFNAMSIZ - 1);

        /*defining the sockaddr_in*/
        addr = (struct sockaddr_in*)&ifr1.ifr_addr;

        /*convert ip address in correct format to write*/
        inet_pton(AF_INET, ip_address, &addr->sin_addr);

        /*Setting the Ip Address using ioctl*/
        ioctl(fd, SIOCSIFADDR, &ifr1);

        close(fd);

        printf("IP Address updated sucessfully.\n");
        /*Getting the Ip Address after Updating.*/

        /*clear ip_address buffer with 0x20- space*/
        memset(ip_address, 0x20, 20);
        printf("IP je: %s", ip_address);
        /*load ip_address from changed interface*/
        strcpy(ip_address, inet_ntoa(((struct sockaddr_in*)&ifr1.ifr_addr)->sin_addr));
        printf("IP je: %s", ip_address);

        /*AF_INET - to define network interface IPv4*/
        /*Creating soket for it.*/
        fd = socket(AF_INET, SOCK_DGRAM, 0);

        /*AF_INET - to define IPv4 Address type.*/
        ifr.ifr_addr.sa_family = AF_INET;

        /* define the ifr_name - port name
        where network attached.*/
        memcpy(ifr.ifr_name, rozhranie, IFNAMSIZ - 1);

        /*defining the sockaddr_in*/
        addr = (struct sockaddr_in*)&ifr.ifr_addr;

        /*convert net mask in correct format to write*/
        inet_pton(AF_INET, ip_mask, &addr->sin_addr);

        /*Setting the net mask using ioctl*/
        ioctl(fd, SIOCSIFNETMASK, &ifr);

        close(fd);

        /*Getting the net mask after Updating.*/

        /*clear ip_mask buffer with 0x20- space*/
        memset(ip_mask, 0x20, 20);

        printf("%s je maska\n", ip_mask);
        /*load ip_address from changed interface*/
        strcpy(ip_mask, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));

        printf("%s je maska\n", ip_mask);

        /*load interface name*/
        rozhranie = ifr.ifr_name;



        printf("Updated IP Address is: %s\n", ip_address);

        /*realloc memory of jsonMessage for the size of loaded information from interface */
        int bodySize = 0;
        bodySize += 7;     //{"ip":"
        bodySize += (int)strlen(ip_address);
        bodySize += 11;      //","maska":"
        bodySize += (int) strlen(ip_mask);
        bodySize += 15;      //","rozhranie":"
        bodySize += (int) strlen(rozhranie);
        bodySize += 3;      //"} + \0
        jsonMessage = realloc(jsonMessage, bodySize);

        /* set all characters in message to 0 */
        memset(jsonMessage, 0, bodySize);

        /*load new information about interface into jsonMessage */
        strcat(jsonMessage, "{\"ip\":\"");
        strcat(jsonMessage, ip_address);
        strcat(jsonMessage, "\",\"maska\":\"");
        strcat(jsonMessage, ip_mask);
        strcat(jsonMessage, "\",\"rozhranie\":\"");
        strcat(jsonMessage, rozhranie);
        strcat(jsonMessage, "\"}");
    }

    free(ipCopy);
    free(ipMaskCopy);
    return jsonMessage;
}




