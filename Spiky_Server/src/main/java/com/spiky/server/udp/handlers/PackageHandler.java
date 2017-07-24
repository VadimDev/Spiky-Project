/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.udp.handlers;

import com.spiky.server.protomodels.MessageModels;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

public class PackageHandler extends SimpleChannelInboundHandler<MessageModels.Wrapper> {
    @Override
    protected void channelRead0(ChannelHandlerContext ctx, MessageModels.Wrapper wrapper) throws Exception {

    }
}
