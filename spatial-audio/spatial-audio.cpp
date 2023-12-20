// play_media.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "spatial-audio.h"
#include <CommCtrl.h >

#define ID_PLAY_MEDIA_BTN 11
#define ID_DISTANCE_BAR_SLIDER 12


void SpatialAudioManager::configureSpatialAudioEngine()
{
    //distanceBarSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
    //distanceBarSlider->SetRange(0, (int)10, TRUE);
    SendMessage(distanceBarSlider, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 10));

    LocalSpatialAudioConfig localSpatialAudioConfig;
    localSpatialAudioConfig.rtcEngine = agoraEngine;
    agoraEngine->queryInterface(AGORA_IID_LOCAL_SPATIAL_AUDIO, (void**)&localSpatial);
    localSpatial->initialize(localSpatialAudioConfig);
    // Set the audio reception range, in meters, of the local user
    localSpatial->setAudioRecvRange(50);
    // Set the length, in meters, of unit distance
    localSpatial->setDistanceUnit(1);

    // Update self position
    float pos[] = { 1.0F, 1.0F, 1.0F };
    float forward[] = { 1.0F, 0.0F, 0.0F };
    float right[] = { 0.0F, 1.0F, 0.0F };
    float up[] = { 0.0F, 0.0F, 1.0F };
    localSpatial->updateSelfPosition(pos, forward, right, up);
}


void SpatialAudioManager::createSpecificGui(HWND& guiWindowReference)
{
    HWND parentWindow = guiWindowReference;
    RECT rect;
    GetClientRect(parentWindow, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // Calculate general button dimensions
    int btnWidth = 100;
    int btnHeight = 30;
    int btnMargin = 10;

    // distancBarSlider
    int sliderWidth = width - 2 * btnMargin; // Adjust as needed
    int sliderHeight = 30; // Adjust as needed
    int sliderX = btnMargin; // Adjust as needed
    int sliderY = btnMargin * 2 + btnHeight; // Positioning it below the button
    distanceBarSlider = CreateWindow(TRACKBAR_CLASS, L"Media Progress", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
        sliderX, sliderY, sliderWidth, sliderHeight, parentWindow, (HMENU)ID_DISTANCE_BAR_SLIDER, NULL, NULL);
    MoveWindow(distanceBarSlider, 150, 150, 350, 30, TRUE);
}


void SpatialAudioManager::handleGuiAction(int commandId)
{
    // Handle new buttons
    switch (commandId)
    {
    case ID_PLAY_MEDIA_BTN:				// On pressing External Audio Button
        // Handle play external audio button click
        OnBnClickedSpatialAudioManagerButton();
        break;
    default:
        // Delegate handling to the base class for unknown button commands
        AgoraManager::handleGuiAction(commandId);
        break;
    }
}

void SpatialAudioManager::handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam)
{
    switch (msgId) {
    case PLAYER_STATE_CHANGED:
        OnEIDMediaPlayerStateChanged(wparam, lparam);
        break;
    case PLAYER_POSITION_CHANGED:
        OnEIDMediaPlayerPositionChanged(wparam, lparam);
        break;
    default:
        // Delegate handling to the base class for unknown GUI User Message
        AgoraManager::handleGuiUserMsg(msgId, wparam, lparam);
        break;
    }
}


void SpatialAudioManager::setupVideoSDKEngine()
{
    AgoraManager::setupVideoSDKEngine();
    
    // Instantiate and configure the spatial audio engine
    configureSpatialAudioEngine();
}

void SpatialAudioManager::updateRemoteSpatialAudioPosition()
{
    int value = distanceBarSlider->GetPos();
    if (remoteUId == NULL)
    {
        MessageBox(NULL, L"No remote user in the channel", L"Notification", NULL);
        return;
    }
    RemoteVoicePositionInfo positionInfo;
    // Set the coordinates in the world coordinate system.
    // This parameter is an array of length 3
    // The three values represent the front, right, and top coordinates
    // Set the unit vector of the x axis in the coordinate system.
    positionInfo.position[0] = value;
    positionInfo.position[1] = 1.0;
    positionInfo.position[2] = 1.0;
    // This parameter is an array of length 3,
    // The three values represent the front, right, and top coordinates
    positionInfo.forward[0] = value;
    positionInfo.forward[1] = 0.0;
    positionInfo.forward[2] = 0.0;
    // Update the spatial position of the specified remote user
    localSpatial->updateRemotePosition(remoteUId, positionInfo);


}