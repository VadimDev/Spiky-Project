An example of MMO games based on Netty and Unreal Engine.

Features:

-- Login/Registration
-- Encryption by AES
-- Creation of game rooms
-- Chats
-- Checking hits on the verification server

Netty is used as an asynchronous server, the data stored in MySQL and used to retrieve Hibernate. Client server communication is built on sockets, the format of messaging Protobuf, each message after entering the game is encrypted by AES algorithm using OpenSSL on the client and javax.crypto* on the server. Key exchange takes place using the Diffie-Hellman protocol. Support for Android and Windows.

See a short video with the features:

[![youtube demo](https://github.com/VadimDev/Spiky-Project/blob/master/screenshot.png)](https://youtu.be/pMYijWJV56w)

Copyright (c) 2017, Vadim Petrov - MIT License