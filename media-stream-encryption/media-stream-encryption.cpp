// media-stream-encryption.cpp 

#include "media-stream-encryption.h"

void MediaStreamEncryptionManager::enableEncryption()
{
    if (encryptionSaltBase64 == "" || encryptionKey == "")
        return;
    //set encrypt mode and encrypt secret
    EncryptionConfig config;
    config.encryptionMode = enCryptionModeInput;
    config.encryptionKey = encryptionKey.c_str();
    memcpy(config.encryptionKdfSalt, encryptionSaltBase64.c_str(), 32);
    // Call the method to enable media encryption.
    if (agoraEngine->enableEncryption(true, config) == 0)
    {
        MessageBox(NULL, L"Encryption Enabled", L"Notification", NULL);
    }
    else
    {        
        MessageBox(NULL, L"Problem in AgoraEngine Media Encryption", L"Error!", MB_ICONEXCLAMATION | MB_OK);
    }
}

void MediaStreamEncryptionManager::setupVideoSDKEngine()
{
    AgoraManager::setupVideoSDKEngine();

    //Enable the encyrption
    setEncryptionConfigInput();
    enableEncryption();    
}

void MediaStreamEncryptionManager::setEncryptionConfigInput()
{
    // A 32-byte string for encryption.
    encryptionKey = AgoraManager::config["encryptionKey"].asString();
    // A 32-byte string in Base64 format for encryption.
    encryptionSaltBase64 = AgoraManager::config["salt"].asString();

    enCryptionModeInput = getEncriptionMode();
}

ENCRYPTION_MODE MediaStreamEncryptionManager::getEncriptionMode()
{
    int encryptionModeFromInputConfig = AgoraManager::config["encryptionMode"].asInt();

    switch (encryptionModeFromInputConfig)
    {
        case 1: return ENCRYPTION_MODE::AES_128_XTS;
        case 2: return ENCRYPTION_MODE::AES_128_ECB;
        case 3: return ENCRYPTION_MODE::AES_256_XTS;
        case 4: return ENCRYPTION_MODE::SM4_128_ECB;
        case 5: return ENCRYPTION_MODE::AES_128_GCM;
        case 6: return ENCRYPTION_MODE::AES_256_GCM;
        case 7: return ENCRYPTION_MODE::AES_128_GCM2;
        case 8: return ENCRYPTION_MODE::AES_256_GCM2;
        default: return ENCRYPTION_MODE::AES_256_GCM2;
        
    }
}




