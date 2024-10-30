#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

int ping_port(const char *ip, int port) {
    int sock;
    struct sockaddr_in server;
    struct timeval timeout;
    fd_set fdset;
    
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        return -1;
    }

    // Set the socket to non-blocking
    fcntl(sock, F_SETFL, O_NONBLOCK);

    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Start connecting
    int result = connect(sock, (struct sockaddr*)&server, sizeof(server));
    if (result < 0) {
        if (errno != EINPROGRESS) {
            perror("Connection error");
            close(sock);
            return -1;
        }
    }

    // Set up the file descriptor set and timeout for select
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    timeout.tv_sec = 1;     // 1 second timeout
    timeout.tv_usec = 0;

    // Wait for the socket to be ready for writing or timeout
    result = select(sock + 1, NULL, &fdset, NULL, &timeout);
    if (result == 1) {
        int so_error;
        socklen_t len = sizeof(so_error);

        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error == 0) {
            printf("Port %d is open on %s\n", port, ip);
            close(sock);
            return 0;
        } else {
            fprintf(stderr, "Port is closed or unreachable\n");
        }
    } else if (result == 0) {
        fprintf(stderr, "Connection timed out\n");
    } else {
        perror("select error");
    }

    close(sock);
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <Port>\n", argv[0]);
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    if (ping_port(ip, port) == 0) {
        printf("Ping to %s:%d successful.\n", ip, port);
    } else {
        printf("Ping to %s:%d failed.\n", ip, port);
    }

    return 0;
}
