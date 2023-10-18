// play_media.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "play_media.h"
#include <CommCtrl.h >

#define ID_PLAY_MEDIA_BTN 11
#define ID_PLAY_MEDIA_SLIDER 12

void PlayMedia::updateChannelPublishOptions(bool publishMediaPlayer)
{
	// You use ChannelMediaOptions to change channel media options.
	ChannelMediaOptions channelOptions;
	channelOptions.publishMediaPlayerAudioTrack = publishMediaPlayer;
	channelOptions.publishMediaPlayerVideoTrack = publishMediaPlayer;
	channelOptions.publishMicrophoneTrack = !publishMediaPlayer;
	channelOptions.publishCameraTrack = !publishMediaPlayer;
	if (publishMediaPlayer) channelOptions.publishMediaPlayerId = mediaPlayer->getMediaPlayerId();
	agoraEngine->updateChannelMediaOptions(channelOptions);
}

void PlayMedia::setupLocalVideo(bool forMediaPlayer)
{
	// Pass the window handle to the engine so that it renders the local video.
	if (forMediaPlayer)
	{
		// Stop previewing the local video.
		agoraEngine->stopPreview();
		// To display the media file output, pass the local view window handle to the setView method of the IMediaPlayer class.
		mediaPlayer->setView(gui->localView);
	}
	else
	{
		// Start the local video preview.
		agoraEngine->startPreview();
	}
}

void PlayMedia::createSpecificGui(HWND& guiWindowReference)
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

    // Media Button
	mediaButton = CreateWindow(L"BUTTON", L"Open Media File", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        0, 0, btnWidth, btnHeight, parentWindow, (HMENU)ID_PLAY_MEDIA_BTN, NULL, NULL);
    MoveWindow(mediaButton, width - btnWidth - btnMargin, btnMargin, btnWidth, btnHeight, TRUE);
    EnableWindow(mediaButton, FALSE);

	// Media Slider
	int sliderWidth = width - 2 * btnMargin; // Adjust as needed
	int sliderHeight = 30; // Adjust as needed
	int sliderX = btnMargin; // Adjust as needed
	int sliderY = btnMargin * 2 + btnHeight; // Positioning it below the button
	mediaProgressBar = CreateWindow(TRACKBAR_CLASS, L"Media Progress", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
		sliderX, sliderY, sliderWidth, sliderHeight, parentWindow, (HMENU)ID_PLAY_MEDIA_SLIDER, NULL, NULL);
	MoveWindow(mediaProgressBar, 150, 150, 350, 30, TRUE);
}

void PlayMedia::OnBnClickedPlayMediaButton()
{
	if (mediaPlayer == NULL) 
	{
		// Create an instance of the media player
		mediaPlayer = agoraEngine->createMediaPlayer().get();
		// Set the mediaPlayerObserver to receive callbacks
		mediaPlayerEventHandler.SetMsgReceiver(gui->getGuiWindowReference());
		mediaPlayer->registerPlayerSourceObserver(&mediaPlayerEventHandler);
		mediaPlayer->open(mediaLocation.c_str(), 0);
		// Update the UI.
		EnableWindow(mediaButton, FALSE);
		SetWindowText(mediaButton, L"Opening File...");
		return;
	}
	// Set up the local video container to handle the media player output
	// or the camera stream, alternately.
	isMediaPlaying = !isMediaPlaying;
	// Set the stream publishing options.
	updateChannelPublishOptions(isMediaPlaying);
	// Display the stream locally.
	setupLocalVideo(isMediaPlaying);

	if (isMediaPlaying) 
	{ // Start or resume playing media
		if (mediaPlayer->getState() == PLAYER_STATE_OPEN_COMPLETED) {
			mediaPlayer->play();
		}
		else if (mediaPlayer->getState() == PLAYER_STATE_PAUSED) {
			mediaPlayer->resume();
		}
		SetWindowText(mediaButton, L"Pause");
	}
	else 
	{
		if (mediaPlayer->getState() == PLAYER_STATE_PLAYING) 
		{
			// Pause media file
			mediaPlayer->pause();
			mediaPlayer->setView(NULL);
			SetWindowText(mediaButton, L"Resume");
		}
	}
}

void PlayMedia::handleGuiAction(int commandId) 
{
    // Handle new buttons
    switch (commandId)
	{
		case ID_PLAY_MEDIA_BTN:				// On pressing External Audio Button
			// Handle play external audio button click
			OnBnClickedPlayMediaButton();
			break;
		default:
			// Delegate handling to the base class for unknown button commands
			AgoraManager::handleGuiAction(commandId);
			break;
    }
}

void PlayMedia::handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam)
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

void PlayMedia::releaseMediaResource()
{
	isMediaPlaying = false;
	SetWindowText(mediaButton, L"Open Media File"); // Set the button label to it's initial state
	SendMessage(mediaProgressBar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);

	setupLocalVideo(false);
	updateChannelPublishOptions(false);
	if (mediaPlayer) // Check whether mediaPlayer is NULL before calling Release.
	{
		mediaPlayer->setView(NULL);
		mediaPlayer->unregisterPlayerSourceObserver(&mediaPlayerEventHandler);
		mediaPlayer->Release();
		mediaPlayer = NULL;
	}
}

LRESULT PlayMedia::OnEIDMediaPlayerStateChanged(WPARAM wParam, LPARAM lParam)
{
	if (mediaPlayer == NULL) // Added this check for safety.
	{
		MessageBox(NULL, L"PlayMedia::OnEIDMediaPlayerStateChanged:: No Media Player.", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	if (wParam == MEDIA_PLAYER_STATE::PLAYER_STATE_OPEN_COMPLETED)
	{
		int64_t mediaDuration;
		mediaPlayer->getDuration(mediaDuration);
		// Scale the mediaDuration, so that big size media can be put in the allowed range.
		// Here 32676 is the max range (positive range for int 16 bit) that can be send to 
		// 'TRACKBAR_CLASS' to send message to mediaProgressBar. So scaling the same.
		// It's facilate slider/media progress bar movement as expected.
		scaleFactor = std::ceil(mediaDuration / 32767.0); 
		int scaledDuration = mediaDuration / scaleFactor;
		SetWindowText(mediaButton, L"Play Media File");
		EnableWindow(mediaButton, TRUE);
		
		SendMessage(mediaProgressBar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, scaledDuration));
		SendMessage(mediaProgressBar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);
	}
	else if (wParam == MEDIA_PLAYER_STATE::PLAYER_STATE_PLAYBACK_ALL_LOOPS_COMPLETED)
	{
		releaseMediaResource();
	}
	else
	{
		::PostMessage(gui->getGuiWindowReference(), WM_MSGID(wParam), TRUE, 0);
	}
	return 0;
}

LRESULT PlayMedia::OnEIDMediaPlayerPositionChanged(WPARAM wParam, LPARAM lParam)
{
	int64_t position = static_cast<int64_t>(lParam);  // Retrieve the position from lParam

	if (mediaProgressBar) // Check if mediaProgressBar is not nullptr
	{
		SendMessage(mediaProgressBar, TBM_SETPOS, (WPARAM)TRUE, position/scaleFactor);
	}
	return 0;
}

void PlayMedia::join()
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
		MessageBox(NULL, L"AgoraManager::joinChannel() error.", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return;

	}
	else
	{
		EnableWindow(mediaButton, TRUE);
	}
}

void PlayMedia::leave()
{
	releaseMediaResource();
	AgoraManager::leave();
}



