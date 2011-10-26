// DrawSlantToolbar.h: interface for the CDrawSlantToolbar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWSLANTTOOLBAR_H__D7832D1D_84F7_4F91_A1A1_893F312898B9__INCLUDED_)
#define AFX_DRAWSLANTTOOLBAR_H__D7832D1D_84F7_4F91_A1A1_893F312898B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STIF1_PRESSED		0x00000001
#define STIF1_CHECKED		0x00000002
#define STIF1_HILIGHT		0x00000004
#define STIF1_NOCLICK		0x00000008
#define STIF1_RIGHTBTN		0x00000010
#define STIF1_HILIGHTALWAYS	0x00000020
#define STIF1_DISABLED		0x00000040
#define STIF1_MOUSEROTATE	0x00000080
#define STIF1_FLASH			0x00000100

typedef struct tagSLANTTOOLBARINFO : LLISTINFO
{
	UINT		cmd;
	HICON		icon;
	HICON		pressed;
	HICON		flash;
	char		tip[ 256 ];
	char		txt[ 256 ];
	DWORD		f1;
	LPBOOL		b;
	RECT		rect;
	RECT		ricon;

} SLANTTOOLBARINFO, *LPSLANTTOOLBARINFO; // end typedef struct

class CDrawSlantToolbar : public CLList  
{
public:

	struct SFlashInfo
	{
		BOOL	bVert;
		RECT	rect;

	}; typedef SFlashInfo* LPSFlashInfo;

public:
	void SetColors( COLORREF rgbBck, COLORREF rgbLtBorder, COLORREF rgbDkBorder, COLORREF rgbText, COLORREF rgbSelection );
	BOOL NeedFlash();
	BOOL DrawFlash( HDC hDc, LPRECT pRect );
	BOOL DrawMouseRotateIcon( HDC hDC, LPRECT pRect, LPSLANTTOOLBARINFO psti );
	BOOL SetTextByCmd( UINT cmd, LPCTSTR text );
	LPSLANTTOOLBARINFO HitTest( LPRECT pRect, LPPOINT pt );
	BOOL OnLButtonUp( UINT nFlags, LPPOINT pt );
	BOOL OnLButtonDown( UINT nFlags, LPPOINT pt );
	BOOL OnMouseMove( UINT nFlags, LPPOINT pt );
	BOOL Draw( LPSLANTTOOLBARINFO psti, HDC hDC, LPRECT pRect, BOOL bVert );
	BOOL Draw( HWND hWnd, HDC hDC, LPRECT pRect );
	BOOL Add( UINT uCmd, HICON hIcon, LPCTSTR pText, LPCTSTR pTip, LPBOOL pBool, HICON hPressed, DWORD flags, HICON hFlash );
	CDrawSlantToolbar();
	virtual ~CDrawSlantToolbar();

	virtual DWORD GetObjSize() { return sizeof( SLANTTOOLBARINFO ); }

	void ClearMouseOver() { m_pMouseOver = NULL; }

	CWinDc					m_bck;

	void SetMessageTarget( HWND hWnd, UINT uMsgCmd, UINT uMsgUpdate )
	{	m_hWnd = hWnd; m_uMsgCmd = uMsgCmd; m_uMsgUpdate = uMsgUpdate; }

	LRESULT SendCommand( WPARAM wParam, LPARAM lParam )
	{	if ( m_hWnd != NULL && m_uMsgCmd != 0 && ::IsWindow( m_hWnd ) )
		return SendMessage( m_hWnd, m_uMsgCmd, wParam, lParam ); return 0; }

	LRESULT SendUpdate( WPARAM wParam, LPARAM lParam )
	{	if ( m_hWnd != NULL && m_uMsgUpdate != 0 && ::IsWindow( m_hWnd ) )
		return SendMessage( m_hWnd, m_uMsgUpdate, wParam, lParam ); return 0; }

	void TextArea( BOOL b, LPCTSTR pTitle = NULL ) 
	{	m_bTextArea = b; SetTitle( pTitle ); }

	LPSLANTTOOLBARINFO GetMouseOver() { return m_pMouseOver; }
	LPSLANTTOOLBARINFO GetClicked() { return m_pClicked; }

	void SetTitle( LPCTSTR pTitle )
	{	if ( pTitle == NULL ) *m_szTitle = 0;
		else strcpy_sz( m_szTitle, pTitle );
	}

	void SetMinTextArea( long min ) { m_lMinTextArea = min; }

	void HiLight( BOOL b ) { m_bHiLight = b; }
	void HiLightBtn( BOOL b ) { m_bHiLightBtn = b; }

	void SetButtonSlant( long s ) { m_lButtonSlant = s; }
	long GetButtonSlant() { return m_lButtonSlant; }

	void ForceTooltips( BOOL b ) { m_bForceTooltips = b; }

	void SetScreenPos( long x, long y ) { m_screenx = x; m_screeny = y; }

	DWORD GetNumFlash() { return m_lstFlash.Size(); }

	COLORREF GetBckColor() { return m_rgbBck; }
	COLORREF GetDkBorderColor() { return m_rgbDkBorder; }
	COLORREF GetLtBorderColor() { return m_rgbLtBorder; }
	COLORREF GetSelectionColor() { return m_rgbSelection; }
	COLORREF GetTextColor() { return m_rgbText; }

	void DestoryTooltips() { m_tooltip.Destroy(); }

private:

	RECT					m_rect;

	LPSLANTTOOLBARINFO		m_pClicked;
	LPSLANTTOOLBARINFO		m_pMouseOver;

	BOOL					m_bFlashState;
	THList< LPSLANTTOOLBARINFO, SFlashInfo > m_lstFlash;

	HWND					m_hWnd;
	UINT					m_uMsgCmd;
	UINT					m_uMsgUpdate;

	BOOL					m_bTextArea;

	CTooltip				m_tooltip;

	char					m_szTitle[ 256 ];

	long					m_lMinTextArea;

	BOOL					m_bHiLight;
	BOOL					m_bHiLightBtn;

	long					m_lButtonSlant;

	BOOL					m_bForceTooltips;
	BOOL					m_bCaptionMode;

	long					m_screenx;
	long					m_screeny;

	COLORREF				m_rgbBck;
	COLORREF				m_rgbLtBorder;
	COLORREF				m_rgbDkBorder;
	COLORREF				m_rgbText;
	COLORREF				m_rgbSelection;
};

#endif // !defined(AFX_DRAWSLANTTOOLBAR_H__D7832D1D_84F7_4F91_A1A1_893F312898B9__INCLUDED_)
