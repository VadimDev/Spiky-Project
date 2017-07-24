/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp.handlers;

import com.google.protobuf.ByteString;
import com.spiky.server.protomodels.MessageModels;
import com.spiky.server.utils.Cryptography;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.MessageToMessageEncoder;

import java.util.List;

import static com.spiky.server.ServerMain.CRYPTOGRAPHY;
import static com.spiky.server.utils.Descriptors.*;

public class EncryptHandler extends MessageToMessageEncoder<MessageModels.Wrapper> {
    @Override
    protected void encode(ChannelHandlerContext ctx, MessageModels.Wrapper wrapper, List<Object> list) throws Exception {

        Cryptography cryptography = ctx.channel().attr(CRYPTOGRAPHY).get();
        MessageModels.CryptogramWrapper cryptogramWrapper = wrapper.getCryptogramWrapper();
        String secretKey = cryptography.getSecretKey();

        /* If not encrypted, just send */
        if(!wrapper.hasCryptogramWrapper())
        {
            ctx.writeAndFlush(wrapper);
        }
        else if(cryptogramWrapper.hasField(registration_cw))
        {
            byte[] cryptogram = cryptography.Crypt(cryptogramWrapper.getRegistration().toByteArray(), secretKey);
            cryptogramWrapper = MessageModels.CryptogramWrapper.newBuilder().clear()
                    .setRegistration(ByteString.copyFrom(cryptogram)).build();
            ctx.writeAndFlush(MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cryptogramWrapper).build());
        }
        else if (cryptogramWrapper.hasField(login_cw))
        {
            byte[] cryptogram = cryptography.Crypt(cryptogramWrapper.getLogin().toByteArray(), secretKey);
            cryptogramWrapper = MessageModels.CryptogramWrapper.newBuilder().clear()
                    .setLogin(ByteString.copyFrom(cryptogram)).build();
            ctx.writeAndFlush(MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cryptogramWrapper).build());
        }
        else if (cryptogramWrapper.hasField(initialState_cw))
        {
            byte[] cryptogram = cryptography.Crypt(cryptogramWrapper.getInitialState().toByteArray(), secretKey);
            cryptogramWrapper = MessageModels.CryptogramWrapper.newBuilder().clear()
                    .setInitialState(ByteString.copyFrom(cryptogram)).build();
            ctx.writeAndFlush(MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cryptogramWrapper).build());
        }
        else if(cryptogramWrapper.hasField(mainMenu_cw))
        {
            byte[] cryptogram = cryptography.Crypt(cryptogramWrapper.getMainMenu().toByteArray(), secretKey);
            cryptogramWrapper = MessageModels.CryptogramWrapper.newBuilder().clear()
                    .setMainMenu(ByteString.copyFrom(cryptogram)).build();
            ctx.writeAndFlush(MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cryptogramWrapper).build());
        }
        else if(cryptogramWrapper.hasField(room_cw))
        {
            byte[] cryptogram = cryptography.Crypt(cryptogramWrapper.getRoom().toByteArray(), secretKey);
            cryptogramWrapper = MessageModels.CryptogramWrapper.newBuilder().clear()
                    .setRoom(ByteString.copyFrom(cryptogram)).build();
            ctx.writeAndFlush(MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cryptogramWrapper).build());
        }
        else if(cryptogramWrapper.hasField(gameModels_cw))
        {
            byte[] cryptogram = cryptography.Crypt(cryptogramWrapper.getGameModels().toByteArray(), secretKey);
            cryptogramWrapper = MessageModels.CryptogramWrapper.newBuilder().clear()
                    .setGameModels(ByteString.copyFrom(cryptogram)).build();
            ctx.writeAndFlush(MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cryptogramWrapper).build());
        }
    }
}
