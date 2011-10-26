#if !defined(AFX_SLANTTOOLBAR_H__5124740B_4468_4351_BA47_84D245DBA232__INCLUDED_)
#define AFX_SLANTTOOLBAR_H__5124740B_4468_4351_BA47_84D245DBA232__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SlantToolbar.h : header file
//

#include "DrawSlantToolbar.h"

/////////////////////////////////////////////////////////////////////////////
// CSlantToolbar window


class CSlantToolbar : public CStatic
{
// Construction
public:
	CSlantToolbar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlantToolbar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSlantToolbar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSlantToolbar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysColorChange();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:

	void SetAutoColor( BOOL b ) { m_bAutoColor = b; }

	void SetColors( COLORREF rgbBck, COLORREF rgbLtBorder, COLORREF rgbDkBorder, COLORREF rgbText, COLORREF rgbSelection )
	{	m_bAutoColor = FALSE; m_tb.SetColors( rgbBck, rgbLtBorder, rgbDkBorder, rgbText, rgbSelection ); }

	BOOL CreateToolbar( CWnd *pParent );
	LRESULT OnNotifyMouseLeave( WPARAM wParam, LPARAM lParam );

	BOOL Add( UINT uCmd, HICON hIcon, LPCTSTR pText, LPCTSTR pTip, LPBOOL pBool = NULL, HICON hPressed = NULL, DWORD flags = 0, HICON hFlash = NULL )
	{	return m_tb.Add( uCmd, hIcon, pText, pTip, pBool, hPressed, flags, hFlash ); }

	void SetMessageTarget( HWND hWnd, UINT uMsgCmd, UINT uMsgUpdate )
	{	m_tb.SetMessageTarget( hWnd, uMsgCmd, uMsgUpdate ); }

	void TextArea( BOOL b, LPCTSTR pTitle = NULL ) { m_tb.TextArea( b, pTitle ); }
	void SetMinTextArea( long min ) { m_tb.SetMinTextArea( min ); }
	void SetTitle( LPCTSTR pTitle ) 
	{ m_tb.SetTitle( pTitle ); if ( ::IsWindow( GetSafeHwnd() ) ) RedrawWindow(); }

	BOOL SetTextByCmd( UINT cmd, LPCTSTR text )
	{	return m_tb.SetTextByCmd( cmd, text ); }

	void ParentEcho( BOOL b ) { m_bParentEcho = b; }

	void HiLight( BOOL b ) { m_tb.HiLight( b ); }
	void HiLightBtn( BOOL b ) { m_tb.HiLightBtn( b ); }

	void SetButtonSlant( long s ) { m_tb.SetButtonSlant( s ); }
	long GetButtonSlant() { return m_tb.GetButtonSlant(); }

	void ForceTooltips( BOOL b ) { m_tb.ForceTooltips( b ); }

	void EmptyMenu() { m_tb.Destroy(); m_tb.DestoryTooltips(); }

	BOOL NeedFlash() { return m_tb.NeedFlash(); }
	
	CDrawSlantToolbar& Toolbar() { return m_tb; }

private:

	CWinDc				m_os;
	CDrawSlantToolbar	m_tb;
	UINT				m_uTimerId;
	BOOL				m_bParentEcho;
	UINT				m_uFlashTimerId;
	BOOL				m_bAutoColor;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLANTTOOLBAR_H__5124740B_4468_4351_BA47_84D245DBA232__INCLUDED_)
