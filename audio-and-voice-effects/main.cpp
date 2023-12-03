//  This file contains the 'main' function. Program execution begins and ends there.
//

#include "audio-and-voice-effects.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    // Get inputs from config.json
    AgoraManager::buildConfigJsonMap(AgoraManager::config_json_file);
    std::string appId = AgoraManager::config["appId"].asString();
    std::string channelName = AgoraManager::config["channel"].asString();
    std::string token = AgoraManager::config["rtcToken"].asString();

    // Create the AgoraManager and associate it with the window
    AudioVoiceEffectsManager* audioVoiceEffectsManager = new AudioVoiceEffectsManager(hInstance, appId, channelName, token);

    audioVoiceEffectsManager->Run();
}