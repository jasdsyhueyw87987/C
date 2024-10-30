#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int ping_port(const char *ip, int port) {
    int sock;
    struct sockaddr_in server;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        return -1;
    }

    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Try to connect to the server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Port is closed or unreachable");
        close(sock);
        return -1;
    }

    printf("Port %d is open on %s\n", port, ip);
    close(sock);
    return 0;
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
