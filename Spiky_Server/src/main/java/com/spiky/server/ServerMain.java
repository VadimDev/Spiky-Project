/*
 * Copyright (c)  2017, Vadim Petrov - MIT License
 */

package com.spiky.server;

import com.spiky.server.tcp.VerificationServerConnection;
import com.spiky.server.tcp.logics.GameRoom;
import com.spiky.server.utils.Cryptography;
import io.netty.bootstrap.Bootstrap;
import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.group.ChannelGroup;
import io.netty.channel.group.DefaultChannelGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.nio.NioDatagramChannel;
import io.netty.channel.socket.nio.NioServerSocketChannel;
import io.netty.util.AttributeKey;
import io.netty.util.concurrent.GlobalEventExecutor;
import org.hibernate.Session;
import org.hibernate.Transaction;

import java.util.*;

public class ServerMain {
    /* The configuration file */
    private static final ResourceBundle configurationBundle = ResourceBundle.getBundle("configuration", Locale.ENGLISH);
    /* Server ports */
    private static final int tcpPort = Integer.valueOf(configurationBundle.getString("tcpPort"));
    private static final int udpPort = Integer.valueOf(configurationBundle.getString("udpPort"));
    private static final int verificationServerPort = Integer.valueOf(configurationBundle.getString("verificationServerPort"));
    /* Whether the data is encrypted */
    public static final boolean bEnableCrypto = Boolean.parseBoolean(configurationBundle.getString("enableCrypt"));

    /* Data available in all handlers of a particular channel */
    public final static AttributeKey<String> SECRETKEY = AttributeKey.valueOf("secret_key");
    public final static AttributeKey<String> CHANNEL_OWNER = AttributeKey.valueOf("channel_owner");
    public final static AttributeKey<Session> HIBER_SESSION = AttributeKey.valueOf("hiber_session");
    public final static AttributeKey<Transaction> HIBER_TRANSACTION = AttributeKey.valueOf("hiber_transaction");
    public final static AttributeKey<Cryptography> CRYPTOGRAPHY = AttributeKey.valueOf("hiber_cryptography");
    public final static AttributeKey<String> ROOM_OWNER = AttributeKey.valueOf("room_owner");

    /* Synchronized set of captcha */
    public static Map<Long,String> captchaBank = Collections.synchronizedMap(new HashMap<>());
    /* Set of users receiving updates to the list of rooms in the main menu */
    public static ChannelGroup roomListUpdateSubscribers = new DefaultChannelGroup(GlobalEventExecutor.INSTANCE);
    /* Set of all game rooms */
    public static Map<String,GameRoom> gameRooms = Collections.synchronizedMap(new HashMap<>());
    /* Connection to the server checking hits */
    public static VerificationServerConnection verificationServerConnection;

    private static void run_tcp() {
        EventLoopGroup bossGroup = new NioEventLoopGroup();
        EventLoopGroup workerGroup = new NioEventLoopGroup();

        try {
            ServerBootstrap b = new ServerBootstrap();
            b.group(bossGroup, workerGroup)
                    .channel(NioServerSocketChannel.class)
                    .childHandler(new com.spiky.server.tcp.ServerInitializer())
                    .childOption(ChannelOption.SO_KEEPALIVE, true)
                    .childOption(ChannelOption.TCP_NODELAY, true);

            ChannelFuture f = b.bind(tcpPort).sync();
            f.channel().closeFuture().sync();

        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            workerGroup.shutdownGracefully();
            bossGroup.shutdownGracefully();
        }
    }

    private static void run_udp() {
        final NioEventLoopGroup group = new NioEventLoopGroup();
        try {
            Bootstrap bootstrap = new Bootstrap();
            bootstrap.group(group).channel(NioDatagramChannel.class)
                    .handler(new com.spiky.server.udp.ServerInitializer());
            bootstrap.bind(udpPort).sync();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /* Remove captchas that are older than 60 seconds */
    private static void captchaCleaner() {
        long lifetime = 60000;
        new Thread(() -> {
            while (true) {
                try {
                    Thread.sleep(10000); // Check for obsolete availability of captch every 10 seconds
                    synchronized (captchaBank) {
                        captchaBank.entrySet().removeIf(e-> System.currentTimeMillis() - e.getKey() > lifetime);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    public static void main(String[] args) {
        new Thread(ServerMain::run_tcp).start();
        //new Thread(ServerMain::run_udp).start();

        captchaCleaner();
        verificationServerConnection = new VerificationServerConnection(verificationServerPort);
    }
}
