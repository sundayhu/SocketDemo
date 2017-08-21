
// QQ_ClientDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxcmn.h"
#include <vector>
#include<iostream>
using namespace std;

struct threadInfo{
	SOCKET sClient;
	//string m_id;
	sockaddr_in addrRemote;
};

UINT ThreadFunc(LPVOID lpParam);
// CQQ_ClientDlg 对话框
class CQQ_ClientDlg : public CDialogEx
{
// 构造
public:
	CQQ_ClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_QQ_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public :
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton4();
	CEdit m_edit;
	CWinThread *pClientThread;
	void connectToServer();
	void closeSocket();
	CListCtrl m_list1;
	CListCtrl m_list2;
	CString m_string;
	void updatelist();
	//vector<string>m_ids;//好友列表
	//string m_id;//
	afx_msg void OnEnChangeEdit3();
};

