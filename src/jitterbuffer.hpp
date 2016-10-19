#ifndef JITTERBUFFER_HPP
#define JITTERBUFFER_HPP

#include <utility>
#include "config.hpp"
#include "rtppacket.hpp"

#define NUM_JITTERBUFFER_SLOTS 4

class Jitterbuffer
{
protected:
    std::pair<int, RtpPacket> data[NUM_JITTERBUFFER_SLOTS];
    int writeSlot;
    int readSlot;
    
public:
    Jitterbuffer() : writeSlot(0), readSlot(0) {}
    
    bool hasData() const {
        return readSlot != writeSlot;
    }
    
    void enqueue(int len, const RtpPacket &packet) {
        data[writeSlot].first = len;
        data[writeSlot].second = packet;
        writeSlot = (writeSlot + 1) % NUM_JITTERBUFFER_SLOTS;
    }
    
    int dequeue(RtpPacket &packet) {
        packet = data[readSlot].second;
        int len = data[readSlot].first;
        readSlot = (readSlot + 1) % NUM_JITTERBUFFER_SLOTS;
        return len;
    }
    
    void reset() {
        writeSlot = 0;
        readSlot = 0;
    }
};

#endif
