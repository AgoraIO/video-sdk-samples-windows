#pragma once
#include "agora_manager.h"

class CustomVideoFrameObserver : public IVideoFrameObserver {
public:
	CustomVideoFrameObserver() : originalYBuffer(nullptr), originalUBuffer(nullptr), originalVBuffer(nullptr) {}
	~CustomVideoFrameObserver() {
		delete[] originalYBuffer;
		delete[] originalUBuffer;
		delete[] originalVBuffer;
	}
	virtual bool onCaptureVideoFrame(agora::rtc::VIDEO_SOURCE_TYPE sourceType, VideoFrame& videoFrame)override;
	virtual bool onPreEncodeVideoFrame(agora::rtc::VIDEO_SOURCE_TYPE sourceType, VideoFrame& videoFrame)override;
	virtual bool onMediaPlayerVideoFrame(VideoFrame& videoFrame, int mediaPlayerId)override;
	virtual bool onRenderVideoFrame(const char* channelId, rtc::uid_t remoteUid, VideoFrame& videoFrame)override;
	virtual bool onTranscodedVideoFrame(VideoFrame& videoFrame)override;
	virtual VIDEO_FRAME_PROCESS_MODE getVideoFrameProcessMode()override;
private:
	uint8_t* originalYBuffer;
	uint8_t* originalUBuffer;
	uint8_t* originalVBuffer;
};

class CustomAudioFrameObserver : public IAudioFrameObserver {
	virtual bool onPlaybackAudioFrameBeforeMixing(const char* channelId, rtc::uid_t uid, AudioFrame& audioFrame)override;
	virtual bool onRecordAudioFrame(const char* channelId, AudioFrame& audioFrame)override;
	virtual bool onPlaybackAudioFrame(const char* channelId, AudioFrame& audioFrame)override;
	virtual bool onMixedAudioFrame(const char* channelId, AudioFrame& audioFrame)override;
	virtual bool onEarMonitoringAudioFrame(AudioFrame& audioFrame)override;
	virtual int getObservedAudioFramePosition()override;
	virtual AudioParams getPlaybackAudioParams()override;
	virtual AudioParams getRecordAudioParams()override;
	virtual AudioParams getMixedAudioParams()override;
	virtual AudioParams getEarMonitoringAudioParams()override;
};

class RawAudioVideoEventHandler : public AgoraManagerEventHandler
{
public:
	virtual ~RawAudioVideoEventHandler() = default;
};

class RawAudioVideoManager : public AgoraManager
{
public:
	RawAudioVideoManager(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
		: AgoraManager(hInstance, appId, channelName, token) 
	{
		AgoraEventStrategy = std::make_shared<RawAudioVideoEventHandler>();
	}

	virtual void createSpecificGui(HWND& guiWindowReference)override;
	virtual void handleGuiAction(int commandId) override;
	virtual void createvideoCanvasAndJoin()override;
	virtual void join()override;
	virtual void leave()override;
	void Run();

	bool EnableAudioVideoCapture(bool bEnable);
	bool setupMediaEngine();
	void OnBnClickedZoomInZoomOutButton();

public:
	HWND zoomButton;// UI element for a button to ZoomIn/ZoomOut
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	
	// Video and audio frame observers objects
	CustomVideoFrameObserver* video_frame_observer = new CustomVideoFrameObserver();
	CustomAudioFrameObserver* audio_frame_observer = new CustomAudioFrameObserver();
	
};