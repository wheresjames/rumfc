// ISession.cpp : implementation file
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

#define ISession_MAXBUFFER		1024

/////////////////////////////////////////////////////////////////////////////
// CISession

CISession::CISession()
#ifdef ENABLE_SOCKETS
	: CAsyncWinSock()
{
	EnableCallbacks();
#else
{
#endif

	m_bDone = FALSE;

	m_bProcessingRx = FALSE;
	m_bBlockingTx = FALSE;

	// Not connected so far
	m_dwConnectStatus = ISSTATUS_NOTCONNECTED;

	m_bAutoTx = TRUE;

	m_pUserList = NULL;
}

CISession::~CISession()
{	

#ifdef ENABLE_SOCKETS

	Destroy();

#else

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

}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CISession, CAsyncSocket)
	//{{AFX_MSG_MAP(CISession)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CISession member functions

#ifdef ENABLE_SOCKETS
BOOL CISession::OnRead(int nErr) 
#else
void CISession::OnReceive(int nErr)
#endif
{
	// For some reason, active-x controls seem to re-enter here...
	if ( m_bProcessingRx ) _SOCKET_RETURN( FALSE );
	m_bProcessingRx = TRUE;

	// Receive the bytes
	BYTE buf[ ISession_MAXBUFFER ];

#ifdef ENABLE_SOCKETS

	DWORD dwBytes;
	while ( ( dwBytes = Recv( buf, sizeof( buf ) ) ) > 0 && dwBytes <= sizeof( buf ) )

#else

	DWORD dwBytes = Receive( buf, sizeof( buf ) );
	if ( dwBytes > 0 && dwBytes <= sizeof( buf ) )

#endif
	{
		// Save into the rx buffer
		m_bufRx.Write( buf, dwBytes );

		// Process the new data
		OnProcessRead();

	} // end if

	// Clear re-entrancy flag
	m_bProcessingRx = FALSE;

	_SOCKET_RETURN( TRUE );
}

BOOL CISession::OnTx()
{
	// Must have a valid socket
	if ( !IsConnected() ) return FALSE;

	// Lock the transmit buffer
	CTlLocalLock ll( m_bufTx );
	if ( !ll.IsLocked() ) return FALSE;

	// Punt if we're blocking
	if ( m_bBlockingTx ) return TRUE;

	BYTE buf[ 1024 ];
	DWORD dwReady;

	// Read blocks of data
	while ( m_bufTx.Peek( buf, sizeof( buf ), &dwReady ) )
	{
		// Send the data on
		int res = Send( buf, dwReady );

		// Check for socket error
		if ( res == 0 || res == SOCKET_ERROR || (DWORD)res > dwReady ) 
		{
			// Check for blocking
			if ( GetLastError() == WSAEWOULDBLOCK )
			{	m_bBlockingTx = TRUE;
				return TRUE;
			} // end if

/*			// Get the error message
			DWORD dwError = GetLastError();
			char szErr[ CWF_STRSIZE ] = { 0 };
			CWin32::GetSystemErrorMsg( dwError, szErr );
			_vLOG( _ES_ERROR, dwError, "Socket::Send - %s", szErr );
*/
			return FALSE;

		} // end if

		// Remove the number of bytes sent from the buffer
		m_bufTx.AdvanceReadPtr( res );

		// Punt if tx'er is full
		if ( (DWORD)res != dwReady ) 
			return TRUE;

	} // end while

	// Disconnect if we're done
	if ( m_bDone ) 
		Disconnect();

	return TRUE;
}

#ifdef ENABLE_SOCKETS
BOOL CISession::OnWrite(int nErr) 
#else
void CISession::OnSend(int nErr)
#endif
{
	// Lock the transmit buffer
	CTlLocalLock ll( m_bufTx );
	if ( !ll.IsLocked() ) _SOCKET_RETURN( FALSE );

	// Not blocking now
	m_bBlockingTx = FALSE;

	// Send more data
	OnTx();

	_SOCKET_RETURN( TRUE );
}

#ifdef ENABLE_SOCKETS
BOOL CISession::OnConnect(int nErr) 
#else
void CISession::OnConnect(int nErr)
#endif
{
	// Update connect status
	if ( !nErr )
	{
		m_dwConnectStatus = ISSTATUS_CONNECTED;

	} // end if
	else m_dwConnectStatus = ISSTATUS_DISCONNECTED;

	// Start any queued data
	OnTx();

	_SOCKET_RETURN( TRUE );
}

#ifdef ENABLE_SOCKETS
BOOL CISession::OnClose(int nErr) 
#else
void CISession::OnClose(int nErr)
#endif
{
	m_dwConnectStatus = ISSTATUS_DISCONNECTED;

	_SOCKET_RETURN( TRUE );
}


BOOL CISession::OnInitSession()
{
	// Set connecting status
	m_dwConnectStatus = ISSTATUS_CONNECTING;

	return TRUE;
}

BOOL CISession::Disconnect()
{
#ifdef ENABLE_SOCKETS
	Destroy();
#else
	Close();
#endif

	return TRUE;
}

void CISession::Done()
{
	m_bDone = TRUE; 

	if ( !Tx().GetMaxRead() ) 
		Disconnect(); 
}

BOOL CISession::Write(LPVOID pBuf, DWORD dwSize)
{
	// Save in the tx buffer
	m_bufTx.Write( pBuf, dwSize );

	// Start tx'er if needed
	if ( m_bAutoTx && !m_bBlockingTx ) OnTx();

	return TRUE;
}

BOOL CISession::Read(LPVOID pBuf, DWORD dwMax, LPDWORD pdwRead)
{
	return m_bufRx.Read( pBuf, dwMax, pdwRead );
}

BOOL CISession::Write(LPCTSTR pString)
{	return Write( (LPVOID)pString, (DWORD)strlen( pString ) );
}

BOOL CISession::StartTx()
{	
	// Anything to send?
	if ( !m_bufTx.GetMaxRead() ) return FALSE;

	// Start the tx'er
	if ( !m_bBlockingTx ) OnTx();

	return TRUE;
}
