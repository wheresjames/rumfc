// WndSlider.cpp : implementation file
//

#include "stdafx.h"
#include "WndSlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndSlider

CWndSlider::CWndSlider()
{
	m_min = 0;
	m_max = 100;
	m_plVal = NULL;
	m_plFb = NULL;
	m_dwFbListSz = 0;
	m_plFbList = NULL;
	m_dwFbListSz2 = 0;
	m_plFbList2 = NULL;
	m_bShowPosText = TRUE;
	m_bSideText = FALSE;
	m_lSpring = MAXLONG;

	m_bValButtonDown = FALSE;
	m_bMaxButtonDown = FALSE;

	m_bEchoMouseMove = FALSE;

	m_rgbBck = RGB( 100, 0, 0 );

	m_bNotifyLeave = FALSE;
	m_lTextOffset = 0;
}

CWndSlider::~CWndSlider()
{
}


BEGIN_MESSAGE_MAP(CWndSlider, CWnd)
	//{{AFX_MSG_MAP(CWndSlider)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_MOUSELEAVE, OnMouseLeave )

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndSlider message handlers

BOOL CWndSlider::CreateSlider(CWnd *pParent, LPRECT pRect, UINT id, LPCTSTR pTitle, long *pVal, long min, long max, long *pFb, long *pMax, long *pFbList, DWORD dwFbListSz, long *pFbList2, DWORD dwFbListSz2)
{
	// Attempt to create the window	
	if ( !CWnd::Create( NULL, pTitle, WS_VISIBLE | WS_CHILD, CRect( pRect ), pParent, 0 ) )
		return FALSE;

	m_bNotifyLeave = FALSE;
	m_plVal = pVal;
	m_min = min;
	m_max = max;
	m_plFb = pFb;
	m_plMax = pMax;
	m_plFbList = pFbList;
	m_dwFbListSz = dwFbListSz;
	m_plFbList2 = pFbList2;
	m_dwFbListSz2 = dwFbListSz2;

	return TRUE;
}

BOOL CWndSlider::OnEraseBkgnd(CDC* pDC) 
{
	RECT rect;
	GetClientRect( &rect );

	HDC hDC = pDC->GetSafeHdc();

	// Create dib section
	if ( m_os.Create( NULL, &rect ) ) hDC = m_os;

	CGrDC::VertGradientFill(	m_os, m_os, 
								CGrDC::ScaleColor( m_rgbBck, 80 ),
								CGrDC::ScaleColor( m_rgbBck, -40 ) );

//	CGrDC::VertGradientFill(	m_os, m_os, 
//								CGrDC::ScaleColor( m_rgbBck, 30 ),
//								CGrDC::ScaleColor( m_rgbBck, -30 ) );
	m_os.Aluminize( 5 );


	CGrDC::Box3d(	hDC, &rect, 1, 
					CGrDC::ScaleColor( m_rgbBck, 30 ),
					CGrDC::ScaleColor( m_rgbBck, -30 ),
					FALSE );

	// Don't let the background be totally black
	if ( m_rgbBck == RGB( 0, 0, 0 ) ) m_rgbBck = RGB( 10, 10, 10 );

	COLORREF rgbGroove, rgbSlider, rgbText;
	if ( CGrDC::IsLightColor( m_rgbBck ) )
	{	rgbGroove = CGrDC::ScaleColor( m_rgbBck, -100 );
		rgbSlider = CGrDC::ScaleColor( m_rgbBck, -100 );
		rgbText = CGrDC::ScaleColor( m_rgbBck, -200 );
	} // end if
	else
	{
		rgbGroove = RGB( 100, 100, 110 );
		rgbSlider = RGB( 180, 180, 200 );
		rgbText = RGB( 255, 255, 255 );

		//	rgbGroove = CGrDC::ScaleColor( m_rgbBck, 100 );
		//	rgbSlider = CGrDC::ScaleColor( m_rgbBck, 100 );
		//	rgbText = CGrDC::ScaleColor( m_rgbBck, 200 );
	} // end if

	char title[ CWF_STRSIZE ];
	GetWindowText( title, sizeof( title ) );

	CText txt;
	m_lTextOffset = 0; 

	if ( *title )
	{	
		txt.SetWeight( FW_BOLD );
		txt.SetColor( rgbText );
		txt.SetFont( 16, "Arial" );

		RECT trect;
		if ( m_bSideText ) 
		{	m_lTextOffset = 40; txt.SetFlags( DT_LEFT | DT_VCENTER | DT_SINGLELINE ); 
			SetRect( &trect, rect.left + 8, rect.top + 2, rect.right - 8, rect.bottom );
		} // end if
		else 
		{	txt.SetFlags( DT_LEFT | DT_SINGLELINE );
			SetRect( &trect, rect.left + 8, rect.top + 2, rect.right - 8, rect.top + 28 );
		} // end else

		OffsetRect( &trect, 1, 1 );
		if ( CGrDC::IsLightColor( rgbText ) ) txt.SetColor( CGrDC::ScaleColor( rgbText, -1500 ) );
		else txt.SetColor( RGB( 100, 100, 100 ) );
		txt.DrawText( hDC, title, &trect );

		OffsetRect( &trect, -1, -1 );
		txt.SetColor( rgbText );
		txt.DrawText( hDC, title, &trect );

	} // end if

	RECT groove; long lButtonY = 6;
	CalculateGroovePosition( &rect, &groove, &lButtonY );

	OffsetRect( &groove, 1, 1 );
	CGrDC::FillSolidRect( hDC, &groove, CGrDC::ScaleColor( rgbGroove, 200 ) );
	OffsetRect( &groove, -1, -1 );
	CGrDC::FillSolidRect( hDC, &groove, rgbGroove );

	if ( m_plVal != NULL && ( m_max - m_min ) != 0 )
	{
		long w = RW( groove );

		if ( *m_plVal < m_min ) *m_plVal = m_min;
		else if ( *m_plVal > m_max ) *m_plVal = m_max;
		if ( m_plMax != NULL && *m_plVal > *m_plMax ) *m_plVal = *m_plMax;
		long pos = *m_plVal * w / ( m_max - m_min );

//		RECT btn;
//		SetRect( &btn, groove.left + pos - 8, groove.top - 4, groove.left + pos + 8, groove.bottom + 4 );
//		CGrDC::FillSolidRect( hDC, &btn, CGrDC::ScaleColor( m_rgbBck, 100 ) );		
		
		RECT btn;
		SetRect( &btn, groove.left + pos - 8, groove.top - lButtonY, groove.left + pos, groove.bottom + lButtonY );
		CGrDC::HorzGradientFill( hDC, &btn, CGrDC::ScaleColor( rgbSlider, 40 ), CGrDC::ScaleColor( rgbSlider, 150 ) );
		OffsetRect( &btn, RW( btn ), 0 );
		CGrDC::HorzGradientFill( hDC, &btn, CGrDC::ScaleColor( rgbSlider, 150 ), CGrDC::ScaleColor( rgbSlider, 40 ) );

		char num[ 256 ];
		if ( m_plMax != NULL )
		{
			if ( *m_plMax < m_min ) *m_plMax = m_min;
			else if ( *m_plMax > m_max ) *m_plMax = m_max;
			if ( *m_plVal > *m_plMax ) *m_plMax = *m_plVal;
			long maxpos = *m_plMax * w / ( m_max - m_min );

			wsprintf( num, "%li - %li", (long)*m_plVal, (long)*m_plMax );

			RECT maxbtn;
			COLORREF rgbMax = RGB( 0, 0, 100 );
			SetRect( &maxbtn, groove.left + maxpos - 8, groove.top - lButtonY, groove.left + maxpos, groove.bottom + lButtonY );
			CGrDC::HorzGradientFill( hDC, &maxbtn, CGrDC::ScaleColor( rgbMax, 40 ), CGrDC::ScaleColor( rgbMax, 150 ) );
			OffsetRect( &maxbtn, RW( maxbtn ), 0 );
			CGrDC::HorzGradientFill( hDC, &maxbtn, CGrDC::ScaleColor( rgbMax, 150 ), CGrDC::ScaleColor( rgbMax, 40 ) );
		} // end if
		else wsprintf( num, "%li", (long)*m_plVal );

		if ( m_bShowPosText )
		{
			txt.SetFlags( DT_RIGHT );
			txt.SetWeight( FW_BOLD );
			txt.SetFont( 18, "Arial" );

			RECT trect;
			SetRect( &trect, rect.left + 8, rect.top + 2, rect.right - 8, rect.top + 28 );
			txt.DrawText( hDC, num, &trect );

		} // end if

		if ( m_plFb != NULL )
		{	
			long fbpos = ( *m_plFb * w ) / ( m_max - m_min );

			RECT fb;
			
			SetRect( &fb, groove.left, groove.top + lButtonY, groove.right, groove.bottom + lButtonY );
			CGrDC::FillSolidRect( hDC, &fb, RGB( 0, 50, 20 ) );

			SetRect( &fb, groove.left, groove.top + lButtonY, groove.left + fbpos, groove.bottom + lButtonY );
			CGrDC::FillSolidRect( hDC, &fb, RGB( 0, 200, 120 ) );
		} // end if

		else if ( m_plFbList != NULL && m_dwFbListSz )
		{
			RECT fb;
			
			SetRect( &fb, groove.left, groove.top + lButtonY + 4, groove.right, groove.bottom + lButtonY + 4 );
			CGrDC::FillSolidRect( hDC, &fb, RGB( 0, 50, 20 ) );

			for ( DWORD i = 0; i < m_dwFbListSz; i++ )
				if ( m_plFbList[ i ] != 0 )
				{	long fbval = m_plFbList[ i ];
					if ( fbval > m_max ) fbval = m_max; else if ( fbval < m_min ) fbval = m_min;
					long fbpos = ( fbval * w ) / ( m_max - m_min );
					SetRect( &fb, groove.left + fbpos - 3, groove.top + lButtonY, groove.left + fbpos + 3, groove.bottom + lButtonY );
					CGrDC::FillSolidRect( hDC, &fb, RGB( 0, 200, 120 ) );
				} // end for

			if ( m_plFbList2 != NULL && m_dwFbListSz2 )	
				for ( DWORD i = 0; i < m_dwFbListSz2; i++ )
					if ( m_plFbList2[ i ] != 0 )
					{	long fbval = m_plFbList2[ i ];
						if ( fbval > m_max ) fbval = m_max; else if ( fbval < m_min ) fbval = m_min;
						long fbpos = ( fbval * w ) / ( m_max - m_min );
						SetRect( &fb, groove.left + fbpos - 3, groove.top + lButtonY, groove.left + fbpos + 3, groove.bottom + lButtonY );
						CGrDC::FillSolidRect( hDC, &fb, RGB( 0, 120, 200 ) );
					} // end for

		} // end if

	} // end if	
	
	m_os.Draw( pDC->GetSafeHdc(), &rect );

	return TRUE;
//	return CWnd::OnEraseBkgnd(pDC);
}

void CWndSlider::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
}

void CWndSlider::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( m_bEchoMouseMove )
	{
		CWnd *pParent = GetParent();
		if ( pParent )
		{
			POINT pt = { point.x, point.y };
			ClientToScreen( &pt );
			pParent->ScreenToClient( &pt );		

			LONG lPt = MAKELONG( pt.x, pt.y );
			pParent->SendMessage( WM_MOUSEMOVE, nFlags, (LPARAM)lPt );

		} // end if
	} // end if

	if ( !m_bNotifyLeave )
    {	m_bNotifyLeave = TRUE;

        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof( tme );
        tme.hwndTrack = GetSafeHwnd();
        tme.dwFlags = TME_LEAVE;
        m_bNotifyLeave = TrackMouseEvent(&tme); 
    } // end if

	if ( ( nFlags & MK_LBUTTON ) == 0 )
	{	m_bValButtonDown = FALSE; m_bMaxButtonDown = FALSE; }
	else if ( !m_bValButtonDown && !m_bMaxButtonDown )
		DoClick( nFlags, point );
								
	if ( m_bValButtonDown == TRUE && m_plVal != NULL )
	{
		RECT rect;
		GetClientRect( &rect );

		RECT groove; long lButtonY = 6;
		CalculateGroovePosition( &rect, &groove, &lButtonY );

		long w = RW( groove );

		// Set new slider position
		*m_plVal = ( ( point.x - groove.left ) * m_max - m_min ) / w;
		if ( *m_plVal < m_min ) *m_plVal = m_min;
		else if ( *m_plVal > m_max ) *m_plVal = m_max;

		RedrawWindow();

		// Notify that value has changed
		MMessage( 0x11, *m_plVal );

	} // end if

	if ( m_bMaxButtonDown == TRUE && m_plMax != NULL )
	{
		RECT rect;
		GetClientRect( &rect );

		RECT groove; long lButtonY = 6;
		CalculateGroovePosition( &rect, &groove, &lButtonY );

		long w = RW( groove );

		// Set new slider position
		*m_plMax = ( ( point.x - groove.left ) * m_max - m_min ) / w;
		if ( *m_plMax < m_min ) *m_plMax = m_min;
		else if ( *m_plMax > m_max ) *m_plMax = m_max;

		RedrawWindow();

		// Notify that value has changed
		MMessage( 0x12, *m_plMax );

	} // end if

	CWnd::OnMouseMove(nFlags, point);
}

void CWndSlider::DoClick(UINT nFlags, CPoint point)
{
	if ( !m_bValButtonDown && m_plMax != NULL && ( m_max - m_min ) != 0 )
	{
		RECT rect;
		GetClientRect( &rect );

		RECT groove; long lButtonY = 6;
		CalculateGroovePosition( &rect, &groove, &lButtonY );

		long w = RW( groove );
		if ( *m_plMax < m_min ) *m_plMax = m_min;
		else if ( *m_plMax > m_max ) *m_plMax = m_max;
		long pos = groove.left + ( *m_plMax * w / ( m_max - m_min ) );

		// Is it on the slide button?
		if ( point.x > pos - 8 && point.x < pos + 8 &&
			 point.y > groove.top - lButtonY && point.y < groove.bottom + lButtonY )
		{	m_ptClick.x = point.x; m_ptClick.y = point.y;
			m_bMaxButtonDown = TRUE;
		} // end if

	} // end if
	else m_bMaxButtonDown = FALSE;

	if ( !m_bMaxButtonDown && m_plVal != NULL && ( m_max - m_min ) != 0 )
	{
		RECT rect;
		GetClientRect( &rect );

		RECT groove; long lButtonY = 6;
		CalculateGroovePosition( &rect, &groove, &lButtonY );

		long w = RW( groove );
		if ( *m_plVal < m_min ) *m_plVal = m_min;
		else if ( *m_plVal > m_max ) *m_plVal = m_max;
		long pos = groove.left + ( *m_plVal * w / ( m_max - m_min ) );

		// Is it on the slide button?
		if ( point.x > groove.left && point.x < groove.right &&
			 point.y > groove.top - lButtonY && point.y < groove.bottom + lButtonY )
		{	m_ptClick.x = point.x; m_ptClick.y = point.y;
			m_bValButtonDown = TRUE;
		} // end if

		if ( m_bValButtonDown == TRUE )
		{
			// Set new slider position
			*m_plVal = ( ( point.x - groove.left ) * m_max - m_min ) / w;
			if ( *m_plVal < m_min ) *m_plVal = m_min;
			else if ( *m_plVal > m_max ) *m_plVal = m_max;

			RedrawWindow();

		} // end if


	} // end if
	else m_bValButtonDown = FALSE;

}

void CWndSlider::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();

	DoClick( nFlags, point );

	CWnd::OnLButtonDown(nFlags, point);
}

void CWndSlider::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();

	m_bValButtonDown = FALSE;
	m_bMaxButtonDown = FALSE;

	if ( m_lSpring != MAXLONG && m_plVal != NULL )
	{
		// Set new slider position
		*m_plVal = m_lSpring;
		if ( *m_plVal < m_min ) *m_plVal = m_min;
		else if ( *m_plVal > m_max ) *m_plVal = m_max;

		RedrawWindow();

	} // end if

	// Notify that value has changed
	MMessage( 0x01, *m_plVal );

	CWnd::OnLButtonUp(nFlags, point);
}


/*
if (!m_bTracking)
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE|TME_HOVER;
        tme.dwHoverTime = 1;

        //Tell Windows that we want to process all mouse Hover and Leave Messages
        m_bTracking = _TrackMouseEvent(&tme); 
        m_point = point;
    }
*/


LRESULT CWndSlider::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{	  
	m_bNotifyLeave = FALSE;

	m_bValButtonDown = FALSE;
	m_bMaxButtonDown = FALSE;

	if ( m_lSpring != MAXLONG && m_plVal != NULL )
	{
		// Set new slider position
		*m_plVal = m_lSpring;
		if ( *m_plVal < m_min ) *m_plVal = m_min;
		else if ( *m_plVal > m_max ) *m_plVal = m_max;

		RedrawWindow();

	} // end if

	return 0;
}


BOOL CWndSlider::CalculateGroovePosition(LPRECT pRect, LPRECT pGroove, long *plButtonY )
{
	if ( pRect == NULL || pGroove == NULL ) return FALSE;

	char title[ CWF_STRSIZE ];
	GetWindowText( title, sizeof( title ) );

	if ( *title )
		SetRect( pGroove, pRect->left + m_lTextOffset + 20, pRect->bottom - 10, pRect->right - 20, pRect->bottom - 12 );
	else
	{	long lMiddle = pRect->top + ( PRH( pRect ) >> 1 );
		SetRect( pGroove, pRect->left + 20, pRect->top + lMiddle - 1, pRect->right - 20, pRect->top + lMiddle + 1 );
	} // end else

	// Height of the button = lButtonY * 2;
	long lButtonY = 6;
	if ( PRH( pRect ) < ( lButtonY * 2 ) )
		lButtonY = ( PRH( pRect ) >> 1 ) - 2;

	if ( plButtonY ) *plButtonY = lButtonY;

	return TRUE;
}
