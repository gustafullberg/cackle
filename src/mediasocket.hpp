#ifndef MEDIASOCKET_HPP
#define MEDIASOCKET_HPP

#include <cstdint>

// Socket for sending and receiving media packets

class MediaSocket
{
protected:
    int s;

public:
    MediaSocket();
    ~MediaSocket();
    void send(const void *buffer, int len, uint64_t ip, uint16_t port);
    bool receive(void *buffer, int bufLen, int &len, uint32_t &ip, uint16_t &port);
    uint16_t getPortNumber();
};

#endif
