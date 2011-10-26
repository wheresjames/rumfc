// BarGraph.cpp : implementation file
//

#include "stdafx.h"
#include "BarGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBarGraph

CBarGraph::CBarGraph()
{
}

CBarGraph::~CBarGraph()
{
}


BEGIN_MESSAGE_MAP(CBarGraph, CStatic)
	//{{AFX_MSG_MAP(CBarGraph)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarGraph message handlers

BOOL CBarGraph::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CBarGraph::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rect;
	GetClientRect( &rect );

	// Create offscreen buffer
	if ( m_os.Create( dc.GetSafeHdc(), &rect ) ) 
	{
		// Draw the graph
		m_graph.Draw( m_os, m_os );

		// Draw from the offscreen buffer
		m_os.Draw( dc.GetSafeHdc(), &rect );

	} // end if

	// Just skip the offscreen buffer
	else m_graph.Draw( dc.GetSafeHdc(), &rect );

}	


void CBarGraph::OnSysColorChange() 
{
	CStatic::OnSysColorChange();

	m_graph.m_bck.Destroy();
	
}
