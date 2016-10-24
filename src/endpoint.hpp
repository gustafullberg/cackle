#ifndef ENDPOINT_HPP
#define ENDPOINT_HPP

#include <cstdint>
#include <vector>
#include <mutex>
#include "jitterbuffer.hpp"

class Endpoint
{
protected:
    uint32_t ip;
    uint16_t port;
    Jitterbuffer jitterbuffer;
    void *decoderState;
    int timeout;

public:
    Endpoint(uint32_t ip, uint16_t port);
    ~Endpoint();
    uint32_t getIP() const { return ip; }
    uint16_t getPort() const { return port; }
    void addPacket(int len, const RtpPacket &packet);
    void dequeueAudio(float *buf);
    void resetTimeout();
    bool timePassed(int timeDelta);
};

class EndpointCollection
{
protected:
    std::vector<Endpoint*> endpoints;
    std::mutex mutex;

public:
    EndpointCollection();
    ~EndpointCollection();
    void update(uint32_t ip, uint16_t port);
    std::vector<Endpoint*>& getAllEndpoints() { return endpoints; }
    void addPacket(int len, const RtpPacket &packet, uint32_t ip, uint16_t port);
    void getAudio(float *buf);
    void handleTimeout(int timeDelta);
};

#endif
