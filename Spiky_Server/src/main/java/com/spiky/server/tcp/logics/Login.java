/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp.logics;

import com.google.protobuf.ByteString;
import com.spiky.server.dbmodels.UserModel;
import com.spiky.server.protomodels.GameRoomModels;
import com.spiky.server.protomodels.MessageModels;
import com.spiky.server.protomodels.RegistrationLoginModels;
import com.spiky.server.utils.Cryptography;
import io.netty.channel.ChannelHandlerContext;
import org.hibernate.Query;
import org.hibernate.Session;

import java.util.List;
import java.util.Map;

import static com.spiky.server.ServerMain.*;
import static com.spiky.server.utils.Descriptors.publicKey_log;

public class Login {

    public Login() {}

    public Login(ChannelHandlerContext ctx, RegistrationLoginModels.Login login) {
        if(login.hasField(publicKey_log))
        {
            Cryptography cryptography = ctx.channel().attr(CRYPTOGRAPHY).get();

            cryptography.DiffieHellman_createKeys();
            ctx.channel().attr(SECRETKEY).set(cryptography.DiffieHellman_createSecretKey(login.getPublicKey()));
            cryptography.setSecretKey(ctx.channel().attr(SECRETKEY).get());

            System.out.println("secret key: " + ctx.channel().attr(SECRETKEY).get()); // todo

            login = RegistrationLoginModels.Login.newBuilder().clear().setPublicKey(cryptography.getClientPublicKey()).build();
            ctx.writeAndFlush(MessageModels.Wrapper.newBuilder().setLogin(login).build());
        }
    }

    public void hasUser(ChannelHandlerContext ctx, RegistrationLoginModels.Login login) {
        Session session = ctx.channel().attr(HIBER_SESSION).get();

        Query query = session.createSQLQuery("SELECT * FROM UserModel WHERE mail = :mail AND hash = :hash").addEntity(UserModel.class);
        query.setParameter("mail", login.getMail());
        query.setParameter("hash", login.getHash());

        List<UserModel> user = query.list();

        if(!user.isEmpty()) {

            ctx.channel().attr(CHANNEL_OWNER).set(user.get(0).getLogin());

            /* Subscribe to room list updates */
            roomListUpdateSubscribers.add(ctx.channel());

            RegistrationLoginModels.InitialState.Builder initialState = RegistrationLoginModels.InitialState.newBuilder()
                    .setSessionId(ctx.channel().id().asShortText())
                    .setLogin(user.get(0).getLogin());

            /* Send the current status of the list of rooms */
            for (Map.Entry<String,GameRoom> pair : gameRooms.entrySet()) {
                /* Send only those rooms that are not in the process of playing */
                if(!pair.getValue().getGameState()) {
                    GameRoomModels.CreateRoom room = GameRoomModels.CreateRoom.newBuilder()
                            .setRoomName(pair.getValue().getRoomName())
                            .setMapName(pair.getValue().getMapName())
                            .setCreator(pair.getValue().getCreator())
                            .setGameTime(pair.getValue().getGameTime() + " minutes")
                            .setMaxPlayers(pair.getValue().getMaxPlayers() + " players")
                            .build();
                    initialState.addCreateRoom(room);
                }
            }

            MessageModels.CryptogramWrapper cryptogramWrapper = MessageModels.CryptogramWrapper.newBuilder()
                    .setInitialState(ByteString.copyFrom(initialState.build().toByteArray())).build();

            ctx.writeAndFlush(MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cryptogramWrapper).build());
        } else {
            RegistrationLoginModels.Login log = RegistrationLoginModels.Login.newBuilder().setStateCode(0).build();

            MessageModels.CryptogramWrapper cryptogramWrapper = MessageModels.CryptogramWrapper.newBuilder()
                    .setLogin(ByteString.copyFrom(log.toByteArray())).build();

            ctx.writeAndFlush(MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cryptogramWrapper).build());
        }
    }
}
