#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#endif
#include <fcntl.h>
#include "discoverysocket.hpp"

#define DISCOVERY_ADDR "226.1.1.1"
#define DISCOVERY_PORT 9876
#define DISCOVERY_HEARTBEAT_TIME_MS 1000

struct HeartbeatMsg
{
    uint16_t port;
};

DiscoverySocket::DiscoverySocket(uint16_t _portToAnnounce)
{
    int rc;

    // The port we want to broadcast in the heartbeat
    portToAnnounce = _portToAnnounce;

    // Time to next heartbeat
    timeToHeartbeat = 0;

    // Create socket
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s < 0) {
        perror("socket failed");
        exit(1);
    }

#ifndef _MSC_VER
    // Allow port to be bound by several sockets on the same system
    int optval = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
#endif

    // Bind socket to port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(DISCOVERY_PORT);
    rc = bind(s, (struct sockaddr*)&addr, sizeof(addr));
    if(rc < 0) {
        perror("bind failed");
        exit(1);
    }

    // Join multicast group
    struct ip_mreq group;
    group.imr_multiaddr.s_addr = inet_addr(DISCOVERY_ADDR);
    group.imr_interface.s_addr = htonl(INADDR_ANY);
    rc = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&group, sizeof(group));
    if(rc < 0) {
        perror("setsockopt failed");
        exit(1);
    }

#if 1
    // Disable loopback
    char loopback = 0;
    rc = setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, &loopback, sizeof(loopback));
    if(rc < 0) {
        perror("setsockopt failed");
        exit(1);
    }
#endif

    // Make socket non-blocking
#ifdef _MSC_VER
    unsigned long nonblocking = 1;
    ioctlsocket(s, FIONBIO, &nonblocking);
#else
    rc = fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);
    if(rc == -1) {
        perror("fcntl failed");
        exit(1);
    }
#endif

}

DiscoverySocket::~DiscoverySocket()
{
    // Close socket
#ifdef _MSC_VER
    closesocket(s);
#else
    close(s);
#endif
}

void DiscoverySocket::sendHeartbeatIfTime(uint64_t deltaTime)
{
    timeToHeartbeat -= deltaTime;
    if(timeToHeartbeat <= 0) {
        timeToHeartbeat += DISCOVERY_HEARTBEAT_TIME_MS;

        // Address of multicast group
        struct sockaddr_in group_addr;
        group_addr.sin_family = AF_INET;
        group_addr.sin_addr.s_addr = inet_addr(DISCOVERY_ADDR);
        group_addr.sin_port = htons(DISCOVERY_PORT);

        // Set up message
        HeartbeatMsg msg;
        msg.port = htons(portToAnnounce);

        // Send heartbeat message
        sendto(s, (const char*)&msg, sizeof(msg), 0, (struct sockaddr*)&group_addr, sizeof(group_addr));
    }
}

bool DiscoverySocket::receiveHeartbeat(uint32_t &ip, uint16_t &port)
{
    struct sockaddr_in remote_addr;
    socklen_t addr_len = sizeof(remote_addr);

    HeartbeatMsg msg;

    // Receive (non-blocking)
    int len = (int)recvfrom(s, (char*)&msg, sizeof(msg), 0, (struct sockaddr*)&remote_addr, &addr_len);
    if(len > 0) {
        ip = ntohl(remote_addr.sin_addr.s_addr);
        port = ntohs(msg.port);
        //fprintf(stderr, "Host %s is announcing port %d\n", inet_ntoa(remote_addr.sin_addr), port, len);
        return true;
    }

    return false;
}
