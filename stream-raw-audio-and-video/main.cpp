// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stream-raw-audio-and-video.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    // Get inputs from config.json
    AgoraManager::buildConfigJsonMap(AgoraManager::config_json_file);
    std::string appId = AgoraManager::config["appId"].asString();
    std::string channelName = AgoraManager::config["channel"].asString();
    std::string token = AgoraManager::config["rtcToken"].asString();

    // Create the RawAudioVideoManager and associate it with the window
    RawAudioVideoManager* pManager = new RawAudioVideoManager(hInstance, appId, channelName, token);

    pManager->Run();
}