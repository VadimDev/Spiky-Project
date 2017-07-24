// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "MessageEncoder.h"
#include "SocketObject.h"
#include "Protobufs/MessageModels.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include "Crypto.h"
#include "SpikyGameInstance.h"
#include "Protobufs/MainMenuModels.pb.h"
#include "Protobufs/GameRoomModels.pb.h"
#include "Protobufs/GameModels.pb.h "

bool UMessageEncoder::Send(google::protobuf::Message * message, bool bCrypt, bool bTCP)
{
	Wrapper wrapper;
	std::shared_ptr<CryptogramWrapper> cw(new CryptogramWrapper);

	// Encrypted or not encrypted
	if (bCrypt)
	{
		if (message->GetTypeName() == "Registration")
		{
			Registration * mes = static_cast<Registration*>(message);
			std::string cipher = UCrypto::EncryptProto(mes, USpikyGameInstance::secretKey);
			cw->set_registration(cipher);
			wrapper.set_allocated_cryptogramwrapper(cw.get());
		}
		else if (message->GetTypeName() == "Login")
		{
			Login * mes = static_cast<Login*>(message);
			std::string cipher = UCrypto::EncryptProto(mes, USpikyGameInstance::secretKey);
			cw->set_login(cipher);
			wrapper.set_allocated_cryptogramwrapper(cw.get());
		}
		else if (message->GetTypeName() == "MainMenu")
		{
			MainMenu * mes = static_cast<MainMenu*>(message);
			std::string cipher = UCrypto::EncryptProto(mes, USpikyGameInstance::secretKey);
			cw->set_mainmenu(cipher);
			wrapper.set_allocated_cryptogramwrapper(cw.get());
		}
		else if (message->GetTypeName() == "Room")
		{
			Room * mes = static_cast<Room*>(message);
			std::string cipher = UCrypto::EncryptProto(mes, USpikyGameInstance::secretKey);
			cw->set_room(cipher);
			wrapper.set_allocated_cryptogramwrapper(cw.get());
		}
		else if (message->GetTypeName() == "GameData")
		{
			GameData * mes = static_cast<GameData*>(message);
			std::string cipher = UCrypto::EncryptProto(mes, USpikyGameInstance::secretKey);
			cw->set_gamemodels(cipher);
			wrapper.set_allocated_cryptogramwrapper(cw.get());
		}
	}
	else
	{
		if (message->GetTypeName() == "Utility")
		{
			Utility * mes = static_cast<Utility*>(message);
			wrapper.set_allocated_utility(mes);
		}
		else if (message->GetTypeName() == "InputChecking")
		{
			InputChecking * mes = static_cast<InputChecking*>(message);
			wrapper.set_allocated_inputchecking(mes);
		}
		else if (message->GetTypeName() == "Registration")
		{
			Registration * mes = static_cast<Registration*>(message);
			wrapper.set_allocated_registration(mes);
		}
		else if (message->GetTypeName() == "Login")
		{
			Login * mes = static_cast<Login*>(message);
			wrapper.set_allocated_login(mes);
		}
	}

	size_t size = wrapper.ByteSize() + 5; // include size, varint32 never takes more than 5 bytes
	uint8_t * buffer = new uint8_t[size];

	google::protobuf::io::ArrayOutputStream arr(buffer, size);
	google::protobuf::io::CodedOutputStream output(&arr);

	// Write message length and message in buffer
	output.WriteVarint32(wrapper.ByteSize());
	wrapper.SerializeToCodedStream(&output);

	// Free up allocated memory
	if (wrapper.has_utility())
	{
		wrapper.release_utility();
	}
	else if (wrapper.has_inputchecking())
	{
		wrapper.release_inputchecking();
	}
	else if (wrapper.has_registration())
	{
		wrapper.release_registration();
	}
	else if (wrapper.has_login())
	{
		wrapper.release_login();
	}
	else if (wrapper.has_cryptogramwrapper())
	{
		wrapper.release_cryptogramwrapper();
	}

	int32 bytesSent = 0;
	bool sentState = false;

	if (bTCP)
	{
		//send by tcp
		sentState = USocketObject::tcp_socket->Send(buffer, output.ByteCount(), bytesSent);
	}
	else
	{
		//send by udp
		sentState = USocketObject::udp_socket->SendTo(buffer, output.ByteCount(), bytesSent, *USocketObject::udp_address);
	}

	delete[] buffer;
	return sentState;
}