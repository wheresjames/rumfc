// PageList.cpp: implementation of the CPageList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PageList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPageList::CPageList()
{
	m_pParent = NULL;
	*m_helproot = 0;
	m_dwPg = MAXDWORD;
	m_pCurPage = NULL;
	m_bSizePages = FALSE;
}

CPageList::~CPageList()
{	Destroy();
}

void CPageList::DeleteObject( void *node )
{
	LPPAGEINFO ppi = (LPPAGEINFO)node;
	if ( ppi == NULL ) return;

	// Release the dialog box
	if ( ppi->pdlg != NULL )
	{
		// Lose the window if any
		if ( ppi->pdlg->GetSafeHwnd() != NULL )
			ppi->pdlg->DestroyWindow();

		// Delete the object
		delete ppi->pdlg;
		ppi->pdlg = NULL;
	} // end if

	CLList::DeleteObject( node );
}

BOOL CPageList::InitObject( void *node )
{
	LPPAGEINFO ppi = (LPPAGEINFO)node;
	if ( ppi == NULL ) return FALSE;

	return TRUE;
}

LPPAGEINFO CPageList::Add(CDialog *pdlg, DWORD idd, LPCTSTR pName, LPCTSTR pHelp)
{
	LPPAGEINFO node = (LPPAGEINFO)New( NULL, 0, pName );
	if ( node == NULL ) return FALSE;

	node->pdlg = pdlg;
	node->idd = idd;
	if ( pName != NULL ) strcpy_sz( node->name, pName );
	else *node->name = 0;
	if ( pHelp != NULL ) strcpy_sz( node->help, pHelp );
	else *node->help = 0;

	return node;
}

LPPAGEINFO CPageList::GetPageInfo(DWORD pg)
{
	return (LPPAGEINFO)GetByIndex( pg );
}

BOOL CPageList::SetPage( DWORD pg )
{
	// Are we already on that page?
	if ( pg == m_dwPg ) return TRUE;

	// Get the old page
	LPPAGEINFO cp = GetPageInfo( m_dwPg );
	
	// Lose old page
	if (	cp != NULL && cp->pdlg != NULL &&
			::IsWindow( cp->pdlg->GetSafeHwnd() ) )
	{
		// old page must approve page switches
		if ( cp->pdlg->SendMessage( WM_PG_CHANGE, 1, 0L ) ) 
			return FALSE;

		// Lose current page
		m_pCurPage = NULL;

		// Lose the page
		cp->pdlg->DestroyWindow();

	} // end if

	// Is page within reason?
	if ( pg >= Size() ) { m_dwPg = MAXDWORD; return FALSE; }

	// Get the new page
	LPPAGEINFO ppi = GetPageInfo( pg );
	if ( ppi == NULL ) { m_dwPg = MAXDWORD; return FALSE; }

	// Set the new page
	m_dwPg = pg; 

	// Create dialog
	if ( !IsWindow( ppi->pdlg->GetSafeHwnd() ) )
	{
		if ( m_pParent != NULL && ::IsWindow( m_pParent->GetSafeHwnd() ) )
			ppi->pdlg->Create( ppi->idd, m_pParent );
		else ppi->pdlg->Create( ppi->idd );
	} // end if

	SizePage();

	// Show the dialog box
	ppi->pdlg->ShowWindow( SW_SHOWNA );
	ppi->pdlg->RedrawWindow();

	// Lose current page
	m_pCurPage = ppi->pdlg;

	return TRUE;
}

BOOL CPageList::SizePage()
{
	if ( m_pParent == NULL || !::IsWindow( m_pParent->GetSafeHwnd() ) ) 
		return FALSE;

	// Get the old page
	LPPAGEINFO ppi = GetPageInfo( m_dwPg );
	if ( ppi == NULL || ppi->pdlg == NULL || !::IsWindow( ppi->pdlg->GetSafeHwnd() ) )
		return FALSE;

	RECT rect, dlg;

	// Adjust window to tab control
	m_pParent->GetClientRect( &rect );

	if ( m_bSizePages ) CopyRect( &dlg, &rect );

	else
	{
		// Set page window position
		ppi->pdlg->GetClientRect( &dlg );

		// Center the dialog
		CGrDC::Center( &dlg, &rect );

	} // end if
	
	// Set the page position
	ppi->pdlg->SetWindowPos(	NULL, dlg.left, dlg.top, RW( dlg ), RH( dlg ),
								SWP_NOZORDER | SWP_NOACTIVATE );

	ppi->pdlg->SendMessage( WM_SIZE, 0, MAKELONG( RW( dlg ), RH( dlg ) ) );

	// Redraw the dialog box
	ppi->pdlg->RedrawWindow();

	return TRUE;
}

BOOL CPageList::SetTabs(CTabCtrl *pTabCtrl)
{
	TC_ITEM		tcitem;	
	tcitem.mask = TCIF_TEXT;

	// Lose all items in the tab control
	pTabCtrl->DeleteAllItems();

	DWORD i = 0;
	LPPAGEINFO pPi = NULL;
	while ( ( pPi = (LPPAGEINFO)GetNext( pPi ) ) != NULL )
	{	tcitem.pszText = pPi->name;
		pTabCtrl->InsertItem( i++, &tcitem );	
	} // end for

	return i > 0;
}

BOOL CPageList::Redraw()
{
	if ( m_pCurPage == NULL ) return FALSE;
	return m_pCurPage->RedrawWindow();
}

HRESULT CPageList::Refresh()
{
	if ( m_pCurPage == NULL ) return S_FALSE;
	if ( !::IsWindow( m_pCurPage->GetSafeHwnd() ) ) return S_FALSE;
	return ::SendMessage( m_pCurPage->GetSafeHwnd(), WM_PG_REFRESH, 0, 0L );
}
