// HttpSession.h: interface for the CHttpSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPSESSION_H__AD6394C0_82DF_42F6_A7CB_5E2C66596621__INCLUDED_)
#define AFX_HTTPSESSION_H__AD6394C0_82DF_42F6_A7CB_5E2C66596621__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class CHttpSession : public CISession
{
public:
	enum
	{ 	HTTP_OK = 200,
		HTTP_CREATED = 201,
		HTTP_ACCEPTED = 202,
		HTTP_NON_AUTHORITATIVE_INFORMATION = 203,
		HTTP_NO_CONTENT = 204,
		HTTP_RESET_CONTENT = 205,
		HTTP_PARTIAL_CONTENT = 206,

		HTTP_MULTIPLE_CHOICES = 300,
		HTTP_MOVED_PERMANENTLY = 301,
		HTTP_FOUND = 302,
		HTTP_SEE_OTHER = 303,
		HTTP_NOT_MODIFIED = 304,
		HTTP_USE_PROXY = 305,
		HTTP_TEMPORARY_REDIRECT = 307,

		HTTP_BAD_REQUEST = 400,
		HTTP_PAYMENT_REQUIRED = 402,
		HTTP_FORBIDDEN = 403,
		HTTP_NOT_FOUND = 404,
		HTTP_METHOD_NOT_ALLOWED = 405,
		HTTP_NOT_ACCEPTABLE = 406,
		HTTP_PROXY_AUTHENTICATION_REQUIRED = 407,
		HTTP_REQUEST_TIMEOUT = 408,
		HTTP_CONFLICT = 409,
		HTTP_GONE = 410,
		HTTP_LENGTH_REQUIRED = 411,
		HTTP_PRECONDITION_FAILED = 412,
		HTTP_REQUEST_ENTITY_TOO_LARGE = 413,
		HTTP_REQUEST_URI_TOO_LONG = 414,
		HTTP_UNSUPPORTED_MEDIA_TYPE = 415,
		HTTP_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
		HTTP_EXPECTED_FAILURE = 417,

		HTTP_SERVER_ERROR = 500,
		HTTP_NOT_IMPLEMENTED = 501,
		HTTP_BAD_GATEWAY = 502,
		HTTP_SERVICE_UNAVAILABLE = 503,
		HTTP_GATEWAY_TIMEOUT = 504,
		HTTP_VERSION_NOT_SUPPORTED = 505,
	};

	enum
	{	HTTP_REQUEST_TYPE_GET = 1,
		HTTP_REQUEST_TYPE_POST,
		HTTP_REQUEST_TYPE_HEAD
	};

	struct SHttpRequest
	{
		SHttpRequest() 
		{	dwType = 0; 
			dVersion = 0;
			dwHttpError = HTTP_OK;
			ZeroMemory( &ui, sizeof( ui ) );
		}
		
		void Reset()
		{	dwType = 0; 
			dVersion = 0;
			dwHttpError = HTTP_OK;
			ZeroMemory( &ui, sizeof( ui ) );
			sDoc = "";
			sFile = "";
			varGet.Destroy();
			varPost.Destroy();
			varVariables.Destroy();
			varHeader.Destroy();
		}

		DWORD			dwType;
		double			dVersion;

		DWORD			dwHttpError;
		std::string		sHttpError;

		URLINFO			ui;

		std::string		sDoc;
		std::string		sFile;

		CVar			varGet;
		CVar			varPost;

		CVar			varVariables;

		CVar			varHeader;

	}; typedef SHttpRequest* LPSHttpRequest;

public:
	BOOL SetContentType( LPCTSTR pExt );
	BOOL SendReply();
	BOOL WriteHeaderItem( LPCTSTR pName, LPCTSTR pValue );
	void DefaultHeader();
	virtual BOOL OnProcessRequest();

	DWORD ParseHeader( LPBYTE pBuf, DWORD dwBytes );
			
	BOOL SendHeader();
	BOOL SendContentType(LPCTSTR pExt);
	BOOL SendErrorMsg( DWORD dwError, LPCTSTR pMsg );
	BOOL IsHeaderComplete( LPBYTE pBuf, DWORD dwSize, LPDWORD pdwHeaderSize );
	virtual BOOL OnProcessRead();
	
	static LPCTSTR GetErrString(DWORD err);

	CHttpSession();
	virtual ~CHttpSession(); 

	void SetHttpError( DWORD dwErr, LPCTSTR pString ) 
	{	m_hr.dwHttpError = dwErr; m_hr.sHttpError = pString; }
	DWORD GetHttpError() { return m_hr.dwHttpError; }
	
	void ClearContent() { m_bufContent.Destroy(); }
	void WriteContent( LPVOID pBuf, DWORD dwSize ) { m_bufContent.Write( pBuf, dwSize ); }
	void WriteContent( LPCTSTR pBuf ) { m_bufContent.Write( pBuf ); }

	void SetServerName( LPCTSTR pServerName ) { m_sServerName = pServerName; }

	/// Resets connection and prepares for another request
	void ResetConnection();

	/// HTTP request details
	SHttpRequest				m_hr;

private:

	/// Non-zero if headers have been recieved
	BOOL						m_bHeaderReceived;

	/// Non-zero if conneciton should persist
	BOOL						m_bPersist;

	/// Holds reply content
	CCircBuf					m_bufContent;

	/// The server name
	std::string					m_sServerName;

};

#endif // !defined(AFX_HTTPSESSION_H__AD6394C0_82DF_42F6_A7CB_5E2C66596621__INCLUDED_)
