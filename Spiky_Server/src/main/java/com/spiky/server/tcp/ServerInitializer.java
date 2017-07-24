/*
 * Copyright (c)  2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp;

import com.spiky.server.protomodels.MessageModels;
import com.spiky.server.tcp.handlers.DecryptHandler;
import com.spiky.server.tcp.handlers.EncryptHandler;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelPipeline;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.codec.protobuf.ProtobufDecoder;
import io.netty.handler.codec.protobuf.ProtobufEncoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32FrameDecoder;
import io.netty.handler.codec.protobuf.ProtobufVarint32LengthFieldPrepender;
import io.netty.handler.timeout.IdleStateHandler;

public class ServerInitializer extends ChannelInitializer<SocketChannel> {
    @Override
    protected void initChannel(SocketChannel ch) throws Exception {
        ChannelPipeline pipeline = ch.pipeline();

        /* debug */
        //pipeline.addLast(new LoggingHandler(LogLevel.INFO));

        /* unwrap messages */
        // Decoders protobuf
        pipeline.addLast(new ProtobufVarint32FrameDecoder());
        pipeline.addLast(new ProtobufDecoder(MessageModels.Wrapper.getDefaultInstance()));
        /* wrap messages */
        // Encoder protobuf
        pipeline.addLast(new ProtobufVarint32LengthFieldPrepender());
        pipeline.addLast(new ProtobufEncoder());

        /* The connection is closed when there is no inbound traffic for N seconds */
        pipeline.addLast(new IdleStateHandler(30, 0, 0));
        /* Encrypt outgoing message */
        pipeline.addLast(new EncryptHandler());
        /* Decipher the incoming message */
        pipeline.addLast(new DecryptHandler());
    }
}
