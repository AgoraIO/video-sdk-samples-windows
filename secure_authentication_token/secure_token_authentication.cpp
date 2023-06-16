#include "secure_token_authentication.h"
#include <curl/curl.h>

#define EID_TOKEN_PRIVILEGE_WILL_EXPIRE	0x00000023

void SecureTokenAuthenticationEventHandler::onTokenPrivilegeWillExpire(const char* token)
{
    // Occurs on requesting new token
    MessageBox(NULL, L"Token is about to expire.", L"Notification", NULL);
    HWND MsgEventHandler = getMsgEventHandler();
    if (MsgEventHandler)
    {
        ::PostMessage(MsgEventHandler, WM_MSGID(EID_TOKEN_PRIVILEGE_WILL_EXPIRE), (WPARAM)token, NULL);
    }
}

void SecureTokenAuthentication::handleGuiAction(int commandId) {
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

void SecureTokenAuthentication::handleGuiUserMsg(int msgId, WPARAM wparam, LPARAM lparam)
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

LRESULT SecureTokenAuthentication::OnEIDTokenPrivilegeWillExpire(WPARAM wParam, LPARAM lParam)
{
    token = fetchToken(serverUrl, channelName, tokenRole, uid, expireTime);
    HWND hwndParent = GetParent(gui->getGuiWindowReference());
    if (token == "")
    {

        MessageBox(NULL, L"Invalid Token", L"Notification", NULL);
        ::PostMessage(hwndParent, WM_MSGID(EID_TOKEN_PRIVILEGE_WILL_EXPIRE), TRUE, 0);
        return 0;
    }
    // Renew Agora engine token by the new one
    if (0 == agoraEngine->renewToken(token.c_str()))
    {

        MessageBox(NULL, L"Token Renewed!!", L"Notification", NULL);
    }

    ::PostMessage(hwndParent, WM_MSGID(EID_TOKEN_PRIVILEGE_WILL_EXPIRE), TRUE, 0);
    return 0;
}

// Callback function writes data to a std::string
size_t SecureTokenAuthentication::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
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

std::string SecureTokenAuthentication::fetchToken(const std::string& serverUrl, const std::string& channelName, int tokenRole, size_t uid, int expireTime) {
    CURL* curl = nullptr;
    CURLcode res = CURLcode::CURLE_OK;
    std::string readBuffer = "";
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "curl_easy_init() failed, could not initialize libcurl\n");

    }
    else {
        std::string rest_url = serverUrl + "/rtc/" + channelName + "/" + std::to_string(tokenRole) + "/uid/" + std::to_string(uid) + "/?expiry=" + std::to_string(expireTime);

        curl_easy_setopt(curl, CURLOPT_URL, rest_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else {
            std::string tokenKey = "\"rtcToken\":\"";
            size_t startPos = readBuffer.find(tokenKey);
            if (startPos != std::string::npos) {
                startPos += tokenKey.size(); // skip past the key to the value
                size_t endPos = readBuffer.find('\"', startPos); // find the next quote after the value
                if (endPos != std::string::npos) {
                    readBuffer = readBuffer.substr(startPos, endPos - startPos);
                }
            }
        }

        // Always cleanup
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return readBuffer;
}



void SecureTokenAuthentication::Run()
{
    AgoraManager::Run();
}


void SecureTokenAuthentication::join()
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

void SecureTokenAuthentication::leave()
{
    AgoraManager::leave();
    token = "";

}