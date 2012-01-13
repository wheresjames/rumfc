// HttpSession.cpp: implementation of the CHttpSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHttpSession::CHttpSession()
{
	m_sServerName = "CHttpSession Web Server";

	SetAutoTx( FALSE );
	m_bHeaderReceived = FALSE;
}

CHttpSession::~CHttpSession()
{

}

BOOL CHttpSession::OnProcessRead()
{
	DWORD dwBytes = 0;
	BYTE buf[ 32 * 1024 ] = { 0 };

	// See if any data is waiting
	if ( !Rx().Peek( buf, sizeof( buf ), &dwBytes ) || !dwBytes )
		return FALSE;

	if ( !m_bHeaderReceived )
	{
		// Wait for the full data
		DWORD dwErr = ParseHeader( buf, dwBytes );
		if ( !dwErr ) return FALSE;
		
		// Everything go ok?
		if ( dwErr != HTTP_OK ) 
		{
			// Send error message
			SendErrorMsg( dwErr, "Error Processing Request" );

			// Start the tx'er
			StartTx();

		} // end if

	} // end if

	DWORD dwContentSize = m_hr.varVariables.GetDword( "Content-Length" );

	LPCTSTR pStr = m_hr.varVariables.GetStr( "User-Agent" );

	// Dump the rx buffer if no content
//	if ( dwContentSize == 0 ) Rx().Destroy();

	// Wait for content if needed
	if ( 0 < dwContentSize && Rx().GetMaxRead() < dwContentSize )
		return FALSE;

	// Read post variables if needed
	if ( m_hr.dwType == HTTP_REQUEST_TYPE_POST )
	{	TMem< BYTE > mem; DWORD dwRead = 0;
		if (	mem.allocate( dwContentSize + 1 ) && 
				Rx().Read( mem.ptr(), dwContentSize, &dwRead ) )
		{	LPBYTE ptr = mem.ptr();
			ptr[ dwRead ] = 0;
			m_hr.varPost.ReadInline( ptr, dwRead );
		} // end if

		// Skip post variables
		Rx().AdvanceReadPtr( dwContentSize );

	} // end if

	// Initialize header values
	DefaultHeader();

	// Persistent connection?
	BOOL bPersist = FALSE;
	LPCTSTR pCon = m_hr.varVariables.GetStr( "Connection" );
	if ( !pCon || !*pCon ) pCon = m_hr.varVariables.GetStr( "connection" );
	if ( pCon && *pCon )
		if ( strstr( pCon, "Keep-Alive" ) || strstr( pCon, "keep-alive" ) )
			bPersist = TRUE;

	bPersist = TRUE;
			
	// Signal that we intend to keep the session alive
	if ( bPersist )
		m_hr.varHeader.AddVar( "Connection", "Keep-Alive" );

	// Process the request
	if ( OnProcessRequest() )
	{
		// Send page
		if ( m_hr.dwHttpError == HTTP_OK ) SendReply();

		// Send the error message
		else SendErrorMsg( m_hr.dwHttpError, m_hr.sHttpError.c_str() );

		// Start the tx'er
		StartTx();

	} // end if

	// Prepare connection
	if ( !bPersist )
		Done();
	else if ( m_hr.dwType != HTTP_REQUEST_TYPE_POST )
		Rx().AdvanceReadPtr( dwContentSize );

	return TRUE;
}

DWORD CHttpSession::ParseHeader(LPBYTE pBuf, DWORD dwBytes)
{
	// Have we already received the header?
	if ( m_bHeaderReceived ) return HTTP_OK;

	// Do we have a full command yet?
	DWORD dwHeaderSize = 0;
	if ( !IsHeaderComplete( pBuf, dwBytes, &dwHeaderSize ) ) 
		return 0;

	// This will never happen...
	if ( dwHeaderSize == 0 ) return HTTP_BAD_REQUEST;

	DWORD i = 0;

	// GET/POST type
	if ( !strnicmp( (char*)&pBuf[ i ], "GET ", 3 ) ) m_hr.dwType |= HTTP_REQUEST_TYPE_GET;
	else if ( !strnicmp( (char*)&pBuf[ i ], "HEAD ", 4 ) ) m_hr.dwType |= HTTP_REQUEST_TYPE_HEAD;
	else if ( !strnicmp( (char*)&pBuf[ i ], "POST ", 4 ) ) m_hr.dwType |= HTTP_REQUEST_TYPE_POST;
	else return HTTP_BAD_REQUEST;

	// Skip token
	i = CReg::NextToken( pBuf, dwHeaderSize, i );

	// Read in the document name
	char szToken[ 4096 ] = { 0 };
	i = CReg::GetNonWhiteSpace( (LPBYTE)szToken, sizeof( szToken ), pBuf, dwHeaderSize - i, i );

	// Decode document name
//	DWORD out = CCfgFile::GetMinDeCanonicalizeBufferSize( strlen( szToken ) );
//	if ( !out ) return HTTP_BAD_REQUEST;
	
//	TMem< char > decon;
//	if ( !decon.allocate( out + 1 ) ) return HTTP_SERVER_ERROR;

//	DWORD sz = strlen( szToken );
//	if ( CCfgFile::DeCanonicalizeBuffer( szToken, (LPBYTE)decon.ptr(), decon.size(), &sz ) )
//		m_hr.sDoc = decon.ptr();

	m_hr.sDoc = szToken;

	DWORD out = strlen( szToken );
	LPBYTE ptr = (LPBYTE)szToken; //(LPBYTE)decon.ptr();
	DWORD k, p = MAXDWORD;
	for ( k = 0; k < out && p == MAXDWORD && ptr[ k ] != 0; k++ )
		if ( ptr[ k ] == '?' ) p = k + 1;

	if ( p != MAXDWORD )
	{	m_hr.varGet.ReadInline( (LPBYTE)&ptr[ p ], out - p );
		if ( p && ptr[ p - 1 ] == '?' ) ptr[ p - 1 ] = 0;
	} // end if

	// Save file name
	m_hr.sFile = (char*)ptr;

	// Break down URL information
	CWinFile::CrackUrl( m_hr.sDoc.c_str(), &m_hr.ui );

	// Ensure HTTP request type
	i = CReg::SkipWhiteSpace( pBuf, dwHeaderSize, i );				
	if ( strnicmp( (char*)&pBuf[ i ], "HTTP", 4 ) )
		return HTTP_BAD_REQUEST;

	// Skip the HTTP type
	i += 5;

	// Read in the version number
	m_hr.dVersion = strtod( (LPCTSTR)&pBuf[ i ], NULL );

	// Skip to next line
	i = CReg::NextLine(	pBuf, dwHeaderSize - i, i );

	// Read in variables if any
	if ( dwHeaderSize > i ) m_hr.varVariables.ReadMIME( &pBuf[ i ], dwHeaderSize - i );

	// We are done with the header
	m_bHeaderReceived = TRUE;

	// Skip the header
	Rx().AdvanceReadPtr( dwHeaderSize );

	return HTTP_OK;
}


BOOL CHttpSession::IsHeaderComplete(LPBYTE pBuf, DWORD dwSize, LPDWORD pdwHeaderSize)
{
	if ( !dwSize ) return FALSE;

	// Search for end of command string '\n\n'
	for ( DWORD i = 0; i < ( dwSize - 3 ); i++ )
		if ( pBuf[ i ] == '\r' && pBuf[ i + 1 ] == '\n' &&
			 pBuf[ i + 2 ] == '\r' && pBuf[ i + 3 ] == '\n' )
		{
			// NULL terminate command
			pBuf[ i ] = 0;

			// Save header length
			if ( pdwHeaderSize ) *pdwHeaderSize = i + 3;

			return TRUE;

		} // end if

	return FALSE;
}

BOOL CHttpSession::SetContentType(LPCTSTR pExt)
{
	char type[ 256 ];
	if ( !CMime::GetContentType( pExt, type ) )
		strcpy( type, "text/html" );

	m_hr.varHeader.AddVar( "Content-type", type );

	return TRUE;
}


BOOL CHttpSession::SendErrorMsg(DWORD dwError, LPCTSTR pMsg)
{	
	// Set the error code
	SetHttpError( dwError, std::string( pMsg ).c_str() );

	// Data type
	SetContentType( "htm" );

	// Write out the body
	WriteContent( "<HTML> <BODY><p><big><big>" );
	WriteContent( m_hr.sHttpError.c_str() );
	WriteContent( "</big></big></p> </BODY></HTML>" );

	// Send it
	SendReply();

	return TRUE;
}

LPCTSTR CHttpSession::GetErrString(DWORD err)
{
	switch( err )
	{
		case HTTP_OK :
			return "HTTP/1.1 200 OK\r\n";
			break;

		case HTTP_FORBIDDEN :
			return "HTTP/1.1 403 Forbidden\r\n";
			break;
		
		case HTTP_NOT_FOUND :
			return "HTTP/1.1 404 Document Not Found\r\n";
			break;

		case HTTP_SERVER_ERROR :
		default :
			return "HTTP/1.1 500 Server Error\r\n";
			break;
	}

	return "HTTP/1.1 500 Server Error\r\n";
}


BOOL CHttpSession::OnProcessRequest()
{
	// This function should be overridden...
	SendErrorMsg( HTTP_NOT_IMPLEMENTED, "Not Implemented" );

	return TRUE;
}


void CHttpSession::DefaultHeader()
{
	// Lose old header values
	m_hr.varHeader.Destroy();

	// Set the server name
	m_hr.varHeader.AddVar( "Server", m_sServerName.c_str() );

	// Set the current time
	CWinTime wt( NULL ); char str[ 256 ];
	wt.GetSystemTime();
	wt.GetString( str, "%w, %d %b %Y %g:%m:%s GMT" );
	m_hr.varHeader.AddVar( "Date", str );

	// Last modified
	m_hr.varHeader.AddVar( "Last-modified", "" );

}

BOOL CHttpSession::WriteHeaderItem(LPCTSTR pName, LPCTSTR pValue)
{
	if ( !pName || !pValue ) return FALSE;

	// Write out the value
	Write( pName );		Write( ":" );
	Write( pValue );	Write( "\r\n" );

	return TRUE;
}


BOOL CHttpSession::SendHeader()
{
	// Write the id string
	Write( GetErrString( m_hr.dwHttpError ) );

	// Write all header values
	LPVAR pv = NULL;
	char value[ 1024 ] = { 0 };
	while ( ( pv = (LPVAR)m_hr.varHeader.GetNext( pv ) ) != NULL )
		if ( m_hr.varHeader.GetVar( pv->name, value, sizeof( value ) ) )
			WriteHeaderItem( pv->name, value );

	// End of header
	Write( "\r\n" );

	return TRUE;
}


BOOL CHttpSession::SendReply()
{
	// How big is the data?
	m_hr.varHeader.AddVar( "Content-length", m_bufContent.GetMaxRead() );

	// Send the header
	SendHeader();

	// Write out the content
	BYTE buf[ 1024 ]; DWORD dwRead = 0;
	while ( m_bufContent.Read( buf, sizeof( buf ), &dwRead ) && dwRead )
		Write( buf, dwRead );

	return TRUE;

}
