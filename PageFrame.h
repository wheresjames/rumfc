#if !defined(AFX_PAGEFRAME_H__36037D44_C9CB_410D_BFB1_46FD36ED8BEA__INCLUDED_)
#define AFX_PAGEFRAME_H__36037D44_C9CB_410D_BFB1_46FD36ED8BEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageFrame.h : header file
//

#include "PageList.h"

/////////////////////////////////////////////////////////////////////////////
// CPageFrame window

class CPageFrame : public CStatic
{
// Construction
public:
	CPageFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPageFrame();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPageFrame)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	void SetData( void *ptr ) { m_pData = ptr; }
	static void* GetData( HWND hPage );
	LRESULT OnGetData( WPARAM wParam, LPARAM lParam );


	LPPAGEINFO Add( CDialog *pdlg, DWORD idd, LPCTSTR pName, LPCTSTR pHelp )
	{	return m_pagelist.Add( pdlg, idd, pName, pHelp ); }

	BOOL SetPage( DWORD pg ) { return m_pagelist.SetPage( pg ); }

	CPageList& GetPageList() { return m_pagelist; }

	HRESULT Refresh() { return m_pagelist.Refresh(); }

	void SizePages( BOOL b ) { m_pagelist.SizePages( b ); }

private:

	void			*m_pData;
	CPageList		m_pagelist;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEFRAME_H__36037D44_C9CB_410D_BFB1_46FD36ED8BEA__INCLUDED_)
