
// QQ_ClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "QQ_Client.h"
#include "QQ_ClientDlg.h"
#include "afxdialogex.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CQQ_ClientDlg 对话框

threadInfo info;
int m_msgNumber = 0;
string m_id="1";
//void updatelist();
vector<string>m_ids;//好友列表
int flag = false;
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
CQQ_ClientDlg::CQQ_ClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQQ_ClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQQ_ClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_LIST1, m_list2);
	DDX_Control(pDX, IDC_LIST2, m_list1);
	DDX_Text(pDX, IDC_EDIT1, m_string);
}

BEGIN_MESSAGE_MAP(CQQ_ClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CQQ_ClientDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON4, &CQQ_ClientDlg::OnBnClickedButton4)
END_MESSAGE_MAP()

//广播接收线程
UINT ThreadFunc(LPVOID lpParam)
{
	threadInfo* info = (threadInfo*)lpParam;

	while (true)
	{
		//接收数据
		vector<string> lines;
		char recvBuf[1024];
		memset(recvBuf, sizeof(recvBuf), '\0');  //清空缓冲区

		int nRecv = ::recv(info->sClient, recvBuf, sizeof(recvBuf), 0);
		if (nRecv == SOCKET_ERROR)
		{
			//cout << "recv Error!" << endl;
			break;
		}
		if (nRecv > 0)
		{
			string str;
			str = recvBuf;
			//ostringstream os;
			//os << nRecv;
			//str = os.str();
			////测试
			//CString cs;
			//cs = str.c_str();
			//CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST2);
			//list->DeleteAllItems(); // 全部清空 
			//list->InsertItem(0, 0);
			////插入编号
			//list->SetItemText(0, 0, cs);
			////插入在线
			//list->SetItemText(0, 1, _T("在线"));

			lines = split(str, "|");
			if (lines[0] == "1")//用户列表信息
			{
				if (flag == false)
				{
					CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST2);
					list->DeleteAllItems(); // 全部清空  
					for (int i = 1; i < nRecv/2; i++)
					{
						CString str_id;
						str_id = lines[i].c_str();
						list->InsertItem(i-1, 0);
						//插入编号
						list->SetItemText(i-1, 0, str_id);
						//插入在线
						list->SetItemText(i-1, 1, _T("在线"));
					}
					m_id = lines[lines.size() - 2];
					flag = true;
				}
				else
				{
					
					CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST2);
					list->DeleteAllItems(); // 全部清空
					int len = lines.size();
					for (int i = 1; i < nRecv/2; i++)
					{
						CString str_id;
						str_id = lines[i].c_str();
						list->InsertItem(i-1, 0);
						//插入编号
						list->SetItemText(i-1, 0, str_id);
						//插入在线
						list->SetItemText(i-1, 1, _T("在线"));
					}
				}
			}
			else//聊天消息
			{
				if (lines[1] != m_id)
				{
					CString send_id, recv_id, msg;
					//TRACE(lines[);
					send_id = lines[1].c_str();//.....
					recv_id = lines[2].c_str();
					msg = lines[3].c_str();
					//界面更新
					CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST1);
					list->InsertItem(m_msgNumber, 0);
					//插入发送端
					list->SetItemText(m_msgNumber, 0, send_id);
					//插入接收端
					list->SetItemText(m_msgNumber, 1, recv_id);
					//插入消息
					list->SetItemText(m_msgNumber, 2, msg);
					m_msgNumber++;
				}
			}		
		}
		lines.clear();
	}
	
	return 0;
}

// CQQ_ClientDlg 消息处理程序

BOOL CQQ_ClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	
	LONG lStyle;
	lStyle = GetWindowLong(m_list1.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_list1.m_hWnd, GWL_STYLE, lStyle);//设置style
	SetWindowLong(m_list2.m_hWnd, GWL_STYLE, lStyle);//设置style

	DWORD dwStyle = m_list1.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	//dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
	m_list1.SetExtendedStyle(dwStyle); //设置扩展风格
	m_list2.SetExtendedStyle(dwStyle); //设置扩展风格

	m_list2.InsertColumn(0, _T("发送端编号"), LVCFMT_LEFT, 80);//插入列，将宽度设置成0，在外观上就看不到此列，用来存放会员号
	m_list2.InsertColumn(1, _T("接受端编号"), LVCFMT_LEFT, 80);
	m_list2.InsertColumn(2, _T("消息"), LVCFMT_LEFT, 200);

	m_list1.InsertColumn(0, _T("编号"), LVCFMT_LEFT, 100);//插入列，将宽度设置成0，在外观上就看不到此列，用来存放会员号
	m_list1.InsertColumn(1, _T("在线信息"), LVCFMT_LEFT, 100);

	//连接到服务器
	connectToServer();
	pClientThread = AfxBeginThread(ThreadFunc, &info);
	//默认选择服务器
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);//选上
	((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);//  不选上


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CQQ_ClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CQQ_ClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CQQ_ClientDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CQQ_ClientDlg::connectToServer()
{
	//初始化WS2_32.dll
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		//cout << "初始化失败" << endl;
		return;
	}

	info.sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (info.sClient == INVALID_SOCKET)
	{
		//printf("Failed socket()\n");
		return;
	}
	//要连接的服务器地址
	info.addrRemote.sin_family = AF_INET;
	info.addrRemote.sin_port = htons(8888);
	info.addrRemote.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if (connect(info.sClient, (sockaddr *)&info.addrRemote, sizeof(info.addrRemote)) == SOCKET_ERROR)
	{
		printf("Failed connect()\n");
		return;
	}
}

void CQQ_ClientDlg::closeSocket()
{
	closesocket(info.sClient);
}
//发送消息
void CQQ_ClientDlg::OnBnClickedButton4()
{
	//提取editctrl的文本
	CString str;//与发送的文本
	GetDlgItem(IDC_EDIT1)->GetWindowText(str);
	int str_n = str.GetLength();
	CString str_m_id;
	str_m_id = m_id.c_str();
	int m_id_n = str_m_id.GetLength();
	int i;
	char *p = new char[1024];

	//如果选中客户端 0|发送端|接收端|消息
	if (((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck() == 1)
	{
		*p = '0';
		*(p + 1) = '|';
		for (i = 0; i < m_id_n; i++)
		{
			*(p + 2 + i) = str_m_id.GetAt(i);
		}
		//*(p + 2) = '1';
		*(p + 2 + m_id_n) = '|';
		CString str_id;
		GetDlgItem(IDC_EDIT2)->GetWindowText(str_id);
		int str_id_n = str_id.GetLength();
		for (i = 0; i < str_id_n; i++)
		{
			*(p + 3 + m_id_n + i) = str_id.GetAt(i);
		}
		*(p + 3 + m_id_n + str_id_n) = '|';
		for (i = 0; i < str_n; i++)
		{
			*(p + 4 + m_id_n + str_id_n + i) = str.GetAt(i);
		}
		*(p + 4 + m_id_n + str_id_n + str_n) = '|';
		*(p + 5 + m_id_n + str_id_n + str_n) = '\0';
		//发送到服务器
		send(info.sClient, p, strlen(p), 0);
		//界面更新
		CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST1);
		list->InsertItem(m_msgNumber, 0);
		//插入发送端
		list->SetItemText(m_msgNumber, 0, str_m_id);
		//插入接收端
		list->SetItemText(m_msgNumber, 1, str_id);
		//插入消息
		list->SetItemText(m_msgNumber, 2, str);
		delete p;
		m_msgNumber++;
	}
	//选择的是服务器
	else if (((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck() == 1)
	{
		*p = '0';
		*(p + 1) = '|';
		for (i = 0; i < m_id_n; i++)
		{
			*(p + 2 + i) = str_m_id.GetAt(i);
		}
		//*(p + 2) = '1';
		*(p + 2 + m_id_n) = '|';
		*(p + 3 + m_id_n) = '0';
		*(p + 4 + m_id_n) = '|';
		for (i = 0; i < str_n; i++)
		{
			*(p + 5 + m_id_n + i) = str.GetAt(i);
		}
		*(p + 5 + m_id_n + str_n) = '|';
		*(p + 6 + m_id_n + str_n) = '\0';
		//发送到服务器
		send(info.sClient, p, strlen(p), 0);
		//界面更新
		CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST1);
		list->InsertItem(m_msgNumber, 0);
		//插入发送端
		list->SetItemText(m_msgNumber, 0, str_m_id);
		//插入接收端
		list->SetItemText(m_msgNumber, 1, _T("0"));
		//插入消息
		list->SetItemText(m_msgNumber, 2, str);
		delete p;
		m_msgNumber++;
	}
}

//更新用户列表窗口
void CQQ_ClientDlg::updatelist()
{
    CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST2);
	list->DeleteAllItems(); // 全部清空  
	for (int i = 0; i < m_ids.size(); i++)
	{
		CString str_id;
		str_id = m_ids[i].c_str();
		list->InsertItem(i, 0);
		//插入编号
		list->SetItemText(i, 0, str_id);
		//插入在线
		list->SetItemText(i, 1, _T("在线"));
	}
}