#pragma once
#include "agora_manager.h"



class GetStarted : public AgoraManager
{
public:
    GetStarted(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
        : AgoraManager(hInstance, appId, channelName, token) {}

    void handleGuiAction(int commandId) override 
    {
        // Delegate handling to the base class
            AgoraManager::handleGuiAction(commandId);
    }

    void handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam) override 
    {
        // Delegate handling to the base class
        AgoraManager::handleGuiUserMsg(msgId, wparam, lparam);
    }
    

    void Run()
    {
        AgoraManager::Run();
    }

};