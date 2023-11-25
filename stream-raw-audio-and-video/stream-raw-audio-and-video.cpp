// stream-raw-audio-and-video.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stream-raw-audio-and-video.h"
#define ID_ZOOM_IN_BUTTON 13

bool isZoomed = false;

void RawAudioVideoManager::createSpecificGui(HWND& guiWindowReference)
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

    // Zoom In Button
    zoomButton = CreateWindow(L"BUTTON", L"Zoom In", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        0, 0, btnWidth, btnHeight, parentWindow, (HMENU)ID_ZOOM_IN_BUTTON, NULL, NULL);
    MoveWindow(zoomButton, width - btnWidth - btnMargin, btnMargin, btnWidth, btnHeight, TRUE);
    EnableWindow(zoomButton, FALSE);

}

bool RawAudioVideoManager::setupMediaEngine()
{
    // Query the interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
    mediaEngine.queryInterface(agoraEngine, agora::rtc::AGORA_IID_MEDIA_ENGINE);

    if (mediaEngine.get() == nullptr) {
        MessageBox(NULL, L"Media Engine error.", L"error", NULL);

        return false;
    }
    return true;
}

void RawAudioVideoManager::Run()
{
    SetupGui();
    setupVideoSDKEngine();
    setupMediaEngine();
    gui->ProcessMessages();
}

void RawAudioVideoManager::OnBnClickedZoomInZoomOutButton()
{
    // Toggle the zoom state
    isZoomed = !isZoomed;

    // Set the button text based on the zoom state
    if (isZoomed)
    {
        SetWindowText(zoomButton, L"Zoom Out");
    }
    else
    {
        SetWindowText(zoomButton, L"Zoom In");
    }
}

void RawAudioVideoManager::handleGuiAction(int commandId) 
{
    // Handle new buttons
    switch (commandId)
    {
    case ID_ZOOM_IN_BUTTON:				
        // On pressing Zoom In/Zoom Out Button
        OnBnClickedZoomInZoomOutButton();
        break;
    default:
        // Delegate handling to the base class for unknown button commands
        AgoraManager::handleGuiAction(commandId);
        break;
    }
}

void RawAudioVideoManager::createvideoCanvasAndJoin()
{
    // Check if the engine is successfully initialized.
    if (agoraEngine == NULL)
    {
        MessageBox(NULL, L"Agora SDK Engine is not initialized", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
    
    // Enable audio & video frame capture by registering the frames.
    EnableAudioVideoCapture(TRUE);

    // Enable the microphone to create the local audio stream.
    agoraEngine->enableAudio();
    // Enable the local video capturer.
    agoraEngine->enableVideo();

    // Setup a video canvas to render the local video.
    VideoCanvas canvas;
    // Assign the local user ID to canvas for identification.
    canvas.uid = 1;
    // Pass the local view window handle to canvas to render the local video.
    canvas.view = gui->localView;
    // Select a local video source.
    canvas.sourceType = VIDEO_SOURCE_CAMERA;
    // Render the local video.
    agoraEngine->setupLocalVideo(canvas);
    //agora::rtc::uid_t uid = this->uid;
    // Preview the local video.
    agoraEngine->startPreview();
    // Join a channel to start video calling.
    join();
}

void RawAudioVideoManager::join()
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
        // Joined successfully. So enabled the required button like "ZoomIN/ZoomOut
        EnableWindow(zoomButton, TRUE);
    }
}

void RawAudioVideoManager::leave()
{
    AgoraManager::leave();

    // Unregister audio & video frame.
    EnableAudioVideoCapture(FALSE);
    // Disable ZoomIn/ZoomOut button
    EnableWindow(zoomButton, FALSE);
}

bool RawAudioVideoManager::EnableAudioVideoCapture(bool bEnable)
{
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    //query interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
    mediaEngine.queryInterface(agoraEngine, agora::rtc::AGORA_IID_MEDIA_ENGINE);
    int nRet = 0;
    agora::base::AParameter apm(agoraEngine);
    if (mediaEngine.get() == NULL)
        return FALSE;
    if (bEnable)
    {
        nRet = mediaEngine->registerVideoFrameObserver(video_frame_observer);
        nRet = mediaEngine->registerAudioFrameObserver(audio_frame_observer);

        // Set the format of the captured raw audio data.
        int SAMPLE_RATE = 16000, SAMPLE_NUM_OF_CHANNEL = 1, SAMPLES_PER_CALL = 1024;

        agoraEngine->setRecordingAudioFrameParameters(SAMPLE_RATE, SAMPLE_NUM_OF_CHANNEL,
            RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, SAMPLES_PER_CALL);

        agoraEngine->setPlaybackAudioFrameParameters(SAMPLE_RATE, SAMPLE_NUM_OF_CHANNEL,
            RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, SAMPLES_PER_CALL);

        agoraEngine->setMixedAudioFrameParameters(SAMPLE_RATE, SAMPLE_NUM_OF_CHANNEL, SAMPLES_PER_CALL);
    }
    else
    {
        nRet = mediaEngine->registerVideoFrameObserver(NULL);
        nRet = mediaEngine->registerAudioFrameObserver(NULL);
    }
    return nRet == 0 ? TRUE : FALSE;
}

// IVideoFrameObserver Implementation
// "onCaptureVideoFrame()" is actually responsible for catching each frame on registering the video frame observer. 
bool CustomVideoFrameObserver::onCaptureVideoFrame(agora::rtc::VIDEO_SOURCE_TYPE sourceType, VideoFrame& videoFrame)
{
    if (isZoomed) 
    {
        int originalWidth = videoFrame.width;
        int originalHeight = videoFrame.height;

        int newWidth = originalWidth / 2;
        int newYStride = newWidth;
        int newHeight = originalHeight / 2;

        // Create temporary buffers for the cropped frame data
        uint8_t* newYBuffer = new uint8_t[newWidth * newHeight];
        uint8_t* newUBuffer = new uint8_t[newWidth * newHeight / 4];
        uint8_t* newVBuffer = new uint8_t[newWidth * newHeight / 4];

        for (int y = 0; y < newHeight; ++y) {
            for (int x = 0; x < newWidth; ++x) {
                // Copy Y plane
                newYBuffer[y * newYStride + x] = videoFrame.yBuffer[y * videoFrame.yStride + x];

                // Copy U and V planes for every 2x2 block of pixels
                if (y % 2 == 0 && x % 2 == 0) {
                    int newIndexUV = (y / 2) * (newWidth / 2) + (x / 2);
                    int originalIndexUV = (y / 2) * (videoFrame.uStride) + (x / 2);

                    newUBuffer[newIndexUV] = videoFrame.uBuffer[originalIndexUV];
                    newVBuffer[newIndexUV] = videoFrame.vBuffer[originalIndexUV];
                }
            }
        }

        // Update videoFrame with the new width, height, and strides
        videoFrame.width = newWidth;
        videoFrame.height = newHeight;
        videoFrame.yStride = newYStride;
        videoFrame.uStride = newWidth / 2;
        videoFrame.vStride = newWidth / 2;

        // Check if the original buffers have already been stored
        if (!originalYBuffer) {
            originalYBuffer = videoFrame.yBuffer;
            originalUBuffer = videoFrame.uBuffer;
            originalVBuffer = videoFrame.vBuffer;
        }

        // Update videoFrame to reference the new buffers
        videoFrame.yBuffer = newYBuffer;
        videoFrame.uBuffer = newUBuffer;
        videoFrame.vBuffer = newVBuffer;
    }
    // Return true to indicate that pre-processing is successful and the frame should not be ignored
    return true;
}

//  getVideoFrameProcessMode() must override PROCESS_MODE_READ_WRITE in order for your raw data changes to take effect.
IVideoFrameObserver::VIDEO_FRAME_PROCESS_MODE CustomVideoFrameObserver::getVideoFrameProcessMode() 
{
    return VIDEO_FRAME_PROCESS_MODE::PROCESS_MODE_READ_WRITE;
}

bool CustomVideoFrameObserver::onRenderVideoFrame(const char* channelId, rtc::uid_t remoteUid, VideoFrame& videoFrame)
{
    return false;
}

bool CustomVideoFrameObserver::onPreEncodeVideoFrame(agora::rtc::VIDEO_SOURCE_TYPE sourceType, VideoFrame& videoFrame)
{
    return false;
}

bool CustomVideoFrameObserver::onMediaPlayerVideoFrame(VideoFrame& videoFrame, int mediaPlayerId)
{
    return false;
}

bool CustomVideoFrameObserver::onTranscodedVideoFrame(VideoFrame& videoFrame)
{
    return false;
}


// IAudioFrameObserver Implementation
// User can provide implementation for "onRecordAudioFrame()", "onPlaybackAudioFrame()" and "onMixedAudioFrame()" as per scenario. 
bool CustomAudioFrameObserver::onRecordAudioFrame(const char* channelId, AudioFrame& audioFrame)
{
    // Gets the captured audio frame.
    // Add code here to process the recorded audio.
    return true;
}

bool CustomAudioFrameObserver::onPlaybackAudioFrame(const char* channelId, AudioFrame& audioFrame)
{
    // Gets the audio frame for playback.
    // Add code here to process the playback audio.
    return true;
}

bool CustomAudioFrameObserver::onMixedAudioFrame(const char* channelId, AudioFrame& audioFrame)
{
    // Retrieves the mixed captured and playback audio frame.
    // Add code here to process the mixed captured playback audio
    return true;
}

// Rest the below functions are dummy "do-nothing" implentaion just to sake for overriding of interface pure virtual functions.
bool CustomAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(const char* channelId, rtc::uid_t uid, AudioFrame& audioFrame)
{
    return false;
}

bool CustomAudioFrameObserver::onEarMonitoringAudioFrame(AudioFrame& audioFrame)
{
    return false;
}

int CustomAudioFrameObserver::getObservedAudioFramePosition()
{
    return 0;
}

IAudioFrameObserver::AudioParams CustomAudioFrameObserver::getPlaybackAudioParams()
{
    return AudioParams();
}

IAudioFrameObserver::AudioParams CustomAudioFrameObserver::getRecordAudioParams()
{
    return AudioParams();
}

IAudioFrameObserver::AudioParams CustomAudioFrameObserver::CustomAudioFrameObserver::getMixedAudioParams()
{
    return AudioParams();
}

IAudioFrameObserver::AudioParams CustomAudioFrameObserver::getEarMonitoringAudioParams()
{
    return AudioParams();
}

