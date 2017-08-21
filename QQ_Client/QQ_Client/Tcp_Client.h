#pragma once
class Tcp_Client
{
public:
	Tcp_Client();
	~Tcp_Client();
	void connectToServer();
	void sendToGroup();

	SOCKET sClient;
};

