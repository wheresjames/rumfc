// PageFrame.cpp : implementation file
//

#include "stdafx.h"
#include "PageFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageFrame

CPageFrame::CPageFrame()
{
	m_pData = NULL;
	m_pagelist.SetParent( this );
}

CPageFrame::~CPageFrame()
{
}


BEGIN_MESSAGE_MAP(CPageFrame, CStatic)
	//{{AFX_MSG_MAP(CPageFrame)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_GETDATA, OnGetData )
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPageFrame message handlers

void CPageFrame::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);

	m_pagelist.SizePage();	
}

BOOL CPageFrame::OnEraseBkgnd(CDC* pDC) 
{
	if ( pDC == NULL ) return TRUE;

	// Ensure the clip children style is set
	DWORD dwStyle = GetWindowLong( GetSafeHwnd(), GWL_STYLE );
	if ( ( dwStyle & WS_CLIPCHILDREN ) != WS_CLIPCHILDREN )
	{	dwStyle |= WS_CLIPCHILDREN;
		SetWindowLong( GetSafeHwnd(), GWL_STYLE, dwStyle );
	} // end if

	RECT rect;
	GetClientRect( &rect );
	CGrDC::FillSolidRect( pDC->GetSafeHdc(), &rect, GetSysColor( COLOR_3DFACE ) );

	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CPageFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
}

BOOL CPageFrame::PreCreateWindow(CREATESTRUCT& cs) 
{	cs.style |= WS_CLIPCHILDREN;	
	return CStatic::PreCreateWindow(cs);
}

void* CPageFrame::GetData( HWND hPage )
{		
	void *ptr = NULL;

	// Get the the page frame
	HWND hWnd = ::GetParent( hPage );

	// Send message
	if ( ::IsWindow( hWnd ) )
	{	ptr = (void*)::SendMessage( hWnd, WM_PG_GETDATA, 0, 0 );
		if ( (DWORD)ptr < 256 ) ptr = NULL;
	} // end if

	return ptr;
}

LRESULT CPageFrame::OnGetData(WPARAM wParam, LPARAM lParam)
{	
	return (LRESULT)m_pData;
}

void CPageFrame::OnTimer(UINT_PTR nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CStatic::OnTimer(nIDEvent);
}
