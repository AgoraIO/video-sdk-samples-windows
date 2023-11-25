// virtual_background.cpp 
//

#include "virtual_background.h"

#define ID_VIRTUAL_BACKGROUND_BTN 14

void VirtualBackground::createSpecificGui(HWND& guiWindowReference)
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

    // Echo Test Button
    virtualBackgroundtButton = CreateWindow(L"BUTTON", L"Set Virual Background", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        0, 0, btnWidth, btnHeight, parentWindow, (HMENU)ID_VIRTUAL_BACKGROUND_BTN, NULL, NULL);
    MoveWindow(virtualBackgroundtButton, width - btnWidth - btnMargin, btnMargin, btnWidth, btnHeight, TRUE);
    EnableWindow(virtualBackgroundtButton, FALSE);
}

void VirtualBackground::handleGuiAction(int commandId) {
    // Handle new buttons
    switch (commandId) {
    case ID_VIRTUAL_BACKGROUND_BTN:				// On pressing External Audio Button
        // Handle play external audio button click
        OnClickedVirualBackgroundButton();
        break;

    default:
        // Delegate handling to the base class for unknown button commands
        AgoraManager::handleGuiAction(commandId);
        break;
    }
}

void VirtualBackground::OnClickedVirualBackgroundButton()
{
    if (!(agoraEngine->isFeatureAvailableOnDevice(FeatureType::VIDEO_VIRTUAL_BACKGROUND)))
    {
        MessageBox(NULL, L"Device doesn't support virtual background", L"Notification", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
    else
    {
        setVirtualBackground();
    }
}

void VirtualBackground::setVirtualBackground()
{
    counter++;
    if (counter > 3) {
        counter = 0;
        isVirtualBackGroundEnabled = false;
        MessageBox(NULL, L"Virtual background turned off", L"Notification", NULL);
    }
    else {
        isVirtualBackGroundEnabled = true;
    }

    VirtualBackgroundSource virtualBackgroundSource;

    // Set the type of virtual background
    if (counter == 1) { // Set background blur
        virtualBackgroundSource.background_source_type = VirtualBackgroundSource::BACKGROUND_BLUR;
        virtualBackgroundSource.blur_degree = VirtualBackgroundSource::BLUR_DEGREE_HIGH;
        MessageBox(NULL, L"Blur background enabled", L"Notification", NULL);
    }
    else if (counter == 2) { // Set a solid background color
        virtualBackgroundSource.background_source_type = VirtualBackgroundSource::BACKGROUND_COLOR;
        virtualBackgroundSource.color = 0x0000FF;
        MessageBox(NULL, L"Color background enabled", L"Notification", NULL);
    }
    else if (counter == 3) { // Set a background image
        virtualBackgroundSource.background_source_type = VirtualBackgroundSource::BACKGROUND_IMG;
        virtualBackgroundSource.source = virtualBackgoundImagePath.c_str();
        MessageBox(NULL, L"Image background enabled", L"Notification", NULL);
    }

    // Set processing properties for background
    SegmentationProperty segmentationProperty;
    segmentationProperty.modelType = SegmentationProperty::SEG_MODEL_AI;; // Use SEG_MODEL_GREEN if you have a green background
    segmentationProperty.greenCapacity = 0.5F; // Accuracy for identifying green colors (range 0-1)

    // Enable or disable virtual background
    agoraEngine->enableVirtualBackground(
        isVirtualBackGroundEnabled,
        virtualBackgroundSource, segmentationProperty);
}

void VirtualBackground::join()
{
   
    expireTime = config["tokenExpiryTime"].asInt() ? config["tokenExpiryTime"].asInt() : 0;
    serverUrl = config["tokenUrl"].asString();

    if (token == "")
    {
        // Fetch new token 
        token = fetchToken(serverUrl, channelName, tokenRole, uid, expireTime);
        if (token == "")
        {
            MessageBox(NULL, L"Invalid Token : token server fetch failed.", L"Error!", MB_ICONEXCLAMATION | MB_OK);
            return;
        }
    }

    if (0 != agoraEngine->joinChannel(token.c_str(), channelName.c_str(), 0, NULL))
    {
        MessageBox(NULL, L"CustomAudioVideoSource::joinChannel() error.", L"Error!", MB_ICONEXCLAMATION | MB_OK);
    }
    else
    {   // Join successfully , so we are enabling virtual background button
        EnableWindow(virtualBackgroundtButton, TRUE);
    }
}

void VirtualBackground::leave()
{
    AgoraManager::leave();

    // Unset virtual background
    counter = 0;
    isVirtualBackGroundEnabled = false;

    // Disable virual background button
    EnableWindow(virtualBackgroundtButton, FALSE);
}

