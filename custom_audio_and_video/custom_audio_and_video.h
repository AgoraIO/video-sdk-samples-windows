
//custom_audio_and_video.h
#pragma once
#include "agora_manager.h"
#include <memory>
#include <thread>
#include <chrono>

#include <fstream>   // Play external audio
#include "opencv2/opencv.hpp"  // Play external video

class CustomAudioVideoSourceEventHandler : public AgoraManagerEventHandler
{
public:
    virtual ~CustomAudioVideoSourceEventHandler() = default;
    
};


class CustomAudioVideoSource : public AgoraManager
{
public:
    CustomAudioVideoSource(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
        : AgoraManager(hInstance, appId, channelName, token)
    {
        AgoraEventStrategy = std::make_shared<CustomAudioVideoSourceEventHandler>();
    }

    virtual void createSpecificGui(HWND& guiWindowReference)override;
    virtual void handleGuiAction(int commandId) override;
    virtual void createvideoCanvasAndJoin() override;
    virtual void join()override;
    virtual void leave()override;
    virtual void Run();

    bool setupMediaEngine();
    void setCutommSourceInput();

    void playExternalVideo(IRtcEngine* agoraEngine);
    void disableExternalVideoSource();
    
    void OnBnClickedExternalAudioButton();
    void playExternalAudio(IRtcEngine* agoraEngine);
    void disableExternalAudioSource();
    
public:
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    bool isCustomAudioSource = false;
    bool isCustomVideoSource = false;
    // Variables for custom video source implementation 
    std::atomic<bool> captureExternalVideo;
    std::thread videoFrameThread;
    std::string customVideoPath = "";
    // Variables for custom audio source implementation 
    ChannelMediaOptions option;
    std::atomic<bool> captureExternalAudio;
    std::thread audioFrameThread;
    std::string customAudioPath = "";
    const int SAMPLE_RATE = 48000;
    const int PUSH_FREQ_PER_SEC = 20;
    const int  SAMPLE_NUM_OF_CHANNEL = 2;
    const int BYTESPERSAMPLE = 2;
    HWND externalAudioBtn; // UI element for a button to play external audio
    
    int tokenRole = 1; // The token role: Broadcaster or Audience
    std::string serverUrl = "";//"<The base URL to your token server>"; //For example, "https://agora-token-service-production-92ff.up.railway.app"
    int expireTime;; //Expire time in Seconds.
    uid_t uid = 1; // An integer that identifies the user
};