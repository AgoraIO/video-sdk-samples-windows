//secure_token_authentication.h
#pragma once
#include "agora_manager.h"

class SecureTokenAuthenticationEventHandler : public AgoraManagerEventHandler
{
public:
    virtual ~SecureTokenAuthenticationEventHandler() = default;
    virtual void onTokenPrivilegeWillExpire(const char* token) override;
};


class SecureTokenAuthentication : public AgoraManager
{
public:
    SecureTokenAuthentication(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
        : AgoraManager(hInstance, appId, channelName, token)
    {
        AgoraEventStrategy = std::make_shared<SecureTokenAuthenticationEventHandler>();
    }
    
    virtual void handleGuiAction(int commandId) override;
    virtual void handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam) override;
    virtual void join()override;
    virtual void leave()override;
    virtual void Run();

    // Callback function writes data to a std::string for fetchToken.
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);
    std::string fetchToken(const std::string& serverUrl, const std::string& channelName, int tokenRole, size_t uid, int expireTime);

    LRESULT OnEIDTokenPrivilegeWillExpire(WPARAM wParam, LPARAM lParam);

public:
    int tokenRole = 1; // The token role: Broadcaster or Audience
    std::string serverUrl = "";//"<The base URL to your token server>"; //For example, "https://agora-token-service-production-92ff.up.railway.app"
    int expireTime;; //Expire time in Seconds.
    uid_t uid = 1; // An integer that identifies the user
};
