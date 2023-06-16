// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "secure_token_authentication.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    tinyxml2::XMLNode* root = AgoraManager::getConfigXMLRoot(AgoraManager::config_file);

    tinyxml2::XMLElement* appIdElement = root->FirstChildElement("appId");
    std::string appId = appIdElement && appIdElement->GetText() ? appIdElement->GetText() : "";

    tinyxml2::XMLElement* channelNameElement = root->FirstChildElement("channelName");
    std::string channelName = channelNameElement && channelNameElement->GetText() ? channelNameElement->GetText() : "";

    tinyxml2::XMLElement* tokenElement = root->FirstChildElement("token");
    std::string token = tokenElement && tokenElement->GetText() ? tokenElement->GetText() : "";

	// Create the AgoraManager and associate it with the window
	SecureTokenAuthentication* pManager = new SecureTokenAuthentication(hInstance, appId, channelName, token);

	pManager->Run();
}


