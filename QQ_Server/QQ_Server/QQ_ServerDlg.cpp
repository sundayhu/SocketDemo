
// QQ_ServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "QQ_Server.h"
#include "QQ_ServerDlg.h"
#include "afxdialogex.h"
#include "EventSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CQQ_ServerDlg 对话框

EventSelect myserver;

CQQ_ServerDlg::CQQ_ServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQQ_ServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQQ_ServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Control(pDX, IDC_LIST2, m_list2);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Text(pDX, IDC_EDIT1, m_string);
}

BEGIN_MESSAGE_MAP(CQQ_ServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CQQ_ServerDlg::OnEnChangeEdit1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CQQ_ServerDlg::OnLvnItemchangedList2)
	ON_BN_CLICKED(IDC_BUTTON2, &CQQ_ServerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CQQ_ServerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CQQ_ServerDlg 消息处理程序

BOOL CQQ_ServerDlg::OnInitDialog()
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

	m_list1.InsertColumn(0, _T("编号"), LVCFMT_LEFT, 50);//插入列，将宽度设置成0，在外观上就看不到此列，用来存放会员号
	m_list1.InsertColumn(1, _T("IP"), LVCFMT_LEFT, 80);
	m_list1.InsertColumn(2, _T("端口"), LVCFMT_LEFT, 80);

	m_list2.InsertColumn(0, _T("发送端编号"), LVCFMT_LEFT, 100);//插入列，将宽度设置成0，在外观上就看不到此列，用来存放会员号
	m_list2.InsertColumn(1, _T("接收端编号"), LVCFMT_LEFT, 100);
	m_list2.InsertColumn(2, _T("消息"), LVCFMT_LEFT, 280);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CQQ_ServerDlg::OnPaint()
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
HCURSOR CQQ_ServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CQQ_ServerDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CQQ_ServerDlg::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}


UINT ThreadFunc(LPVOID lpParam)
{
	myserver.define();
	myserver.initSocket();
	myserver.initEvent();
	myserver.dealEvent();
	return 0;
}

void CQQ_ServerDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	pServerThread = AfxBeginThread(ThreadFunc, this);
}



//发送系统广播0|0|0|消息
void CQQ_ServerDlg::OnBnClickedButton1()
{
	//提取editctrl的文本
	CString str;
	GetDlgItem(IDC_EDIT1)->GetWindowText(str);
	int n = str.GetLength();
	int len = myserver.clients.size();
	int i;
	char *p = new char[1024];
	*p = '0';
	*(p + 1) = '|';
	*(p + 2) = '0';
	*(p + 3) = '|';
	*(p + 4) = '0';
	*(p + 5) = '|';
	for (i = 0; i < n; i++)
	{
		*(p + 6 + i) = str.GetAt(i);
	}
	*(p + 6 + n) = '|';
	*(p + 7 + n) = '\0';
	//发送到客户端	
	for (int i = 1; i < len+1; i++)
	{
		send(myserver.sockArray[i], p, strlen(p), 0);
	}

	//界面显示
	CListCtrl* list = (CListCtrl*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST2);
	list->InsertItem(myserver.msgNumber, 0);
	//插入发送端
	list->SetItemText(myserver.msgNumber, 0, _T("0"));
	//插入接收端
	list->SetItemText(myserver.msgNumber, 1, _T("0"));
	//插入消息
	list->SetItemText(myserver.msgNumber, 2, str);
	delete p;
	myserver.msgNumber++;

}

