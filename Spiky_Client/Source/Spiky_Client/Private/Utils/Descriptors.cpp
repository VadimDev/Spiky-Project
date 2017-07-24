// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "Descriptors.h"
#include "Protobufs/RegLogModels.pb.h"
#include "Protobufs/MessageModels.pb.h"
#include "Protobufs/MainMenuModels.pb.h"

const google::protobuf::FieldDescriptor* Descriptors::captchaDataField_ich = InputChecking::default_instance().descriptor()->FindFieldByName("captchaData");
const google::protobuf::FieldDescriptor* Descriptors::loginCheckStatus_ich = InputChecking::default_instance().descriptor()->FindFieldByName("loginCheckStatus");
const google::protobuf::FieldDescriptor* Descriptors::mailCheckStatus_ich = InputChecking::default_instance().descriptor()->FindFieldByName("mailCheckStatus");
const google::protobuf::FieldDescriptor* Descriptors::captchaCheckStatus_ich = InputChecking::default_instance().descriptor()->FindFieldByName("captchaCheckStatus");

const google::protobuf::FieldDescriptor* Descriptors::publicKey_reg = Registration::default_instance().descriptor()->FindFieldByName("publicKey");
const google::protobuf::FieldDescriptor* Descriptors::stateCode_reg = Registration::default_instance().descriptor()->FindFieldByName("stateCode");

const google::protobuf::FieldDescriptor* Descriptors::publicKey_log = Login::default_instance().descriptor()->FindFieldByName("publicKey");
const google::protobuf::FieldDescriptor* Descriptors::stateCode_log = Login::default_instance().descriptor()->FindFieldByName("stateCode");

const google::protobuf::FieldDescriptor* Descriptors::registration_cw = CryptogramWrapper::default_instance().descriptor()->FindFieldByName("registration");
const google::protobuf::FieldDescriptor* Descriptors::login_cw = CryptogramWrapper::default_instance().descriptor()->FindFieldByName("login");
const google::protobuf::FieldDescriptor* Descriptors::initialState_cw = CryptogramWrapper::default_instance().descriptor()->FindFieldByName("initialState");
const google::protobuf::FieldDescriptor* Descriptors::room_cw = CryptogramWrapper::default_instance().descriptor()->FindFieldByName("room");
const google::protobuf::FieldDescriptor* Descriptors::mainMenu_cw = CryptogramWrapper::default_instance().descriptor()->FindFieldByName("mainMenu");
const google::protobuf::FieldDescriptor* Descriptors::gameModels_cw = CryptogramWrapper::default_instance().descriptor()->FindFieldByName("gameModels");

const google::protobuf::FieldDescriptor* Descriptors::chat_mm = MainMenu::default_instance().descriptor()->FindFieldByName("chat");

const google::protobuf::FieldDescriptor* Descriptors::nameField_chat = Chat::default_instance().descriptor()->FindFieldByName("name");

const google::protobuf::FieldDescriptor* Descriptors::player_sub = SubscribeRoom::default_instance().descriptor()->FindFieldByName("player");
const google::protobuf::FieldDescriptor* Descriptors::player_team = SubscribeRoom::default_instance().descriptor()->FindFieldByName("team");
const google::protobuf::FieldDescriptor* Descriptors::chat_room = RoomDescribe::default_instance().descriptor()->FindFieldByName("chat");
