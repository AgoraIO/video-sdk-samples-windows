// call_quality.cpp 

#include "call_quality.h"

#define EID_NETWORK_QUALITY 10
#define EID_LAST_MILE_RESULT 11
#define EID_RTC_STATS 12
#define EID_REMOTE_VIDEO_STATE_CHANGED 13
#define EID_REMOTE_VIDEO_STATS 14
#define EID_LAST_MILE_QUALITY 15

void CallQualityEventHandler::onLastmileQuality(int quality)
{
	// Send the call results to the corresponding function in the CAgoraImplementationDlg class.
	HWND m_hMsgHandler = getMsgEventHandler();
	if (m_hMsgHandler)
	{
		::PostMessage(m_hMsgHandler, WM_MSGID(EID_LAST_MILE_QUALITY), (WPARAM)quality, (LPARAM)NULL);
	}
}
void CallQualityEventHandler::onRtcStats(const RtcStats& stats)
{
	// Send the call results to the corresponding function in the CAgoraImplementationDlg class.
	HWND m_hMsgHandler = getMsgEventHandler();
	if (m_hMsgHandler)
	{
		::PostMessage(m_hMsgHandler, WM_MSGID(EID_RTC_STATS), (WPARAM)stats.userCount, (LPARAM)stats.rxPacketLossRate);
	}
}
void CallQualityEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	// Send the call results to the corresponding function in the CAgoraImplementationDlg class.
	HWND m_hMsgHandler = getMsgEventHandler();
	if (m_hMsgHandler)
	{
		::PostMessage(m_hMsgHandler, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)state, (LPARAM)reason);
	}
}
void CallQualityEventHandler::onRemoteVideoStats(const RemoteVideoStats& stats)
{
	// Send the call results to the corresponding function in the CAgoraImplementationDlg class.
	HWND m_hMsgHandler = getMsgEventHandler();
	if (m_hMsgHandler)
	{
		::PostMessage(m_hMsgHandler, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)stats.uid, (LPARAM)stats.receivedBitrate);
	}
}
void CallQualityEventHandler::onLastmileProbeResult(const LastmileProbeResult& result)
{
	// Send the call results to the corresponding function in the CAgoraImplementationDlg class.
	// The result object contains the detailed test results that help you
	// manage call quality, for example, the downlink jitter.
	HWND m_hMsgHandler = getMsgEventHandler();
	if (m_hMsgHandler)
	{
		::PostMessage(m_hMsgHandler, WM_MSGID(EID_LAST_MILE_RESULT), (WPARAM)result.downlinkReport.jitter, (LPARAM)NULL);
	}
}
void CallQualityEventHandler::onNetworkQuality(uid_t uid, int txQuality, int rxQuality)
{
	// Send the call results to the corresponding function in the CAgoraImplementationDlg class.
	HWND m_hMsgHandler = getMsgEventHandler();
	if (m_hMsgHandler)
	{
		::PostMessage(m_hMsgHandler, WM_MSGID(EID_NETWORK_QUALITY), (WPARAM)txQuality, (LPARAM)rxQuality);
	}
}


LRESULT CallQuality::OnEIDRemoteVideoStateChanged(WPARAM Uid, LPARAM NewState)
{
	// Display the callback result.
	wchar_t a[256];
	swprintf(a, sizeof(a) / sizeof(wchar_t), L"Remote video state changed: \n Uid %d NewState: %d", Uid, NewState);

	// textBox should be the HWND of your TextBox control
	SetWindowText(textBox, a);
	return true;
}

LRESULT CallQuality::OnEIDRemoteVideoStats(WPARAM uid, LPARAM receivedBitrate)
{
	// Display the callback result.
	wchar_t a[256];
	swprintf(a, sizeof(a) / sizeof(wchar_t), L"Remote Video Stats: \n User id = %d\n Received bitrate: %d", uid, receivedBitrate);

	// textBox should be the HWND of your TextBox control
	SetWindowText(textBox, a);
	return true;
}


LRESULT CallQuality::OnEIDLastMileQuality(WPARAM quality, LPARAM lParam)
{
	// Display the callback result.
	updateNetworkStatus(quality);
	return true;
}

LRESULT CallQuality::OnEIDNetworkQuality(WPARAM txQuality, LPARAM rxQuality)
{
	// Display the callback result.
	updateNetworkStatus((int)txQuality);
	return true;
}

LRESULT CallQuality::OnEIDRtcStats(WPARAM userCount, LPARAM packetLoss)
{
	// Display the callback result.
	wchar_t a[256];
	swprintf(a, sizeof(a) / sizeof(wchar_t), L"User(s): %d \t Packet loss rate: % d ", userCount, packetLoss);

	// textBox should be the HWND of your TextBox control
	SetWindowText(textBox, a);
	return true;
}

LRESULT CallQuality::OnEIDLastMileProbeResult(WPARAM jitter, LPARAM lParam)
{
	// Display the callback result.
	wchar_t a[256];
	swprintf(a, sizeof(a) / sizeof(wchar_t), L"Jitter in the Network = %d", jitter);

	// textBox should be the HWND of your TextBox control
	SetWindowText(textBox, a);
	return true;
}

void CallQuality::updateNetworkStatus(int quality)
{
	if (quality > 0 && quality < 3)
	{
		SetWindowText(networkStatus, L"Network Quality: Excellent");
	}
	else if (quality <= 4)
	{
		SetWindowText(networkStatus, L"Network Quality: Good");
	}
	else
	{
		SetWindowText(networkStatus, L"Network Quality: Poor");
	}
}

void CallQuality::handleGuiAction(int commandId) {
	// Handle new buttons
	switch (commandId) {
	case 4:						// For Echo Test
		// Handle echo test button click
		EchoTest();
		break;
		// ... other buttons can be handled here ...
	case 7:						//For Video Quality
		// Handle video quality button click
		VideoQuality();
	default:
		// Delegate handling to the base class for unknown button commands
		AgoraManager::handleGuiAction(commandId);
		break;
	}
}

void CallQuality::handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam)
{
	switch (msgId) {
	case EID_NETWORK_QUALITY:
		OnEIDNetworkQuality(wparam, lparam);
		break;
	case EID_LAST_MILE_RESULT:
		OnEIDLastMileProbeResult(wparam, lparam);
		break;
	case EID_RTC_STATS:
		OnEIDRtcStats(wparam, lparam);
		break;
	case EID_REMOTE_VIDEO_STATE_CHANGED:
		OnEIDRemoteVideoStateChanged(wparam, lparam);
		break;
	case EID_REMOTE_VIDEO_STATS:
		OnEIDRemoteVideoStats(wparam, lparam);
		break;
	case EID_LAST_MILE_QUALITY:
		OnEIDLastMileQuality(wparam, lparam);
		break;
	default:
		// Delegate handling to the base class for unknown GUI User Message
		AgoraManager::handleGuiUserMsg(msgId, wparam, lparam);
		break;
	}
}


void CallQuality::createSpecificGui(HWND& guiWindowReference)
{
	
	HWND parentWindow = guiWindowReference;
	RECT rect;
	GetClientRect(parentWindow, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// Calculate general button dimensions
	int btnWidth = 100;
	int btnHeight = 30;
	int btnMargin = 10;

	// Echo Test Button
	echoTestButton = CreateWindow(L"BUTTON", L"Echo Test", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		0, 0, btnWidth, btnHeight, parentWindow, (HMENU)4, NULL, NULL);
	MoveWindow(echoTestButton, width - btnWidth - btnMargin, btnMargin, btnWidth, btnHeight, TRUE);

	// Network Status
	networkStatus = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE,
		0, 0, 100, 30, parentWindow, (HMENU)5, NULL, NULL);
	MoveWindow(networkStatus, btnMargin, 2 * (btnHeight + btnMargin), 350, 30, TRUE);

	// Text Box
	textBox = CreateWindow(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
		0, 0, 100, 30, parentWindow, (HMENU)6, NULL, NULL);
	MoveWindow(textBox, btnMargin, 3 * (btnHeight + btnMargin), 500, 230, TRUE);

	// Video Quality Button
	videoQualityButton = CreateWindow(L"BUTTON", L"Video Quality", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		0, 0, btnWidth, btnHeight, parentWindow, (HMENU)7, NULL, NULL);
	MoveWindow(videoQualityButton, width - 2 * (btnWidth + btnMargin), btnMargin, btnWidth, btnHeight, TRUE);
}


void CallQuality::EchoTest()
{
	if (!isEchoTestRunning)
	{
		// Setup a config for the echo test.
		EchoTestConfiguration echoConfig;
		// To test your microphone, set it to true.
		echoConfig.enableAudio = true;
		// To test your video device, set it to true..
		echoConfig.enableVideo = true;
		// Pass your token.
		echoConfig.token = token.c_str();
		// Pass the channel name.
		echoConfig.channelId = channelName.c_str();
		// Pass the window handle to test the local video device.
		echoConfig.view = gui->localView;
		// Start the echo test.
		agoraEngine->startEchoTest(echoConfig);
		// Setup a canvas to render video from your video device.
		VideoCanvas canvas;
		canvas.uid = 0;
		canvas.sourceType = VIDEO_SOURCE_CAMERA;
		canvas.view = gui->localView;
		agoraEngine->setupLocalVideo(canvas);
		// Update the test state.
		isEchoTestRunning = true;
		// Update the button text.
		SetWindowTextW(echoTestButton, L"Stop test");
	}
	else
	{
		// Stop the test.
		agoraEngine->stopEchoTest();
		// Update the test state.
		isEchoTestRunning = false;
		// Update the button text.
		SetWindowTextW(echoTestButton,L"Echo test");
	}

}


void CallQuality::setupVideoSDKEngine()
{
	// Check if the engine initialized successfully.
	agoraEngine = createAgoraRtcEngine();
	if (!agoraEngine)
	{
		return;
	}
	AgoraEventStrategy->SetMsgReceiver(gui->getGuiWindowReference());

	// Create an instance of RtcEngineContext to initialize the engine.
	RtcEngineContext context;
	// Pass your app ID to the context.
	context.appId = appId.c_str();
	// Pass an object of agoraEventHandler class to receive callbacks.
	//HWND agoraEventHandler = AgoraEventStrategy->getMsgEventHandler();
	context.eventHandler = AgoraEventStrategy.get();
	// Set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	context.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
	// Create log file to check the status
	std::string user_name = AgoraManager::config["user_name"].asString();
	std::string file_path = "C:\\Users\\"+ user_name + "\\AppData\\Local\\Agora\\AgoraImplementation\\agorasdk.log";
	context.logConfig.filePath = file_path.c_str();
	context.logConfig.fileSizeInKB = 256;
	context.logConfig.level = agora::commons::LOG_LEVEL::LOG_LEVEL_WARN;
	// Initialize the engine using the context variable.
	agoraEngine->initialize(context);
	// Set the user role to Host.
	agoraEngine->setClientRole(CLIENT_ROLE_TYPE::CLIENT_ROLE_BROADCASTER);
	// Enable the dual stream mode
	agoraEngine->enableDualStreamMode(true);
	// Set audio profile and audio scenario.
	agoraEngine->setAudioProfile(AUDIO_PROFILE_DEFAULT, AUDIO_SCENARIO_GAME_STREAMING);
	// Set the video profile
	VideoEncoderConfiguration videoConfig;
	// Set mirror mode
	videoConfig.mirrorMode = VIDEO_MIRROR_MODE_AUTO;
	// Set framerate
	videoConfig.frameRate = FRAME_RATE_FPS_10;
	// Set bitrate
	videoConfig.bitrate = STANDARD_BITRATE;
	// Set dimensions
	videoConfig.dimensions = VideoDimensions(360, 360);
	// Set orientation mode
	videoConfig.orientationMode = ORIENTATION_MODE_ADAPTIVE;
	// Set degradation preference
	videoConfig.degradationPreference = MAINTAIN_BALANCED;
	// Set compression preference: low latency or quality.
	//videoConfig.compressionPreference = PREFER_LOW_LATENCY;    // Apply the configuration
	agoraEngine->setVideoEncoderConfiguration(videoConfig);
	// Start the probe test
	startProbeTest();
}

void CallQuality::startProbeTest()
{
	// Configure a LastmileProbeConfig instance.
	LastmileProbeConfig config;
	// Probe the uplink network quality.
	config.probeUplink = true;
	// Probe the downlink network quality.
	config.probeDownlink = true;
	// The expected uplink bitrate (bps). The value range is [100000,5000000].
	config.expectedUplinkBitrate = 100000;
	// The expected downlink bitrate (bps). The value range is [100000,5000000].
	config.expectedDownlinkBitrate = 100000;
	int res = agoraEngine->startLastmileProbeTest(config);
}


void CallQuality::VideoQuality()
{
	highQuality = !highQuality;
	if (highQuality)
	{
		// Switch the remote user video quality to high video quality.
		agoraEngine->setRemoteVideoStreamType(remoteUId, VIDEO_STREAM_HIGH);
		MessageBox(NULL, L"Switching to high-quality video", L"Notification", NULL);
	}
	else
	{
		// Switch the remote user video quality to low video quality.
		agoraEngine->setRemoteVideoStreamType(remoteUId, VIDEO_STREAM_LOW);
		MessageBox(NULL, L"Switching to low-quality video", L"Notification", NULL);
	}

}

