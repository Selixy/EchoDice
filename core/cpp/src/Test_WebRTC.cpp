#include <string>

#ifdef HAVE_WEBRTC
    #include <rtc/rtc.hpp>
#endif

extern "C" const char* Test_WebRTC() {
    static std::string result;

#ifdef HAVE_WEBRTC
    result = " WebRTC détecté et accessible.";
#else
    result = " WebRTC non détecté (fallback).";
#endif

    return result.c_str();
}
