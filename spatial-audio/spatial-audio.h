#pragma once
#include "agora_manager.h"
#include <IAgoraMediaPlayer.h>
#include <IAgoraMediaPlayerSource.h>
#include <IAgoraSpatialAudio.h>


#include <cmath>

using namespace agora::base;
using namespace agora::media::base;
//using namespace std;

#define PLAYER_POSITION_CHANGED         0x00000005
#define PLAYER_STATE_CHANGED          0x00000006

class SpatialAudioEventHandler : public AgoraManagerEventHandler
{
public:
    virtual ~SpatialAudioEventHandler() = default;
};

class MediaPlayerSourceObserver : public IMediaPlayerSourceObserver
{
public:
    //set the message notify window handler
    void SetMsgReceiver(HWND hWnd) { m_hMediaMsgHanlder = hWnd; }
    HWND getMediaEventHandler() { return m_hMediaMsgHanlder; }
    virtual void onPlayerSourceStateChanged(MEDIA_PLAYER_STATE state, MEDIA_PLAYER_ERROR ec)
    {
        HWND mediaMsgHandler = getMediaEventHandler();
        if (mediaMsgHandler)
        {
            // Call OnEIDMediaPlayerStateChanged() when the state of the player changes.
            ::PostMessage(mediaMsgHandler, WM_MSGID(PLAYER_STATE_CHANGED), (WPARAM)state, (LPARAM)ec);
        }
    }
    virtual void onPositionChanged(int64_t position)
    {
        HWND mediaMsgHandler = getMediaEventHandler();
        if (mediaMsgHandler)
        {
            // Call OnEIDMediaPlayerPositionChanged() when the state of the slider(progress bar) position changes.
            ::PostMessage(mediaMsgHandler, WM_MSGID(PLAYER_POSITION_CHANGED), (WPARAM)NULL, (LPARAM)position);
        }
    }
    virtual void onPlayerEvent(MEDIA_PLAYER_EVENT event, int64_t elapsedTime, const char* message) override
    {
        // Required to implement IMediaPlayerObserver.
    }
    virtual void onMetaData(const void* data, int length)
    {
        // Required to implement IMediaPlayerObserver
    }
    virtual void onPlayBufferUpdated(int64_t playCachedBuffer)
    {
        // Required to implement IMediaPlayerObserver
    }
    virtual void onPreloadEvent(const char* src, PLAYER_PRELOAD_EVENT event)
    {
        // Required to implement IMediaPlayerObserver
    }
    virtual void onAgoraCDNTokenWillExpire()
    {
        // Required to implement IMediaPlayerObserver
    }
    virtual void onPlayerSrcInfoChanged(const SrcInfo& from, const SrcInfo& to) override
    {
        // Required to implement IMediaPlayerSourceObserver
    }
    virtual void onPlayerInfoUpdated(const PlayerUpdatedInfo& info) override
    {
        // Required to implement IMediaPlayerObserver
    }
    virtual void onAudioVolumeIndication(int volume) override
    {
        // Required to implement IMediaPlayerObserver
    }
    virtual void onCompleted()
    {
        // Occurs when the media player finishes playing a media file.
    }
private:
    HWND m_hMediaMsgHanlder;
};

class SpatialAudioManager : public AgoraManager
{
public:
    SpatialAudioManager(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
        : AgoraManager(hInstance, appId, channelName, token)
    {
        AgoraEventStrategy = std::make_shared<SpatialAudioEventHandler>();
    }

    virtual void setupVideoSDKEngine()override;
    virtual void createSpecificGui(HWND& guiWindowReference)override;
    virtual void join() override;
    virtual void leave() override;
    virtual void handleGuiAction(int commandId) override;
    virtual void handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam) override;

    void configureSpatialAudioEngine();
    void updateRemoteSpatialAudioPosition();

public:
    ILocalSpatialAudioEngine* localSpatial;
    BOOL isSpatialAudio = false;
    HWND distanceBarSlider;

};
