/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.udp;

import com.spiky.server.udp.handlers.PackageHandler;
import com.spiky.server.udp.handlers.ProtoDecoderHandler;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelPipeline;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioDatagramChannel;
import io.netty.handler.logging.LogLevel;
import io.netty.handler.logging.LoggingHandler;

public class ServerInitializer extends ChannelInitializer<NioDatagramChannel> {
    @Override
    protected void initChannel(NioDatagramChannel ch) throws Exception {
        ChannelPipeline pipeline = ch.pipeline();

        /* debug */
        pipeline.addLast(new LoggingHandler(LogLevel.INFO));
        /* proto decoder */
        pipeline.addLast(new ProtoDecoderHandler());
        /* message handler */
        //pipeline.addLast(new PackageHandler());
    }
}