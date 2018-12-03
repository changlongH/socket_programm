#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>


/* For service
 * 1. Create socket
 * 2. Bind to address and port
 * 3. Put in listening mode
 * 4. Accept connections and process there after
 * http://man7.org/linux/man-pages/man2/socket.2.html
 */

#define SOCKET_PORT     (8888)
int StartBlockSocket()
{
    int socket_desc, client_sock, read_size, client_len;
    // sockaddr_in equal sockaddr
    struct sockaddr_in server, client;

    // Create args=ipv4, tcp stream, default protocol
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1){
        perror("create socket error");
        return -1;
    }

    // Bind
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; //inet_addr("0.0.0.0");
    server.sin_port = htons(SOCKET_PORT);
    if ( bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0){
        perror("bind socket error");
        return -1;
    }

    // Listen 
    /* 1. syns queue system default 
     * 2. accept queue  min(backlog, system)
     * */
    printf("waiting for connections...\n");
    int backlog = 1;
    client_len = sizeof(struct sockaddr_in);
    if (listen(socket_desc, backlog) < 0){
        perror("listen socket error");
        return -1;
    }

    // Accept
    client_sock = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&client_len);
    if (client_sock < 0){
        perror("accpt client error");
        return -1;
    }

    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client.sin_addr), str, INET_ADDRSTRLEN);
    printf("connection accepted. ip=%s\n", str);

    // send msg to client
    char buff[1024] = "hello client. I am server.";
    write(client_sock, buff, 30);
    memset(&buff, 0, sizeof(buff));

    // recv client msg until close
    while(read_size = recv(client_sock, buff, 1024, 0) > 0){
        printf("recv client msg=%s\n", buff);
    }

    printf("client disconnect...\n");
    close(client_sock);

    return 0;
}

int main()
{
    StartBlockSocket();
    return 0;
}

