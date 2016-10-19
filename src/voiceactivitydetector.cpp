#include <stdio.h>
#include "voiceactivitydetector.hpp"
#include "config.hpp"

bool VoiceActivityDetector::isSpeech(const float *buf)
{
    const float adaptUp   = 0.00001;
    const float adaptDown = 0.05;
    
    float energy = 0.0f;
    for(int i = 0; i < FRAME_LEN; i++) {
        energy += buf[i]*buf[i];
    }
    
    // Initialize noise floor with energy of first frame
    if(noiseFloor < 0.0f) noiseFloor = energy;
    
    float a = (energy > noiseFloor) ? adaptUp : adaptDown;
    noiseFloor = (1.0f - a) * noiseFloor + a * energy;
    
    bool voice = energy > noiseFloor * 20.0f;
    
    if(voice) {
        hangover = 25;
    } else if(hangover > 0) {
        hangover--;
        voice = true;
    }
    
    //fprintf(stderr, "VAD %d, Energy %f, Noise %f, hangover %d\n", voice, energy, noiseFloor, hangover);
    
    
    return voice;
}
