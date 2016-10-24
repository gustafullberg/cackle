#include <cstdio>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "mediasocket.hpp"

MediaSocket::MediaSocket()
{
    int rc;

    // Create socket
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s < 0) {
        perror("socket failed");
        exit(1);
    }

    // Bind socket to random port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(0);
    rc = bind(s, (struct sockaddr*)&addr, sizeof(addr));
    if(rc < 0) {
        perror("bind failed");
        exit(1);
    }

    // Make socket non-blocking
    rc = fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);
    if(rc == -1) {
        perror("fcntl failed");
        exit(1);
    }
}

MediaSocket::~MediaSocket()
{
    // Close socket
    close(s);
}

void MediaSocket::send(const void *buffer, int len, uint32_t ip, uint16_t port)
{
    // Address
    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = htonl(ip);
    remote_addr.sin_port = htons(port);

    // Send
    sendto(s, buffer, len, 0, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
}

bool MediaSocket::receive(void *buffer, int bufLen, int &len, uint32_t &ip, uint16_t &port)
{
    struct sockaddr_in remote_addr;
    socklen_t addr_len = sizeof(remote_addr);

    // Receive (non-blocking)
    len = (int)recvfrom(s, buffer, bufLen, 0, (struct sockaddr*)&remote_addr, &addr_len);
    if(len > 0) {
        ip = ntohl(remote_addr.sin_addr.s_addr);
        port = ntohs(remote_addr.sin_port);
        return true;
    } else {
        len  = 0;
        return false;
    }
}

uint16_t MediaSocket::getPortNumber()
{
    // Get number of bound port
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    getsockname(s, (struct sockaddr*)&addr, &len);
    return ntohs(addr.sin_port);
}
