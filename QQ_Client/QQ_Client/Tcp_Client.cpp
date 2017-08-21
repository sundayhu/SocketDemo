#include "stdafx.h"
#include "Tcp_Client.h"
#include "QQ_ClientDlg.h"
#include "resource.h"

Tcp_Client::Tcp_Client()
{
}


Tcp_Client::~Tcp_Client()
{
}

void Tcp_Client::connectToServer()
{
	//初始化WS2_32.dll
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		//cout << "初始化失败" << endl;
		return ;
	}

	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sClient == INVALID_SOCKET)
	{
		//printf("Failed socket()\n");
		return ;
	}
	//要连接的服务器地址
	sockaddr_in addrRemote;
	addrRemote.sin_family = AF_INET;
	addrRemote.sin_port = htons(8888);
	addrRemote.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if (connect(sClient, (sockaddr *)&addrRemote, sizeof(addrRemote)) == SOCKET_ERROR)
	{
		printf("Failed connect()\n");
		return ;
	}
}


void Tcp_Client::sendToGroup()
{
	CEdit* edit = (CEdit*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT1);

}