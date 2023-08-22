//agora_manager.h

#pragma once
#ifndef AGORA_MANAGER_H
#define AGORA_MANAGER_H

#include <pch.h>
#include <agora_manager_gui.h>
#include <IGuiHandler.h>

class AgoraManagerEventHandler : public IRtcEngineEventHandler
{
public:
    // Set the message notify window handler
    void SetMsgReceiver(HWND hWnd) { m_hMsgHandler = hWnd; }
    
    virtual HWND getMsgEventHandler() { return m_hMsgHandler; }
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    virtual void onLeaveChannel(const RtcStats& stats) override;
    virtual void onTokenPrivilegeWillExpire(const char* token) override;
public:
    HWND m_hMsgHandler;
};


class AgoraManager : public IGuiHandler
{
public:
    AgoraManager(HINSTANCE hInstance, std::string& _appId,  std::string& _channelName,  std::string& _token)
     : gui(std::make_shared<AgoraManagerGui>(hInstance, this)),
       appId(_appId), channelName(_channelName), token(_token),
        AgoraEventStrategy(std::make_shared<AgoraManagerEventHandler>()){}
    
    void SetupGui();
    void Run();
    virtual void setupVideoSDKEngine();

    virtual void createvideoCanvasAndJoin();
    virtual void join();
    virtual void leave();
    void signalStop();
    bool isStopping();
    void handleGuiAction(int commandId) override;
    void handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam) override;
    virtual void createSpecificGui(HWND& guiWindowReference);

    LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    LRESULT OnEIDTokenPrivilegeWillExpire(WPARAM wParam, LPARAM lParam);


    // Callback function writes data to a std::string for fetchToken.
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);
    std::string fetchToken(const std::string& serverUrl, const std::string& channelName, int tokenRole, size_t uid, int expireTime);


    // Load and read "config.json" to get user input
    static void buildConfigJsonMap(const std::string config_json_file);

public:
    std::shared_ptr<AgoraManagerGui> gui;
    IRtcEngine* agoraEngine = nullptr;

    static const std::string config_json_file;
    static Json::Value config;
    
    std::string appId = "";
    std::string channelName = "";
    std::string token = "";
    unsigned int  remoteUId;
    std::shared_ptr<AgoraManagerEventHandler> AgoraEventStrategy;
    bool shouldStop = false;
    int tokenRole = 1; // The token role: Broadcaster or Audience
    std::string serverUrl = "";//"<The base URL to your token server>"; //For example, "https://agora-token-service-production-92ff.up.railway.app"
    int expireTime; //Expire time in Seconds.
    uid_t uid = 1; // An integer that identifies the user

    virtual ~AgoraManager() noexcept override {}
};

#endif // AGORA_MANAGER_H
