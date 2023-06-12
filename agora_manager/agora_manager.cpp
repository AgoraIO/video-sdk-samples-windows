// agora_manager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <agora_manager.h>

const std::string AgoraManager::config_file = "..//configuration.xml";
std::unique_ptr<tinyxml2::XMLDocument> AgoraManager::doc = std::make_unique<tinyxml2::XMLDocument>();

void AgoraManagerEventHandler::onUserJoined(uid_t uid, int elapsed)
{
	HWND m_hMsgHandler = getMsgEventHandler();
	// Send a notification to CAgoraImplementationDlg class to setup a remote video view.
	if (m_hMsgHandler)
	{
		::PostMessage(m_hMsgHandler, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
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

	//MessageBox(L"Remote user Leave the channel");
}
void AgoraManagerEventHandler::onLeaveChannel(const RtcStats& stats)
{
	// Occurs when you leave a channel.
	MessageBox(NULL, L"You left the channel", L"Notification", NULL);
}
/*
void AgoraManagerEventHandler::onError(int err, const char* msg) {
	MessageBox(NULL, L"Got error ", L"Notification", NULL);
}

HWND AgoraManager::getMsgEventHandler()
{
	
	AgoraManagerEventHandler* agoraManagerEventHandler =
		dynamic_cast<AgoraManagerEventHandler*>(AgoraEventStrategy.get());
	if (agoraManagerEventHandler != nullptr) {
		return agoraManagerEventHandler->getMsgEventHandler();
	}
	else
		return nullptr;
}
*/



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
	//HWND agoraEventHandler = AgoraEventStrategy->getMsgEventHandler();
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
		MessageBox(NULL, L"Engine is not initialized", L"Notification", NULL);
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

	if (0 != agoraEngine->joinChannel(token.c_str(), channelName.c_str(), 0, NULL))
	{
		MessageBox(NULL, L"AgoraManager::joinChannel() error", L"Notification", NULL);
		//Enable join Button again n case of error 
		//EnableWindow(gui->joinButton, TRUE); // Join button
		//EnableWindow(gui->leaveButton, FALSE); // Leave button
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
	token = "";
	//agoraEngine->renewToken(token.c_str());
	
}


void AgoraManager::signalStop() {
	shouldStop = true;
}

bool AgoraManager::isStopping() {
	return shouldStop;
}

void AgoraManager::handleGuiAction(int commandId)  {
	switch (commandId) {
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
	switch (msgId) {
	case EID_USER_JOINED:
		if (this != nullptr) {
			OnEIDUserJoined(wparam, lparam);
		}
		break;
	default:
		// Handle unknown GUI User Message
		
		break;
	}
}

void AgoraManager::SetupGui() {
	gui->RegisterAndCreateWindow();
	gui->ShowAndUpdateWindow();
	// More GUI setup code here, if needed...
}

void setupVideoSDKEngine();

void AgoraManager::Run() {
	SetupGui();
	// Setup an instance of the Video SDK.
	setupVideoSDKEngine();
	// Process messages
	gui->ProcessMessages();
}

tinyxml2::XMLNode* AgoraManager::getConfigXMLRoot(const std::string config_file)
{
	
	if (doc->LoadFile(config_file.c_str()) != tinyxml2::XML_SUCCESS)
	{
		// Handle the error...
		return nullptr;
	}

	tinyxml2::XMLNode* root = doc->FirstChild();
	
	if (!root)
	{
		// Handle the error...
		return nullptr;
	}
	return root;
}


void AgoraManager::ceateSpecificGui(HWND& guiWindowReference)
{
	//do_nothing
}



