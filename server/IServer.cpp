// NcServer.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef ENABLE_SOCKETS
#	define _SOCKET_RETURN( val ) return val;
#else
#	define _SOCKET_RETURN( val ) return;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIServer

CIServer::CIServer()  
#ifdef ENABLE_SOCKETS
	: CAsyncWinSock()
{
	EnableCallbacks();
#else
{
#endif

	m_pUserList = NULL;
	m_tSessionFactory = NULL;
}

CIServer::~CIServer()
{
	Stop();

	Destroy();

#ifndef ENABLE_SOCKETS

	try
	{
		// It would be nice if MS would do this internally but...
		AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
		if ( pState && pState->m_pmapSocketHandle )
		{
			// Ensure the socket is shutdown correctly
			// This avoids the CAsyncSocket bug
			if ( m_hSocket != INVALID_SOCKET && LookupHandle( m_hSocket ) )
			{	AsyncSelect( 0 ); ShutDown(); Close(); }

		} // end if

	} catch( ... ) {}
	m_hSocket = INVALID_SOCKET;

#endif

	// Lose the session factory
	if ( m_tSessionFactory ) 
	{	delete m_tSessionFactory;
		m_tSessionFactory = NULL;
	} // end if
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CIServer, CAsyncSocket)
	//{{AFX_MSG_MAP(CIServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CIServer member functions

#ifdef ENABLE_SOCKETS
BOOL CIServer::OnAccept(int nErr) 
#else
void CIServer::OnAccept(int nErr)
#endif
{
	// Acquire lock
	CTlLocalLock ll( *this );
	if ( !ll.IsLocked() ) _SOCKET_RETURN( FALSE );

	// Ensure sockets are initialized
	AfxSocketInit();

	// Cleanup old sessions
	Cleanup();

	// Create new session
	CISession* pNs = CreateSession();
	if ( pNs == NULL ) _SOCKET_RETURN( FALSE );

	// Set the user list pointer
	pNs->SetUserList( m_pUserList );

	// Accept the connection
	SOCKADDR saAddr; int iAddr = sizeof( saAddr );

	try
	{ 
#ifdef ENABLE_SOCKETS
		if ( !Accept( pNs, &saAddr, &iAddr ) ) _SOCKET_RETURN( FALSE );
#else
		if ( !Accept( *pNs, &saAddr, &iAddr ) ) _SOCKET_RETURN( FALSE );
#endif
	} catch( ... ) {}

	// Add this socket to the list
	GUID guidId; GetId( &guidId, &saAddr );
	if ( !m_lstSession.push_back( guidId, pNs ) )
	{	_PTR_DELETE( pNs ); _SOCKET_RETURN( FALSE ); }

	// Let the session know it's connected
	pNs->OnConnect( 0 );
	
	_SOCKET_RETURN( TRUE );
}

void CIServer::Destroy()
{
	m_lstSession.Destroy();

#ifdef ENABLE_SOCKETS
	CAsyncWinSock::Destroy();
#else
	Close();
#endif

}

BOOL CIServer::GetId(GUID *pguid, SOCKADDR *psaAddr)
{
	// Get in address
	SOCKADDR_IN * pAddrIn = (SOCKADDR_IN*)psaAddr; 	

	// Just use the address as an id
	ZeroMemory( pguid, sizeof( GUID ) );
	memcpy( pguid, &pAddrIn->sin_addr.S_un.S_addr, sizeof( DWORD ) );
	memcpy( &( (LPBYTE)pguid )[ sizeof( DWORD ) ], &pAddrIn->sin_port, sizeof( pAddrIn->sin_port ) );

	return TRUE;
}

void CIServer::Cleanup()
{
	THList< GUID, CISession >::iterator it = NULL;
	while ( ( it = m_lstSession.next( it ) ) != NULL )
	{
		// Lose this node if it's done
		if ( 
#ifndef ENABLE_SOCKETS
			(*it)->m_hSocket == INVALID_SOCKET || 
#endif
			(*it)->IsDone() )
		{	
#ifndef ENABLE_SOCKETS
			(*it)->Close(); 
#else
			(*it)->Destroy();
#endif
			it = m_lstSession.erase( it ); 
		}

	} // end while
}

CISession* CIServer::ConnectTo(LPCTSTR pServer, DWORD dwPort)
{
	// Acquire lock
	CTlLocalLock ll( *this );
	if ( !ll.IsLocked() ) return NULL;

	// Ensure sockets are initialized
	AfxSocketInit();

	// Cleanup old sessions
	Cleanup();

	// Does the session already exist?
	CISession* pNs = (CISession*)FindSession( pServer, dwPort );
	if ( pNs ) return pNs;

	// Get the connection id
	GUID guidId; SOCKADDR_IN sai;
	if ( !GetId( &guidId, pServer, dwPort, (SOCKADDR*)&sai ) )
		return NULL;
	
	// Create a session object
	pNs = CreateSession();
	if ( pNs == NULL ) return NULL;

	try
	{
		// Add session to the list
		if ( !m_lstSession.push_back( guidId, pNs ) )
		{	_PTR_DELETE( pNs ); return NULL; }
		
		// Connect to the client
#ifdef ENABLE_SOCKETS
		if ( !pNs->OpenSocket() ) return NULL;
#else
		if ( !pNs->Create() ) return NULL;
#endif

		// Attempt connection
		if ( !pNs->Connect( (SOCKADDR*)&sai, sizeof( sai ) ) )
			if ( GetLastError() != WSAEWOULDBLOCK )
				return NULL;

	} // end try

	catch( ... )
	{
		m_lstSession.erase( guidId );

		return NULL;

	} // end catch

	// Success?
	return pNs;
}

BOOL CIServer::GetId(GUID *pguid, LPCTSTR pServer, DWORD dwPort, SOCKADDR *psa)
{
	// Lookup the host name
	HOSTENT *phe;
	phe = gethostbyname( pServer );
	if ( phe == NULL ) return FALSE;

	// Save address information
	SOCKADDR_IN sai;
	if ( psa == NULL ) psa = (SOCKADDR*)&sai;
	SOCKADDR_IN *psai = (SOCKADDR_IN*)psa;

	ZeroMemory( &sai, sizeof( sai ) );
	psai->sin_family = AF_INET;
	psai->sin_port = htons( (WORD)dwPort );
	psai->sin_addr = *( (LPIN_ADDR)*phe->h_addr_list );

	return GetId( pguid, (SOCKADDR*)psai );
}


CISession* CIServer::FindSession(LPCTSTR pServer, DWORD dwPort)
{
	// Get the connection id
	GUID guidId;
	if ( !GetId( &guidId, pServer, dwPort ) )
		return NULL;

	return m_lstSession[ guidId ];
}

BOOL CIServer::Start( DWORD dwPort )
{
	// Ensure sockets are initialized
	AfxSocketInit();
	
	Stop();

#ifdef ENABLE_SOCKETS

	// Start the server if needed
	if ( dwPort )
		if ( !OpenSocket() || !Bind( dwPort ) || !Listen() )
			return FALSE;

#else

	// Start the server if needed
	if ( dwPort )
		if ( !Create( dwPort ) || !Listen() )
			return FALSE;

#endif

	return TRUE;
}

BOOL CIServer::Stop()
{
	// Lose client connections
	Destroy();
	
	return TRUE;
}

BOOL CIServer::CloseAllSessions()
{
	// Acquire lock
	CTlLocalLock ll( *this );
	if ( !ll.IsLocked() ) return FALSE;

	Destroy();

	return TRUE;
}


