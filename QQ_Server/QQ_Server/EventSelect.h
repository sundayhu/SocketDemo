
#include <vector>
using namespace std;


#pragma once
class EventSelect 
{
public:
	EventSelect();
	~EventSelect();

public:
	void define();
	void initSocket();
	void initEvent();

public:
	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET sockArray[WSA_MAXIMUM_WAIT_EVENTS];

	int m_nEventTotal;
	USHORT m_nPort;
	WSADATA m_ws;

	SOCKET sListen;
	sockaddr_in sin;

	WSANETWORKEVENTS event1;

	//vector <ClientInfo*> clients;
	struct client
	{
		string id;
		string ip;
		string port;
	};
	vector<client>clients;

	void dealEvent();
	void accept(int i);
	void read(int i);
	void close(int i);
	void write(int i);

	int msgNumber;
	int m_id;
	void updateList();
	void sendlisttoclient();
};

