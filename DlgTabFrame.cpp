/*******************************************************************
// Copyright (c) 2002, 2003, 2004, 2005, 
// Robert Umbehant
// mailto:rumbehant@wheresjames.com
//
// This program is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as 
// published by the Free Software Foundation; either version 2 of 
// the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public 
// License along with this program; if not, write to the Free 
// Software Foundation, Inc., 59 Temple Place, Suite 330, 
// Boston, MA 02111-1307 USA
//
*******************************************************************/
// DlgTabFrame.cpp : implementation file
//

#include "stdafx.h"
#include "DlgTabFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTabFrame dialog

// Set IDC
WORD CDlgTabFrame::m_idcBlank = 0;
WORD CDlgTabFrame::m_idcTab = 0;
WORD CDlgTabFrame::m_idcHelpBtn = 0;


CDlgTabFrame::CDlgTabFrame(UINT uIDD, CWnd* pParent /*=NULL*/)
	: CDialog(uIDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTabFrame)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_dwPages = 0;
	m_dwPg = 0;
	ZeroMemory( &m_pg, sizeof( m_pg ) );

	*m_helproot = NULL;
	ZeroMemory( &m_help, sizeof( m_help ) );

	ZeroMemory( &m_icons, sizeof( m_icons ) );

	*m_szRestore = 0;
	m_bSize = FALSE;

	m_pData = NULL;

    m_bPersistPages = FALSE;

    m_bTab = TRUE;
    m_bBlank = TRUE;
    m_bHelpBtn = TRUE;
}

CDlgTabFrame::~CDlgTabFrame()
{
	Destroy();
}

void CDlgTabFrame::Destroy()
{
	// Lose all pages
	for ( DWORD i = 0; i < TABDLG_MAXPAGES; i++ )
		if ( m_pg[ i ] != NULL )
		{
			if( ::IsWindow( m_pg[ i ]->GetSafeHwnd() ) )
				m_pg[ i ]->DestroyWindow();

			delete m_pg[ i ];

			m_pg[ i ] = NULL;

		} // end if

	// Reinit variables
	ZeroMemory( &m_pg, sizeof( m_pg ) );
	ZeroMemory( &m_help, sizeof( m_help ) );
	m_dwPg = 0;
	m_dwPages = 0;
}

void CDlgTabFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTabFrame)
	//}}AFX_DATA_MAP
//    if ( m_bBlank && m_idcBlank ) { DDX_Control(pDX, m_idcBlank, m_blank); }
    if ( m_bTab && m_idcTab ) { DDX_Control(pDX, m_idcTab, m_tab); }
}


BEGIN_MESSAGE_MAP(CDlgTabFrame, CDialog)
	//{{AFX_MSG_MAP(CDlgTabFrame)
	ON_NOTIFY(TCN_SELCHANGE, m_idcTab, OnSelchangeTab)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(m_idcHelpBtn, OnHelpbtn)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_GETDATA, OnGetData )
	
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_SHOWWINDOW()

    ON_MESSAGE( WM_PG_SET_TAB_TEXT, OnSetTabText )
    ON_MESSAGE( WM_PG_GET_TAB, OnGetTab )
    

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTabFrame message handlers

BOOL CDlgTabFrame::OnInitDialog() 
{
	CDialog::OnInitDialog();

    m_tab.SetRefresh( &m_blank );

    if ( m_bBlank && m_idcBlank )
    {   CWnd *pCtrl = GetDlgItem( m_idcBlank );
        if ( pCtrl ) pCtrl->ShowWindow( SW_HIDE );
    } // end if

    PositionBlank();

	// Create image list
	if ( m_images.GetSafeHandle() != NULL ) m_images.DeleteImageList();
	m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 0, 32 );

	AddPages();

	if ( m_images.GetImageCount() )
		m_tab.SetImageList( &m_images );
	else m_tab.SetImageList( NULL );

	// Start on the first page
	SetPage( m_dwPg );
	m_tab.SetCurSel( m_dwPg );
    
	// Restore position
//	if ( *m_szRestore != NULL )
//		CFG().RestoreWindowPos( GetSafeHwnd(), "Positions", m_szRestore, FALSE );

	// Size everything correctly
	Size();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTabFrame::PositionBlank()
{
    RECT rect;
    m_tab.GetWindowRect( &rect );
    m_tab.AdjustRect( FALSE, &rect );
    m_tab.ScreenToClient( &rect );
//    m_blank.ScreenToClient( &rect );

    // Create the blank if needed
    if ( !m_blank.GetSafeHwnd() || !::IsWindow( m_blank.GetSafeHwnd() ) )
        if ( !m_blank.Create( "", WS_CHILD | WS_VISIBLE, rect, &m_tab ) )
            return;

    m_blank.MoveWindow( &rect );
}

void CDlgTabFrame::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
    Size();
}

void CDlgTabFrame::Size()
{
	CWnd *pTab = GetDlgItem( m_idcTab );
    if ( !pTab || !::IsWindow( pTab->GetSafeHwnd() ) ) return;

	RECT rect;

	if ( m_bSize )
	{
		GetClientRect( &rect );	
//		InflateRect( &rect, -4, -4 );

		// Set tab control position
		pTab->SetWindowPos(	NULL, rect.left, rect.top,
							rect.right - rect.left,
							rect.bottom - rect.top,
							SWP_NOZORDER | SWP_NOACTIVATE );

	} // end if

    // Position the blank into the tab control
    PositionBlank();

	// Set page window position
	if ( m_pg[ m_dwPg ] != NULL && ::IsWindow( m_pg[ m_dwPg ]->GetSafeHwnd() ) )
	{
        // Get the blank position
        m_blank.GetClientRect( &rect );

		// Set the page position
		m_pg[ m_dwPg ]->SetWindowPos(	NULL, rect.left, rect.top,
										rect.right - rect.left,
										rect.bottom - rect.top,
										SWP_NOZORDER | SWP_NOACTIVATE );
		m_pg[ m_dwPg ]->RedrawWindow();

	} // end if


/*
	
	// Adjust window to tab control
	if ( pTab != NULL && ::IsWindow( pTab->GetSafeHwnd() ) )
	{
		if ( !m_bSize ) 
		{	pTab->GetWindowRect( &rect );
			ScreenToClient( &rect );
		} // end if

		m_tab.AdjustRect( FALSE, &rect );
		rect.bottom -= 2;

		// Set page parent window position
        if ( ::IsWindow( m_blank.GetSafeHwnd() ) )
		{
            m_blank.ScreenToClient( &rect );
            m_blank.MoveWindow( &rect );


/*			// Set page parent position
			pBlank->SetWindowPos(	NULL, rect.left, rect.top,
									rect.right - rect.left,
									rect.bottom - rect.top,
									SWP_NOZORDER | SWP_NOACTIVATE );
			m_blank.GetClientRect( &rect );
	//		m_blank.GetWindowRect( &rect );
	//		ScreenToClient( &rect );
* /
			m_blank.GetClientRect( &rect );

		} // end if
*/
}


BOOL CDlgTabFrame::SetPage(DWORD pg)
{
	// Do we need to add the pages?
	if ( m_dwPages == 0 ) AddPages();

	// Is page within reason?
	if ( pg >= TABDLG_MAXPAGES || m_pg[ pg ] == NULL )
		return FALSE;

	// Lose current page
	if (	m_pg[ m_dwPg ] != NULL && 
			IsWindow( m_pg[ m_dwPg ]->GetSafeHwnd() ) )
	{
		// Current page must approve page switches
		if ( m_pg[ m_dwPg ]->SendMessage( WM_PG_CHANGE, 1, 0L ) ) 
			return FALSE;

        if ( m_bPersistPages ) m_pg[ m_dwPg ]->ShowWindow( SW_HIDE );

		// Lose the page
		else m_pg[ m_dwPg ]->DestroyWindow();

	} // end if
	
	CWnd *pHelp = GetDlgItem( m_idcHelpBtn );

	// Enable / Disable help button
	if ( pHelp != NULL )
	{
		// Show help button if link
		if ( m_help[ pg ][ 0 ] != 0 )
		{
			pHelp->ShowWindow( SW_SHOWNORMAL );

			char link[ CWF_STRSIZE * 2 ];
			CWinFile::BuildPath( link, m_helproot, m_help[ pg ] );
			pHelp->EnableWindow( CWinFile::DoesExist( link ) );
		} // end if

		// Hide help button if no help
		else pHelp->ShowWindow( SW_HIDE );

	} // end if

	// Set title text
//	m_title.SetTitle( m_name[ pg ] );
//	m_title.RedrawWindow();

    if ( m_bPersistPages )
    {   for ( DWORD i = 0; i < m_dwPages; i++ )
	        if ( !IsWindow( m_pg[ i ]->GetSafeHwnd() ) )
		        m_pg[ i ]->Create( m_idd[ i ], &m_blank );
    } // end if

	// Create dialog
	else if ( !IsWindow( m_pg[ pg ]->GetSafeHwnd() ) )
		m_pg[ pg ]->Create( m_idd[ pg ], &m_blank );

	// Save current page
	m_dwPg = pg;

	Size();

    // Redraw dialog box
    RedrawWindow();

	// Show the dialog box
    m_blank.ShowWindow( SW_SHOWNA );
	m_pg[ pg ]->ShowWindow( SW_SHOWNA );
	m_pg[ pg ]->RedrawWindow();

	return TRUE;
}

void CDlgTabFrame::RefreshPages()
{
	// Refresh all pages
	for ( DWORD i = 0; i < TABDLG_MAXPAGES; i++ )
		if ( m_pg[ i ] != NULL && ::IsWindow( m_pg[ i ]->GetSafeHwnd() ) )
			m_pg[ i ]->SendMessage( WM_PG_REFRESH, 0, (LPARAM)this );
}

void CDlgTabFrame::AddPages()
{
	TC_ITEM		tcitem;	
	tcitem.mask = TCIF_TEXT;

	// Lose all items in the tab control
    if ( ::IsWindow( m_tab.GetSafeHwnd() ) )
	    m_tab.DeleteAllItems();

	// Add pages
	int image = 0;
	for ( DWORD i = 0; i < TABDLG_MAXPAGES && m_pg[ i ] != NULL; i++ )
	{
		tcitem.pszText = m_name[ i ];

		// Is there an images?
		tcitem.mask = TCIF_TEXT;
		if ( m_icons[ i ] )
		{	m_images.Add( AfxGetApp()->LoadIcon( m_icons[ i ] ) );
			tcitem.mask |= TCIF_IMAGE;
			tcitem.iImage = image++;
		} // end if

		m_tab.InsertItem( i, &tcitem );	
		m_dwPages++;
	} // end for
}

void CDlgTabFrame::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Select new view
	SetPage( m_tab.GetCurSel() );
	
	*pResult = 0;
}

void CDlgTabFrame::OnClose() 
{
	SavePosition();
	CDialog::OnClose();
	DoDoneCallback( 0, 0L );
//	if ( ::IsWindow( GetSafeHwnd() ) ) DestroyWindow();	
}

BOOL CDlgTabFrame::DestroyWindow() 
{
	SavePosition();
	return CDialog::DestroyWindow();
}


void CDlgTabFrame::SavePosition()
{
//	if ( *m_szRestore != NULL )
//		CFG().SaveWindowPos( GetSafeHwnd(), "Positions", m_szRestore );
}

LRESULT CDlgTabFrame::OnGetData(WPARAM wParam, LPARAM lParam)
{	
	return (LRESULT)m_pData;
}

void* CDlgTabFrame::GetData( HWND hPage )
{		
	void *ptr = NULL;

	// Get the tab control
	HWND hWnd = ::GetParent( hPage );

	// Get the blank
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );

	// Get the dialog
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );

	// Send message
	if ( ::IsWindow( hWnd ) )
	{	ptr = (void*)::SendMessage( hWnd, WM_PG_GETDATA, 0, 0 );
		if ( (DWORD)ptr < 256 ) ptr = NULL;
	} // end if

	return ptr;
}

HRESULT CDlgTabFrame::SendTabDlgMsg( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
{		
    if ( !::IsWindow( hPage ) )
        return ERROR_INVALID_PARAMETER;

	// Get the tab control
	HWND hWnd = ::GetParent( hPage );
	if ( !::IsWindow( hWnd ) )
        return ERROR_INVALID_PARAMETER;

    // Get the blank
	hWnd = ::GetParent( hWnd );
	if ( !::IsWindow( hWnd ) )
        return ERROR_INVALID_PARAMETER;

    // Get the dialog box
    hWnd = ::GetParent( hWnd );
	if ( !::IsWindow( hWnd ) )
        return ERROR_INVALID_PARAMETER;

    return ::SendMessage( hWnd, uMsg, wParam, lParam );
}

LRESULT CDlgTabFrame::OnSetTabText(WPARAM wParam, LPARAM lParam)
{	
    SetTabText( (long)wParam, (LPCTSTR)lParam );

	return (LRESULT)0;
}

LRESULT CDlgTabFrame::OnGetTab(WPARAM wParam, LPARAM lParam)
{	
    HWND hPage = (HWND)wParam;
    if ( !::IsWindow( hPage ) ) return -1;

    for ( DWORD i = 0; i < m_dwPages; i++ )
    	if ( m_pg[ i ] != NULL && m_pg[ i ]->GetSafeHwnd() == hPage )
            return (LRESULT)i;

	return (LRESULT)-1;
}


void CDlgTabFrame::OnHelpbtn() 
{
	// Open help if any
	if ( m_help[ m_dwPg ][ 0 ] == NULL ) return;

	// Open the help file
	char link[ CWF_STRSIZE ];
	CWinFile::BuildPath( link, m_helproot, m_help[ m_dwPg ] );
	ShellExecute( NULL, "open", link, NULL, NULL, SW_SHOWNORMAL );  		
}


BOOL CDlgTabFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// Send to pages
    for ( DWORD i = 0; i < m_dwPages; i++ )
    	if ( m_pg[ i ] != NULL && IsWindow( m_pg[ i ]->GetSafeHwnd() ) )
            m_pg[ i ]->SendMessage( WM_COMMAND, wParam, lParam );

    return CDialog::OnCommand(wParam, lParam);
}

void CDlgTabFrame::OnOK()
{_STT();
	DoDoneCallback( 1, 0L );

	CDialog::OnOK();
}

void CDlgTabFrame::OnPaint()
{
    CPaintDC dc(this); // device context for painting

/*    if ( TABDLG_MAXPAGES > m_dwPg && m_pg[ m_dwPg ] 
         && ::IsWindow( m_pg[ m_dwPg ]->GetSafeHwnd() ) )
        m_pg[ m_dwPg ]->RedrawWindow();
*/
}

BOOL CDlgTabFrame::OnEraseBkgnd(CDC* pDC)
{
    CDialog::OnEraseBkgnd(pDC);

    if ( TABDLG_MAXPAGES > m_dwPg && m_pg[ m_dwPg ] 
         && ::IsWindow( m_pg[ m_dwPg ]->GetSafeHwnd() ) )
        m_pg[ m_dwPg ]->RedrawWindow();

    return TRUE;    
}

void CDlgTabFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    if ( bShow && ::IsWindow( m_blank.GetSafeHwnd() ) )
        SetPage( m_dwPg );

//        if ( TABDLG_MAXPAGES > m_dwPg && m_pg[ m_dwPg ] 
//             && ::IsWindow( m_pg[ m_dwPg ]->GetSafeHwnd() ) )
//           m_pg[ m_dwPg ]->RedrawWindow();

}
