#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

/* for client
 * 1. creat
 * 2. connect
 * 3. accept
 * */
int BlockClient(char* host, int port)
{
    struct sockaddr_in server;
    int fd, read_size;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("create socket");
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(host);
    server.sin_port = htons(port);
    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("Connect");
        return -1;
    }

    char buf[1024] = "hello server. I am client.\n";
    read_size = recv(fd, buf, sizeof(buf), 0);
    if (read_size > 0) printf("recv server msg=%s\n", buf);

    memset(&buf, 0, sizeof(buf));
    if (send(fd, buf, 30, 0) < 0){
        perror("Send()");
        return -1;
    }

    close(fd);
}

int main(int argc, char* argv[])
{
    char* host = "127.0.0.1";
    BlockClient(host, 8888);
}

