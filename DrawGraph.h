// DrawGraph.h: interface for the CDrawGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWGRAPH_H__44CF80DF_0FC5_40C4_981E_7EC425B15F5C__INCLUDED_)
#define AFX_DRAWGRAPH_H__44CF80DF_0FC5_40C4_981E_7EC425B15F5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagGRAPHELEMENTINFO : LLISTINFO
{
	DWORD			type;

	// Graph
	DWORD			pts;
	long			*ppts;
	DWORD			*ptimes;
	DWORD			validpts;
	COLORREF		rgbline;

	// Event
	HICON			icon;
	DWORD			time;

} GRAPHELEMENTINFO, *LPGRAPHELEMENTINFO;


class CDrawGraph : public CHList
{

public:
	BOOL Clean();
	void ReleaseMemory( LPGRAPHELEMENTINFO pgei );
	BOOL AllocateMemory( LPGRAPHELEMENTINFO pgei, DWORD size );
	BOOL Tick( DWORD dwTick = 0 );
	BOOL Add( LPCTSTR pName, DWORD dwTime, DWORD dwValue, COLORREF rgbLine, HICON hIcon );
	virtual void DeleteObject( void *node );
	BOOL Draw( HDC hDC, LPRECT pRect );
	CDrawGraph();
	virtual ~CDrawGraph();

	virtual DWORD GetObjSize() { return sizeof( GRAPHELEMENTINFO ); }

	void SetTimePerPixel( DWORD tpp ) { m_dwTimePerPixel = tpp; }
	DWORD GetTimePerPixel() { return m_dwTimePerPixel; }
	void SetRange( DWORD r ) { m_dwRange = r; }
	DWORD GetRange() { return m_dwRange; }

	CWinDc			m_bck;

private:

	DWORD			m_dwTime;
	DWORD			m_dwTimePerPixel;
	DWORD			m_dwGraphSize;
	DWORD			m_dwLastWindow;
	DWORD			m_dwRange;

};

#endif // !defined(AFX_DRAWGRAPH_H__44CF80DF_0FC5_40C4_981E_7EC425B15F5C__INCLUDED_)
