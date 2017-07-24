// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include <google/protobuf/descriptor.h>

class Descriptors
{
public:
	static const google::protobuf::FieldDescriptor* captchaDataField_ich;
	static const google::protobuf::FieldDescriptor* loginCheckStatus_ich;
	static const google::protobuf::FieldDescriptor* mailCheckStatus_ich;
	static const google::protobuf::FieldDescriptor* captchaCheckStatus_ich;

	static const google::protobuf::FieldDescriptor* publicKey_reg;
	static const google::protobuf::FieldDescriptor* stateCode_reg;

	static const google::protobuf::FieldDescriptor* publicKey_log;
	static const google::protobuf::FieldDescriptor* stateCode_log;

	static const google::protobuf::FieldDescriptor* registration_cw;
	static const google::protobuf::FieldDescriptor* login_cw;
	static const google::protobuf::FieldDescriptor* initialState_cw;
	static const google::protobuf::FieldDescriptor* room_cw;
	static const google::protobuf::FieldDescriptor* mainMenu_cw;
	static const google::protobuf::FieldDescriptor* gameModels_cw;

	static const google::protobuf::FieldDescriptor* chat_mm;

	static const google::protobuf::FieldDescriptor* nameField_chat;

	static const google::protobuf::FieldDescriptor* player_sub;
	static const google::protobuf::FieldDescriptor* player_team;
	static const google::protobuf::FieldDescriptor* chat_room;
};