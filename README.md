# Agora Video SDK for Windows sample projects

This repository holds the code examples used for the [Agora Video SDK for Windows ](https://docs.agora.io/en/video-calling/get-started/get-started-sdk?platform=windows) documentation. Clone the repo, run and test the samples, and use the code in your own project. Enjoy.

- [Samples](#samples)
- [Prerequisites](#prerequisites)
- [Run this project](#run-this-project)
- [Contact](#contact)

## Samples

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

## Run this project

To run a sample project in this repository, take the following steps:

1. **Clone the repository**

   To clone the repository to your local machine, open Terminal and navigate to the directory where you want to clone the repository. Then, use the following command:

    ```bash
    git clone https://github.com/AgoraIO/video-sdk-samples-windows
    ```

1. **Open the project**
   1. Double-click `video-sdk-samples-windows/agora_manager/agora_manager.sln`. The project opens in Visual Studio.
   1. Unzip [Video SDK for Windows](https://docs.agora.io/en/sdks?platform=windows) to a local directory.  
   1. Replace `video-sdk-samples-windows/agora_manager/sdk` with the contents of 
      `Agora_Native_SDK_for_Windows_FULL\sdk`
   1. In `video-sdk-samples-windows/agora_manager` create a folder using the following naming convention: `<x64|x86>`/`<Debug|Release>`. For example, for a debug profile on a 64 bit development device: `x64/Debug` 
   1. Copy the contents of `video-sdk-samples-windows/agora_manager/SDK/<x86_64|x86>` to `video-sdk-samples-windows/agora_manager/<x64|x86>/<Debug|Release>`.
   
1. **Install the required third party libraries**

   1. In the command prompt, navigate to `video-sdk-samples-windows`. 
   1. Install `vcpkg`: 
      ```bash
      1. git clone https://github.com/Microsoft/vcpkg.git
      1. cd vcpkg
      1. .\bootstrap-vcpkg.bat
      ```  
   1. Install the required packages:
      ```bash
        .\vcpkg.exe install jsoncpp:x64-windows
        .\vcpkg.exe install curl:x64-windows
        .\vcpkg.exe install opencv:x64-windows
      ```

1. **Modify the project configuration**

   The app loads connection parameters from [`./config.json`](./config.json)
   . Ensure that the file is populated with the required parameter values before running the application.

    - `uid`: The user ID associated with the application.
    - `appId`: (Required) The unique ID for the application obtained from [Agora Console](https://console.agora.io). 
    - `channelName`: The default name of the channel to join.
    - `rtcToken`:An token generated for `channelName`. You generate a temporary token using the [Agora token builder](https://agora-token-generator-demo.vercel.app/).
    - `serverUrl`: The URL for the token generator. See [Secure authentication with tokens](authentication-workflow) for information on how to set up a token server.
    - `tokenExpiryTime`: The time in seconds after which a token expires.

    If a valid `serverUrl` is provided, all samples use the token server to obtain a token except the **SDK quickstart** project that uses the `rtcToken`. If a `serverUrl` is not specified, all samples except **Secure authentication with tokens** use the `rtcToken` from `config.json`.

1. **Build and run the project**

   1. In Solution Explorer, right-click the sample you want to run, then click `set as startup project'
   1. Build and run the project. 

## Contact

If you have any questions, issues, or suggestions, please file an issue in our [GitHub Issue Tracker](https://github.com/AgoraIO/video-sdk-samples-windows/issues).
