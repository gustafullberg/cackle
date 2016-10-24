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
    float zeroFrame[FRAME_LEN] = { 0.0f };
    
public:
    Encoder();
    ~Encoder();
    void enqueueAudio(float *buf);
    bool dequeuePacket(RtpPacket &packet, int &len);
};

#endif
