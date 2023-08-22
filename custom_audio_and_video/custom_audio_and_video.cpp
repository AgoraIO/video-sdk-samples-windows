// custom_audio_and_video.cpp 
//

#include "custom_audio_and_video.h"

#define ID_EXTERNAL_AUDIO_BTN 8


void CustomAudioVideoSource::setCutommSourceInput()
{
   isCustomVideoSource = AgoraManager::config["isCustomVideoSource"].asBool();
    if (isCustomVideoSource)
    {
        customVideoPath = AgoraManager::config["customVideoPath"].asString();
    }

    isCustomAudioSource = AgoraManager::config["isCustomAudioSource"].asBool();
    if (isCustomAudioSource)
    {
        customAudioPath = AgoraManager::config["customAudioPath"].asString();
    }
}

void CustomAudioVideoSource::createSpecificGui(HWND& guiWindowReference)
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
    externalAudioBtn = CreateWindow(L"BUTTON", L"Play External Audio", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        0, 0, btnWidth, btnHeight, parentWindow, (HMENU)ID_EXTERNAL_AUDIO_BTN, NULL, NULL);
    MoveWindow(externalAudioBtn, width - btnWidth - btnMargin, btnMargin, btnWidth, btnHeight, TRUE);
}


void CustomAudioVideoSource::handleGuiAction(int commandId) {
    // Handle new buttons
    switch (commandId) {
    case ID_EXTERNAL_AUDIO_BTN:				// On pressing External Audio Button
        // Handle play external audio button click
        OnBnClickedExternalAudioButton();
        break;
        
    default:
        // Delegate handling to the base class for unknown button commands
        AgoraManager::handleGuiAction(commandId);
        break;
    }
}



void CustomAudioVideoSource::OnBnClickedExternalAudioButton()
{
    const int bufferSize = 1024;
    wchar_t buttonTextBuffer[bufferSize];

    // Get the window text
    GetWindowTextW(externalAudioBtn, buttonTextBuffer, bufferSize);

    // Convert to wstring
    std::wstring buttonText = buttonTextBuffer;

    if (buttonText == L"Play External Audio" || buttonText == L"Play")
    {
        if (audioFrameThread.joinable())
        {
            audioFrameThread.join();
        }
        captureExternalAudio = true;
        SetWindowTextW(externalAudioBtn, L"Pause");
        audioFrameThread = std::thread([&] { this->playExternalAudio(agoraEngine); });
    }
    else //if (buttonText == L"Pause")
    {
        captureExternalAudio = false;
        SetWindowTextW(externalAudioBtn, L"Play");
    }
}

void CustomAudioVideoSource::Run()
{
	SetupGui();
	// Setup an instance of the Video SDK.
	setupVideoSDKEngine();
	
	setupMediaEngine();

    setCutommSourceInput();

	// Process messages
	gui->ProcessMessages();
}


bool CustomAudioVideoSource::setupMediaEngine()
{
	// Query the interface agora::AGORA_IID_MEDIA_ENGINE in the engine.
	mediaEngine.queryInterface(agoraEngine, agora::rtc::AGORA_IID_MEDIA_ENGINE);

	if (mediaEngine.get() == nullptr) {
		MessageBox(NULL, L"Media Engine error.", L"error", NULL);
		
		return false;
	}
	return true;
}

void CustomAudioVideoSource::createvideoCanvasAndJoin()
{
	// Check if the engine is successfully initialized.
	if (agoraEngine == NULL)
	{
        MessageBox(NULL, L"Agora SDK Engine is not initialized", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
	}
    if (isCustomAudioSource){
        // Eanble external audio source
        mediaEngine->setExternalAudioSource(true, SAMPLE_RATE, SAMPLE_NUM_OF_CHANNEL, 1, false, true);
    }
    // Enable the microphone to create the local audio stream.
	agoraEngine->enableAudio();

    if (isCustomVideoSource) {
        // Enable external video source 
        mediaEngine->setExternalVideoSource(true, false);
    }
    // Enable the local video capturer.
	agoraEngine->enableVideo();

	// Setup a video canvas to render the local video.
	VideoCanvas canvas;
	// Assign the local user ID to canvas for identification.
	canvas.uid = 1;
	// Pass the local view window handle to canvas to render the local video.
	canvas.view = gui->localView;
	// Select a local video source as custom video source.
	canvas.sourceType = VIDEO_SOURCE_CUSTOM;
	// Render the local video.
	agoraEngine->setupLocalVideo(canvas);
	
	//As the preview starts after inducing external video frames from custom source, removed startPreview().
    // Join a channel to start video calling.
	join();
}

void CustomAudioVideoSource::join()
{
    if (isCustomAudioSource)
    {
        option.publishMicrophoneTrack = false;
        option.publishCustomAudioTrack = true;
        option.enableAudioRecordingOrPlayout = true;
        agoraEngine->updateChannelMediaOptions(option);
        // Enable custom audio local playback 
        agoraEngine->enableCustomAudioLocalPlayback(0, true);
    }
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

    if (0 != agoraEngine->joinChannel(token.c_str(), channelName.c_str(), 0, option))
    {
        MessageBox(NULL, L"CustomAudioVideoSource::joinChannel() error.", L"Error!", MB_ICONEXCLAMATION | MB_OK);
    }
    else
    {
        if (isCustomVideoSource)
        {
            captureExternalVideo = true;
            videoFrameThread = std::thread([&] { this->playExternalVideo(agoraEngine); });
            videoFrameThread.detach();
        }
    }
}

void CustomAudioVideoSource::playExternalVideo(IRtcEngine* agoraEngine)
{
    cv::VideoCapture cap(customVideoPath);
    if (!cap.isOpened())
    {
        MessageBox(NULL, L"Error: Could not open the video file.", L"Error", NULL);
        return;
    }
    
    int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));

    // Preview the local video.
    agoraEngine->startPreview();

    while (captureExternalVideo) {
        cv::Mat frame;
        if (!cap.read(frame)) {
            break;
        }
        cv::Mat bgra_frame;
        cv::cvtColor(frame, bgra_frame, cv::COLOR_BGR2BGRA);
        cv::flip(bgra_frame, bgra_frame, 1);


        agora::media::base::ExternalVideoFrame external_frame;
        external_frame.type = agora::media::base::ExternalVideoFrame::VIDEO_BUFFER_TYPE::VIDEO_BUFFER_RAW_DATA;
        external_frame.format = agora::media::base::VIDEO_PIXEL_BGRA;
        external_frame.buffer = bgra_frame.data;
        external_frame.stride = bgra_frame.cols;
        external_frame.height = bgra_frame.rows;
        external_frame.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

        if (agoraEngine && mediaEngine)
        {
            // Push external frame to Agora SDK
            mediaEngine->pushVideoFrame(&external_frame);
        }
        else
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
    }
}

void CustomAudioVideoSource::leave()
{
    AgoraManager::leave();
    if (isCustomVideoSource) {
        disableExternalVideoSource();
    }
    if (isCustomAudioSource) {
        disableExternalAudioSource();
    }

}


void CustomAudioVideoSource::disableExternalVideoSource()
{
    captureExternalVideo = false; // Signal the thread to stop video capture
    if (videoFrameThread.joinable())
    {
        videoFrameThread.join(); // Wait for thread to finish
    }
    mediaEngine->setExternalVideoSource(false, false);
}

void CustomAudioVideoSource::disableExternalAudioSource()
{
    captureExternalAudio = false; // Signal the thread to stop audio capture
    if (audioFrameThread.joinable())
    {
        audioFrameThread.join(); // Wait for thread to finish
    }
    mediaEngine->setExternalAudioSource(false, SAMPLE_RATE, SAMPLE_NUM_OF_CHANNEL, 1, false, false);

    // Set window text to  "Play External Audio"
   SetWindowTextW(externalAudioBtn,L"Play External Audio");
}

void CustomAudioVideoSource::playExternalAudio(IRtcEngine* agoraEngine)
{
    std::ifstream stream(customAudioPath, std::ios::binary);
    if (!stream.is_open())
    {
        MessageBox(NULL, L"Error: Could not open the audio file.", L"Error", NULL);
        return;
    }

    IAudioFrameObserverBase::AUDIO_FRAME_TYPE type = IAudioFrameObserverBase::AUDIO_FRAME_TYPE::FRAME_TYPE_PCM16;
    int samples = SAMPLE_RATE / PUSH_FREQ_PER_SEC;
    std::vector<char> buffer(samples * BYTESPERSAMPLE * SAMPLE_NUM_OF_CHANNEL);
    int freq = 1000 / PUSH_FREQ_PER_SEC;

    auto tic = std::chrono::steady_clock::now();

    // Allocate memory for the audio frame buffer
    std::unique_ptr<char[]> audioFrameBuffer(new char[buffer.size()]);
    IAudioFrameObserverBase::AudioFrame audioFrame;

    audioFrame.bytesPerSample = agora::rtc::BYTES_PER_SAMPLE::TWO_BYTES_PER_SAMPLE;
    audioFrame.type = type;
    audioFrame.samplesPerChannel = samples;
    audioFrame.samplesPerSec = SAMPLE_RATE;
    audioFrame.channels = SAMPLE_NUM_OF_CHANNEL;

    audioFrame.buffer = audioFrameBuffer.get();
    audioFrame.renderTimeMs = freq;

    while (captureExternalAudio)
    {
        auto toc = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count() >= freq)
        {
            stream.read(buffer.data(), buffer.size());
            int n = static_cast<int>(stream.gcount());

            if (n <= 0) // Break the loop if the end of the file is reached or an error occurs
            {
                SetWindowTextW(externalAudioBtn,L"Play External Audio");
                captureExternalAudio = false;
                break;
            }
            std::memcpy(audioFrameBuffer.get(), buffer.data(), buffer.size());
            if (agoraEngine && mediaEngine)
            {
                mediaEngine->pushAudioFrame(MEDIA_SOURCE_TYPE::AUDIO_PLAYOUT_SOURCE, &audioFrame);
            }
            else
            {
                break;
            }
            tic = toc;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}




