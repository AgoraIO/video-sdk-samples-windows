#pragma once
#include "agora_manager.h"


class VirtualBackgroundEventHandler : public AgoraManagerEventHandler
{
public:

	virtual ~VirtualBackgroundEventHandler() = default;
	
};

class VirtualBackground : public AgoraManager
{
public:
	VirtualBackground(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
		: AgoraManager(hInstance, appId, channelName, token)
	{
		AgoraEventStrategy = std::make_shared<VirtualBackgroundEventHandler>();

	}

	virtual void createSpecificGui(HWND& guiWindowReference)override;
	virtual void handleGuiAction(int commandId) override;
	virtual void join()override;
	virtual void leave()override;

	void OnClickedVirualBackgroundButton();
	void setVirtualBackground();



public:
	int counter = 0; // to cycle through the different types of backgrounds
	bool isVirtualBackGroundEnabled = false;

	// Get Virual background Image from configuation (config.json
	std::string virtualBackgoundImagePath = AgoraManager::config["virtualBackgoundImagePath"].asString();

	HWND virtualBackgroundtButton; // A HWND to reference the button

};