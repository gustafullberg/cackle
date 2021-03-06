project(cackle)
set(CMAKE_CXX_STANDARD 11)

if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    # As much compiler warnings as possible
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wconversion")
endif()

set(SRC
    src/audioqueue.hpp
    src/config.hpp
    src/discoverysocket.cpp
    src/discoverysocket.hpp
    src/encoder.cpp
    src/encoder.hpp
    src/endpoint.cpp
    src/endpoint.hpp
    src/jitterbuffer.hpp
    src/main.cpp
    src/mediasocket.cpp
    src/mediasocket.hpp
    src/rtppacket.hpp
    src/voiceactivitydetector.cpp
    src/voiceactivitydetector.hpp
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} portaudio)
target_link_libraries(${PROJECT_NAME} opus)
