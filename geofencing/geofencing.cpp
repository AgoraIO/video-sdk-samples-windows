// geofencing.cpp 

#include "geofencing.h"

void GeofencingManager::setupVideoSDKEngine()
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
	// Get Area Code from config for geofncing
	context.areaCode = getAreaCode();
	std::wstring message = L"Geofencing Support:Setting region for connection : " + areaCodeToString(getAreaCode());
	MessageBox(NULL, message.c_str(), L"Notification", NULL);
	// Initialize the engine using the context variable.
	agoraEngine->initialize(context);
	// Set the user role to Host.
	agoraEngine->setClientRole(CLIENT_ROLE_TYPE::CLIENT_ROLE_BROADCASTER);
}

AREA_CODE GeofencingManager::getAreaCode()
{
	area_code = AgoraManager::config["areaCode"].asString();

	if (area_code == "AREA_CODE_CN") return AREA_CODE::AREA_CODE_CN;
	else if (area_code == "AREA_CODE_NA") return AREA_CODE::AREA_CODE_NA;
	else if (area_code == "AREA_CODE_EU") return AREA_CODE::AREA_CODE_EU;
	else if (area_code == "AREA_CODE_AS") return AREA_CODE::AREA_CODE_AS;
	else if (area_code == "AREA_CODE_JP") return AREA_CODE::AREA_CODE_JP;
	else if (area_code == "AREA_CODE_IN") return AREA_CODE::AREA_CODE_IN;
	else return AREA_CODE::AREA_CODE_GLOB;
}

std::wstring GeofencingManager::areaCodeToString(AREA_CODE code) {
	switch (code) 
	{
		case AREA_CODE::AREA_CODE_CN: return L"AREA_CODE_CN";
		case AREA_CODE::AREA_CODE_NA: return L"AREA_CODE_NA";
		case AREA_CODE::AREA_CODE_EU: return L"AREA_CODE_EU";
		case AREA_CODE::AREA_CODE_AS: return L"AREA_CODE_AS";
		case AREA_CODE::AREA_CODE_JP: return L"AREA_CODE_JP";
		case AREA_CODE::AREA_CODE_IN: return L"AREA_CODE_IN";
		case AREA_CODE::AREA_CODE_GLOB: return L"AREA_CODE_GLOB";
		default: return L"Error, while getting Area Code";
	}
}