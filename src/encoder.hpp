#ifndef ENCODER_HPP
#define ENCODER_HPP

#include "config.hpp"
#include "audioqueue.hpp"
#include "rtppacket.hpp"
#include "voiceactivitydetector.hpp"

class Encoder
{
protected:
    void *encoderState;
    AudioQueue queue;
    VoiceActivityDetector vad;
    uint16_t seqNo;
    uint32_t timestamp;
    
public:
    Encoder();
    ~Encoder();
    void enqueueAudio(const float *buf);
    bool dequeuePacket(RtpPacket &packet, int &len);
};

#endif
