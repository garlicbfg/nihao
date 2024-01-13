#pragma once
#include <Core/Types.h>
#include <Network/Network.h>
#include <string>
#include <vector>




class LogSend
{
private:
	LogSend() {}

	~LogSend()
	{
		if(socket != -1)
			netio_terminate(socket);
	}

	static LogSend& getInstance()
	{
		static LogSend theInstance;
		return theInstance;
	}


public:
	static void connect(const std::string& addr)
	{
		LogSend& inst = getInstance();
		if (inst.socket == -1)
			inst.socket = netio_connect(addr.c_str(), ConnType::CONN_4BE);
	}

	static void send(const std::vector<u8>& data)
	{
		LogSend& inst = getInstance();
		netio_send(inst.socket, data.data(), data.size());
	}

private:
	int socket{ -1 };
};
