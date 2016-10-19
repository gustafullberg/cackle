#include <cassert>
#include <opus/opus.h>
#include "endpoint.hpp"
#include "config.hpp"

Endpoint::Endpoint(uint32_t ip, uint16_t port) : ip(ip), port(port)
{
    int error;
    decoderState = opus_decoder_create(FS, 1, &error);
}

Endpoint::~Endpoint()
{
    opus_decoder_destroy((OpusDecoder*)decoderState);
}

void Endpoint::addPacket(int len, const RtpPacket &packet)
{
    jitterbuffer.enqueue(len, packet);
}

void Endpoint::dequeueAudio(float *buf)
{
    RtpPacket packet;
    int packetLen = 0;

    bool packetAvailable = jitterbuffer.hasData();
    if(packetAvailable) {
        packetLen = jitterbuffer.dequeue(packet);
    }

    int numSamples = opus_decode_float((OpusDecoder*)decoderState, (packetAvailable) ? packet.payload : nullptr, packetLen, buf, FRAME_LEN, 0);
    assert(numSamples == FRAME_LEN);
}

EndpointCollection::EndpointCollection()
{
    endpoints.reserve(16);
}

EndpointCollection::~EndpointCollection()
{
    std::lock_guard<std::mutex> guard(mutex);
    for(int i = 0; i < endpoints.size(); i++) {
        delete endpoints[i];
    }
}

void EndpointCollection::addEndpointIfMissing(uint32_t ip, uint16_t port)
{
    // Check id endpoint already exists
    for(const Endpoint *e : endpoints) {
        if(e->getIP() == ip && e->getPort() == port) return;
    }

    // Add otherwise
    std::lock_guard<std::mutex> guard(mutex);
    endpoints.push_back(new Endpoint(ip, port));
}

void EndpointCollection::addPacket(int len, const RtpPacket &packet, uint32_t ip, uint16_t port)
{
    for(Endpoint *e : endpoints) {
        if(e->getIP() == ip && e->getPort() == port) {
            e->addPacket(len, packet);
            return;
        }
    }
}

void EndpointCollection::getAudio(float *buf)
{
    float frame[FRAME_LEN];

    // Zero buffer
    for(int i = 0; i < FRAME_LEN; i++) buf[i] = 0.0f;

    // Mix channels
    std::lock_guard<std::mutex> guard(mutex);
    for(Endpoint *e : endpoints) {
        e->dequeueAudio(frame);
        for(int i = 0; i < FRAME_LEN; i++) buf[i] += frame[i];
    }
}
