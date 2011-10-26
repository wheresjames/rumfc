// PageList.h: interface for the CPageList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAGELIST_H__0CDC06EC_6BDC_4E7B_BF33_1120261AF6E2__INCLUDED_)
#define AFX_PAGELIST_H__0CDC06EC_6BDC_4E7B_BF33_1120261AF6E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_PG_SAVE			( WM_USER + 701 )
#define WM_PG_RESTORE		( WM_USER + 702 )
#define WM_PG_APPLY			( WM_USER + 703 )
#define WM_PG_ENAPPLY		( WM_USER + 704 )

#define WM_PG_CHANGE		( WM_USER + 720 )
#define WM_PG_REFRESH		( WM_USER + 721 )
#define WM_PG_GETDATA		( WM_USER + 722 )

#define WM_CB_ENABLEBACK	( WM_USER + 741 )
#define WM_CB_ENABLENEXT	( WM_USER + 742 )
#define WM_CB_ENABLEFINISH	( WM_USER + 743 )
#define WM_CB_ENABLEHELP	( WM_USER + 744 )
#define WM_CB_NEXT			( WM_USER + 745 )
#define WM_CB_BACK			( WM_USER + 746 )
#define WM_CB_SETPAGE		( WM_USER + 747 )

typedef struct tagPAGEINFO : LLISTINFO
{
	CDialog		*pdlg;
	DWORD		idd;
	char		name[ CWF_STRSIZE ];
	char		help[ CWF_STRSIZE ];

} PAGEINFO, *LPPAGEINFO; // end typedef struct


class CPageList : public CLList  
{
public:
	HRESULT Refresh();
	BOOL Redraw();
	BOOL SetTabs( CTabCtrl *pTabCtrl );
	BOOL SizePage();
	LPPAGEINFO GetPageInfo( DWORD pg );
	LPPAGEINFO Add( CDialog *pdlg, DWORD idd, LPCTSTR pName, LPCTSTR pHelp );
	CPageList();
	virtual ~CPageList();

	virtual DWORD GetObjSize() { return sizeof( PAGEINFO ); }

	virtual void DeleteObject( void *node );
	virtual BOOL InitObject( void *node );

	virtual void Destroy() { m_dwPg = MAXDWORD; m_pCurPage = NULL; CLList::Destroy(); }

	DWORD GetPage() { return m_dwPg; }
	BOOL SetPage( DWORD pg );

	CDialog* GetPage( DWORD dwPg ) 
	{	LPPAGEINFO pi = (LPPAGEINFO)GetByIndex( dwPg );
		if ( pi == NULL ) return NULL;
		return pi->pdlg;
	}

	void SetHelpRoot( LPCTSTR pRoot )
	{	strcpy_sz( m_helproot, pRoot ); }

	void SetParent( CWnd *pWnd ) { m_pParent = pWnd; }

	void SizePages( BOOL b ) { m_bSizePages = b; }


private:

	char		m_helproot[ CWF_STRSIZE ];

	DWORD		m_dwPg;

	CWnd		*m_pParent;

	CDialog		*m_pCurPage;

	BOOL		m_bSizePages;
};

#endif // !defined(AFX_PAGELIST_H__0CDC06EC_6BDC_4E7B_BF33_1120261AF6E2__INCLUDED_)
