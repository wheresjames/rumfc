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
#if !defined(AFX_DLGTABFRAME_H__4265348D_364C_44D9_B429_91724D6B6FBC__INCLUDED_)
#define AFX_DLGTABFRAME_H__4265348D_364C_44D9_B429_91724D6B6FBC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgTabFrame.h : header file
//

#include "BlankWnd.h"
#include "NoDrawTabControl.h"

#define TABDLG_MAXPAGES		16

#ifndef WM_PG_MSGS
#define WM_PG_MSGS

#define WM_PG_SAVE		    ( WM_USER + 701 )
#define WM_PG_RESTORE	    ( WM_USER + 702 )
#define WM_PG_APPLY		    ( WM_USER + 703 )
#define WM_PG_ENAPPLY	    ( WM_USER + 704 )

#define WM_PG_CHANGE	    ( WM_USER + 720 )
#define WM_PG_REFRESH	    ( WM_USER + 721 )
#define WM_PG_GETDATA	    ( WM_USER + 722 )
#define WM_PG_PAGECLOSE 	( WM_USER + 723 )

#define WM_PG_GET_TAB       ( WM_USER + 750 )
#define WM_PG_SET_TAB_TEXT  ( WM_USER + 751 )

#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTabFrame dialog

class CDlgTabFrame : public CDialog
{
// Construction
public:
	void Destroy();
	void Size();
	CDlgTabFrame(UINT uIDD = 0, CWnd* pParent = NULL);   // standard constructor
	~CDlgTabFrame();


// Dialog Data
	//{{AFX_DATA(CDlgTabFrame)
//	enum { IDD = IDD_PROPERTIES };
	int IDD;
	CBlankWnd	        m_blank;
	CNoDrawTabControl	m_tab;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTabFrame)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTabFrame)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnHelpbtn();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	static void* GetData( HWND hPage );
	LRESULT OnGetData( WPARAM wParam, LPARAM lParam );
	void SavePosition();
	void AddPages();
	void RefreshPages();
	BOOL SetPage( DWORD pg );
    void PositionBlank();

	void SetPage( DWORD pg, CDialog *dlg, DWORD idd, LPCTSTR name, LPCTSTR help = NULL, UINT uIcon = 0 )
	{	if ( pg < TABDLG_MAXPAGES ) 
		{ 	m_pg[ pg ] = dlg; 
			m_idd[ pg ] = idd; 
			if ( name != NULL ) strcpy_sz( m_name[ pg ], name ); 
			if ( help != NULL ) strcpy_sz( m_help[ pg ], help ); 			
			m_icons[ pg ] = uIcon;
		} 
	}
	CDialog* GetPage( DWORD pg )
	{	if ( pg < TABDLG_MAXPAGES ) return m_pg[ pg ]; return NULL; }

	void SetHelpRoot( LPCTSTR ptr ) 
	{	if ( ptr == NULL ) *m_helproot = 0; else strcpy_sz( m_helproot, ptr ); }

	void SetRestore( char *restore ) { strcpy_sz( m_szRestore, restore ); }

	void SizeDialog( BOOL b ) { m_bSize = b; }

	void SetDataPtr( void *ptr ) { m_pData = ptr; }

	void StartPage( DWORD pg ) { m_dwPg = pg; }

	static void SetButtons( WORD idcBlank, WORD idcTab, WORD idcHelpBtn )
	{	m_idcBlank = idcBlank; m_idcTab = idcTab; m_idcHelpBtn = idcHelpBtn; }

    void PersistPages( BOOL b ) { m_bPersistPages = b; }

	void SetDoneCallback( HWND hWnd, UINT uMsg )
	{	m_hDoneWnd = hWnd; m_uDoneMsg = uMsg; }
	LRESULT DoDoneCallback( WPARAM wParam, LPARAM lParam )
	{	if ( ::IsWindow( m_hDoneWnd ) )
			return ::SendMessage( m_hDoneWnd, m_uDoneMsg, wParam, lParam );
		return 0;
	}

    void SetControls( BOOL bTab, BOOL bBlank, BOOL bHelpBtn )
    {   m_bTab = bTab; m_bBlank = bBlank; m_bHelpBtn = bHelpBtn; }

    HRESULT SendPageMsg( long lPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {   if ( TABDLG_MAXPAGES <= lPage ) return ERROR_INVALID_PARAMETER;
        if ( !m_pg[ lPage ] || !::IsWindow( m_pg[ lPage ]->GetSafeHwnd() ) ) return ERROR_INVALID_PARAMETER;
        return m_pg[ lPage ]->SendMessage( uMsg, wParam, lParam );
    }

    HRESULT PostPageMsg( long lPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {   if ( TABDLG_MAXPAGES <= lPage ) return ERROR_INVALID_PARAMETER;
        if ( !m_pg[ lPage ] || !::IsWindow( m_pg[ lPage ]->GetSafeHwnd() ) ) return ERROR_INVALID_PARAMETER;
        return m_pg[ lPage ]->PostMessage( uMsg, wParam, lParam );
    }
	
    void SetTabText( long lTab, LPCTSTR pText )
    {   if ( TABDLG_MAXPAGES <= lTab ) return;
        if ( !::IsWindow( m_tab.GetSafeHwnd() ) ) return;
        TCITEM ti; ZeroMemory( &ti, sizeof( ti ) );
        ti.mask = TCIF_TEXT;
        ti.pszText = (LPSTR)pText;
        ti.cchTextMax = strlen( pText );
        m_tab.SetItem( lTab, &ti );
    }

    static HRESULT SendTabDlgMsg( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam );
    LRESULT OnSetTabText(WPARAM wParam, LPARAM lParam);
    LRESULT OnGetTab(WPARAM wParam, LPARAM lParam);


private:
	
    BOOL            m_bTab;
    BOOL            m_bBlank;
    BOOL            m_bHelpBtn;

	static WORD		m_idcBlank;
	static WORD		m_idcTab;
	static WORD		m_idcHelpBtn;

	char			m_helproot[ CWF_STRSIZE ];
				
	DWORD			m_dwPages;
	DWORD			m_dwPg;
	CDialog			*m_pg[ TABDLG_MAXPAGES ];
	DWORD			m_idd[ TABDLG_MAXPAGES ];
	char			m_name[ TABDLG_MAXPAGES ][ 64 ];
	char			m_help[ TABDLG_MAXPAGES ][ MAX_PATH ];
	UINT			m_icons[ TABDLG_MAXPAGES ];

	char			m_szRestore[ CWF_STRSIZE ];
	BOOL			m_bSize;
	void			*m_pData;

	CImageList		m_images;

    BOOL            m_bPersistPages;

	HWND		    m_hDoneWnd;
	UINT		    m_uDoneMsg;

protected:

    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTABFRAME_H__4265348D_364C_44D9_B429_91724D6B6FBC__INCLUDED_)
