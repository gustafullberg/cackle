#include <iostream>
#include <thread>
#include <chrono>
#include <portaudio.h>
#include <iostream>
#include <csignal>
#include "encoder.hpp"
#include "mediasocket.hpp"
#include "discoverysocket.hpp"
#include "endpoint.hpp"

struct State {
    Encoder encoder;
    EndpointCollection endpoints;
};

static volatile bool quit = 0;

int audioCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
    RtpPacket packet;
    
    State *state = (State*)userData;
    Encoder *encoder = &state->encoder;
    EndpointCollection &endpoints = state->endpoints;
    float *in  = (float*)inputBuffer;
    float *out = (float*)outputBuffer;
    
    // Queue audio for encoding
    encoder->enqueueAudio(in);
    
    // Decode audio or conceal loss/DTX
    endpoints.getAudio(out);
    
    return 0;
}

int64_t getTimeInMilliseconds()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void intHandler(int dummy)
{
    quit = true;
}

int main()
{
    Pa_Initialize();
    PaStream *stream;
    PaError err;
    State state;
    RtpPacket sendPacket; 
    RtpPacket receivePacket;
    MediaSocket mediaSocket;
    DiscoverySocket discoverySocket(mediaSocket.getPortNumber());

    // Take care of Ctrl-C
    signal(SIGINT, intHandler);
    
    err = Pa_OpenDefaultStream(&stream, 1, 1, paFloat32, FS, FRAME_LEN, audioCallback, (void*)&state);
    if(err != paNoError) return 1;
    
    Pa_StartStream(stream);

    int64_t time = getTimeInMilliseconds();
    uint32_t ip;
    uint16_t port;
    
    while(!quit) {
        // Time elapsed since last iteration
        uint64_t timeDelta = getTimeInMilliseconds() - time;
        time += timeDelta;

        // Handle heartbeats
        discoverySocket.sendHeartbeatIfTime(timeDelta);
        while(discoverySocket.receiveHeartbeat(ip, port)) {
            state.endpoints.update(ip, port);
        }
        state.endpoints.handleTimeout((int)timeDelta);

        // Send media packets queued for transmission
        int packetLen;
        while(state.encoder.dequeuePacket(sendPacket, packetLen)) {
            for(Endpoint *e : state.endpoints.getAllEndpoints()) {
                mediaSocket.send((const void*)&sendPacket, packetLen, e->getIP(), e->getPort());
            }
        }

        // Receive media packets
        int recvlen;
        uint32_t ip;
        uint16_t port;
        while(mediaSocket.receive(&receivePacket, sizeof(receivePacket), recvlen, ip, port)) {
            state.endpoints.addPacket(recvlen, receivePacket, ip, port);
        }

        // Give the CPU some slack
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }
    
    Pa_StopStream(stream);
    Pa_Terminate();

    fprintf(stderr, "\nBye!\n");
    
    return 0;
}
