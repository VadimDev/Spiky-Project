// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "Config.h"

bool Config::bEnableCrypt = true;

std::string Config::address = "127.0.0.1";

size_t Config::tcp_local_port = 7678;

size_t Config::tcp_server_port = 7680;

size_t Config::udp_local_port = 7679;

size_t Config::udp_server_port = 7681;