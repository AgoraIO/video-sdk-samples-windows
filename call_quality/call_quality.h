#pragma once
#include "agora_manager.h"


class CallQualityEventHandler : public AgoraManagerEventHandler
{
public:
	
    virtual ~CallQualityEventHandler() = default;
	virtual void onLastmileQuality(int quality) override;
	virtual void onLastmileProbeResult(const LastmileProbeResult& result) override;
	virtual void onNetworkQuality(uid_t uid, int txQuality, int rxQuality) override;
	virtual void onRtcStats(const RtcStats& stats) override;
	virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed);
	virtual void onRemoteVideoStats(const RemoteVideoStats& stats);
};

class CallQuality : public AgoraManager
{
public:
	CallQuality(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
		: AgoraManager(hInstance, appId, channelName, token)
	{
		AgoraEventStrategy = std::make_shared<CallQualityEventHandler>();
		
	}

	virtual void createSpecificGui(HWND& guiWindowReference)override;
	virtual void setupVideoSDKEngine()override;
	virtual void handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam)override;
	virtual void handleGuiAction(int commandId) override;

	void updateNetworkStatus(int quality);
	void EchoTest();
	void VideoQuality();
	void startProbeTest();


	 LRESULT OnEIDLastMileProbeResult(WPARAM jitter, LPARAM lParam);
	 LRESULT OnEIDLastMileQuality(WPARAM quality, LPARAM lParam);
	 LRESULT OnEIDRemoteVideoStateChanged(WPARAM state, LPARAM reason);
	 LRESULT OnEIDRemoteVideoStats(WPARAM uid, LPARAM receivedBitrate);
	 LRESULT OnEIDRtcStats(WPARAM userCount, LPARAM packetLoss);
	 LRESULT OnEIDNetworkQuality(WPARAM txQuality, LPARAM rxQuality);

public:
	BOOL highQuality = true; // A boolean variable to track the remote user video quality.
	BOOL isEchoTestRunning = false; // Keeps track of the echo test.
	HWND echoTestButton; // A HWND to reference the button
	HWND networkStatus; // A HWND to reference the static text
	HWND videoQualityButton; // A HWND to reference the button
	HWND textBox; // A HWND to reference the edit control
};
