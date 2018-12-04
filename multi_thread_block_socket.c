#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/tcp.h> 
#include <netinet/in.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <pthread.h>

void* server_handler(void* pfd)
{
    int fd = *((int*)pfd);
    int read_size, writ_size;
    char client_message[1024];
    memset(&client_message, 0, 1024);

    while((read_size = read(fd, client_message, 1024)) > 0){
        //printf("client[%d]:%s\n", fd, client_message);
        writ_size = write(fd, client_message, read_size);
        memset(&client_message, 0, 1024);
    }

    if (read_size == 0) {
        int flag = 1; 
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
    }else if (read_size < 0) puts("recv error");

    printf("client[%d] disconnected.\n", fd);

    close(fd); // send fin to client
    free(pfd);
    return 0;
}

int main(int argc, char* argv[])
{
    int fd, port;
    int* new_sock;
    struct sockaddr_in server, client;

    port = 8888; // default port
    if (argc > 1) port = atoi(argv[1]);

    // create
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0){
        perror("create socket");
        return -1;
    }

    // bind
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    if ( bind(fd, (struct sockaddr*)&server, sizeof(server)) < 0){
        perror("bind socket error");
        return -1;
    }

    // listen 
    if (listen(fd, 1024) < 0){
        perror("listen socket error");
        return -1;
    }

    puts("waiting for connectios...\n");

    // accept
    int conn_fd, client_len;
    client_len = sizeof(struct sockaddr_in);
    while((conn_fd = accept(fd, (struct sockaddr*)&client, (socklen_t*)&client_len)) > 0) {
        pthread_t thread;
        new_sock = malloc(1);
        *new_sock = conn_fd;
        int error = pthread_create(&thread, NULL, server_handler, new_sock);
        if (error) {
            perror("pthread_create.");
            continue;
        }
        pthread_detach(thread);
    }

    if (conn_fd < 0){
        perror("accept fail");
        return 1;
    }

    close(fd);
    return 0;
}
