/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp.handlers;

import com.spiky.server.protomodels.*;
import com.spiky.server.tcp.logics.*;
import com.spiky.server.utils.Cryptography;
import com.spiky.server.utils.SessionUtil;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.MessageToMessageDecoder;
import io.netty.handler.timeout.IdleState;
import io.netty.handler.timeout.IdleStateEvent;
import org.hibernate.Session;
import org.hibernate.Transaction;

import java.util.List;

import static com.spiky.server.ServerMain.CRYPTOGRAPHY;
import static com.spiky.server.ServerMain.HIBER_SESSION;
import static com.spiky.server.ServerMain.HIBER_TRANSACTION;
import static com.spiky.server.tcp.logics.RoomManager.clearRoomData;
import static com.spiky.server.utils.Descriptors.*;

public class DecryptHandler extends MessageToMessageDecoder<MessageModels.Wrapper> {

    private Session session = new SessionUtil().getSession();
    private Transaction transaction  = session.beginTransaction();
    private Cryptography cryptography = new Cryptography();

    /* Initialize once */
    private boolean bInit = false;

    /* Save the values necessary in all handlers of this channel */
    private void init(ChannelHandlerContext ctx) {
        if(!bInit) {
            bInit = true;

            ctx.channel().attr(HIBER_SESSION).set(session);
            ctx.channel().attr(HIBER_TRANSACTION).set(transaction);
            ctx.channel().attr(CRYPTOGRAPHY).set(cryptography);
        }
    }

    @Override
    protected void decode(ChannelHandlerContext ctx, MessageModels.Wrapper wrapper, List<Object> list) throws Exception {
        init(ctx);
        /* Some messages are not encrypted */
        if(wrapper.hasCryptogramWrapper())
        {
            if(wrapper.getCryptogramWrapper().hasField(registration_cw))
            {
                byte[] cryptogram = wrapper.getCryptogramWrapper().getRegistration().toByteArray();
                byte[] original = cryptography.Decrypt(cryptogram, cryptography.getSecretKey());

                RegistrationLoginModels.Registration registration = RegistrationLoginModels.Registration.parseFrom(original);
                new Registration().saveUser(ctx, registration);
            }
            else if (wrapper.getCryptogramWrapper().hasField(login_cw))
            {
                byte[] cryptogram = wrapper.getCryptogramWrapper().getLogin().toByteArray();
                byte[] original = cryptography.Decrypt(cryptogram, cryptography.getSecretKey());

                RegistrationLoginModels.Login login = RegistrationLoginModels.Login.parseFrom(original);
                new Login().hasUser(ctx, login);
            }
            else if(wrapper.getCryptogramWrapper().hasField(mainMenu_cw))
            {
                byte[] cryptogram = wrapper.getCryptogramWrapper().getMainMenu().toByteArray();
                byte[] original = cryptography.Decrypt(cryptogram, cryptography.getSecretKey());

                MainMenuModels.MainMenu mainMenu = MainMenuModels.MainMenu.parseFrom(original);
                new MainMenu(ctx, mainMenu);
            }
            else if(wrapper.getCryptogramWrapper().hasField(room_cw))
            {
                byte[] cryptogram = wrapper.getCryptogramWrapper().getRoom().toByteArray();
                byte[] original = cryptography.Decrypt(cryptogram, cryptography.getSecretKey());

                GameRoomModels.Room room = GameRoomModels.Room.parseFrom(original);
                new RoomManager(ctx, room);
            }
            else if(wrapper.getCryptogramWrapper().hasField(gameModels_cw))
            {
                byte[] cryptogram = wrapper.getCryptogramWrapper().getGameModels().toByteArray();
                byte[] original = cryptography.Decrypt(cryptogram, cryptography.getSecretKey());

                GameModels.GameData gameData = GameModels.GameData.parseFrom(original);
                new GameState(ctx, gameData);
            }
        }
        else if(wrapper.hasInputChecking())
        {
            new InputChecking(ctx, wrapper);
        }
        else if(wrapper.hasRegistration())
        {
            new Registration(ctx, wrapper.getRegistration());
        }
        else if(wrapper.hasLogin())
        {
            new Login(ctx, wrapper.getLogin());
        }
    }

    private void clearPlayerData(ChannelHandlerContext ctx) {
        if(session.isOpen()) session.close();
        clearRoomData(ctx);
        ctx.close();
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        cause.printStackTrace();
        clearPlayerData(ctx);
    }

    @Override
    public void channelUnregistered(ChannelHandlerContext ctx) throws Exception {
        super.channelUnregistered(ctx);
        System.out.println("Close connection id: " + ctx.channel().id().asShortText() + " cause ChannelUnregistered");
        clearPlayerData(ctx);
    }

    @Override
    public void userEventTriggered(ChannelHandlerContext ctx, Object evt) throws Exception {
        if (evt instanceof IdleStateEvent) {
            IdleStateEvent e = (IdleStateEvent) evt;
            if (e.state() == IdleState.READER_IDLE) {
                System.out.println("Close connection id: " + ctx.channel().id().asShortText() + " cause READER_IDLE");
                clearPlayerData(ctx);
            }
        }
    }
}
