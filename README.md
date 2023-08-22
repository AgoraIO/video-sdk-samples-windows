# Agora Video SDK for Windows sample projects

This repository holds the code examples used for the [Agora Video SDK for Windows ](https://docs.agora.io/en/video-calling/get-started/get-started-sdk?platform=windows) documentation. Clone the repo, run and test the samples, use the code in your own project. Enjoy.

## Sample projects

The runnable code examples are:

- [SDK quickstart](./get_started/) - The minimum code you need to integrate high-quality, low-latency Video Calling features into your app using Video SDK.
- [Call quality best practice](./call_quality/) - How to use Video SDK features to ensure optimal audio and video quality in your app. 
- [Custom video and audio surces](./custom_audio_and_video/) - How to integrate a custom audio or video source in to your app using Video SDK.

## Prerequisites
1. Git account,
1. A device running Windows 7 or higher.
1. Microsoft Visual Studio 2017 or higher with Desktop development with C++ support.
1. An [Agora account and project](https://console.agora.io/projects).
1. Latest Agora SDK for the particular product, you want to run.  Go to [SDKs](https://docs.agora.io/en/sdks?platform=windows), download the latest version of the Agora Video SDK, and unzip the downloaded SDK to a local folder.  

## Install the required third party liberaries

Follow the link [vcpkg](https://vcpkg.io/en/getting-started) to install required library. Please note we need to install x64-windows version of librarry, as this sample is 64-bit version of Windows. For example in a given sample to read config.json, `jsoncpp` lib is used. in nutshell below steps required:
 ```bash
 1. git clone https://github.com/Microsoft/vcpkg.git  -- execute once during vcpkg setup
 2. .\vcpkg\bootstrap-vcpkg.bat   -- execute once during vcpkg setup
 3. .\vcpkg integrate install     -- execute once during vcpkg setup 
 4. .\vcpkg install jsoncpp:x64-windows   -- execute specific command for each specific liberary
 ```
## Run a sample project

To run a sample project in this repository, take the following steps:

1. Clone this Git repository by executing the following command in a terminal window:
    ```bash
    git clone https://github.com/AgoraIO/video-sdk-samples-windows
    ```
2. Double click on `<root>/agora_manager/agora_manager.sln` . It will open the solution in Visual Studio.
3. Replace the `<root>/agora_manager/SDK` with the latest Agora Video SDK, you downloaded and unzipped to a local folder.
4. Install project specific third party through vcpkg:
	```bash
	--  Go to the path where vcpkg repo is cloned 
	-- for base project and SDK quickstart(agora_manager and get_started):
		.\vcpkg install jsoncpp:x64-windows
		.\vcpkg install curl:x64-windows
	-- for Call quality best practice: None
	-- for Custom video and audio surces:
		.\vcpkg install opencv:x64-windows
	```
4. Select the project you want to run, right click and `set as startup project'
5. Open `<root>/config.json` and put the input as per your selected project like appId, channel, rtcToken, tokenExpiryTime, etc.
6. Build and run the project.
7. Refer to the README file in the selected project folder and follow the link to view complete project documentation for your product of interest.

