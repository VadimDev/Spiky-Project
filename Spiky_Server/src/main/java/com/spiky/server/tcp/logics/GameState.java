/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp.logics;

import com.google.protobuf.ByteString;
import com.spiky.server.ServerMain;
import com.spiky.server.protomodels.GameModels;
import com.spiky.server.protomodels.MessageModels;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.group.ChannelMatchers;

import java.util.Map;

import static com.spiky.server.ServerMain.CHANNEL_OWNER;
import static com.spiky.server.ServerMain.ROOM_OWNER;
import static com.spiky.server.ServerMain.gameRooms;
import static com.spiky.server.utils.Descriptors.requestTo_shot_gd;

public class GameState {
    public GameState(ChannelHandlerContext ctx, GameModels.GameData gameData) {
        if(gameData.hasPlayerPosition()) {
            updatePosition(ctx, gameData.getPlayerPosition());
        }
        else if(gameData.hasShot()) {
            /* Find a goal */
            GameRoom gameRoom = gameRooms.get(ctx.channel().attr(ROOM_OWNER).get());
            /* Check the hit. If someone -- hit it. If there is no -- dispatch to all the shot event */
            if(gameData.getShot().hasField(requestTo_shot_gd)) {
                gameData = gameData.toBuilder().setShot(gameData.getShot().toBuilder().clearTimeStamp().build()).build();

                MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setGameModels(ByteString.copyFrom(gameData.toByteArray())).build();
                MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

                gameRoom.recipients.writeAndFlush(wrapper, ChannelMatchers.isNot(ctx.channel()));
            } else {
                PlayerState state = gameRoom.playersState.get(gameData.getShot().getRequestTo());
                /* Find the player's nearest position by the time */
                Map.Entry<Long, PlayerState.Position> pos = state.getClosestMs(gameData.getShot().getTimeStamp());

                //System.out.println("TimeStamp: " + gameData.getShot().getTimeStamp());
                //System.out.println("ClosestMs: " + pos.getKey());

                GameModels.PlayerPosition.Location loc = GameModels.PlayerPosition.Location.newBuilder()
                        .setX(pos.getValue().location.getX())
                        .setY(pos.getValue().location.getY())
                        .setZ(pos.getValue().location.getZ())
                        .build();

                GameModels.PlayerPosition.Rotation rot = GameModels.PlayerPosition.Rotation.newBuilder()
                        .setPitch(pos.getValue().rotation.getPitch())
                        .setYaw(pos.getValue().rotation.getYaw())
                        .setRoll(pos.getValue().rotation.getRoll())
                        .build();

                GameModels.PlayerPosition playerPosition = GameModels.PlayerPosition.newBuilder().setLoc(loc).setRot(rot).build();

                GameModels.Shot shot = gameData.getShot().toBuilder().setRequestFrom(ctx.channel().attr(CHANNEL_OWNER).get())
                        .setRoomOwner(ctx.channel().attr(ROOM_OWNER).get()).setPlayerPosition(playerPosition).build();

                GameModels.GameData gd = GameModels.GameData.newBuilder().setShot(shot).build();

                ServerMain.verificationServerConnection.SendToVerificationServer(gd);
            }
        }
        else if(gameData.hasPing()) {
            MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setGameModels(ByteString.copyFrom(gameData.toByteArray())).build();
            MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();
            ctx.writeAndFlush(wrapper);
        }
    }

    public GameState(GameModels.GameData gameData) {
        if(gameData.hasShot()) {
            System.out.println(gameData);

            GameRoom gameRoom = gameRooms.get(gameData.getShot().getRoomOwner());

            gameData = gameData.toBuilder().setShot(gameData.getShot().toBuilder()
                    .clearTimeStamp()
                    .clearPlayerPosition()
                    .clearRoomOwner().build()
            ).build();

            MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setGameModels(ByteString.copyFrom(gameData.toByteArray())).build();
            MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

            gameRoom.recipients.writeAndFlush(wrapper);
        }
    }

    private void updatePosition(ChannelHandlerContext ctx, GameModels.PlayerPosition playerPosition) {
        GameRoom gameRoom = gameRooms.get(ctx.channel().attr(ROOM_OWNER).get());

        /* Keep the player's position in history */
        PlayerState state = gameRoom.playersState.get(ctx.channel().attr(CHANNEL_OWNER).get());

        PlayerState.Location l = state.new Location(playerPosition.getLoc().getX(), playerPosition.getLoc().getY(), playerPosition.getLoc().getZ());
        PlayerState.Rotation r = state.new Rotation(playerPosition.getRot().getPitch(), playerPosition.getRot().getYaw(), playerPosition.getRot().getRoll());
        state.addPosition(playerPosition.getTimeStamp(), state.new Position(l, r));

        //System.out.println(playerPosition.getTimeStamp());

        /* Send to subscribers */
        playerPosition = playerPosition.toBuilder().setPlayerName(ctx.channel().attr(CHANNEL_OWNER).get()).build();

        GameModels.GameData gameData = GameModels.GameData.newBuilder().setPlayerPosition(playerPosition).build();

        MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setGameModels(ByteString.copyFrom(gameData.toByteArray())).build();
        MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

        gameRoom.recipients.writeAndFlush(wrapper, ChannelMatchers.isNot(ctx.channel()));
    }
}
