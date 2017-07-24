/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp.logics;

import com.spiky.server.protomodels.GameRoomModels;
import com.spiky.server.protomodels.MainMenuModels;
import com.spiky.server.utils.CircularArrayList;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.group.ChannelGroup;
import io.netty.channel.group.DefaultChannelGroup;
import io.netty.util.concurrent.GlobalEventExecutor;

import java.util.*;

import static com.spiky.server.ServerMain.CHANNEL_OWNER;

public class GameRoom {
    private String creator;
    private String mapName;
    private String roomName;
    private int gameTime;
    private int maxPlayers;
    private String creatorSessionId;
    private boolean gameState = false;

    Map<String,Channel> players = Collections.synchronizedMap(new HashMap<>());
    Map<String,Channel> team1 = Collections.synchronizedMap(new HashMap<>());
    Map<String,Channel> team2 = Collections.synchronizedMap(new HashMap<>());
    Map<String,Channel> undistributed = Collections.synchronizedMap(new HashMap<>());

    Map<String,PlayerState> playersState = Collections.synchronizedMap(new HashMap<>());

    /* List of subscribers for room updates*/
    ChannelGroup recipients = new DefaultChannelGroup(GlobalEventExecutor.INSTANCE);
    /* List of 100 chat messages */
    List<MainMenuModels.Chat> syncMessageList = Collections.synchronizedList(new CircularArrayList<MainMenuModels.Chat>(100));

    GameRoom(ChannelHandlerContext ctx, GameRoomModels.CreateRoom roomDescribe) {
        creator = ctx.channel().attr(CHANNEL_OWNER).get();
        /* If the player left or lost the connection, the room is deleted after a few seconds by its session id */
        creatorSessionId = ctx.channel().id().asShortText();
        roomName = roomDescribe.getRoomName();
        mapName = roomDescribe.getMapName();

        if(Objects.equals(roomDescribe.getGameTime(), "5 minutes")) {
            gameTime = 5;
        } else if(Objects.equals(roomDescribe.getGameTime(), "10 minutes")) {
            gameTime = 10;
        } else if(Objects.equals(roomDescribe.getGameTime(), "15 minutes")) {
            gameTime = 15;
        } else if(Objects.equals(roomDescribe.getGameTime(), "20 minutes")) {
            gameTime = 20;
        }

        maxPlayers = Integer.parseInt(roomDescribe.getMaxPlayers());
        addPlayer(ctx, creator);
    }

    void addPlayer(ChannelHandlerContext ctx, String player) {
        if(players.size() < maxPlayers) {
            /* Add to the general list of players */
            players.put(player, ctx.channel());
            /* Add to unallocated list */
            undistributed.put(player, ctx.channel());
            /* Subscribe for events */
            recipients.add(ctx.channel());
        }
    }

    public String getCreator() {
        return creator;
    }

    public void setCreator(String creator) {
        this.creator = creator;
    }

    public String getMapName() {
        return mapName;
    }

    public void setMapName(String mapName) {
        this.mapName = mapName;
    }

    public String getRoomName() {
        return roomName;
    }

    public void setRoomName(String roomName) {
        this.roomName = roomName;
    }

    public int getGameTime() {
        return gameTime;
    }

    public void setGameTime(int gameTime) {
        this.gameTime = gameTime;
    }

    public int getMaxPlayers() {
        return maxPlayers;
    }

    public void setMaxPlayers(int maxPlayers) {
        this.maxPlayers = maxPlayers;
    }

    public String getCreatorSessionId() {
        return creatorSessionId;
    }

    public void setCreatorSessionId(String creatorSessionId) {
        this.creatorSessionId = creatorSessionId;
    }

    public boolean getGameState() {
        return gameState;
    }

    public void setGameState(boolean gameState) {
        this.gameState = gameState;
    }
}
