// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include <string>

class Config
{
public:

	static std::string address;

	static size_t tcp_local_port;

	static size_t tcp_server_port;

	static size_t udp_local_port;

	static size_t udp_server_port;

	static bool bEnableCrypt;

};