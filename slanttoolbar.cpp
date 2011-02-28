// SlantToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "SlantToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MOUSELEAVE			101
#define FLASHTIMER			102

/////////////////////////////////////////////////////////////////////////////
// CSlantToolbar

CSlantToolbar::CSlantToolbar()
{
	m_uTimerId = 0;
	m_uFlashTimerId = 0;
	m_bParentEcho = FALSE;
	m_bAutoColor = TRUE;
}

CSlantToolbar::~CSlantToolbar()
{
}


BEGIN_MESSAGE_MAP(CSlantToolbar, CStatic)
	//{{AFX_MSG_MAP(CSlantToolbar)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlantToolbar message handlers

BOOL CSlantToolbar::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CSlantToolbar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	RECT rect, win;
	GetWindowRect( &win );
	GetClientRect( &rect );

	// Set window offset
	m_tb.SetScreenPos( win.left, win.top );

	// Attempt to create offscreen buffer
	if ( m_os.Create( dc.GetSafeHdc(), &rect ) )
	{
		// Draw to offscreen
		m_tb.Draw( GetSafeHwnd(), m_os, m_os );

		// Draw from offscreen
		m_os.Draw( dc.GetSafeHdc(), &rect );

	} // end if

	// Draw the toolbar
	else m_tb.Draw( GetSafeHwnd(), dc.GetSafeHdc(), &rect );
}

void CSlantToolbar::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( m_tb.OnMouseMove( nFlags, &point ) )
		RedrawWindow();
	
	// Do we want to echo to parent?
	if ( m_bParentEcho && m_tb.GetMouseOver() == NULL && m_tb.GetClicked() == NULL )
	{
		HWND hWnd = ::GetParent( GetSafeHwnd() );
		if ( ::IsWindow( hWnd ) )
		{	POINT pt; pt.x = point.x; pt.y = point.y;
			ClientToScreen( &point );
			::ScreenToClient( hWnd, &point );
			::SendMessage( hWnd, WM_MOUSEMOVE, (WPARAM)nFlags, MAKELONG( pt.x, pt.y ) );
		} // end if
	} // end if

	// Watch for mouse to leave
	else if ( m_uTimerId == 0 )
		m_uTimerId = SetTimer( MOUSELEAVE, 250, NULL );

	CStatic::OnMouseMove(nFlags, point);
}

void CSlantToolbar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if ( m_tb.OnLButtonDown( nFlags, &point ) )
		RedrawWindow();

	// Do we want to echo to parent?
	if ( m_bParentEcho && m_tb.GetMouseOver() == NULL && m_tb.GetClicked() == NULL )
	{
		HWND hWnd = ::GetParent( GetSafeHwnd() );
		if ( ::IsWindow( hWnd ) )
		{	POINT pt; pt.x = point.x; pt.y = point.y;
			ClientToScreen( &point );
			::ScreenToClient( hWnd, &point );
			::SendMessage( hWnd, WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELONG( pt.x, pt.y ) );
		} // end if
	} // end if
	
	CWnd ::OnLButtonDblClk(nFlags, point);
}

void CSlantToolbar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( m_tb.OnLButtonDown( nFlags, &point ) )
		RedrawWindow();

	// Do we want to echo to parent?
	if ( m_bParentEcho && m_tb.GetMouseOver() == NULL && m_tb.GetClicked() == NULL )
	{
		HWND hWnd = ::GetParent( GetSafeHwnd() );
		if ( ::IsWindow( hWnd ) )
		{	POINT pt; pt.x = point.x; pt.y = point.y;
			ClientToScreen( &point );
			::ScreenToClient( hWnd, &point );
			::SendMessage( hWnd, WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELONG( pt.x, pt.y ) );
		} // end if
	} // end if
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CSlantToolbar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( m_tb.OnLButtonUp( nFlags, &point ) )
	{	if ( !::IsWindow( GetSafeHwnd() ) ) return;
		RedrawWindow();
	} // en dif
	
	// Do we want to echo to parent?
	if ( m_bParentEcho && m_tb.GetMouseOver() == NULL && m_tb.GetClicked() == NULL )
	{
		HWND hWnd = ::GetParent( GetSafeHwnd() );
		if ( ::IsWindow( hWnd ) )
		{	POINT pt; pt.x = point.x; pt.y = point.y;
			ClientToScreen( &point );
			::ScreenToClient( hWnd, &point );
			::SendMessage( hWnd, WM_LBUTTONUP, (WPARAM)nFlags, MAKELONG( pt.x, pt.y ) );
		} // end if
	} // end if

	CStatic::OnLButtonUp(nFlags, point);
}

void CSlantToolbar::OnTimer(UINT_PTR nIDEvent) 
{
	if ( nIDEvent == MOUSELEAVE )
	{
		POINT pt;
		RECT rect;
		GetCursorPos( &pt );
		GetWindowRect( &rect );
		if ( !PtInRect( &rect, pt ) )
		{	OnNotifyMouseLeave( 0, 0L );
			if ( m_uTimerId != 0 )
			{	KillTimer( m_uTimerId );
				m_uTimerId = 0;
			} // end if
		} // end if

	} // end if

	else if ( nIDEvent == FLASHTIMER )
		if ( m_tb.NeedFlash() )
			RedrawWindow();
/*		{	RECT rect; GetClientRect( &rect );
			HDC hDc = ::GetDC( GetSafeHwnd() );
			if ( hDc )
			{	m_tb.DrawFlash( hDc, &rect );
				::ReleaseDC( GetSafeHwnd(), hDc );
			} // end if
		} // end if
*/	
	CStatic::OnTimer(nIDEvent);
}

LRESULT CSlantToolbar::OnNotifyMouseLeave(WPARAM wParam, LPARAM lParam)
{
	// Clear effects
	m_tb.ClearMouseOver();
	RedrawWindow();

	return 0;
}

void CSlantToolbar::OnSysColorChange() 
{
	CStatic::OnSysColorChange();

	// Are we tracking colors?
	if ( m_bAutoColor )
	{
		// Grab new system colors
		m_tb.SetColors( MAXDWORD, MAXDWORD, MAXDWORD, MAXDWORD, MAXDWORD );

		// Lose toolbar bacground
		m_tb.m_bck.Destroy();	

	} // end if
}

BOOL CSlantToolbar::CreateToolbar(CWnd *pParent)
{
	// Lose any current window
	if ( ::IsWindow( GetSafeHwnd() ) )
		DestroyWindow();

	// Create the toolbar
	RECT rectCtrl = { 0, 0, 0, 0 };
	CStatic::Create( "Toolbar", SS_NOTIFY | WS_CHILD | WS_VISIBLE, rectCtrl, pParent );

	// Must have a window
	if ( !::IsWindow( GetSafeHwnd() ) )
		return FALSE;

	// Set the flash timer
	m_uFlashTimerId = SetTimer( FLASHTIMER, 500, NULL );

	return TRUE;
}
