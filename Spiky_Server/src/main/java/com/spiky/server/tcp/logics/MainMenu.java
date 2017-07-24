/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp.logics;

import com.spiky.server.protomodels.MainMenuModels;
import io.netty.channel.ChannelHandlerContext;

import static com.spiky.server.utils.Descriptors.chat_mm;

public class MainMenu {
    public MainMenu(ChannelHandlerContext ctx, MainMenuModels.MainMenu mainMenu) {
        if(mainMenu.hasField(chat_mm))
        {
            new MainMenuChat(ctx, mainMenu.getChat());
        }
    }
}
