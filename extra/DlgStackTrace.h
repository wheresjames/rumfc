#if !defined(AFX_DLGSTACKTRACE_H__6EE018C9_21D5_4460_A440_5F6593E5A194__INCLUDED_)
#define AFX_DLGSTACKTRACE_H__6EE018C9_21D5_4460_A440_5F6593E5A194__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgStackTrace.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgStackTrace dialog

class CDlgStackTrace : public CDialog
{
// Construction
public:
	void Sort( DWORD dwItem, BOOL bAscending );
	BOOL InitColumns();
	BOOL Capture();
	BOOL FillCombo( CReg *pReg );
	BOOL Refresh();
	BOOL FillList( CReg *pReg );
	void Size();
	CDlgStackTrace(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgStackTrace)
	enum { IDD = IDD_STACK_TRACE };
	CListCtrl	m_list;
	CComboBox	m_comboThread;
	CTabCtrl	m_tabSource;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgStackTrace)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgStackTrace)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnRefresh();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeThread();
	afx_msg void OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	
	CImageList				m_images;

	CReg					m_regCallstack;
	CReg					m_regHistory;
	CReg					m_regProfile;

	BOOL					m_bAscending;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSTACKTRACE_H__6EE018C9_21D5_4460_A440_5F6593E5A194__INCLUDED_)
