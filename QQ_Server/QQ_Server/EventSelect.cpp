#include "stdafx.h"
#include "EventSelect.h"
#include "QQ_ServerDlg.h"
#include <vector>
#include<sstream>
#include<iostream>
using namespace std;


//字符串分割函数
std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();

	for (int i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}
EventSelect::EventSelect()
{	
}


EventSelect::~EventSelect()
{
}

//定义变量
void EventSelect::define()
{
	m_nEventTotal = 0;	//注册的事件总数
	m_nPort = 8888; //此服务器监听的端口号
	msgNumber = 0; //消息总数
	m_id = 1; //socket编号
}


//创建和绑定Socket，并设置为监听状态
void EventSelect::initSocket()
{
	//初始化windows Sockets 环境
	WSAStartup(MAKEWORD(2, 2), &m_ws);
	//创建监听socket
	sListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//设置地址并绑定socket
	sin.sin_family = AF_INET;
	sin.sin_port = htons(m_nPort);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	::bind(sListen, (struct sockaddr *)&sin, sizeof(struct sockaddr));
	::listen(sListen, 10);
}

//创建事件对象，并注册网络事件
void EventSelect::initEvent()
{
	//创建事件对象，并关联到监听Socket，注册FD_ACCEPT和FD_CLOSE两个事件
	WSAEVENT myevent = ::WSACreateEvent();
	::WSAEventSelect(sListen, myevent, FD_ACCEPT | FD_CLOSE);
	//将新建的时间myevent保存到eventArray数组中
	eventArray[m_nEventTotal] = myevent;
	//将监听socket slisten 保存到 sockArray数组中
	sockArray[m_nEventTotal] = sListen;
	m_nEventTotal++;
}


//处理网络事件
void EventSelect::dealEvent()
{
	while (true)
	{
		//在所有时间对象上等待，只要有一个时间对象变为已授信状态，则函数返回，无限期等待
		int nIndex = ::WSAWaitForMultipleEvents(m_nEventTotal, eventArray, FALSE, WSA_INFINITE, FALSE);
		//发生的时间对象的索引，一般是句柄数组中最前面的那一个，然后再用循环依次处理后面的事件对象
		nIndex = nIndex - WSA_WAIT_EVENT_0;
		//对每个事件调用WSAWaitForMultipleEvents函数，以便确定它的状态
		for (int i = nIndex; i < m_nEventTotal; i++)
		{
			int ret;
			ret = :: WSAWaitForMultipleEvents(1, &eventArray[i], TRUE, 1000, FALSE);
			if (ret == WSA_WAIT_FAILED || ret == WSA_WAIT_TIMEOUT)
			{
				continue;
			}
			else
			{
				//获取到来的通知信息，WSAWaitForMultipleEvents函数会自动重置受信事件
				::WSAEnumNetworkEvents(sockArray[i], eventArray[i], &event1);//event1保存发生的网络事件记录和相关联的错误编码
				if (event1.lNetworkEvents & FD_ACCEPT)  //处理FD_ACCEPT事件
				{
					accept(i);
				}
				else if (event1.lNetworkEvents & FD_READ) //处理FD_READ事件
				{
					read(i);
				}
				else if (event1.lNetworkEvents & FD_CLOSE) //处理FD_CLOSE事件
				{
					close(i);
				}
				else if (event1.lNetworkEvents & FD_WRITE) //处理FD_WRITE事件
				{
					write(i);
				}
			}
		}
	}
}


void EventSelect::accept(int i)
{
	struct sockaddr_in from; //接受接入地址

	//如果处理FD_ACCEPT消息时没有错误
	if (event1.iErrorCode[FD_ACCEPT_BIT] == 0)
	{
		//连接太多，暂时不处理
		if (m_nEventTotal > WSA_MAXIMUM_WAIT_EVENTS)
		{
			cout << "超过最大连接数" << endl;
			return;
		}
		//接受连接请求，得到与客户端进行通信的socket snew
		int len = sizeof(from);
		SOCKET sNew = ::accept(sockArray[i], (struct sockaddr*)&from, &len);
		WSAEVENT newEvent = ::WSACreateEvent(); //为新套接字创建事件对象
		//将新建的事件对象newEvent关联到socket snew上
		//注册FD_READ|FD_CLOSE|FD_WRITE网络事件
		::WSAEventSelect(sNew, newEvent, FD_READ | FD_CLOSE | FD_WRITE);
		//将新建的事件newEvent保存到eventArray数组中 
		eventArray[m_nEventTotal] = newEvent;
		//将新建的socketsnew保存到sockarray数组中
		sockArray[m_nEventTotal] = sNew;

		client c;
		string ip = inet_ntoa(from.sin_addr);
		int port = from.sin_port;
		ostringstream os;
		string str_port;
		os << port;
		str_port = os.str();
		ostringstream oss;
		string str_id;
		oss << m_id;
		str_id = oss.str();
		c.id = str_id;
		c.ip = ip;
		c.port = str_port;
		clients.push_back(c);
		
		//显示当前客户连接成功
		CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST2);
		list->InsertItem(msgNumber, 0);
		//插入发送端
		list->SetItemText(msgNumber, 0, _T("0"));
		//插入接收端
		list->SetItemText(msgNumber, 1, _T("0"));
		CString msg;
		msg += "客户端";
		CString id;
		id.Format(_T("%d"), m_id);
		msg += id;
		msg += "连接到服务器";
		list->SetItemText(msgNumber, 2, msg);
		msgNumber++;
		//回显客户列表
		sendlisttoclient();
		//调试语句
		TRACE("This is a TRACE statement\n");
		TRACE("The value of ip is %d\n", from.sin_addr);
		TRACE("The value of port is %d\n", from.sin_port);

		m_nEventTotal++;
		m_id++;
		//更新当前连接窗口
		updateList();
	}
}


void EventSelect::read(int i)
{
	//如果处理FD_READ消息时没有错误
	if (event1.iErrorCode[FD_READ_BIT] == 0)
	{
		//接收消息  用户列表1|id|id|id|.....
		//聊天  0|1|2|msg
		vector<string> lines;
		char recvBuf[1024];
		int nrev = ::recv(sockArray[i], recvBuf, sizeof(recvBuf), 0);
		if (nrev > 0)
		{
			string s;
			s = recvBuf;
			lines = split(s, "|");
			//确定接收端socketarray
			char *p = new char[1024];
			int n1, n2, n3, n;
			CString str1, str2, str3, str;
			str1 = lines[0].c_str();
			str2 = lines[1].c_str();
			str3 = lines[2].c_str();
			str = lines[3].c_str();
			n1 = str1.GetLength();
			n2 = str2.GetLength();
			n3 = str3.GetLength();
			n = str.GetLength();
			int i;
			/*for (i = 0; i < n1; i++)
			{
				*(p + i) = str1.GetAt(i);
			}*/
			*p = str1.GetAt(0);
			*(p + 1) = '|';
			for (i = 0; i < n2; i++)
			{
				*(p + 2 + i) = str2.GetAt(i);
			}
			*(p + 2 + n2) = '|';
			for (i = 0; i < n3; i++)
			{
				*(p + 3 + n2 + i) = str3.GetAt(i);
			}
			*(p + 3 + n2 + n3) = '|';
			for (i = 0; i < n; i++)
			{
				*(p + n2 + n3 + 4 + i) = str.GetAt(i);
			}
			*(p + n2 + n3 + 4 + n) = '|';
			*(p + n2 + n3 + 5 + n) = '\0';
			if (lines[2] == "0")//群发
			{
				for (int j = 1; j < clients.size() + 1; j++)
				{
					send(sockArray[j], p, strlen(p), 0);
				}
			}
			else//转播
			{
				int n;
				for (int j = 0; j < clients.size(); j++)
				{
					if (lines[2] == clients[j].id)
					{
						n = j+1;
						break;
					}
				}
				send(sockArray[n], p, strlen(p), 0);
			}
		}
		CString str_send, str_recv, str;
		str_send = lines[1].c_str();
		str_recv = lines[2].c_str();
		str = lines[3].c_str();
		//显示已退出
		CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST2);
		list->InsertItem(msgNumber, 0);
		//插入发送端
		list->SetItemText(msgNumber, 0, str_send);
		//插入接收端
		list->SetItemText(msgNumber, 1, str_recv);
		list->SetItemText(msgNumber, 2, str);
		msgNumber++;
		lines.clear();
	}
	
}

void EventSelect::write(int i)
{
	//如果处理FD_WRITE消息时没有错误
	if (event1.iErrorCode[FD_WRITE_BIT] == 0)
	{
		//接收消息
		char sendBuf[256] = "成功连接服务器";
	}
}

void EventSelect::close(int i)
{
	//关闭socket，删除数组中对应的记录
	
	if (event1.iErrorCode[FD_CLOSE_BIT] != 0)
	{
		//显示已退出
		CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST2);
		list->InsertItem(msgNumber, 0);
		//插入发送端
		list->SetItemText(msgNumber, 0, _T("0"));
		//插入接收端
		list->SetItemText(msgNumber, 1, _T("0"));
		CString msg;
		msg += "客户端";
		CString id;
		//id.Format(_T("%d"), clients[i-1].id);
		id = clients[i - 1].id.c_str();
		msg += id;
		msg += "退出服务器";
		list->SetItemText(msgNumber, 2, msg);
		msgNumber++;

		//删除客户端信息
		vector<client>::iterator it = clients.begin() + i - 1;
		clients.erase(clients.begin() + i - 1);
		//回显客户列表
		//sendlisttoclient();
		int k, j;
		int sum = 0;
		int str_id_n;
		CString str_id;
		char p[1024];
		memset(p, sizeof(p), '\0');
		p[0] = '1';
		p[1] = '|';
		for (k = 0; k < clients.size(); k++)
		{
			str_id = clients[k].id.c_str();
			str_id_n = str_id.GetLength();

			for (j = 0; j < str_id_n; j++)
			{
				p[2 + sum + j] = str_id.GetAt(j);
			}
			p[2 + sum + str_id_n] = '|';
			sum += str_id_n + 1;
		}
		p[2 + sum] = '\0';
		//测试
		//list->InsertItem(msgNumber, 0);
		////插入发送端
		//list->SetItemText(msgNumber, 0, _T("0"));
		////插入接收端
		//list->SetItemText(msgNumber, 1, _T("0"));
		//string s = p;
		//msg= s.c_str();
		//list->SetItemText(msgNumber, 2, msg);
		//msgNumber++;

		int len = clients.size();
		for (k = 1; k < len + 2; k++)
		{
			send(sockArray[k], p, strlen(p), 0);
		}
		//delete p;
		//更新列表
		updateList();
		::closesocket(sockArray[i]);
		for (int j = i; j < m_nEventTotal -1; j++)
		{
			sockArray[j] = sockArray[j + 1];
			eventArray[j] = eventArray[j + 1];
		}
		m_nEventTotal--;
	}
}

void EventSelect::updateList()
{
	CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST1);
	list->DeleteAllItems(); // 全部清空  
	for (int i = 0; i < clients.size(); i++)
	{
		CString str_id,str_ip,str_port;
		str_id = clients[i].id.c_str();
		str_ip = clients[i].ip.c_str();
		str_port = clients[i].port.c_str();
		list->InsertItem(i, 0);
		//插入编号
		list->SetItemText(i, 0, str_id);
		//插入ip
		list->SetItemText(i, 1, str_ip);
		//插入port
		list->SetItemText(i, 2, str_port);
	}
}

//把当前用户信息发送给客户端1|编号|编号|。。。。|
void EventSelect::sendlisttoclient()
{
	int k, j;
	int sum = 0;
	int str_id_n;
	CString str_id;
	char *p = new char[1024];
	*p = '1';
	*(p + 1) = '|';
	for (k = 0; k < clients.size(); k++)
	{
		str_id = clients[k].id.c_str();
		str_id_n = str_id.GetLength();

		for (j = 0; j < str_id_n; j++)
		{
			*(p + 2 + sum + j) = str_id.GetAt(j);
		}
		*(p + 2 + sum + str_id_n) = '|';
		sum += str_id_n + 1;
	}
	*(p + 2 + sum) = '\0';
	int len = clients.size();
	for (k = 1; k < len + 1; k++)
	{
		send(sockArray[k], p, strlen(p), 0);
	}
}