#pragma once

/////////////////////////////////////////////////////////////////////////////
// CCheckboxTree window

class CCheckboxTree : public CTreeCtrl
{
// Construction
public:
	CCheckboxTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckboxTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Undetermine( HTREEITEM hItem );

	BOOL SetIcons( DWORD dwIcons, ... ) { return vSetIcons( dwIcons, ( (LPVOID*)&dwIcons ) + 1 ); }

	BOOL vSetIcons( DWORD dwIcons, LPVOID *pArgs );
	
	UINT GetState( HTREEITEM hItem );
	
	BOOL SetState( HTREEITEM hItem, UINT uState );
	
	BOOL SetChildStates( HTREEITEM hRoot, UINT uState );
	
	/// Constructor
	virtual ~CCheckboxTree();

	void SetStates( UINT uStates, UINT uUndetermined )
	{	m_uStates = uStates; m_uUndetermined = uUndetermined; }

	void SetChildren( BOOL bSetChildren ) { m_bSetChildren = bSetChildren; }

	/// Number of icons in each group
	long IconsPerItem() { return m_uStates + ( m_uUndetermined ? 1 : 0 ); }

	/// Number of icons for each possible state
	long IconsPerState() { return 1; }

	/// Calculates the actual index of the specified icon in the image list
	long IconIndex( long uGroup, long uState )
	{	return ( ( uGroup * IconsPerItem() ) + uState ) * IconsPerState(); } 

	/// Translates an image list position into group,state
	void TranslateIndex( long lIndex, UINT *puGroup, UINT *puState )
	{	lIndex /= IconsPerState();
		*puState = lIndex % IconsPerItem();
		*puGroup = ( lIndex / IconsPerItem() );		
	}

	/// Override to handle state changes
	virtual void OnChangeState( HTREEITEM hItem, UINT uState ) {}

	// Generated message map functions
protected:
	//{{AFX_MSG(CCheckboxTree)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	BOOL				m_bSetChildren;
	UINT				m_uStates;
	UINT				m_uUndetermined;

	CImageList			m_imagelist;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

