/*
 * Copyright (c)  2017, Vadim Petrov - MIT License
 */

package com.spiky.server.udp.handlers;

import com.spiky.server.protomodels.MessageModels;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.channel.socket.DatagramPacket;

import java.io.ByteArrayInputStream;

public class ProtoDecoderHandler extends SimpleChannelInboundHandler<DatagramPacket> {
    @Override
    protected void channelRead0(ChannelHandlerContext ctx, DatagramPacket datagramPacket) throws Exception {
        ByteBuf buf = datagramPacket.content();
        byte[] bytes = new byte[buf.readableBytes()];
        int readerIndex = buf.readerIndex();
        buf.getBytes(readerIndex, bytes);

        ByteArrayInputStream input = new ByteArrayInputStream(bytes);
        MessageModels.Wrapper wrapper = MessageModels.Wrapper.parseDelimitedFrom(input);

        System.out.println("udp: ");
        System.out.println(wrapper.toString());
        System.out.println(datagramPacket.sender().getAddress() + " " + datagramPacket.sender().getPort());

        // send echo
        ctx.write(new DatagramPacket(Unpooled.copiedBuffer(datagramPacket.content()), datagramPacket.sender()));
    }

    @Override
    public void channelReadComplete(ChannelHandlerContext ctx) throws Exception {
        ctx.flush();
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        cause.printStackTrace();
        ctx.close();
    }
}
