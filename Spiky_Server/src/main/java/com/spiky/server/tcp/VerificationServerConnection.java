/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp;

import com.spiky.server.protomodels.GameModels;
import com.spiky.server.tcp.logics.GameState;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

public class VerificationServerConnection {
    private OutputStream socketWriter;

    public VerificationServerConnection(int verificationServerPort) {
        try {
            ServerSocket serverSocket = new ServerSocket(verificationServerPort);
            Socket activeSocket = serverSocket.accept();

            InputStream socketReader = activeSocket.getInputStream();
            socketWriter = new DataOutputStream(activeSocket.getOutputStream());

            // recipient
            new Thread(() -> {
                try {
                    while (true) {
                        byte[] messageByte = new byte[1024];
                        socketReader.read(messageByte);

                        ByteArrayInputStream input = new ByteArrayInputStream(messageByte);
                        GameModels.GameData gameData = GameModels.GameData.parseDelimitedFrom(input);

                        new GameState(gameData);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }).start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void SendToVerificationServer(GameModels.GameData gameData) {
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream(1024);
            gameData.writeDelimitedTo(output);
            byte sendData[] = output.toByteArray();
            socketWriter.write(sendData);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
