Secure authentication with tokens
Authentication is the act of validating the identity of each user before they access a system. Agora uses digital tokens to authenticate users and their privileges before they access Agora SD-RTN™ to join Video Calling. Each token is valid for a limited period and works only for a specific channel. For example, you cannot use the token generated for a channel called AgoraChannel to join the AppTest channel.

This page shows you how to quickly set up an authentication token server, retrieve a token from the server, and use it to connect securely to a specific Video Calling channel. You use this server for development purposes. To see how to develop your own token generator and integrate it into your production IAM system, read Token generators.

## Understand the code

For context on this sample, and a full explanation of the essential code snippets used in this project, read the following guides:

[Agora's full token authentication guide](https://docs.agora.io/en/video-calling/get-started/authentication-workflow?platform=windows)

## How to run this project

To see how to run this project, refer to [README](../README.md) in the root folder or one of the complete product guides.
