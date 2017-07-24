/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp.logics;

import com.google.protobuf.ByteString;
import com.spiky.server.protomodels.GameModels;
import com.spiky.server.protomodels.GameRoomModels;
import com.spiky.server.protomodels.MainMenuModels;
import com.spiky.server.protomodels.MessageModels;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.group.ChannelMatchers;

import java.util.*;

import static com.spiky.server.ServerMain.*;
import static com.spiky.server.utils.Descriptors.deleteRoom_room;
import static com.spiky.server.utils.Descriptors.startGame_room;

public class RoomManager {
    public RoomManager(ChannelHandlerContext ctx, GameRoomModels.Room room) {
        if(room.hasCreateRoom())
        {
            createRoom(ctx, room.getCreateRoom());
        }
        else if(room.hasRoomsListUpdate()) {
            if(room.getRoomsListUpdate().hasField(deleteRoom_room)) {
                deleteRoom(ctx);
            }
        }
        else if(room.hasSubscribeRoom()) {
            if(room.getSubscribeRoom().getSubscribe()) {
                roomSubscribe(ctx, room);
            } else {
                roomUnsubscribe(ctx, room);
            }
        }
        else if(room.hasRoomUpdate()) {
            /* A chat message has arrived */
            if(room.getRoomUpdate().getRoomDescribe().hasChat()) {
                chatHandler(ctx, room.getRoomUpdate());
            } else {
                updateRoom(ctx, room.getRoomUpdate());
            }
        }
        else if(room.hasField(startGame_room)) {
            startGame(ctx, room.getRoomName());
        }
    }

    private void startGame(ChannelHandlerContext ctx, String roomName) {
        /* Find room */
        for(String owner : gameRooms.keySet()) {
            if (Objects.equals(gameRooms.get(owner).getRoomName(), roomName)) {
                GameRoom gameRoom = gameRooms.get(owner);
                /* Check the owner */
                if (Objects.equals(gameRoom.getCreator(), ctx.channel().attr(CHANNEL_OWNER).get())) {
                    /* From repeated pressing */
                    if(gameRoom.getGameState()) return;
                    /* Remove the room from the public, mark that it is in the process of the game */
                    gameRoom.setGameState(true);

                    GameRoomModels.RoomsListUpdate roomsListUpdate =  GameRoomModels.RoomsListUpdate.newBuilder().setRoomOwner(gameRoom.getCreator()).build();

                    GameRoomModels.Room room = GameRoomModels.Room.newBuilder().setStartGame(true).setRoomsListUpdate(roomsListUpdate).build();
                    MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setRoom(ByteString.copyFrom(room.toByteArray())).build();
                    MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();
                    /* Remove the room from the list, without any consequences */
                    roomListUpdateSubscribers.writeAndFlush(wrapper);

                    /* Delete and unsubscribe from the undistributed players room */
                    for (String p : gameRoom.undistributed.keySet()) {
                        /* But not the owner */
                        if(!Objects.equals(gameRoom.getCreator(), p)) {
                            gameRoom.players.remove(p);
                            gameRoom.recipients.remove(gameRoom.undistributed.get(p));
                        }
                    }
                    gameRoom.undistributed.clear();

                    /* Send to the players in the room the game start signal, includes the initial data */
                    GameModels.GameInitialState initialState = GameModels.GameInitialState.newBuilder().setStartGame(true).build();

                    Random rand = new Random();

                    for (String name : gameRoom.players.keySet()) {
                        String team = "";
                        if(gameRoom.team1.containsKey(name)) { team = "team1"; }
                        else if(gameRoom.team2.containsKey(name)) { team = "team2"; }

                        PlayerState playerState = new PlayerState();
                        playerState.setTeam(team);

                        GameModels.Player playerProto = GameModels.Player.newBuilder().build();

                        int randomX = rand.nextInt((2000 - (-2000)) + 1) + (-2000);
                        int randomY = rand.nextInt((2000 - (-2000)) + 1) + (-2000);

                        PlayerState.Location l = playerState.new Location(randomX, randomY, 0);
                        PlayerState.Rotation r = playerState.new Rotation(0, 0, 0);

                        playerState.addPosition(System.currentTimeMillis(), playerState.new Position(l, r));

                        gameRoom.playersState.put(name, playerState);

                        GameModels.PlayerPosition.Location loc = GameModels.PlayerPosition.Location.newBuilder()
                                .setX(playerState.getLastPosition().getValue().location.getX())
                                .setY(playerState.getLastPosition().getValue().location.getY())
                                .setZ(playerState.getLastPosition().getValue().location.getZ())
                                .build();

                        GameModels.PlayerPosition.Rotation rot = GameModels.PlayerPosition.Rotation.newBuilder()
                                .setPitch(playerState.getLastPosition().getValue().rotation.getPitch())
                                .setYaw(playerState.getLastPosition().getValue().rotation.getYaw())
                                .setRoll(playerState.getLastPosition().getValue().rotation.getRoll())
                                .build();

                        GameModels.PlayerPosition playerPosition = GameModels.PlayerPosition.newBuilder().setLoc(loc).setRot(rot).build();
                        /* The player's name, team name, position (location and rotation) */
                        playerProto = playerProto.toBuilder().setPlayerName(name).setTeam(playerState.getTeam()).setPlayerPosition(playerPosition).build();

                        initialState = initialState.toBuilder().addPlayer(playerProto).build();
                    }

                    GameModels.GameData gameData = GameModels.GameData.newBuilder().setGameInitialState(initialState).build();

                    MessageModels.CryptogramWrapper cw2 = MessageModels.CryptogramWrapper.newBuilder().setGameModels(ByteString.copyFrom(gameData.toByteArray())).build();
                    MessageModels.Wrapper wrapper2 = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw2).build();

                    for (Channel c : gameRoom.players.values()) c.writeAndFlush(wrapper2);
                }
            }
        }
    }

    private void chatHandler(ChannelHandlerContext ctx, GameRoomModels.RoomUpdate roomUpdate) {
        /* Find room */
        for(String owner : gameRooms.keySet()) {
            if (Objects.equals(gameRooms.get(owner).getRoomName(), roomUpdate.getRoomName())) {
                GameRoom gameRoom = gameRooms.get(owner);
                String channel_owner = ctx.channel().attr(CHANNEL_OWNER).get();

                if(gameRoom.players.containsKey(channel_owner)) {
                    Calendar c = Calendar.getInstance();
                    long ms = c.get(Calendar.HOUR_OF_DAY) * 60 * 60 * 1000 + c.get(Calendar.MINUTE) * 60 * 1000 + c.get(Calendar.SECOND) * 1000;

                    MainMenuModels.Chat chat = roomUpdate.getRoomDescribe().getChat();

                    String str = chat.getText();

                    chat = MainMenuModels.Chat.newBuilder().clear()
                            .setName(ctx.channel().attr(CHANNEL_OWNER).get())
                            .setText(str)
                            .setTime(ms)
                            .build();

                    /* Placed in the message list */
                    gameRoom.syncMessageList.add(chat);

                    GameRoomModels.RoomUpdate update = GameRoomModels.RoomUpdate.newBuilder().setRoomDescribe(
                            GameRoomModels.RoomDescribe.newBuilder().setChat(chat).build()).build();

                    GameRoomModels.Room room = GameRoomModels.Room.newBuilder().setRoomUpdate(update).build();
                    MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setRoom(ByteString.copyFrom(room.toByteArray())).build();
                    MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

                    gameRoom.recipients.writeAndFlush(wrapper);
                }
            }
        }
    }

    private void updateRoom(ChannelHandlerContext ctx, GameRoomModels.RoomUpdate roomUpdate) {
        /* Find room */
        for(String owner : gameRooms.keySet()) {
            if (Objects.equals(gameRooms.get(owner).getRoomName(), roomUpdate.getRoomName())) {
                GameRoom gameRoom = gameRooms.get(owner);
                String channel_owner = ctx.channel().attr(CHANNEL_OWNER).get();

                /* Check whether this user owns the requested room */
                if(Objects.equals(channel_owner, gameRoom.getCreator())) {
                    GameRoomModels.RoomDescribe roomDescribe = roomUpdate.getRoomDescribe();

                    Map<String,Channel> list = new HashMap<>();

                    /* Remove from old lists */
                    for (GameRoomModels.TeamPlayer p : roomDescribe.getTeam1List()) {
                        list.put(p.getPlayerName(), gameRoom.team1.get(p.getPlayerName()));
                        gameRoom.team1.remove(p.getPlayerName());
                    }

                    for (GameRoomModels.TeamPlayer p : roomDescribe.getTeam2List()) {
                        list.put(p.getPlayerName(), gameRoom.team2.get(p.getPlayerName()));
                        gameRoom.team2.remove(p.getPlayerName());
                    }

                    for (GameRoomModels.TeamPlayer p : roomDescribe.getUndistributedList()) {
                        list.put(p.getPlayerName(), gameRoom.undistributed.get(p.getPlayerName()));
                        gameRoom.undistributed.remove(p.getPlayerName());
                    }

                    /* Add to target */
                    if(Objects.equals(roomUpdate.getTargetTeam(), "team1")) {
                        gameRoom.team1.putAll(list);
                    } else if(Objects.equals(roomUpdate.getTargetTeam(), "team2")) {
                        gameRoom.team2.putAll(list);
                    } else if(Objects.equals(roomUpdate.getTargetTeam(), "undistributed")) {
                        gameRoom.undistributed.putAll(list);
                    }

                    /* Update lists on clients */
                    GameRoomModels.Room out = GameRoomModels.Room.newBuilder().setRoomUpdate(roomUpdate).build();

                    MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setRoom(ByteString.copyFrom(out.toByteArray())).build();
                    MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

                    gameRoom.recipients.writeAndFlush(wrapper);
                }
            }
        }
    }

    private static void roomUnsubscribe(ChannelHandlerContext ctx, GameRoomModels.Room room) {
        /* Find room */
        for(String owner : gameRooms.keySet()) {
            if (Objects.equals(gameRooms.get(owner).getRoomName(), room.getSubscribeRoom().getRoomName())) {
                GameRoom gameRoom = gameRooms.get(owner);
                String player = ctx.channel().attr(CHANNEL_OWNER).get();
                String team = "";

                /* The player can not leave the room if it is in a battle */
                if(!gameRoom.getGameState()) {
                    gameRoom.players.remove(player);

                    if(gameRoom.team1.containsKey(player)) {
                        team = "team1";
                        gameRoom.team1.remove(player);
                    } else if(gameRoom.team2.containsKey(player)) {
                        team = "team2";
                        gameRoom.team2.remove(player);
                    }  else if(gameRoom.undistributed.containsKey(player)) {
                        team = "undistributed";
                        gameRoom.undistributed.remove(player);
                    }

                    GameRoomModels.SubscribeRoom subscribe = room.getSubscribeRoom().toBuilder().setPlayer(player).setTeam(team).build();
                    GameRoomModels.Room out = GameRoomModels.Room.newBuilder().setSubscribeRoom(subscribe).build();

                    System.out.println(out);

                    MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setRoom(ByteString.copyFrom(out.toByteArray())).build();
                    MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

                    gameRoom.recipients.writeAndFlush(wrapper);
                }
            }
        }
    }

    private void roomSubscribe(ChannelHandlerContext ctx, GameRoomModels.Room room) {
        /* Find room */
        for(String owner : gameRooms.keySet()) {
            if(Objects.equals(gameRooms.get(owner).getRoomName(), room.getSubscribeRoom().getRoomName())) {
                GameRoom gameRoom = gameRooms.get(owner);
                /* Return the signal in case the room is full */
                if(gameRoom.players.size() > gameRoom.getMaxPlayers()) {
                    GameRoomModels.SubscribeRoom subscribe = GameRoomModels.SubscribeRoom.newBuilder().setStateCode(1).build();
                    GameRoomModels.Room out = GameRoomModels.Room.newBuilder().setSubscribeRoom(subscribe).build();

                    MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setRoom(ByteString.copyFrom(out.toByteArray())).build();
                    MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

                    ctx.writeAndFlush(wrapper);
                    return;
                }

                String player = ctx.channel().attr(CHANNEL_OWNER).get();
                /* Add if the player with such a nickname is not in the room */
                if(!gameRoom.players.containsKey(player)) {
                    gameRoom.addPlayer(ctx, player);
                } else {
                    GameRoomModels.SubscribeRoom subscribe = GameRoomModels.SubscribeRoom.newBuilder().setStateCode(2).build();
                    GameRoomModels.Room out = GameRoomModels.Room.newBuilder().setSubscribeRoom(subscribe).build();

                    MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setRoom(ByteString.copyFrom(out.toByteArray())).build();
                    MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

                    ctx.writeAndFlush(wrapper);
                    return;
                }

                /* Return the current state of players in the room */
                GameRoomModels.RoomDescribe.Builder builder = GameRoomModels.RoomDescribe.newBuilder();

                for (String p : gameRoom.undistributed.keySet()) {
                    GameRoomModels.TeamPlayer undistributed = GameRoomModels.TeamPlayer.newBuilder()
                            .setPlayerName(p).build();
                    builder.addUndistributed(undistributed);
                }

                for (String p : gameRoom.team1.keySet()) {
                    GameRoomModels.TeamPlayer team1 = GameRoomModels.TeamPlayer.newBuilder()
                            .setPlayerName(p).build();
                    builder.addTeam1(team1);
                }

                for (String p : gameRoom.team2.keySet()) {
                    GameRoomModels.TeamPlayer team2 = GameRoomModels.TeamPlayer.newBuilder()
                            .setPlayerName(p).build();
                    builder.addTeam2(team2);
                }

                GameRoomModels.RoomDescribe roomDescribe = builder.setRoomName(gameRoom.getRoomName())
                        .setMapName(gameRoom.getMapName())
                        .setGameTime(gameRoom.getGameTime() + " minutes")
                        .setMaxPlayers(gameRoom.getMaxPlayers() + " players")
                        .setCreator(gameRoom.getCreator())
                        .build();

                MainMenuModels.Chat.Builder chatBuilder = MainMenuModels.Chat.newBuilder();

                /* Create a list of the last 100 messages */
                for (int i = 0; i < gameRoom.syncMessageList.size(); i++) {
                    MainMenuModels.ChatMessage message = MainMenuModels.ChatMessage.newBuilder()
                            .setTime(gameRoom.syncMessageList.get(i).getTime())
                            .setName(gameRoom.syncMessageList.get(i).getName())
                            .setText(gameRoom.syncMessageList.get(i).getText())
                            .build();
                    chatBuilder.addMessages(message);
                }

                roomDescribe = roomDescribe.toBuilder().setChat(chatBuilder.build()).build();

                System.out.println(roomDescribe);

                GameRoomModels.SubscribeRoom subscribe = GameRoomModels.SubscribeRoom.newBuilder().setRoomDescribe(roomDescribe).build();
                GameRoomModels.Room out = GameRoomModels.Room.newBuilder().setSubscribeRoom(subscribe).build();

                MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setRoom(ByteString.copyFrom(out.toByteArray())).build();
                MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

                ctx.writeAndFlush(wrapper);

                /* Update lists from all the rest */
                updateRoom(ctx, gameRoom , player);

                ctx.channel().attr(ROOM_OWNER).set(gameRoom.getCreator());
            }
        }
    }

    private void updateRoom(ChannelHandlerContext ctx, GameRoom gameRoom, String player) {
        /* Send all information about a new player has been added */
        GameRoomModels.SubscribeRoom subscribe = GameRoomModels.SubscribeRoom.newBuilder().setPlayer(player).build();
        GameRoomModels.Room out = GameRoomModels.Room.newBuilder().setSubscribeRoom(subscribe).build();

        MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setRoom(ByteString.copyFrom(out.toByteArray())).build();
        MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

        /* To yourself do not send */
        gameRoom.recipients.writeAndFlush(wrapper, ChannelMatchers.isNot(ctx.channel()));
    }

    private void createRoom(ChannelHandlerContext ctx, GameRoomModels.CreateRoom createRoom) {
        /* Create a new room based on this data */
        GameRoom gameRoom = new GameRoom(ctx, createRoom);
        /* Check availability of room with such owner */
        if(!gameRooms.containsKey(gameRoom.getCreator())) {
            gameRooms.put(gameRoom.getCreator(), gameRoom);
            ctx.channel().attr(ROOM_OWNER).set(ctx.channel().attr(CHANNEL_OWNER).get());
            createRoom = createRoom.toBuilder().setRoomName(gameRoom.getRoomName()).setCreator(gameRoom.getCreator()).build();
            /* Return room data to the client  */
            GameRoomModels.Room room = GameRoomModels.Room.newBuilder().setCreateRoom(createRoom).build();
            MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setRoom(ByteString.copyFrom(room.toByteArray())).build();
            MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();
            /* Refresh the list of rooms for all */
            roomListUpdateSubscribers.writeAndFlush(wrapper);
        }
    }

    public static void deleteRoom(ChannelHandlerContext ctx) {
        /* If the channel owner owns the room */
        String channel_owner = ctx.channel().attr(CHANNEL_OWNER).get();
        if(gameRooms.containsKey(channel_owner)) {
            GameRoom gameRoom = gameRooms.get(channel_owner);
            /* Nothing can be done if the room is started */
            if(gameRoom.getGameState()) return;
            /* Remove from the list of rooms */
            gameRooms.remove(channel_owner);
            /* Inform subscribers about this */
            GameRoomModels.RoomsListUpdate update = GameRoomModels.RoomsListUpdate.newBuilder().setRoomOwner(gameRoom.getCreator()).build();

            GameRoomModels.Room room = GameRoomModels.Room.newBuilder().setRoomsListUpdate(update).build();
            MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setRoom(ByteString.copyFrom(room.toByteArray())).build();
            MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

            roomListUpdateSubscribers.writeAndFlush(wrapper);
        }
    }

    public static void clearRoomData(ChannelHandlerContext ctx) {
        String player = ctx.channel().attr(CHANNEL_OWNER).get();

        if(gameRooms.containsKey(player)) {
            GameRoom gameRoom = gameRooms.get(player);
            System.out.println(gameRoom.getRoomName());
            /* Delete from the list of rooms if the game is not running */
            if(!gameRoom.getGameState()) {
                gameRooms.remove(player);
                /* Inform subscribers about this */
                GameRoomModels.RoomsListUpdate update = GameRoomModels.RoomsListUpdate.newBuilder().setDeleteRoom(true).setRoomOwner(gameRoom.getCreator()).build();

                GameRoomModels.Room room = GameRoomModels.Room.newBuilder().setRoomsListUpdate(update).build();
                MessageModels.CryptogramWrapper cw = MessageModels.CryptogramWrapper.newBuilder().setRoom(ByteString.copyFrom(room.toByteArray())).build();
                MessageModels.Wrapper wrapper = MessageModels.Wrapper.newBuilder().setCryptogramWrapper(cw).build();

                roomListUpdateSubscribers.writeAndFlush(wrapper);
            }
        }
        /* There are no rooms, then check the player in the lists and delete if he is there */
        else {
            for(String owner : gameRooms.keySet()) {
                if (gameRooms.get(owner).players.containsKey(player)) {
                    /* The room is not in the process of playing */
                    if(!gameRooms.get(owner).getGameState()) {
                        GameRoom gameRoom = gameRooms.get(owner);
                        GameRoomModels.SubscribeRoom.Builder subscribe = GameRoomModels.SubscribeRoom.newBuilder()
                                .setRoomName(gameRoom.getRoomName());

                        if(gameRoom.team1.containsKey(player)) {
                            subscribe.setTeam("team1");
                        } else if (gameRoom.team2.containsKey(player)) {
                            subscribe.setTeam("team2");
                        } else if (gameRoom.undistributed.containsKey(player)) {
                            subscribe.setTeam("undistributed");
                        }

                        GameRoomModels.Room room = GameRoomModels.Room.newBuilder().setSubscribeRoom(subscribe.build()).build();
                        roomUnsubscribe(ctx, room);
                    }
                }
            }
        }
    }
}
