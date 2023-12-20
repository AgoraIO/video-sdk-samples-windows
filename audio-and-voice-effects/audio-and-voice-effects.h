//audio_and_voice_effects.h
#pragma once
#include "agora_manager.h"

class AudioVoiceEffectsEventHandler : public AgoraManagerEventHandler
{
public:
    virtual ~AudioVoiceEffectsEventHandler() = default;
    virtual void onAudioEffectFinished(int soundId) override;
};

class AudioVoiceEffectsManager : public AgoraManager
{
public:
    AudioVoiceEffectsManager(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
        : AgoraManager(hInstance, appId, channelName, token)
    {
        AgoraEventStrategy = std::make_shared<AudioVoiceEffectsEventHandler>();
    }

    virtual void setupVideoSDKEngine() override;
    virtual void createSpecificGui(HWND& guiWindowReference) override;
    virtual void handleGuiAction(int commandId) override;
    virtual void handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam)override;
    virtual void join() override;
    virtual void leave() override;

    void setSourceInputfromConfiguration();
    
    void OnClickedAudioEffect();
    void OnClickedVoiceEffects();
    void OnClickedAudioMixing();

    LRESULT OnEIDStopAudioEffect(WPARAM soundId, LPARAM lParam);

public:
    int soundEffectId = 1; // Unique identifier for the sound effect file
    std::string soundEffectURL = ""; // like "https://www.soundjay.com/human/applause-01.mp3" : read from configuration
    int soundEffectStatus = 0;
    int voiceEffectIndex = 0;
    bool audioPlaying = false; // Manage the audio mixing state
    std::string audioURL = ""; // like  "https://www.kozco.com/tech/organfinale.mp3" : read from configuration
    HWND playAudioEffectButton; // A HWND to reference the play audio effect button
    HWND voiceEffectButton; // A HWND to reference the voice effect button
    HWND audioMixingButton; // A HWND to reference the audio mixing button
};
