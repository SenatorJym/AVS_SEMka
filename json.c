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
    struct ifreq ifr;
    struct sockaddr_in* addr;
    char* ipCopy = malloc(20*sizeof(char));
    char* ipMaskCopy = malloc(20*sizeof(char));
    char ip_address[15];
    char ip_mask[15];
    char* rozhranie;
    int count = 0;
    char* separatedText[20];

    char* token = strtok(jsonMessage, "\"");
    while(token != NULL) {
        printf("%s\n", token);
        separatedText[count] = token;
        count++;
        token = strtok(NULL, "\"");
    }

    for(int i=0; i<count; i++) {
        if(strstr(separatedText[i], "ROZHRANIE")!=NULL) {
            rozhranie = separatedText[i+2];
        }
        if(strstr(separatedText[i], "IP")!=NULL) {
            strcpy(ip_address, separatedText[i+2]);
            strcpy(ipCopy, separatedText[i+2]);
        }
        if(strstr(separatedText[i], "MASK")!=NULL) {
            strcpy(ip_mask, separatedText[i+2]);
            strcpy(ipMaskCopy, separatedText[i+2]);
        }
    }

    printf("%s is ip", ip_address);
    printf("%s is interface", rozhranie);

    if((validate_ip(ipCopy) == 0) && (validate_ip(ipMaskCopy) == 0)) {
        return NULL;
    } else {
        printf("Spravny format IPv4.");

        /*AF_INET - to define network interface IPv4*/
        /*Creating soket for it.*/
        fd = socket(AF_INET, SOCK_DGRAM, 0);

        /*AF_INET - to define IPv4 Address type.*/
        ifr.ifr_addr.sa_family = AF_INET;

        /*eth0 - define the ifr_name - port name
        where network attached.*/
        memcpy(ifr.ifr_name, rozhranie, IFNAMSIZ - 1);

        /*defining the sockaddr_in*/
        addr = (struct sockaddr_in*)&ifr.ifr_addr;


        /*convert ip address in correct format to write*/
        inet_pton(AF_INET, ip_address, &addr->sin_addr);
        ioctl(fd, SIOCSIFADDR, &ifr);

        inet_pton(AF_INET, ip_mask, &addr->sin_addr);
        ioctl(fd, SIOCSIFNETMASK, &ifr);
        /*Setting the Ip Address using ioctl*/

        close(fd);

        printf("IP Address updated sucessfully.\n");

        /*Getting the Ip Address after Updating.*/

        /*clear ip_address buffer with 0x20- space*/
        memset(ip_address, 0x20, 15);
        ioctl(fd, SIOCGIFADDR, &ifr);
        /*Extracting Ip Address*/
        strcpy(ip_address, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
        rozhranie = ifr.ifr_name;

        printf("Updated IP Address is: %s\n", ip_address);

        memset(jsonMessage, 0, sizeof(jsonMessage));
        strcat(jsonMessage, "{\"ip\":\"");
        strcat(jsonMessage, ip_address);
        strcat(jsonMessage, "\",\"rozhranie\":\"");
        strcat(jsonMessage, rozhranie);
        strcat(jsonMessage, "\"}");
    }

    free(ipCopy);
    free(ipMaskCopy);
    return jsonMessage;
}




