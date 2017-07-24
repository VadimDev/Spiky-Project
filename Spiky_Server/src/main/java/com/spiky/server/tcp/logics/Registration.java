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
import org.hibernate.Transaction;
import org.patchca.background.SingleColorBackgroundFactory;
import org.patchca.color.SingleColorFactory;
import org.patchca.filter.predefined.CurvesRippleFilterFactory;
import org.patchca.service.ConfigurableCaptchaService;
import org.patchca.utils.encoder.EncoderHelper;

import java.awt.*;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.*;

import static com.spiky.server.ServerMain.*;
import static com.spiky.server.utils.Descriptors.*;

public class Registration {

    public Registration() {}

    public Registration(ChannelHandlerContext ctx, RegistrationLoginModels.Registration registration) {
        if(registration.hasField(publicKey_reg))
        {
            Cryptography cryptography = ctx.channel().attr(CRYPTOGRAPHY).get();

            ctx.channel().attr(SECRETKEY).set(cryptography.DiffieHellman_createSecretKey(registration.getPublicKey()));
            cryptography.setSecretKey(ctx.channel().attr(SECRETKEY).get());

            System.out.println("secret key: " + ctx.channel().attr(SECRETKEY).get()); // todo

            registration = RegistrationLoginModels.Registration.newBuilder().clear().setStateCode(1).build();
            ctx.writeAndFlush(MessageModels.Wrapper.newBuilder().setRegistration(registration).build());
        }
        else if(!registration.hasField(login_reg) || !registration.hasField(mail_reg) || !registration.hasField(captcha_reg))
        {   // Check for required fields
            registration = RegistrationLoginModels.Registration.newBuilder()
                    .clear()
                    .setStateCode(0) // Do not specify the reason
                    .build();
            ctx.writeAndFlush(MessageModels.Wrapper.newBuilder().setRegistration(registration).build());
        }
        else
        {   // Check again
            ctx.writeAndFlush(checkAll(ctx, registration));
        }
    }

    private MessageModels.Wrapper checkAll(ChannelHandlerContext ctx, RegistrationLoginModels.Registration registration) {

        Session session = ctx.channel().attr(HIBER_SESSION).get();

        Query query = session.createQuery("SELECT login, mail FROM UserModel WHERE login = :login OR mail = :mail ");
        query.setParameter("login", registration.getLogin());
        query.setParameter("mail", registration.getMail());

        java.util.List<String[]> userdata = query.list();

        boolean challengeFind = false;
        synchronized (captchaBank) {
            for (Map.Entry<Long, String> entry : captchaBank.entrySet())
                if (entry.getValue().equals(registration.getCaptcha())) challengeFind = true;
        }

        if(!userdata.isEmpty() || !challengeFind) {
            if(!userdata.isEmpty())
                System.out.println("!userdata.isEmpty()");

            if(!challengeFind)
                System.out.println("!challengeFind");

            registration = RegistrationLoginModels.Registration.newBuilder().clear().setStateCode(0).build();
            return MessageModels.Wrapper.newBuilder().setRegistration(registration).build();
        } else {
            System.out.println("Registration initialization, send public key..");

            Cryptography cryptography = ctx.channel().attr(CRYPTOGRAPHY).get();

            cryptography.DiffieHellman_createKeys();
            registration = RegistrationLoginModels.Registration.newBuilder().clear().setPublicKey(cryptography.getClientPublicKey()).build();
            return MessageModels.Wrapper.newBuilder().setRegistration(registration).build();
        }
    }

    public MessageModels.Wrapper getCaptcha() {
        try {
            ConfigurableCaptchaService cs = new ConfigurableCaptchaService();

            cs.setBackgroundFactory(new SingleColorBackgroundFactory(new Color(0, 0, 0)));
            cs.setColorFactory(new SingleColorFactory(new Color(255, 255, 255)));
            cs.setFilterFactory(new CurvesRippleFilterFactory(cs.getColorFactory()));
            cs.setHeight(100);
            cs.setWidth(250);

            ByteArrayOutputStream bos = new ByteArrayOutputStream();

            captchaBank.put(System.currentTimeMillis(), EncoderHelper.getChallangeAndWriteImage(cs, "png", bos));

            byte[] captchaBytes = bos.toByteArray();
            bos.close();

            RegistrationLoginModels.InputChecking inputChecking = RegistrationLoginModels.InputChecking.newBuilder()
                    .setCaptchaData(ByteString.copyFrom(captchaBytes))
                    .build();

            return MessageModels.Wrapper.newBuilder().setInputChecking(inputChecking).build();

        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public void saveUser(ChannelHandlerContext ctx, RegistrationLoginModels.Registration registration) {
        UserModel user = new UserModel();
        user.setLogin(registration.getLogin());
        user.setHashPass(registration.getHash());
        user.setMail(registration.getMail());

        Session session = ctx.channel().attr(HIBER_SESSION).get();
        Transaction transaction = ctx.channel().attr(HIBER_TRANSACTION).get();

        session.save(user);
        transaction.commit();

        RegistrationLoginModels.InitialState.Builder initialState = RegistrationLoginModels.InitialState.newBuilder()
                .setSessionId(ctx.channel().id().asShortText())
                .setLogin(user.getLogin());

        ctx.channel().attr(CHANNEL_OWNER).set(user.getLogin());

        /* Subscribe to room list updates */
        roomListUpdateSubscribers.add(ctx.channel());

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
    }
}
