// DlgStackTrace.cpp : implementation file
//

#include "stdafx.h"
#include "praesidiumadmin.h"
#include "DlgStackTrace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgStackTrace dialog


CDlgStackTrace::CDlgStackTrace(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStackTrace::IDD, pParent)
{_STT();
	//{{AFX_DATA_INIT(CDlgStackTrace)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bAscending = FALSE;
}


void CDlgStackTrace::DoDataExchange(CDataExchange* pDX)
{_STT();
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgStackTrace)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_THREAD, m_comboThread);
	DDX_Control(pDX, IDC_TAB, m_tabSource);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgStackTrace, CDialog)
	//{{AFX_MSG_MAP(CDlgStackTrace)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_CBN_SELCHANGE(IDC_THREAD, OnSelchangeThread)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST, OnColumnclickList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStackTrace message handlers

BOOL CDlgStackTrace::OnInitDialog() 
{_STT();
	CDialog::OnInitDialog();

	if ( m_images.GetSafeHandle() == NULL )
	{	m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 0, 32 );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_GOLDDOT ) );		
	} // end if

	ListCtrl_SetExtendedListViewStyle( m_list.GetSafeHwnd(), LVS_EX_FULLROWSELECT );

	m_list.SetImageList( &m_images, LVSIL_NORMAL );
	m_list.SetImageList( &m_images, LVSIL_SMALL );

	m_tabSource.InsertItem(	0, "Call Stack" );
#ifdef ENABLE_STACK_HISTORY
	m_tabSource.InsertItem(	1, "Stack History" );
#endif
#ifdef ENABLE_STACK_PROFILE
	m_tabSource.InsertItem(	2, "Stack Profile" );
#endif

	m_tabSource.SetCurSel( 0 );

/*
	m_comboSource.SetItemData( m_comboSource.AddString( "Call Stack" ), 0 );

#ifdef ENABLE_STACK_HISTORY
	m_comboSource.SetItemData( m_comboSource.AddString( "Stack History" ), 1 );
#endif

#ifdef ENABLE_STACK_PROFILE
	m_comboSource.SetItemData( m_comboSource.AddString( "Stack Profile" ), 2 );
#endif

	m_comboSource.SetCurSel( 0 );
*/
	Size();

	Capture();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgStackTrace::OnSize(UINT nType, int cx, int cy) 
{_STT();
	CDialog::OnSize(nType, cx, cy);

	Size();
}

void CDlgStackTrace::Size()
{_STT();
	if ( !::IsWindow( GetSafeHwnd() ) ) return;

	CWnd *pCtrl;
	RECT rect, ctrl;
	GetClientRect( &rect );

	// Combo
	SetRect( &ctrl, rect.left + 2, rect.top + 2, rect.right - ( 80 * 1 ), rect.top + 24 );
	pCtrl = GetDlgItem( IDC_TAB );
	if ( pCtrl ) { pCtrl->MoveWindow( &ctrl ); pCtrl->RedrawWindow(); }

	// Refresh
	SetRect( &ctrl, rect.right - 80, rect.top, rect.right, rect.top + 24 );
	pCtrl = GetDlgItem( IDC_REFRESH );
	if ( pCtrl ) { pCtrl->MoveWindow( &ctrl ); pCtrl->RedrawWindow(); }

	// Thread combo
	SetRect( &ctrl, rect.left, rect.top + 25, rect.right, rect.top + 50 );
	pCtrl = GetDlgItem( IDC_THREAD );
	if ( pCtrl ) { pCtrl->MoveWindow( &ctrl ); pCtrl->RedrawWindow(); }

	// Tree
	SetRect( &ctrl, rect.left, rect.top + 50, rect.right, rect.bottom );
	pCtrl = GetDlgItem( IDC_LIST );
	if ( pCtrl ) { pCtrl->MoveWindow( &ctrl ); pCtrl->RedrawWindow(); }

	// Set columns
	long w = ( rect.right - rect.left ) - GetSystemMetrics( SM_CXVSCROLL ) - 4;
	if ( ::IsWindow( m_list.GetSafeHwnd() ) )
	{
		int sel = m_tabSource.GetCurSel();
		if ( sel == 2 )
		{	m_list.SetColumnWidth( 0, w - ( w / 4 ) );
			m_list.SetColumnWidth( 1, w / 4 );
		} // end if			
		else m_list.SetColumnWidth( 0, w );
	} // end if

}

BOOL CDlgStackTrace::FillCombo( CReg *pReg )
{_STT();
	m_comboThread.ResetContent();

	int nCurThread = -1;
	if ( pReg == NULL ) return FALSE;

	LPREGKEY pRk = NULL;
	char szKey[ CWF_STRSIZE ] = "";
	while ( NULL != ( pRk = (LPREGKEY)pReg->GetNext( pRk ) ) && pRk->key )
	{
		// Add item to combo box
		if ( pRk->key->GetDword( "current_thread" ) )
		{	strcpy( szKey, "* " );
			strcat( szKey, pRk->key->GetName() );
			nCurThread = m_comboThread.InsertString( -1, szKey );
		} // end if
		else m_comboThread.InsertString( -1, pRk->key->GetName() );

	} // end while

	// Select current thread
	if ( nCurThread >= 0 ) m_comboThread.SetCurSel( nCurThread );

	return TRUE;
}


BOOL CDlgStackTrace::FillList( CReg *pReg )
{_STT();

	// Lose previous tree values
	m_list.DeleteAllItems();

	if ( pReg == NULL ) return FALSE;

	// Get selected tab
	int tab = m_tabSource.GetCurSel();
	if ( tab < 0 ) return FALSE;

	// Get thread selection
	int sel = m_comboThread.GetCurSel();
	if ( sel < 0 ) return FALSE;

	// Get seleted thread
	char szKey[ CWF_STRSIZE ] = "";
	m_comboThread.GetLBText( sel, szKey );

	// Find selected thread information
	CRKey *pRk = NULL;
	if ( '*' == *szKey ) pRk = pReg->FindKey( &szKey[ 2 ] );
	else pRk = pReg->FindKey( szKey );
	if ( !pRk ) return FALSE;

	UINT i = 0;
	int item = 0;
	LPCTSTR pFn = NULL;

	// Write out the call stack
	do
	{
		// Create key
		wsprintf( szKey, "f_%lu", i );

		// Is there a function at this index
		pFn = pRk->GetSz( szKey );
		if ( *pFn ) item = m_list.InsertItem( m_list.GetItemCount(), pFn, 0 );

		if ( tab == 2 )
		{
			wsprintf( szKey, "t_%lu", i );

			// Set the total time spent in this function
			LPCTSTR pT = pRk->GetSz( szKey );
			if ( *pT ) m_list.SetItemText( item, 1, pT );

		} // end if

		i++;

	} while ( *pFn );

	return TRUE;
}

BOOL CDlgStackTrace::Refresh()
{_STT();

	InitColumns();

	int sel = m_tabSource.GetCurSel();
	
	CReg reg;
	switch( sel )
	{

#ifdef ENABLE_STACK_HISTORY

		case 1 :
			FillCombo( &m_regHistory );	
			FillList( &m_regHistory );	
			break;
#endif
	
#ifdef ENABLE_STACK_PROFILE

		case 2 :
			FillCombo( &m_regProfile );	
			FillList( &m_regProfile );	
			Sort( 1, FALSE );
			break;
#endif
	
		default : 
			FillCombo( &m_regCallstack );	
			FillList( &m_regCallstack );	
			break;

	} // end switch

	return TRUE;
}

BOOL CDlgStackTrace::Capture()
{_STT();
	CStackReport::CallStack( &m_regCallstack, CStackTrace::St() );
#ifdef ENABLE_STACK_HISTORY
	CStackReport::History( &m_regHistory, CStackTrace::St() );
#endif
#ifdef ENABLE_STACK_PROFILE
	CStackReport::Profile( &m_regProfile, CStackTrace::St() );
#endif

	return TRUE;
}

void CDlgStackTrace::OnRefresh() 
{_STT();
	Capture();
	Refresh();	
}

void CDlgStackTrace::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{_STT();
	Refresh();	
	*pResult = 0;
}

void CDlgStackTrace::OnSelchangeThread() 
{_STT();
	Refresh();	
}

BOOL CDlgStackTrace::InitColumns()
{_STT();
	int sel = m_tabSource.GetCurSel();

	RECT rect;
	m_list.GetClientRect( &rect );
	long w = ( rect.right - rect.left ) - GetSystemMetrics( SM_CXVSCROLL ) - 4;

	// Delete items
	m_list.DeleteAllItems();
	m_list.DeleteColumn( 0 );
	m_list.DeleteColumn( 0 );

	if ( sel == 2 )
	{
		m_list.InsertColumn( 0, "Function", LVCFMT_LEFT, w - ( w / 4 ) );
		m_list.InsertColumn( 1, "Time", LVCFMT_LEFT, w / 4 );
	} // end if

	else
	{
		m_list.InsertColumn( 0, "Function", LVCFMT_LEFT, w );
	} // end else

	return TRUE;
}

void CDlgStackTrace::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{_STT();
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_bAscending = !m_bAscending;
	Sort( pNMListView->iSubItem, m_bAscending );

	*pResult = 0;
}

void CDlgStackTrace::Sort(DWORD dwItem, BOOL bAscending)
{_STT();
	int sel = m_tabSource.GetCurSel();
	if ( sel != 2 ) return;

	// bAscending will be use to order from lower to higher or higher to lower
	bool bByData = false;

	CSortListCtrl slc( &m_list, dwItem, bByData );
	slc.Sort( bAscending );	
}
