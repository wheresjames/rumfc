#if !defined(AFX_ISESSION_H__7F916370_87AF_4848_8AE8_C114E3DC6E50__INCLUDED_)
#define AFX_ISESSION_H__7F916370_87AF_4848_8AE8_C114E3DC6E50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ISession.h : header file
//

#define ISSTATUS_NOTCONNECTED		0
#define ISSTATUS_CONNECTED			1
#define ISSTATUS_CONNECTING			2
#define ISSTATUS_DISCONNECTED		3

/////////////////////////////////////////////////////////////////////////////
// CISession command target

class CISession : 
#ifdef ENABLE_SOCKETS
	public CAsyncWinSock
#else
	public CAsyncSocket
#endif
{

// Attributes
public:

// Operations
public:

	CISession();
	virtual ~CISession();

	virtual BOOL OnTx();

	virtual BOOL OnProcessRead() { return FALSE; }

	virtual BOOL OnInitSession();

	virtual BOOL Disconnect();

	virtual BOOL IsValid() 
	{	
		// What's our connection status?
		return m_dwConnectStatus != ISSTATUS_DISCONNECTED; 
	}

	BOOL IsConnecting()
	{	return GetSocket() != INVALID_SOCKET && m_dwConnectStatus == ISSTATUS_CONNECTING; }

	BOOL IsConnected() 
	{	return GetSocket() != INVALID_SOCKET && m_dwConnectStatus == ISSTATUS_CONNECTED; }

	CCircBuf& Rx() { return m_bufRx; }
	CCircBuf& Tx() { return m_bufTx; }

	void SetAutoTx( BOOL b ) { m_bAutoTx = b; }
	BOOL GetAutoTx() { return m_bAutoTx; }

	void Done(); 

	BOOL IsDone() { return ( m_bDone && !Tx().GetMaxRead() ); }

	BOOL IsTimeout() { return m_dwTime < time( 0 ); }

// Overrides
public:

#ifdef ENABLE_SOCKETS

	virtual BOOL OnRead( int nErr );
	virtual BOOL OnWrite( int nErr );
	virtual BOOL OnConnect( int nErr );
	virtual BOOL OnClose( int nErr );

#else

	SOCKET GetSocket() { return m_hSocket; }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CISession)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CISession)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

#endif

public:
	BOOL StartTx();
	BOOL Write( LPCTSTR pString );
	BOOL Read( LPVOID pBuf, DWORD dwMax, LPDWORD pdwRead );
	BOOL Write( LPVOID pBuf, DWORD dwSize );

	void SetUserList( CReg *pUserList ) { m_pUserList = pUserList; }
	CReg* GetUserList() { return m_pUserList; }

// Implementation
protected:

private:

	CCircBuf								m_bufRx;
	CCircBuf								m_bufTx;

	DWORD									m_dwConnectStatus;

	BOOL									m_bBlockingTx;
	BOOL									m_bProcessingRx;

	BOOL									m_bAutoTx;

	BOOL									m_bDone;

	CReg									*m_pUserList;

	DWORD									m_dwTime;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISESSION_H__7F916370_87AF_4848_8AE8_C114E3DC6E50__INCLUDED_)
