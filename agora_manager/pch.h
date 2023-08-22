#pragma once

// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code-completion build speed.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this will negate the performance advantage.

#ifndef PCH_H
#define PCH_H

#define _AFX_ALL_WARNINGS
#include <string>

// For handling config.json for user input
#include <json/json.h>

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
#define EID_TOKEN_PRIVILEGE_WILL_EXPIRE	0x00000023

#endif //PCH_H

