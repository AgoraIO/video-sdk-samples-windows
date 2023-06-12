
#include <agora_manager_gui.h>
#include <agora_manager.h>


void AgoraManagerGui::RegisterAndCreateWindow() {
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);

        return;
    }


    hwnd = CreateWindow(CLASS_NAME, L"Sample Window Class", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, this);

    if (hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    // Then associate the AgoraManagerGui instance with the window
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    joinButton = CreateWindow(L"BUTTON", L"Join", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 10, 100, 30, hwnd, (HMENU)1, NULL, NULL);

    leaveButton = CreateWindow(L"BUTTON", L"Leave", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 10, 100, 30, hwnd, (HMENU)2, NULL, NULL);

    cancelButton = CreateWindow(L"BUTTON", L"Cancel", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 10, 100, 30, hwnd, (HMENU)3, NULL, NULL);

    // Create a window for the remote view
    remoteView = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0, 20, 20, hwnd, (HMENU)4, NULL, NULL);

    // Create a window for the local view
    localView = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0, 20, 20, hwnd, (HMENU)5, NULL, NULL);

    // Disable the Leave button by default
    EnableWindow(joinButton, TRUE);
    EnableWindow(leaveButton, FALSE);

    pManager->ceateSpecificGui(hwnd);
}

void AgoraManagerGui::ShowAndUpdateWindow()
{

    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // Move the remoteView window
    MoveWindow(remoteView, 550, 200, 350, 350, TRUE);

    // Move the localView window
    MoveWindow(localView, 150, 200, 350, 350, TRUE);

    // Move the Join button
    MoveWindow(GetDlgItem(hwnd, 1), width - 220, height - 80, 100, 30, TRUE);

    // Move the Leave button
    MoveWindow(GetDlgItem(hwnd, 2), width - 110, height - 80, 100, 30, TRUE);

    // Move Cancel Button
    MoveWindow(GetDlgItem(hwnd, 3), width - 220, height - 40, 100, 30, TRUE);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}

void AgoraManagerGui::ProcessMessages() {
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HWND& AgoraManagerGui::getGuiWindowReference()
{
    return hwnd;
}

void AgoraManagerGui::setHWND(HWND hwnd)
{
    this->hwnd = hwnd;
}


AgoraManager* AgoraManagerGui::getAgoraManager()
{
    return pManager;
}

LRESULT CALLBACK AgoraManagerGui::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    AgoraManagerGui* pGui;
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lparam;
        pGui = (AgoraManagerGui*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pGui);

        pGui->setHWND(hwnd);
    }
    else
    {
        LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
        pGui = reinterpret_cast<AgoraManagerGui*>(ptr);
    }

    if (!pGui)
    {
        // Handle the case where AgoraManagerGui is not associated with the window yet
        // Return the default window procedure for now
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    
    AgoraManager* pAgoraManager = pGui->getAgoraManager();

    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_COMMAND:
        if (pAgoraManager != nullptr) {
            pAgoraManager->handleGuiAction(LOWORD(wparam)); // Pass the button/command ID
        }
        break;

    case WM_DESTROY:
        if (pAgoraManager != nullptr) {
            pAgoraManager->signalStop();
        }
        break;
    default:
        if (msg >= WM_USER && msg <= 0x7FFF) {
            // We are in the range of WM_USER messages.
            pAgoraManager->handleGuiUserMsg(msg - WM_USER- 0x200, wparam, lparam);
        }
        else {
            return DefWindowProc(hwnd, msg, wparam, lparam);
        }
    }
    return 0;
}


