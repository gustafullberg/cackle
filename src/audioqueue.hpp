#ifndef AUDIOQUEUE_HPP
#define AUDIOQUEUE_HPP

#include "config.hpp"

#define NUM_QUEUE_SLOTS 4

class AudioQueue
{
protected:
    float data[NUM_QUEUE_SLOTS * FRAME_LEN];
    int writeSlot;
    int readSlot;
    
public:
    AudioQueue() : writeSlot(0), readSlot(0) {}
    
    bool hasData() const {
        return readSlot != writeSlot;
    }
    
    void enqueue(const float *buffer) {
        float *p = data + writeSlot * FRAME_LEN;
        for(int i = 0; i < FRAME_LEN; i++) {
            p[i] = buffer[i];
        }
        writeSlot = (writeSlot + 1) % NUM_QUEUE_SLOTS;
    }
    
    void dequeue(float *buffer) {
        float *p = data + readSlot * FRAME_LEN;
        for(int i = 0; i < FRAME_LEN; i++) {
            buffer[i] = p[i];
        }
        readSlot = (readSlot + 1) % NUM_QUEUE_SLOTS;
    }
    
    void reset() {
        writeSlot = 0;
        readSlot = 0;
    }
};

#endif
