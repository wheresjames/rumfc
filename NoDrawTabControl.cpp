// NoDrawTabControl.cpp : implementation file
//

#include "stdafx.h"
#include "NoDrawTabControl.h"


// CNoDrawTabControl

IMPLEMENT_DYNAMIC(CNoDrawTabControl, CTabCtrl)

CNoDrawTabControl::CNoDrawTabControl()
{
    m_pRefresh = NULL;
}

CNoDrawTabControl::~CNoDrawTabControl()
{
}


BEGIN_MESSAGE_MAP(CNoDrawTabControl, CTabCtrl)
    ON_WM_ERASEBKGND()
//    ON_WM_PAINT()
END_MESSAGE_MAP()



// CNoDrawTabControl message handlers



BOOL CNoDrawTabControl::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
//    return CTabCtrl::OnEraseBkgnd(pDC);

/*    RECT rect;
    GetClientRect( &rect );
    AdjustRect( FALSE, &rect );
    ValidateRect( &rect );
*/

//    if ( m_pRefresh )
//        m_pRefresh->Invalidate();
//        m_pRefresh->RedrawWindow();

//    return TRUE;
}

void CNoDrawTabControl::OnPaint()
{
    CPaintDC dc(this); // device context for painting

//    if ( m_pRefresh )
//        m_pRefresh->Invalidate();

//        m_pRefresh->RedrawWindow();

/*    RECT rect;
    GetClientRect( &rect );
    AdjustRect( FALSE, &rect );
    ValidateRect( &rect );
*/
}
