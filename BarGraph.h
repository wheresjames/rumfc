#if !defined(AFX_BARGRAPH_H__097E1E56_FB55_406F_B104_33EE2703B4A5__INCLUDED_)
#define AFX_BARGRAPH_H__097E1E56_FB55_406F_B104_33EE2703B4A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BarGraph.h : header file
//

#include "DrawGraph.h"

/////////////////////////////////////////////////////////////////////////////
// CBarGraph window

class CBarGraph : public CStatic
{
// Construction
public:
	CBarGraph();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBarGraph)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBarGraph();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBarGraph)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:

	BOOL Tick() { return m_graph.Tick(); }
	BOOL Add(LPCTSTR pName, DWORD dwTime, DWORD dwValue, COLORREF rgbLine, HICON hIcon)
	{	return m_graph.Add( pName, dwTime, dwValue, rgbLine, hIcon ); }

	CDrawGraph* GetGraph() { return &m_graph; }

	void Reset() { m_graph.Destroy(); }

private:

	CWinDc				m_os;
	CDrawGraph			m_graph;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BARGRAPH_H__097E1E56_FB55_406F_B104_33EE2703B4A5__INCLUDED_)
