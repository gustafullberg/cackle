#include <opus/opus.h>
#include "encoder.hpp"

Encoder::Encoder()
{
    int error;
    encoderState = (void*)opus_encoder_create(FS, 1, OPUS_APPLICATION_VOIP, &error);
    opus_encoder_ctl((OpusEncoder*)encoderState, OPUS_SET_BITRATE(BITRATE));
    opus_encoder_ctl((OpusEncoder*)encoderState, OPUS_SET_DTX(1));
    seqNo = 0;
    timestamp = 0;
}

void Encoder::enqueueAudio(float *buf)
{
    float *p = vad.isSpeech(buf) ? buf : zeroFrame;
    queue.enqueue(p);
}

bool Encoder::dequeuePacket(RtpPacket &packet, int &len)
{
    float inputFrame[FRAME_LEN];
    int packetLen = 0;
        
    bool audioAvailable = queue.hasData();
    if(!audioAvailable) return false;
    
    // Increase RTP timestamp
    timestamp += FRAME_LEN;
    
    queue.dequeue(inputFrame);
    len = opus_encode_float((OpusEncoder*)encoderState, inputFrame, FRAME_LEN, packet.payload, sizeof(packet.payload));
    if(len <= 2) {
        // DTX
        len = 0;
        return false;
    }
    
    // Increase RTP sequence number
    seqNo++;
    
    // Construct header of RTP packet
    packet.setHeader(seqNo, timestamp);
    len += sizeof(packet.header);

    return true;
}
