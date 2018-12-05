#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

static int InitListenSocket(int port, int backlog)
{
    int fd;
    struct sockaddr_in server;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1){
        perror("create socket");
        return fd;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if ( bind(fd, (struct sockaddr*)&server, sizeof(server)) < 0){
        perror("bind socket");
        return -1;
    }

    printf("socket listening port %d\n", port);
    if (listen(fd, backlog) < 0){
        perror("listen socket");
        return -1;
    }

    return fd;
}

int AcceptNewConnect(int epfd, int listen_fd)
{
    struct epoll_event event;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("accept");
        return -1;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip, INET_ADDRSTRLEN);
    printf("new fd=[%d]. connectioned hostinfo= %s:%d\n", client_fd, ip, ntohs(client_addr.sin_port));

    event.events = EPOLLIN | EPOLLET;
    event.data.fd = client_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);
    return 0;
}

#define BUFFER_SIZE     (4096)
int RecvMsgFormEvent(int epfd, struct epoll_event ev)
{
    int client_fd = ev.data.fd;
    char buffer[BUFFER_SIZE] = {0};
    int n = read(client_fd, buffer, BUFFER_SIZE);
    if (n > 0) {
        printf("read[%d]:%s\n", client_fd, buffer);
        int wsize = write(client_fd, buffer, n);
        if (wsize <= 0){
            perror("write error");
        }
        //TODO what you want.
        memset(&buffer, 0, BUFFER_SIZE);
    } else if (n == 0){
        printf("fd=[%d] disconnected\n", client_fd);
        epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
    } else {
        // if error == Connection reset by peer(RST)
        perror("read");
    }

    return 0;
}

#define SOCKET_PORT     (8888)
#define BACK_LOG        (1024)  // listen min cnt
#define MAX_EVENTS      (10)
int main()
{
    int epfd, listen_fd, nfds;
    struct epoll_event ev, events[MAX_EVENTS];

    // Since Linux 2.6.8,the size argument is ignored
    epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        return -1;
    }

    listen_fd = InitListenSocket(SOCKET_PORT, BACK_LOG);
    if (listen_fd == -1) return -1;

    fflush(NULL);
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    if ( epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev) == -1 ) {
        perror("epoll_ctl");
        goto END;
    }

    // new event listen signal to exit
    int loop = 1;
    int n;
    while(loop){
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1){
            perror("epoll_wait");
            goto END;
        }

        for(n = 0; n < nfds; ++n){
            if (events[n].data.fd == listen_fd) {
                AcceptNewConnect(epfd, listen_fd);
            }else if (events[n].events & EPOLLIN){
                // read data
                RecvMsgFormEvent(epfd, events[n]);
            }else if (events[n].events & EPOLLOUT){
                // write data
                printf("new you can write date to kernel cache. you are welcome\n");
            }else{
                printf("error no deal events event=0x%x\n", events[n].events);
            }
        }
    }

    // the file descriptor returned by epoll_create() should be closed by using close(2)
    /*  A successful close does not guarantee that the data has been
    successfully saved to disk, as the kernel uses the buffer cache to
    defer writes.  Typically, filesystems do not flush buffers when a
    file is closed.  If you need to be sure that the data is physically
    stored on the underlying disk, use fsync(2).  (It will depend on the disk hardware at this point.)*/
END:
    close(listen_fd);
    close(epfd);
    return 0;
}

