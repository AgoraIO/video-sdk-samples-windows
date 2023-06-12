//secure_token_authentication.h
#pragma once
#include "agora_manager.h"
#include <curl/curl.h>



#define EID_TOKEN_PRIVILEGE_WILL_EXPIRE	0x00000023


class SecureTokenAuthenticationEventHandler : public IEventHandlerStrategy
{
public:
    virtual ~SecureTokenAuthenticationEventHandler() = default;
    //void onTokenPrivilegeWillExpire(const char* token) override;
    
    void onTokenPrivilegeWillExpire(const char* token)override
    {
        // Occurs on requesting new token
        //AfxMessageBox(L"Token is about to expire");
        HWND this_hMsgHandler = getMsgEventHandler();
            if (this_hMsgHandler)
            {
                ::PostMessage(this_hMsgHandler, WM_MSGID(EID_TOKEN_PRIVILEGE_WILL_EXPIRE), (WPARAM)token, NULL);
            }
    }

};


class SecureTokenAuthentication : public AgoraManager
{
public:
    SecureTokenAuthentication(HINSTANCE hInstance, std::string& appId, std::string& channelName, std::string& token)
        : AgoraManager(hInstance, appId, channelName, token), token(token), channelName(channelName)
    {
        AgoraEventStrategy = std::make_shared<SecureTokenAuthenticationEventHandler>();
    }
    
   

    void handleGuiAction(int commandId) override {
        // Handle new buttons
        switch (commandId) {
        case 4: // Assume that ID 4 corresponds to a new button
            // Handle new button click
            break;
            // ... other buttons can be handled here ...
        default:
            // Delegate handling to the base class for unknown button commands
            AgoraManager::handleGuiAction(commandId);
            break;
        }
    }

   LRESULT OnEIDTokenPrivilegeWillExpire(WPARAM wParam, LPARAM lParam)
    {
        token = fetchToken(serverUrl, channelName, tokenRole, uid, expireTime);
        HWND this_hMsgHandler = AgoraEventStrategy->getMsgEventHandler();

        if (token == "")
        {
            
            MessageBox(NULL, L"Invalid Token", L"Notification", NULL);
            ::PostMessage(this_hMsgHandler, WM_MSGID(EID_TOKEN_PRIVILEGE_WILL_EXPIRE), TRUE, 0);
            return 0;
        }
        // Renew Agora engine token by the new one
        if (0 == agoraEngine->renewToken(token.c_str()))
        {
            
            MessageBox(NULL, L"Token Renewed", L"Notification", NULL);
        }

        ::PostMessage(this_hMsgHandler, WM_MSGID(EID_TOKEN_PRIVILEGE_WILL_EXPIRE), TRUE, 0);
        return 0;
    }



   // Callback function writes data to a std::string
   static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
       size_t newLength = size * nmemb;
       try {
           s->append((char*)contents, newLength);
       }
       catch (std::bad_alloc& e) {
           // handle memory problem
           return 0;
       }
       return newLength;
   }

   std::string fetchToken(const std::string& serverUrl, const std::string& channelName, int tokenRole, size_t uid, int expireTime) {
       CURL* curl;
       CURLcode res;
       std::string readBuffer;

       curl_global_init(CURL_GLOBAL_DEFAULT);
       curl = curl_easy_init();

       if (curl) {
           std::string rest_url = serverUrl + "/rtc/" + channelName + "/" + std::to_string(tokenRole) + "/uid/" + std::to_string(uid) + "/?expiry=" + std::to_string(expireTime);

           curl_easy_setopt(curl, CURLOPT_URL, rest_url.c_str());
           curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
           curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
           res = curl_easy_perform(curl);

           // Check for errors
           if (res != CURLE_OK) {
               fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
           }

           // Always cleanup
           curl_easy_cleanup(curl);
       }
       curl_global_cleanup();

       return readBuffer;
   }




    void handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam)override
    {
        switch (msgId) {
        case EID_TOKEN_PRIVILEGE_WILL_EXPIRE:
            OnEIDTokenPrivilegeWillExpire(wparam, lparam);
            break;
        default:
            // Delegate handling to the base class for unknown GUI User Message
            AgoraManager::handleGuiUserMsg(msgId, wparam, lparam);
            break;
        }
    }

    void Run()
    {
        AgoraManager::Run();
    }


    void join()
    {
        tinyxml2::XMLNode* root = AgoraManager::getConfigXMLRoot(AgoraManager::config_file);

        tinyxml2::XMLElement* expireTimeElement = root->FirstChildElement("expireTime");
        expireTime = expireTimeElement && expireTimeElement->GetText() ? std::atoi(expireTimeElement->GetText()) : 0;

        tinyxml2::XMLElement* serverUrlElement = root->FirstChildElement("serverUrl");
        serverUrl = serverUrlElement && serverUrlElement->GetText() ? serverUrlElement->GetText() : "";

        if (token == "")
        {
            // Fetch new token 
            token = fetchToken(serverUrl, channelName, tokenRole, uid, expireTime);
            if (token == "")
            {
                MessageBox(NULL, L"Invalid Token.", L"Notification", NULL);
                return;
            }
        }
        if (0 != agoraEngine->joinChannel(token.c_str(), channelName.c_str(), NULL, uid))
        {
            MessageBox(NULL, L"SecureTokenAuthentication::joinChannel() error", L"Notification", NULL);
        }

    }



public:
    std::string token = "";
    std::string channelName = "";
    int tokenRole = 1; // The token role: Broadcaster or Audience
    std::string serverUrl = "";//"<The base URL to your token server>"; //For example, "https://agora-token-service-production-92ff.up.railway.app"
    int expireTime = 60; //Expire time in Seconds.
    uid_t uid = 1; // An integer that identifies the user

};
