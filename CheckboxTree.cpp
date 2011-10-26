// CheckboxTree.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckboxTree

CCheckboxTree::CCheckboxTree()
{
	m_uStates = 2;
	m_uUndetermined = 2;
	m_bSetChildren = TRUE;
}

CCheckboxTree::~CCheckboxTree()
{
}


BEGIN_MESSAGE_MAP(CCheckboxTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CCheckboxTree)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckboxTree message handlers

void CCheckboxTree::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Find out where the cursor was
	POINT pt;
	DWORD dwPos = GetMessagePos();
	pt.x = LOWORD( dwPos );
	pt.y = HIWORD( dwPos );

	ScreenToClient( &pt );

	// Where did the user click?
	TVHITTESTINFO hti; 
	ZeroMemory( &hti, sizeof( hti ) );
	hti.pt = pt;

	HTREEITEM hItem = HitTest( &hti );
	if ( NULL == hItem ) return;

	// Did the click land on the icon?
	if ( ( hti.flags & TVHT_ONITEMICON ) != 0 )
	{
		// New state
		UINT uState = GetState( hItem );
		uState = ++uState % m_uStates;

		// Set this items state
		SetState( hItem, uState );

		// Set all child states to match
		if ( m_bSetChildren ) SetChildStates( hItem, uState );

		// Determination
		if ( m_uUndetermined ) Undetermine( hItem );

		// Inform derived classes
		OnChangeState( hItem, uState );

		RedrawWindow();

	} // end if	
	
	*pResult = 0;
}

BOOL CCheckboxTree::SetChildStates(HTREEITEM hRoot, UINT uState)
{
	// Update all children
	HTREEITEM hChild = GetChildItem( hRoot );
	if ( hChild ) do
	{
		// Set this items state
		SetState( hChild, uState );

		// Set child item states
		SetChildStates( hChild, uState );

	} while ( NULL != ( hChild = GetNextSiblingItem( hChild ) ) );

	return TRUE;
}

BOOL CCheckboxTree::SetState(HTREEITEM hItem, UINT uState)
{
	// Get current image
	int iImage = CComCtrl::TreeGetItemImage( GetSafeHwnd(), hItem );

	// Get index of new image representing desired state
	UINT uCurGroup, uCurState;
	TranslateIndex( iImage, &uCurGroup, &uCurState );
	iImage = IconIndex( uCurGroup, uState );

	// Toggle this item image
	CComCtrl::TreeSetItemImage( GetSafeHwnd(), hItem, iImage );

	return TRUE;
}

UINT CCheckboxTree::GetState(HTREEITEM hItem)
{
	// What is the current image?
	int iImage = CComCtrl::TreeGetItemImage( GetSafeHwnd(), hItem );

	// Get current state
	UINT uCurGroup, uCurState;
	TranslateIndex( iImage, &uCurGroup, &uCurState );
	return uCurState;
}

BOOL CCheckboxTree::vSetIcons(DWORD dwIcons, LPVOID *pArgs)
{
	if ( m_imagelist.GetSafeHandle() != NULL )
		m_imagelist.DeleteImageList();

	// Image
	if ( !m_imagelist.Create( 32, 16, ILC_MASK | ILC_COLOR24, 0, 32 ) )
		return FALSE;
	
	// For each icon
	for ( DWORD i = 0; i < dwIcons; i++ )
	{	
		// Add icon
		m_imagelist.Add( AfxGetApp()->LoadIcon( *(LPUINT)pArgs ) );		

		pArgs++;

	} // end for

	SetImageList( &m_imagelist, TVSIL_NORMAL );
//	SetImageList( &m_imagelist, TVSIL_STATE );

	return TRUE;
}

BOOL CCheckboxTree::Undetermine(HTREEITEM hItem)
{	
	// Get our state
	UINT uState = GetState( hItem );

	// Update parent(s)
	HTREEITEM hParent = GetParentItem( hItem );
	if ( hParent )
	{
		// Get first child item
		HTREEITEM hChild = GetChildItem( hParent );
		do
		{
			// Set to undetermined state if two are different
			if ( GetState( hChild ) != uState )
				uState = m_uUndetermined;

		// As long as items are the same state and we have more siblings
		} while ( m_uUndetermined != uState && 
				  NULL != ( hChild = GetNextSiblingItem( hChild ) ) );

		// Set parent state
		SetState( hParent, uState );

		// Keep updating parents
		Undetermine( hParent );

	} // end if	

	return TRUE;
}
