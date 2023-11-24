#pragma once
#include "agora_manager.h"

class CloudProxyEventHandler : public AgoraManagerEventHandler
{
public:
	virtual ~CloudProxyEventHandler() = default;
	virtual virtual void onConnectionStateChanged(
		CONNECTION_STATE_TYPE state, CONNECTION_CHANGED_REASON_TYPE reason) override;
};

class CloudProxy : public AgoraManager
{
public:
	CloudProxy(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
		: AgoraManager(hInstance, appId, channelName, token)
	{
		AgoraEventStrategy = std::make_shared<CloudProxyEventHandler>();

	}

	virtual void setupVideoSDKEngine()override;
	virtual void join()override;
};

