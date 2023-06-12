// agora_manager_gui.h

#pragma once
#ifndef AGORA_MANAGER_GUI_H
#define AGORA_MANAGER_GUI_H


#include <windows.h>

class AgoraManager; // Forward Declaration

class AgoraManagerGui {

public:
    AgoraManagerGui(HINSTANCE hInstance, AgoraManager* pManager)
        : hInstance(hInstance), pManager(pManager), hwnd(NULL), joinButton(NULL), leaveButton(NULL), cancelButton(NULL), localView(NULL), remoteView(NULL) {}
        
    void RegisterAndCreateWindow();
    void ShowAndUpdateWindow();
    void ProcessMessages();
    HWND& getGuiWindowReference();
    void setHWND(HWND hwnd);
    AgoraManager* getAgoraManager();

    static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
    AgoraManager* pManager = nullptr;
    
    HINSTANCE hInstance;
    HWND hwnd;
    HWND joinButton;
    HWND leaveButton;
    HWND cancelButton;

    HWND localView;
    HWND remoteView;
};

#endif
