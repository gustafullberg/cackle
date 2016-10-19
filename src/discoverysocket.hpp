#ifndef DISCOVERYSOCKET_HPP
#define DISCOVERYSOCKET_HPP

#include <cstdint>

// Use multicast to broadcast presence and find other users

class DiscoverySocket
{
protected:
    int s;
    uint16_t portToAnnounce;
    int64_t timeToHeartbeat;

public:
    DiscoverySocket(uint16_t _portToAnnounce);
    ~DiscoverySocket();
    void sendHeartbeatIfTime(uint64_t deltaTime);
    bool receiveHeartbeat(uint32_t &ip, uint16_t &port);
};

#endif
