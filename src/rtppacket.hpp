#ifndef RTPPACKET_HPP
#define RTPPACKET_HPP

#include <cstdint>

struct RtpPacket
{
    unsigned char header[12] = { 0 };
    unsigned char payload[1024];
    
    void setHeader(uint16_t seqNo, uint32_t timestamp) {
        // RTP version 2
        header[ 0] = 128;
        
        // PT
        header[ 1] = 0;
        
        // Sequence number
        header[ 2] = (seqNo >> 8);
        header[ 3] = (seqNo & 0xFF);
        
        // Timestamp
        header[ 4] = (timestamp >> 24) & 0xFF;
        header[ 5] = (timestamp >> 16) & 0xFF;
        header[ 6] = (timestamp >>  8) & 0xFF;
        header[ 7] = (timestamp >>  0) & 0xFF;
        
        // SSRC identifier
        header[ 8] = 0;
        header[ 9] = 0;
        header[10] = 0;
        header[11] = 0;
    }
};

#endif