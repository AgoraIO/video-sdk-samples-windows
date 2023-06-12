#pragma once

// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code-completion build speed.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this will negate the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
//#include "framework.h"

#define _AFX_ALL_WARNINGS
#include <string>

// For handling configuration.xml for user input
#include <tinyxml2.h>

#include <IAgoraRtcEngine.h>
#include <IAgoraMediaEngine.h>

#pragma comment(lib, "agora_rtc_sdk.dll.lib")
#pragma comment(lib, "libagora_segmentation_extension.dll.lib")
#pragma comment(lib, "libagora-ffmpeg.dll.lib")

using namespace agora;
using namespace agora::rtc;
using namespace agora::media;

#define WM_MSGID(code) (WM_USER+0x200+code)
#define EID_USER_JOINED					0x00000003


//#define WM_MSGID(code) (WM_USER+code)
//#define EID_USER_JOINED					3

#endif //PCH_H

