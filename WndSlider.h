#if !defined(AFX_WNDSLIDER_H__513C3F68_CA1C_4755_ADD8_619B3101C674__INCLUDED_)
#define AFX_WNDSLIDER_H__513C3F68_CA1C_4755_ADD8_619B3101C674__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndSlider.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndSlider window

class CWndSlider : public CWnd, public CMessageList
{
// Construction
public:
	CWndSlider();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndSlider)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL CreateSlider( CWnd *pParent, LPRECT pRect, UINT id, LPCTSTR pTitle,  long *pVal,  long min, long max, long *pFb = NULL, long *pMax = NULL, long *pFbList = NULL, DWORD dwFbListSz = 0, long *pFbList2 = NULL, DWORD dwFbListSz2 = 0 );
	virtual ~CWndSlider();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndSlider)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL CalculateGroovePosition( LPRECT pRect, LPRECT pGroove, long *plButtonY );
	void DoClick( UINT nFlags, CPoint point );
	LRESULT OnMouseLeave( WPARAM wParam, LPARAM lParam );

	void SetBckColor( COLORREF rgb ) { m_rgbBck = rgb; }

	void ShowPosText( BOOL b ) { m_bShowPosText = b; }

	void SetSpring( long s ) 
	{	m_lSpring = s; if ( s != MAXLONG && m_plVal != NULL ) *m_plVal = s; }

	void SideText( BOOL b ) { m_bSideText = b; }

	void EchoMouseMove( BOOL b ) { m_bEchoMouseMove = b; }

private:

	CWinDc			m_os;
//	CWinDc			m_bck;

	long			*m_plVal;
	long			m_min;
	long			m_max;

	long			*m_plMax;
	long			*m_plFb;

	long			*m_plFbList;
	long			*m_plFbList2;

	long			m_lSpring;

	DWORD			m_dwFbListSz;
	DWORD			m_dwFbListSz2;

	BOOL			m_bValButtonDown;	 
	BOOL			m_bMaxButtonDown;	 

	BOOL			m_bShowPosText;
	BOOL			m_bSideText;

	POINT			m_ptClick;

	COLORREF		m_rgbBck;

	BOOL			m_bNotifyLeave;	

	long			m_lTextOffset;

	BOOL			m_bEchoMouseMove;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDSLIDER_H__513C3F68_CA1C_4755_ADD8_619B3101C674__INCLUDED_)
