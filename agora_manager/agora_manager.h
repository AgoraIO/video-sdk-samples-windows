//agora_manager.h


#pragma once

#ifndef AGORA_MANAGER_H
#define AGORA_MANAGER_H



#include <pch.h>
#include <agora_manager_gui.h>
#include <IGuiHandler.h>
#include <IEventHandlerStrategy.h>



class AgoraManagerEventHandler : public IEventHandlerStrategy
{
public:

    virtual ~AgoraManagerEventHandler() = default;
    //void SetMsgReceiver(HWND hWnd) { m_hMsgHandler = hWnd; }
    //HWND getMsgEventHandler() {return m_hMsgHandler;}
    void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    void onUserJoined(uid_t uid, int elapsed) override;
    void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    void onLeaveChannel(const RtcStats& stats) override;
    //void onError(int err, const char* msg)override;
//private:
    //HWND m_hMsgHandler;
};

/*

class AgoraManagerEventHandler : public IRtcEngineEventHandler
{
public:
    // Set the message notify window handler
    void SetMsgReceiver(HWND hWnd) { m_hMsgHandler = hWnd; }
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    virtual void onLeaveChannel(const RtcStats& stats) override;
private:
    HWND m_hMsgHandler;
};

*/

class AgoraManager : public IGuiHandler
{
public:
    AgoraManager(HINSTANCE hInstance, std::string& _appId,  std::string& _channelName,  std::string& _token)
     : gui(std::make_shared<AgoraManagerGui>(hInstance, this)),
       appId(_appId), channelName(_channelName), token(_token),
        AgoraEventStrategy(std::make_shared<AgoraManagerEventHandler>()) {}
    
 
    void SetupGui();
    void Run();
    virtual void setupVideoSDKEngine();

    LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    void createvideoCanvasAndJoin();
    virtual void join();
    void leave();
    void signalStop();
    bool isStopping();
    void handleGuiAction(int commandId) override;
    void handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam) override;
   // HWND getMsgEventHandler();

    // Load and read "configuration.xml" to get user input
    static tinyxml2::XMLNode*  getConfigXMLRoot(const std::string config_file);

    virtual void ceateSpecificGui(HWND &guiWindowReference);


public:
    std::shared_ptr<AgoraManagerGui> gui;
    IRtcEngine* agoraEngine = nullptr;

    static const std::string config_file;
    //static tinyxml2::XMLDocument doc;
    static std::unique_ptr<tinyxml2::XMLDocument> doc;
    std::string appId = "";
    std::string channelName = "";
    std::string token = "";
    unsigned int  remoteUId;
    //AgoraEventHandler agoraEventHandler;
    //std::unique_ptr<IEventHandlerStrategy> AgoraEventStrategy;
    std::shared_ptr<IEventHandlerStrategy> AgoraEventStrategy;
    bool shouldStop = false;

    virtual ~AgoraManager() noexcept override {}
};

#endif // AGORA_MANAGER_H