
// QQ_ServerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "EventSelect.h"
#include "Resource.h"


// CQQ_ServerDlg 对话框
class CQQ_ServerDlg : public CDialogEx
{
// 构造
public:
	CQQ_ServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_QQ_SERVER_DIALOG };

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
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_list1;
	CListCtrl m_list2;
	CEdit m_edit;
	CString m_string;
	CWinThread *pServerThread;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
};
