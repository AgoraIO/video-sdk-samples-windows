#pragma once
#include "agora_manager.h"

class GeofencingEventhandler : public AgoraManagerEventHandler
{
public:
	virtual ~GeofencingEventhandler() = default;
};

class GeofencingManager : public AgoraManager
{
public:
	GeofencingManager(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
		: AgoraManager(hInstance, appId, channelName, token)
	{
		AgoraEventStrategy = std::make_shared<GeofencingEventhandler>();

	}

	virtual void setupVideoSDKEngine()override;
	AREA_CODE getAreaCode();
	std::wstring areaCodeToString(AREA_CODE code);

public:
	std::string area_code = "AREA_CODE_GLOB";
};
