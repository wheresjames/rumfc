#pragma once


// CNoDrawTabControl

class CNoDrawTabControl : public CTabCtrl
{
	DECLARE_DYNAMIC(CNoDrawTabControl)

public:
	CNoDrawTabControl();
	virtual ~CNoDrawTabControl();

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();

    void SetRefresh( CWnd *pRefresh )
    {   m_pRefresh = pRefresh; }

private:

    CWnd        *m_pRefresh;
};


