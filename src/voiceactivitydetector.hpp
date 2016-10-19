#ifndef VOICEACTIVITYDETECTOR_HPP
#define VOICEACTIVITYDETECTOR_HPP

class VoiceActivityDetector
{
protected:
    float noiseFloor;
    int hangover;
    
public:
    VoiceActivityDetector() : noiseFloor(-1.0f), hangover(0) {}
    bool isSpeech(const float *buf);
};

#endif
