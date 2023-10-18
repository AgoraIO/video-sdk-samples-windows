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
        0, 0, btnWidth, btnHeight, parentWindow, (HMENU)4, NULL, NULL);
    MoveWindow(zoomButton, width - btnWidth - btnMargin, btnMargin, btnWidth, btnHeight, TRUE);

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
    // Setup an instance of the Video SDK.
    setupVideoSDKEngine();

    setupMediaEngine();

    //setCutommSourceInput();

    // Process messages
    gui->ProcessMessages();
}

void RawAudioVideoManager::OnBnClickedZoomInZoomOutButton()
{
    // Toggle the zoom state
    isZoomed = !isZoomed;

    // Get a handle to the parent control of the button
    // We implemented the Agora Windows Get Started guide, using the button with ID 'IDC_BUTTON3' 
    // for zoom in and zoom out functionality. However, please note that the button ID may differ 
    // in your specific scenario.
    CWnd* pParent = GetDlgItem(IDC_BUTTON3);

    // Cast the control to a CButton object
    CButton* pButtonCtrl = static_cast<CButton*>(pParent);

    // Set the button text based on the zoom state
    if (isZoomed)
    {
        video_frame_observer->onCaptureVideoFrame(VideoFrame & videoFrame);
        pButtonCtrl->SetWindowText(_T("Zoom Out"));
    }
    else
    {
        pButtonCtrl->SetWindowText(_T("Zoom In"));
    }
}




void RawAudioVideoManager::handleGuiAction(int commandId) 
{
    // Handle new buttons
    switch (commandId)
    {
    case ID_ZOOM_IN_BUTTON:				// On pressing Zoom In Button
        // Handle play external audio button click
        OnBnClickedZoomInZoomOutButton();
        break;
    default:
        // Delegate handling to the base class for unknown button commands
        AgoraManager::handleGuiAction(commandId);
        break;
    }
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
        // Register video & audio frame observer
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
bool CustomVideoFrameObserver::onCaptureVideoFrame(VideoFrame& videoFrame)
{
    if (isZoomed) {

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
IVideoFrameObserver::VIDEO_FRAME_PROCESS_MODE CustomVideoFrameObserver::getVideoFrameProcessMode() {
    return VIDEO_FRAME_PROCESS_MODE::PROCESS_MODE_READ_WRITE;
}

// Rest the below functions are dummy "do-nothing" implentaion just to sake for overriding of interface pure virtual functions.
bool CustomVideoFrameObserver::onSecondaryCameraCaptureVideoFrame(VideoFrame& videoFrame)
{
    return false;
}

bool CustomVideoFrameObserver::onSecondaryPreEncodeCameraVideoFrame(VideoFrame& videoFrame)
{
    return false;
}

bool CustomVideoFrameObserver::onScreenCaptureVideoFrame(VideoFrame& videoFrame)
{
    return false;
}

bool CustomVideoFrameObserver::onPreEncodeScreenVideoFrame(VideoFrame& videoFrame)
{
    return false;
}

bool CustomVideoFrameObserver::onSecondaryScreenCaptureVideoFrame(VideoFrame& videoFrame)
{
    return false;
}

bool CustomVideoFrameObserver::CustomVideoFrameObserver::onSecondaryPreEncodeScreenVideoFrame(VideoFrame& videoFrame)
{
    return false;
}

bool CustomVideoFrameObserver::onRenderVideoFrame(const char* channelId, rtc::uid_t remoteUid, VideoFrame& videoFrame)
{
    return false;
}

bool CustomVideoFrameObserver::onPreEncodeVideoFrame(VideoFrame& videoFrame)
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

