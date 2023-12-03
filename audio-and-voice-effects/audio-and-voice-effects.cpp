// audio-and-voice-effects.cpp 

#include "audio-and-voice-effects.h"

#define ID_AUDIO_EFFECT_BUTTON 20
#define ID_VOICE_EEFECT_BUTTON 21
#define ID_AUDIO_MIXING_BUTTON 22

#define EID_STOP_EFFECT 25

void AudioVoiceEffectsManager::handleGuiAction(int commandId)
{
	switch (commandId) {
	case ID_AUDIO_EFFECT_BUTTON:						
		// Handle audio effect button click
		OnClickedAudioEffect();
		break;
		// Handle voice effect button click
	case ID_VOICE_EEFECT_BUTTON:						
		// Handle video quality button click
		OnClickedVoiceEffects();
		break;
	case ID_AUDIO_MIXING_BUTTON:						
		// Handle audio mixing button click
		OnClickedAudioMixing();
		break;
	default:
		// Delegate handling to the base class for unknown button commands
		AgoraManager::handleGuiAction(commandId);
		break;
	}
}

void AudioVoiceEffectsManager::OnClickedAudioEffect()
{
	if (soundEffectStatus == 0)
	{
		// Stopped
		agoraEngine->playEffect(
			soundEffectId,   // The ID of the sound effect file.
			soundEffectFilePath.c_str(),   // The path of the sound effect file.
			0,  // The number of sound effect loops. -1 means an infinite loop. 0 means once.
			1,   // The pitch of the audio effect. 1 represents the original pitch.
			0.0, // The spatial position of the audio effect. 0.0 represents that the audio effect plays in the front.
			100, // The volume of the audio effect. 100 represents the original volume.
			true,// Whether to publish the audio effect to remote users.
			0    // The playback starting position of the audio effect file in ms.
		);
		SetWindowText(playAudioEffectButton, L"Pause effect");
		soundEffectStatus = 1;
	}
	else if (soundEffectStatus == 1)
	{ // Playing
		agoraEngine->pauseEffect(soundEffectId);
		soundEffectStatus = 2;
		SetWindowText(playAudioEffectButton, L"Resume effect");		
	}
	else if (soundEffectStatus == 2)
	{ // Paused
		agoraEngine->resumeEffect(soundEffectId);
		soundEffectStatus = 1;
		SetWindowText(playAudioEffectButton, L"Pause effect");
	}
}

void AudioVoiceEffectsManager::OnClickedVoiceEffects()
{
	voiceEffectIndex++;
	// Turn off all previous effects
	agoraEngine->setVoiceBeautifierPreset(VOICE_BEAUTIFIER_OFF);
	agoraEngine->setAudioEffectPreset(AUDIO_EFFECT_OFF);
	agoraEngine->setVoiceConversionPreset(VOICE_CONVERSION_OFF);
	agoraEngine->setLocalVoiceFormant(0.0);

	if (voiceEffectIndex == 1) {
		agoraEngine->setVoiceBeautifierPreset(CHAT_BEAUTIFIER_MAGNETIC);
		SetWindowText(voiceEffectButton, L"Effect: Chat Beautifier");		
	}
	else if (voiceEffectIndex == 2) {
		agoraEngine->setVoiceBeautifierPreset(SINGING_BEAUTIFIER);
		SetWindowText(voiceEffectButton, L"Effect: Singing Beautifier");
	}
	else if (voiceEffectIndex == 3) {
		// Modify the timbre using the formantRatio
		// Range is [-1.0, 1.0], [giant, child] default value is 0.
		agoraEngine->setLocalVoiceFormant(0.6);
		SetWindowText(voiceEffectButton, L"Voice effect: Adjust Formant");
	}
	else if (voiceEffectIndex == 4) {
		agoraEngine->setAudioEffectPreset(VOICE_CHANGER_EFFECT_HULK);
		SetWindowText(voiceEffectButton, L"Effect: Hulk");
	}
	else if (voiceEffectIndex == 5) {
		agoraEngine->setVoiceConversionPreset(VOICE_CHANGER_BASS);
		SetWindowText(voiceEffectButton, L"Effect: Voice Changer");
	}
	else if (voiceEffectIndex == 6) {
		// Sets the local voice equalization.
		// The first parameter sets the band frequency. The value ranges between 0 and 9.
		// Each value represents the center frequency of the band: 
		// 31, 62, 125, 250, 500, 1k, 2k, 4k, 8k, and 16k Hz.
		// The second parameter sets the gain of each band between -15 and 15 dB.
		// The default value is 0.
		agoraEngine->setLocalVoiceEqualization(AUDIO_EQUALIZATION_BAND_FREQUENCY::AUDIO_EQUALIZATION_BAND_4K, 3);
		agoraEngine->setLocalVoicePitch(0.5);
		SetWindowText(voiceEffectButton, L"Effect: Voice Equalization");
	}
	else if (voiceEffectIndex > 6) { // Remove all effects
		voiceEffectIndex = 0;
		agoraEngine->setLocalVoicePitch(1.0);
		agoraEngine->setLocalVoiceEqualization(AUDIO_EQUALIZATION_BAND_FREQUENCY::AUDIO_EQUALIZATION_BAND_4K, 0);
		SetWindowText(voiceEffectButton, L"Apply voice effect");
	}
}

void AudioVoiceEffectsManager::OnClickedAudioMixing()
{
	audioPlaying = !audioPlaying;
	if (audioPlaying)
	{
		SetWindowText(audioMixingButton, L"Stop Mixing");
		agoraEngine->startAudioMixing(audioFilePath.c_str(), false, 1, 0);
	}
	else
	{
		agoraEngine->stopAudioMixing();
		SetWindowText(audioMixingButton, L"Mix Audio");
	}
}

void AudioVoiceEffectsManager::handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam)
{
	switch (msgId) {
	case EID_STOP_EFFECT:
		OnEIDStopAudioEffect(wparam, lparam);
		break;
	default:
		// Delegate handling to the base class for unknown GUI User Message
		AgoraManager::handleGuiUserMsg(msgId, wparam, lparam);
		break;
	}
}

void AudioVoiceEffectsManager::setupVideoSDKEngine()
{
	AgoraManager::setupVideoSDKEngine();

	// Load sound effects in advance to improve performance
	agoraEngine->preloadEffect(soundEffectId, soundEffectFilePath.c_str());

	// Set an Audio Profile
	// Specify the audio scenario and audio profile
	agoraEngine->setAudioProfile(AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO);
	agoraEngine->setAudioScenario(AUDIO_SCENARIO_GAME_STREAMING);
}


void AudioVoiceEffectsManager::createSpecificGui(HWND& guiWindowReference)
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

	// Play Audio Effect Button
	playAudioEffectButton = CreateWindow(L"BUTTON", L"Audio Effect", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		0, 0, btnWidth, btnHeight, parentWindow, (HMENU)ID_AUDIO_EFFECT_BUTTON, NULL, NULL);
	MoveWindow(playAudioEffectButton, width - 3 * (btnWidth + btnMargin), btnMargin, btnWidth, btnHeight, TRUE);
	
	// Voice Effect Button
	voiceEffectButton = CreateWindow(L"BUTTON", L"Voice Effect", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		0, 0, btnWidth, btnHeight, parentWindow, (HMENU)ID_VOICE_EEFECT_BUTTON, NULL, NULL);
	MoveWindow(voiceEffectButton, width - 2 * (btnWidth + btnMargin), btnMargin, btnWidth, btnHeight, TRUE);

	// Audio Mixing Button
	audioMixingButton = CreateWindow(L"BUTTON", L"Mix Audio", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		0, 0, btnWidth, btnHeight, parentWindow, (HMENU)ID_AUDIO_MIXING_BUTTON, NULL, NULL);
	MoveWindow(audioMixingButton, width - btnWidth - btnMargin, btnMargin, btnWidth, btnHeight, TRUE);

	EnableWindow(playAudioEffectButton, FALSE);
	EnableWindow(voiceEffectButton, FALSE);
	EnableWindow(audioMixingButton, FALSE);
}

LRESULT AudioVoiceEffectsManager::OnEIDStopAudioEffect(WPARAM soundId, LPARAM lParam)
{
	MessageBox(NULL, L"Audio effect finished", L"Notification", NULL);
	agoraEngine->stopEffect(soundId);
	soundEffectStatus = 0; // Stopped
	SetWindowText(playAudioEffectButton, L"Audio Effect");
	return 0;
}

void AudioVoiceEffectsEventHandler::onAudioEffectFinished(int soundId)
{
	// Send the call results to the corresponding function in the CAgoraImplementationDlg class.
	HWND m_hMsgHandler = getMsgEventHandler();
	if (m_hMsgHandler)
	{
		::PostMessage(m_hMsgHandler, WM_MSGID(EID_STOP_EFFECT), (WPARAM)soundId, (LPARAM)NULL);
	}
}

void AudioVoiceEffectsManager::join()
{
	AgoraManager::join();

	if (agoraEngine->getConnectionState() == CONNECTION_STATE_TYPE::CONNECTION_STATE_CONNECTED || CONNECTION_STATE_TYPE::CONNECTION_STATE_CONNECTING)
	{
		EnableWindow(playAudioEffectButton, TRUE);
		EnableWindow(voiceEffectButton, TRUE);
		EnableWindow(audioMixingButton, TRUE);
	}
}

void AudioVoiceEffectsManager::leave()
{
	AgoraManager::leave();

	// Set button text to it's initial state and disable the buttons
	SetWindowText(playAudioEffectButton, L"Audio Effect");
	SetWindowText(voiceEffectButton, L"Voice Effect");
	SetWindowText(audioMixingButton, L"Mix Audio");
	EnableWindow(playAudioEffectButton, FALSE);
	EnableWindow(voiceEffectButton, FALSE);
	EnableWindow(audioMixingButton, FALSE);
}



