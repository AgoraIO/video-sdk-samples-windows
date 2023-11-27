#pragma once
#include "agora_manager.h"
#include "opencv2/opencv.hpp" 


class MediaStreamEncryptionEventHandler : public AgoraManagerEventHandler
{
public:
	virtual ~MediaStreamEncryptionEventHandler() = default;
};

class MediaStreamEncryptionManager : public AgoraManager
{
public:
	MediaStreamEncryptionManager(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
		: AgoraManager(hInstance, appId, channelName, token)
	{
		AgoraEventStrategy = std::make_shared<MediaStreamEncryptionEventHandler>();

	}

	virtual void setupVideoSDKEngine() override;

	void enableEncryption();
	ENCRYPTION_MODE getEncriptionMode();
	void setEncryptionConfigInput();

public:
	// In a production environment, you retrieve the key and salt from
	// an authentication server. For this code example you generate them locally.

	// Encryption Mode
	ENCRYPTION_MODE enCryptionModeInput = ENCRYPTION_MODE::AES_256_GCM2;

	// A 32-byte string for encryption.
	std::string encryptionKey = "";

	// A 32-byte string in Base64 format for encryption.
	std::string encryptionSaltBase64 = "";
};


