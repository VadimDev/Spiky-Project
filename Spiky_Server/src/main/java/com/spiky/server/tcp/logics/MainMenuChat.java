/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp.logics;

import com.google.protobuf.ByteString;
import com.spiky.server.protomodels.MainMenuModels;
import com.spiky.server.protomodels.MessageModels;
import com.spiky.server.utils.CircularArrayList;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.group.ChannelGroup;
import io.netty.channel.group.DefaultChannelGroup;
import io.netty.util.concurrent.GlobalEventExecutor;

import java.util.Calendar;
import java.util.Collections;
import java.util.List;

import static com.spiky.server.ServerMain.CHANNEL_OWNER;
import static com.spiky.server.utils.Descriptors.subscribe_chat;

public class MainMenuChat {

    /* List of main chat subscribers */
    private static ChannelGroup recipients = new DefaultChannelGroup(GlobalEventExecutor.INSTANCE);

    /* List of 100 posts */
    private static List<MainMenuModels.Chat> syncMessageList = Collections.synchronizedList(new CircularArrayList<MainMenuModels.Chat>(100));

    public MainMenuChat(ChannelHandlerContext ctx, MainMenuModels.Chat chat) {
        if(chat.hasField(subscribe_chat))
        {
            if(chat.getSubscribe())
            {
                /* Subscribe */
                recipients.add(ctx.channel());

                MainMenuModels.Chat.Builder builder = MainMenuModels.Chat.newBuilder();

                /* Create a list of the last 100 messages */
                for (int i = 0; i < syncMessageList.size(); i++) {
                    MainMenuModels.ChatMessage message = MainMenuModels.ChatMessage.newBuilder()
                            .setTime(syncMessageList.get(i).getTime())
                            .setName(syncMessageList.get(i).getName())
                            .setText(syncMessageList.get(i).getText())
                            .build();
                    builder.addMessages(message);
                }

                MainMenuModels.MainMenu mainMenu = MainMenuModels.MainMenu.newBuilder().setChat(builder.build()).build();

                MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setMainMenu(ByteString.copyFrom(mainMenu.toByteArray())).build();
                MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();
                ctx.writeAndFlush(wrapper);
            } else {
                /* Unsubscribe */
                recipients.remove(ctx.channel());
            }
        }
        else /* received a message */
        {
            //System.currentTimeMillis(); // Gives wrong hour because of system settings
            Calendar c = Calendar.getInstance();
            long ms = c.get(Calendar.HOUR_OF_DAY) * 60 * 60 * 1000 + c.get(Calendar.MINUTE) * 60 * 1000 + c.get(Calendar.SECOND) * 1000;

            String str = chat.getText();

            chat = MainMenuModels.Chat.newBuilder().clear()
                    .setName(ctx.channel().attr(CHANNEL_OWNER).get())
                    .setText(str)
                    .setTime(ms)
                    .build();

            /* Placed in the message list */
            syncMessageList.add(chat);

            MainMenuModels.MainMenu mainMenu = MainMenuModels.MainMenu.newBuilder().setChat(chat).build();

            MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setMainMenu(ByteString.copyFrom(mainMenu.toByteArray())).build();
            MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();
            /* Sent to all recipients */
            recipients.writeAndFlush(wrapper);
        }
    }
}
