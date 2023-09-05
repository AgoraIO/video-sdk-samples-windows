// agora_manager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <fstream>
#include <agora_manager.h>
#include <curl/curl.h>


const std::string AgoraManager::config_json_file = "..//config.json";
Json::Value AgoraManager::config;

void AgoraManagerEventHandler::onUserJoined(uid_t uid, int elapsed)
{
	HWND MsgEventHandler = getMsgEventHandler();
	// Send a notification to CAgoraImplementationDlg class to setup a remote video view.
	if (MsgEventHandler)
	{
		::PostMessage(MsgEventHandler, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
	}
	
}

void AgoraManagerEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
	// Occurs when you join a channel.
}
void AgoraManagerEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	// Occurs when the remote user drops offline or leave the channel.
	MessageBox(NULL, L"Remote user Leave the channel", L"Notification", NULL);

}
void AgoraManagerEventHandler::onLeaveChannel(const RtcStats& stats)
{
	// Occurs when you leave a channel.
	MessageBox(NULL, L"You left the channel", L"Notification", NULL);
}

void AgoraManagerEventHandler::onTokenPrivilegeWillExpire(const char* token)
{
	// Occurs on requesting new token
	MessageBox(NULL, L"Token is about to expire.", L"Notification", NULL);
	HWND MsgEventHandler = getMsgEventHandler();
	if (MsgEventHandler)
	{
		::PostMessage(MsgEventHandler, WM_MSGID(EID_TOKEN_PRIVILEGE_WILL_EXPIRE), (WPARAM)token, NULL);
	}
}

void AgoraManager::setupVideoSDKEngine()
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
	context.eventHandler = AgoraEventStrategy.get();
	// Set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	context.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
	// Initialize the engine using the context variable.
	agoraEngine->initialize(context);
	// Set the user role to Host.
	agoraEngine->setClientRole(CLIENT_ROLE_TYPE::CLIENT_ROLE_BROADCASTER);
}

LRESULT AgoraManager::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	// Setup a video canvas to render the remote video.
	VideoCanvas canvas;
	// Choose a video render mode.
	canvas.renderMode = media::base::RENDER_MODE_FIT;
	// Assign the remote user ID to the canvas for identification.
	canvas.uid = wParam;
	// Pass the remote view window handle to canvas to render the remote video.
	canvas.view = gui->remoteView;
	// Render the remote video.
	agoraEngine->setupRemoteVideo(canvas);
	// Save the remote user ID for reuse.
	remoteUId = wParam;
	// Notify the parent window
	HWND hwndParent = GetParent(gui->getGuiWindowReference());

	if (hwndParent != NULL)
	{
		PostMessage(hwndParent, WM_MSGID(EID_USER_JOINED), TRUE, 0);
	}
	
	return 0;
}

void AgoraManager::createvideoCanvasAndJoin()
{
	// Check if the engine is successfully initialized.
	if (agoraEngine == NULL)
	{
		MessageBox(NULL, L"Agora SDK Engine is not initialized", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	// Enable the microphone to create the local audio stream.
	agoraEngine->enableAudio();
	// Enable the local video capturer.
	agoraEngine->enableVideo();
	// Setup a video canvas to render the local video.
	VideoCanvas canvas;
	// Assign the local user ID to canvas for identification.
	canvas.uid = 1;
	// Pass the local view window handle to canvas to render the local video.
	canvas.view = gui->localView;
	// Select a local video source.
	canvas.sourceType = VIDEO_SOURCE_CAMERA;
	// Render the local video.
	agoraEngine->setupLocalVideo(canvas);
	//agora::rtc::uid_t uid = this->uid;
	// Preview the local video.
	agoraEngine->startPreview();
	// Join a channel to start video calling.
	join();
}


void AgoraManager::join()
{
	expireTime = config["tokenExpiryTime"].asInt() ? config["tokenExpiryTime"].asInt() : 0;
	serverUrl = config["tokenUrl"].asString();

	if (token == "")
	{
		// Fetch new token 
		token = fetchToken(serverUrl, channelName, tokenRole, uid, expireTime);
		if (token == "")
		{
			MessageBox(NULL, L"Invalid Token : token server fetch failed.", L"Error!", MB_ICONEXCLAMATION | MB_OK);
			return;
		}
	}

	if (0 != agoraEngine->joinChannel(token.c_str(), channelName.c_str(), 0, NULL))
	{
		MessageBox(NULL, L"AgoraManager::joinChannel() error.", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return;

	}
}

void AgoraManager::leave()
{
	// Leave the channel to end the call.
	agoraEngine->leaveChannel();
	// Stop the local video preview.
	agoraEngine->stopPreview();
	// Disable the local video capturer.
	agoraEngine->disableVideo();
	// Disable the local microphone.
	agoraEngine->disableAudio();

	//token = "";
}

void AgoraManager::signalStop() {
	shouldStop = true;
}

bool AgoraManager::isStopping() {
	return shouldStop;
}

void AgoraManager::handleGuiAction(int commandId)
{
	switch (commandId)
	{
	case 1: // Join Button ID is 1
		// Handle join button click
		EnableWindow(gui->joinButton, FALSE); // Join button
		EnableWindow(gui->leaveButton, TRUE); // Leave button
		createvideoCanvasAndJoin();
		//join();
		break;
	case 2: // Leave Button ID is 2
		// Handle leave button click
		EnableWindow(gui->joinButton, TRUE); // Join button
		EnableWindow(gui->leaveButton, FALSE); // Leave button
		leave();
		break;
	case 3: // Cancel Button ID is 3
		MessageBox(gui->getGuiWindowReference(), L"Cancel button clicked!", L"Info", MB_OK | MB_ICONINFORMATION);
		DestroyWindow(gui->getGuiWindowReference()); // this will trigger WM_DESTROY
		PostQuitMessage(0);
		break;
		// ... other buttons can be handled here ...
	default:
		// Handle unknown button command
		break;
	}
}


void AgoraManager::handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam) 
{

	//AgoraManager* _pAgoraManager = gui->getAgoraManager();
	// Handle WM_USER messages based on the passed msgId
	switch (msgId)
	{
	case EID_USER_JOINED:
		if (this != nullptr) {
			OnEIDUserJoined(wparam, lparam);
		}
		break;
	case EID_TOKEN_PRIVILEGE_WILL_EXPIRE:
		OnEIDTokenPrivilegeWillExpire(wparam, lparam);
		break;
	default:
		// Handle unknown GUI User Message
		break;
	}
}

void AgoraManager::SetupGui() 
{
	gui->RegisterAndCreateWindow();
	gui->ShowAndUpdateWindow();
}

void AgoraManager::Run()
{
	SetupGui();
	// Setup an instance of the Video SDK.
	setupVideoSDKEngine();
	// Process messages
	gui->ProcessMessages();
}

void AgoraManager::buildConfigJsonMap(const std::string config_json_file)
{
	std::ifstream configFile(config_json_file, std::ifstream::binary);
	if (!configFile.is_open()) {
		MessageBox(NULL, L"Failed to open config.json.", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return ;
	}

	Json::CharReaderBuilder readerBuilder;
	std::string errors;

	if (!Json::parseFromStream(readerBuilder, configFile, &config, &errors)) {
		MessageBox(NULL, L"Failed to parse config.json.", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		OutputDebugStringA(("JSON Parsing Error: " + errors + "\n").c_str());
		return ;
	}
}

void AgoraManager::createSpecificGui(HWND& guiWindowReference)
{
	//do_nothing
}


LRESULT AgoraManager::OnEIDTokenPrivilegeWillExpire(WPARAM wParam, LPARAM lParam)
{
	token = fetchToken(serverUrl, channelName, tokenRole, uid, expireTime);
	HWND hwndParent = GetParent(gui->getGuiWindowReference());
	if (token == "")
	{
		MessageBox(NULL, L"Invalid Token : token server fetch failed.", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		::PostMessage(hwndParent, WM_MSGID(EID_TOKEN_PRIVILEGE_WILL_EXPIRE), TRUE, 0);
		return 0;
	}
	// Renew Agora engine token by the new one
	if (0 == agoraEngine->renewToken(token.c_str()))
	{

		MessageBox(NULL, L"Token Renewed!!", L"Notification", NULL);
	}

	::PostMessage(hwndParent, WM_MSGID(EID_TOKEN_PRIVILEGE_WILL_EXPIRE), TRUE, 0);
	return 0;
}

// Callback function writes data to a std::string
size_t AgoraManager::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
	size_t newLength = size * nmemb;
	try {
		s->append((char*)contents, newLength);
	}
	catch (std::bad_alloc& e) {
		// handle memory problem
		return 0;
	}
	return newLength;
}

std::string AgoraManager::fetchToken(const std::string& serverUrl, const std::string& channelName, int tokenRole, size_t uid, int expireTime) {
	CURL* curl = nullptr;
	CURLcode res = CURLcode::CURLE_OK;
	std::string readBuffer = "";
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (!curl) {
		fprintf(stderr, "curl_easy_init() failed, could not initialize libcurl\n");

	}
	else {
		std::string rest_url = serverUrl + "/rtc/" + channelName + "/" + std::to_string(tokenRole) + "/uid/" + std::to_string(uid) + "/?expiry=" + std::to_string(expireTime);

		curl_easy_setopt(curl, CURLOPT_URL, rest_url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else {
			std::string tokenKey = "\"rtcToken\":\"";
			size_t startPos = readBuffer.find(tokenKey);
			if (startPos != std::string::npos) {
				startPos += tokenKey.size(); // skip past the key to the value
				size_t endPos = readBuffer.find('\"', startPos); // find the next quote after the value
				if (endPos != std::string::npos) {
					readBuffer = readBuffer.substr(startPos, endPos - startPos);
				}
			}
			else
			{
				readBuffer = "";  // making buffer empty to return as empty token
			}
		}

		// Always cleanup
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	return readBuffer;
}



