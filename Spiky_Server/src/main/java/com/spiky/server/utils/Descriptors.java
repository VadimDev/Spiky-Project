/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.utils;

import com.spiky.server.protomodels.*;

/**
 * Different descriptors to determine the contents of messages
 * */
public class Descriptors {

    public static com.google.protobuf.Descriptors.FieldDescriptor registration_cw = MessageModels.CryptogramWrapper.getDefaultInstance().getDescriptorForType().findFieldByName("registration");
    public static com.google.protobuf.Descriptors.FieldDescriptor login_cw = MessageModels.CryptogramWrapper.getDefaultInstance().getDescriptorForType().findFieldByName("login");
    public static com.google.protobuf.Descriptors.FieldDescriptor initialState_cw = MessageModels.CryptogramWrapper.getDefaultInstance().getDescriptorForType().findFieldByName("initialState");
    public static com.google.protobuf.Descriptors.FieldDescriptor room_cw = MessageModels.CryptogramWrapper.getDefaultInstance().getDescriptorForType().findFieldByName("room");
    public static com.google.protobuf.Descriptors.FieldDescriptor mainMenu_cw = MessageModels.CryptogramWrapper.getDefaultInstance().getDescriptorForType().findFieldByName("mainMenu");
    public static com.google.protobuf.Descriptors.FieldDescriptor gameModels_cw = MessageModels.CryptogramWrapper.getDefaultInstance().getDescriptorForType().findFieldByName("gameModels");

    public static com.google.protobuf.Descriptors.FieldDescriptor getCaptcha_ich = RegistrationLoginModels.InputChecking.getDefaultInstance().getDescriptorForType().findFieldByName("getCaptcha");
    public static com.google.protobuf.Descriptors.FieldDescriptor login_ich = RegistrationLoginModels.InputChecking.getDefaultInstance().getDescriptorForType().findFieldByName("login");
    public static com.google.protobuf.Descriptors.FieldDescriptor mail_ich = RegistrationLoginModels.InputChecking.getDefaultInstance().getDescriptorForType().findFieldByName("mail");
    public static com.google.protobuf.Descriptors.FieldDescriptor captcha_ich = RegistrationLoginModels.InputChecking.getDefaultInstance().getDescriptorForType().findFieldByName("captcha");

    public static com.google.protobuf.Descriptors.FieldDescriptor login_reg = RegistrationLoginModels.Registration.getDefaultInstance().getDescriptorForType().findFieldByName("login");
    public static com.google.protobuf.Descriptors.FieldDescriptor mail_reg = RegistrationLoginModels.Registration.getDefaultInstance().getDescriptorForType().findFieldByName("mail");
    public static com.google.protobuf.Descriptors.FieldDescriptor captcha_reg = RegistrationLoginModels.Registration.getDefaultInstance().getDescriptorForType().findFieldByName("captcha");
    public static com.google.protobuf.Descriptors.FieldDescriptor publicKey_reg = RegistrationLoginModels.Registration.getDefaultInstance().getDescriptorForType().findFieldByName("publicKey");

    public static com.google.protobuf.Descriptors.FieldDescriptor publicKey_log = RegistrationLoginModels.Login.getDefaultInstance().getDescriptorForType().findFieldByName("publicKey");

    public static com.google.protobuf.Descriptors.FieldDescriptor subscribe_chat = MainMenuModels.Chat.getDefaultInstance().getDescriptorForType().findFieldByName("subscribe");
    public static com.google.protobuf.Descriptors.FieldDescriptor chat_mm = MainMenuModels.MainMenu.getDefaultInstance().getDescriptorForType().findFieldByName("chat");

    public static com.google.protobuf.Descriptors.FieldDescriptor deleteRoom_room = GameRoomModels.RoomsListUpdate.getDefaultInstance().getDescriptorForType().findFieldByName("deleteRoom");
    public static com.google.protobuf.Descriptors.FieldDescriptor startGame_room = GameRoomModels.Room.getDefaultInstance().getDescriptorForType().findFieldByName("startGame");

    public static com.google.protobuf.Descriptors.FieldDescriptor requestTo_shot_gd = GameModels.Shot.getDefaultInstance().getDescriptorForType().findFieldByName("requestTo");
}
