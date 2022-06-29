#include "http.h"

#define SUCCESS 0
#define ERROR 1
#define DST_IP "127.0.0.1"
#define DST_PORT 80
#define BACKLOG 10
#define BUFFER_SIZE 500
int main() {
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM,0))==-1){
        perror("Error while creating socket");
        exit(ERROR);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if(inet_aton(DST_IP, &addr.sin_addr) == 0){
        perror("inet error");
        close(sock);
        exit(ERROR);
    }
    addr.sin_port = htons(DST_PORT);
    if(bind(sock,(struct sockaddr*)&addr, sizeof(addr) )== -1){
        perror("Error while binding");
        close(sock);
        exit(ERROR);
    }
    if(listen(sock,BACKLOG) == -1){
        perror("Error while listening");
        close(sock);
        exit(ERROR);
    }

    char buffer[BUFFER_SIZE];
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    int clientSock;

    int test = 0;
    while(test == 0){
        memset(buffer,0, sizeof(buffer));
        memset(&addr,0, sizeof(addr));
        socklen_t addrlen = sizeof(addr);
        if((clientSock = accept(sock,(struct sockaddr*)&addr, &addrlen)) ==-1){
            continue;
        }
        printf("Prijalo socket\n");
        if((int)read(clientSock,buffer,BUFFER_SIZE) <= 0){
            continue;
        }
        printf("NAcitalo socket:>%s<\n", buffer);
        char * strToSend = NULL;
        strToSend = httpResolver(buffer, strlen(buffer), strToSend);
        printf("Vysledna sprava:>%s<\n", strToSend);
        //printf("Message from: (%s:%d):%s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), buffer);
        printf("%d\n", strlen(strToSend));
        write(clientSock, strToSend , strlen(strToSend));
        //write(clientSock, hello , strlen(hello));
        free(strToSend);
        close(clientSock);
        test++;
    }
    return 0;
}

/*
int main() {

    char * test = "GET api/mamvsetkyryzeny/interfaces HTTP/1.1\r\n";
    //fgets(test, 200, stdin);
    printf(">%s<\n", test);
    httpResolver(test, 200);
    return 0;


}
*/
