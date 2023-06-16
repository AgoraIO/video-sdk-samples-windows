// call_quality.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "call_quality.h"

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
    CallQuality* callQualityManager = new CallQuality(hInstance, appId, channelName, token);

    callQualityManager->Run();
}