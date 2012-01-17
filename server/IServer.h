#if !defined(AFX_ISERVER_H__00D112BE_0C42_438B_A00C_A9CE796D32E1__INCLUDED_)
#define AFX_ISERVER_H__00D112BE_0C42_438B_A00C_A9CE796D32E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IServer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIServer command target

class CIServer : 
#ifdef ENABLE_SOCKETS
	public CAsyncWinSock
#else
	public CAsyncSocket
#endif
{
// Attributes
public:

	typedef THList< GUID, CISession > t_sessionlist;

	class _TISessionFactory
	{
	public:
		virtual CISession* CreateSession() { return NULL; }
		virtual void DeleteSession( CISession* p ) { }
	};

	template < class T > class TISessionFactory : public _TISessionFactory
	{
	public:
		virtual CISession* CreateSession() { return new T; }
//		virtual void DeleteSession( CISession* p ) { delete (T*)p; }
	};

// Operations
public:
	CIServer();
	virtual ~CIServer();

// Overrides
public:
	void Cleanup();

	BOOL Stop();
	BOOL Start( DWORD dwPort = 0 );
	CISession* ConnectTo( LPCTSTR pServer, DWORD dwPort );
	BOOL CloseAllSessions();

	void SetUserList( CReg *pUserList ) { m_pUserList = pUserList; }
	CReg* GetUserList() { return m_pUserList; }

	CISession* FindSession( LPCTSTR pServer, DWORD dwPort );

	void SetSessionFactory( _TISessionFactory *pSf )
	{	m_tSessionFactory = pSf; }

	CISession* CreateSession()
	{	if ( m_tSessionFactory == NULL ) return NULL;
		return m_tSessionFactory->CreateSession();
	}

	t_sessionlist* GetSessionList() { return &m_lstSession; }

protected:

	HRESULT TCmdCloseAllSessions();
	HRESULT TCmdNewSession();
	HRESULT WaitCmd();
	BOOL Cmd( DWORD dwCmd );

	BOOL GetId( GUID *pguid, LPCTSTR pServer, DWORD dwPort, SOCKADDR *psa = NULL );
	BOOL GetId( GUID *pguid, SOCKADDR *psaAddr );

#ifdef ENABLE_SOCKETS

	virtual BOOL OnAccept( int nErr );

#else

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIServer)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CIServer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

#endif

	operator CTlLock*() { return &m_lock; }

// Implementation
protected:

	virtual void Destroy();

private:

	CTlLock						m_lock;

	DWORD						m_dwPort;

	CReg						*m_pUserList;

	_TISessionFactory			*m_tSessionFactory;

	t_sessionlist				m_lstSession;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISERVER_H__00D112BE_0C42_438B_A00C_A9CE796D32E1__INCLUDED_)
