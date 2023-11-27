// call_quality.cpp 

#include "cloud_proxy.h"

bool directConnectionFailed = false;

void CloudProxyEventHandler::onConnectionStateChanged(CONNECTION_STATE_TYPE state, CONNECTION_CHANGED_REASON_TYPE reason)
{
	{
		if (state == CONNECTION_STATE_FAILED
			&& reason == CONNECTION_CHANGED_JOIN_FAILED) {
			directConnectionFailed = true;
			std::wstring message = L"Join failed, reason: " + std::to_wstring(reason);
			MessageBox(NULL, message.c_str(), L"Notification", NULL);
        }
		else if (state == CONNECTION_CHANGED_SETTING_PROXY_SERVER) {
			MessageBox(NULL, L"Proxy server setting changed", L"Notification", NULL);
		}
	}
}

void CloudProxy::join() {
    if (directConnectionFailed) 
    {
        // Attempt to set up UDP proxy
        int proxyStatus = agoraEngine->setCloudProxy(CLOUD_PROXY_TYPE::UDP_PROXY);
        if (proxyStatus == 0) {
            MessageBox(NULL, L"UDP Proxy service setup successful", L"Notification", NULL);
            AgoraManager::join();
            return;
        }

        // Notify about UDP setup failure
        std::wstring udpErrorMessage = L"UDP Proxy service setup failed with error: " + std::to_wstring(proxyStatus) + L". Trying for TCP Proxy.";
        MessageBox(NULL, udpErrorMessage.c_str(), L"Notification", NULL);

        // Attempt to set up TCP proxy
        proxyStatus = agoraEngine->setCloudProxy(CLOUD_PROXY_TYPE::TCP_PROXY);
        if (proxyStatus == 0) {
            MessageBox(NULL, L"TCP Proxy service setup successful", L"Notification", NULL);
            AgoraManager::join();
            return;
        }

        // Notify about TCP setup failure
        std::wstring tcpErrorMessage = L"TCP Proxy service setup failed with error: " + std::to_wstring(proxyStatus);
        MessageBox(NULL, tcpErrorMessage.c_str(), L"Notification", NULL);
    }
    else 
    {
        AgoraManager::join();
    }
}

void CloudProxy::setupVideoSDKEngine()
{
	AgoraManager::setupVideoSDKEngine();
	// Start cloud proxy service and set automatic transmission mode
	int proxyStatus = agoraEngine->setCloudProxy(CLOUD_PROXY_TYPE::NONE_PROXY);
	if (proxyStatus == 0) {
		MessageBox(NULL, L"Proxy service started successfully", L"Notification", NULL);
	}
	else {
		std::wstring message = L"Proxy service failed with error: " + std::to_wstring(proxyStatus);
		MessageBox(NULL, message.c_str(), L"Notification", NULL);
	}
}