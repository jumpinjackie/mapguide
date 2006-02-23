
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <winreg.h>
#include <winsock2.h>
#include <process.h>
#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <httpext.h>
#include <httpfilt.h>
#include <httpext.h>

#include "SAPI.h"

#include "fcgi_server.h"
#include "FCGIProcMgr.h"

////////////////////////////////////////////////////////////////
//
// A fairly generic ISAPI application with thread pooling
//
// TODO:
//
// Thread management to start or stop threads as needed by
// the servers current load
//
#define ISAPI_NAME "ISAPI_FASTCGI"

/* Variables available in IIS per request */
static char * aszServerVariables[] =
	    {"ALL_HTTP", "APPL_MD_PATH", "APPL_PHYSICAL_PATH", "AUTH_PASSWORD",
		"AUTH_TYPE", "AUTH_USER", "CERT_COOKIE", "CERT_FLAGS",
		"CERT_ISSUER", "CERT_KEYSIZE", "CERT_SECRETKEYSIZE",
		"CERT_SERIALNUMBER", "CERT_SERVER_ISSUER",
		"CERT_SERVER_SUBJECT", "CERT_SUBJECT", /*"CONTENT_LENGTH",*/
		/*"CONTENT_TYPE",*/ "HTTP_ACCEPT", "HTTPS", "HTTPS_KEYSIZE",
		"HTTPS_SECRETKEYSIZE", "HTTPS_SERVER_ISSUER",
		"HTTPS_SERVER_SUBJECT", "INSTANCE_ID", "INSTANCE_META_PATH",
		/*"PATH_INFO", "PATH_TRANSLATED",*/ "QUERY_STRING",
		"REMOTE_ADDR", "REMOTE_HOST", "REMOTE_USER",
		/*"REQUEST_METHOD",*/ "SCRIPT_NAME", "SERVER_NAME",
		"SERVER_PORT", "SERVER_PORT_SECURE", "SERVER_PROTOCOL",
		"SERVER_SOFTWARE", "URL"};

/* comment out to dissable thread pooling */
#define USE_THREAD_POOL

#ifdef USE_THREAD_POOL
HANDLE*	m_phThreads;
HANDLE	m_hIoPort;
long	m_nThreadCount;
static const long THREAD_COUNT = 10; // Number of threads to create in pool.
#endif
unsigned long	m_nMaxPostSize = 0;
#define MAX_CUSTOM_VARS 50
char    *m_szCustomVars[MAX_CUSTOM_VARS];
char    m_szCustomVarsBuf[1024];
DWORD    m_nCountCustomVars = 0;
unsigned long	m_nBypassAuth = 0;
int bFilterLoaded = 0;
#define MAX_BUF_SIZE (49152) // Max number of bytes for each read - 48K

#define DO_ASYNC_READ
VOID WINAPI
AsyncReadClientIoCompletion(
            IN LPEXTENSION_CONTROL_BLOCK pECB,
            IN PVOID pContext,
            IN DWORD cbIO,
            IN DWORD dwError);


BOOL SendError(EXTENSION_CONTROL_BLOCK *pecb, CHAR *pszStatus, CHAR *pszMessage)
{
    BOOL  fRet = FALSE;
	char *header = "Content-Type: text/html\r\n\r\n";
	DWORD dwSize = strlen( header );
    fRet = pecb->ServerSupportFunction( pecb->ConnID,
                                        HSE_REQ_SEND_RESPONSE_HEADER,
                                        pszStatus,
                                        &dwSize,
                                        (LPDWORD) header );
    if (fRet) {
        dwSize = strlen( pszMessage );
        fRet = pecb->WriteClient( pecb->ConnID, pszMessage, &dwSize, 0);
    }
    return fRet;
}

/* create an environment for fastcgi to send */
inline void _prepare_environment(sapi_request_info *req_info)
{
	char buf[1024];
	char http_all[2048] = {0};
	DWORD dwHttpAll = sizeof(http_all);
	LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK) req_info->server_context;
	DWORD dwNumVars = ( sizeof aszServerVariables )/( sizeof aszServerVariables[0] );
	DWORD szEnv = dwNumVars + m_nCountCustomVars + 6; /* 6 is the number of special vars below */
	DWORD count = 0;

	if (lpECB->GetServerVariable( 
                lpECB->ConnID, 
                "ALL_HTTP", 
                http_all, 
                &dwHttpAll) && http_all[0] != 0) {
			char *p = http_all;
			while (*p) {
				char *eol = strchr(p, '\n');
				if (!eol) break;
				p = eol + 1;
				++szEnv;
			}
	}

	req_info->env = (char **)calloc(szEnv, sizeof(char *));

	if (http_all[0]) {
		char *p = http_all;
		while (*p) {
			char *colon = strchr(p, ':');
			char *eol = strchr(p, '\n');
			if (!colon || !eol) break;
			*colon = '=';
			*eol = 0;
			if (*(eol-1) == '\r') *(eol-1)=0;
			_snprintf(buf, sizeof(buf)-1,"%s",p);
			req_info->env[count++] = strdup(buf);
			p = eol + 1;
		}
	}

	/* special vars */
	if (lpECB->lpszMethod[0] != 0) {
		_snprintf(buf, sizeof(buf)-1, "REQUEST_METHOD=%s",lpECB->lpszMethod);
		req_info->env[count++] = strdup(buf);
	}

	if (lpECB->lpszQueryString[0] != 0) {
		_snprintf(buf, sizeof(buf)-1, "QUERY_STRING=%s",lpECB->lpszQueryString);
		req_info->env[count++] = strdup(buf);
	}

	if (lpECB->lpszPathInfo[0] != 0) {
		_snprintf(buf, sizeof(buf)-1, "PATH_INFO=%s",lpECB->lpszPathInfo);
		req_info->env[count++] = strdup(buf);
	}

	if (lpECB->lpszPathTranslated[0] != 0) {
		_snprintf(buf, sizeof(buf)-1, "PATH_TRANSLATED=%s",lpECB->lpszPathTranslated);
		req_info->env[count++] = strdup(buf);
	}

	_snprintf(buf, sizeof(buf)-1,"CONTENT_LENGTH=%d",lpECB->cbTotalBytes);
	req_info->env[count++] = strdup(buf);

	if (lpECB->lpszContentType[0] != 0) {
		_snprintf(buf, sizeof(buf)-1,"CONTENT_TYPE=%s",lpECB->lpszContentType);
		req_info->env[count++] = strdup(buf);
	}

	/* other vars */
	DWORD x;
	DWORD dwBuffSize;
	char szValue[1024];
	char *envname;
	for (x = 0; x < dwNumVars; x++ ) {
		dwBuffSize = sizeof(szValue);
        szValue[0] = '\0';
		envname = aszServerVariables[x];
		if (lpECB->GetServerVariable( 
                lpECB->ConnID, 
                envname, 
                szValue, 
                &dwBuffSize) && szValue[0] != 0) {
			_snprintf(buf, sizeof(buf)-1,"%s=%s",envname,szValue);
			req_info->env[count++] = strdup(buf);
		}
    }

	/* custom vars */
	for (x = 0; x < m_nCountCustomVars; x++ ) {
		dwBuffSize = sizeof(szValue);
        szValue[0] = '\0';
		envname = aszServerVariables[x];
		if (lpECB->GetServerVariable( 
                lpECB->ConnID, 
                envname, 
                szValue, 
                &dwBuffSize) && szValue[0] != 0) {
			_snprintf(buf, sizeof(buf)-1,"%s=%s",envname,szValue);
			req_info->env[count++] = strdup(buf);
		}
	}
	req_info->env[count] = NULL;
}

/* pre-read post data into a temp file 
	TODO: this could use async reading to lower cpu usage and improve performance
*/
int _prepare_postdata(sapi_request_info *req_info)
{
	LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK) req_info->server_context;
	if (lpECB->cbTotalBytes == 0) return HSE_STATUS_SUCCESS;

	if (lpECB->cbTotalBytes == lpECB->cbAvailable) {
		if (lpECB->lpbData && lpECB->cbAvailable) {
			req_info->post_content = lpECB->lpbData;
			req_info->post_available = lpECB->cbAvailable;
		}
		return HSE_STATUS_SUCCESS;
	} else {
		/* get a temp file */
		DWORD dwWritten = 0;

		GetTempFileName(getenv("TEMP"), "fcgi", 0, req_info->post_tmp_file);
		req_info->post_file = fopen(req_info->post_tmp_file, "wb+");
		if (!req_info->post_file) return 0;
		/* write what IIS has already retreived */
		if (lpECB->cbAvailable) {
			if ((dwWritten = fwrite(lpECB->lpbData, 1, lpECB->cbAvailable, req_info->post_file)) != lpECB->cbAvailable) {
				fclose(req_info->post_file);
				return HSE_STATUS_ERROR;
			}
			req_info->post_read = lpECB->cbAvailable;
		}

#ifdef DO_ASYNC_READ
		req_info->post_content = (unsigned char *)malloc(MAX_BUF_SIZE);
		if (!req_info->post_content) return HSE_STATUS_ERROR;

		//
		//  Set a call back function and context that will 
		//  be used for handling asynchrnous IO operations.
		//  This only needs to set up once.
		//

		DWORD fReturn =
		lpECB->ServerSupportFunction(
                    lpECB->ConnID,
                    HSE_REQ_IO_COMPLETION,
                    AsyncReadClientIoCompletion,
                    0,
                    (LPDWORD)req_info);  

		if ( !fReturn) {
			return (HSE_STATUS_ERROR);
		}

		DWORD cbTotalToRead = MAX_BUF_SIZE;
		DWORD dwFlags = HSE_IO_ASYNC;
		fReturn = 
		lpECB->ServerSupportFunction(
							lpECB->ConnID
						  , HSE_REQ_ASYNC_READ_CLIENT
						  , req_info->post_content
						  , &cbTotalToRead
						  , &dwFlags);

		if (!fReturn) {
			return (HSE_STATUS_ERROR);
		}

		return (HSE_STATUS_PENDING); // HSE_STATUS_PENDING or HSE_STATUS_ERROR;

#else

		/* send any additional data to client */
		DWORD totalBytes = lpECB->cbTotalBytes;
		unsigned char work_buffer[MAX_BUF_SIZE];
		DWORD bytesToRead = sizeof(work_buffer);
		DWORD bytesRead = lpECB->cbAvailable;
		while (bytesToRead &&
			lpECB->ReadClient(lpECB->ConnID, work_buffer, &bytesToRead)
			&& bytesToRead > 0) {

			bytesRead+=bytesToRead;
			
			if ((dwWritten = fwrite(work_buffer, 1, bytesToRead, req_info->post_file)) != bytesToRead) {
				fclose(req_info->post_file);
				return HSE_STATUS_ERROR;
			}

			// how many more bytes to read
			bytesToRead = sizeof(work_buffer);
			if (bytesToRead > (totalBytes - bytesRead)) 
				bytesToRead = (totalBytes - bytesRead);

			if (m_nMaxPostSize > 0 && bytesRead > m_nMaxPostSize) {
				fclose(req_info->post_file);
				return HSE_STATUS_ERROR;
			}
		}
		/* reset to beginning of file */
		fseek(req_info->post_file, 0, SEEK_SET);
#endif
	}
	return HSE_STATUS_SUCCESS;
}




inline sapi_request_info *_init_request_info(LPEXTENSION_CONTROL_BLOCK lpECB)
{
	sapi_request_info *req_info = (sapi_request_info *)malloc(sizeof(sapi_request_info));
	if (!req_info) return NULL;

	req_info->server_context = lpECB;

	/* get the security token for this request */
	if (dwServerImpersonate) {
		lpECB->ServerSupportFunction(lpECB->ConnID, 
			HSE_REQ_GET_IMPERSONATION_TOKEN, &req_info->security_token, NULL, 0);
	} else {
		req_info->security_token = NULL;
	}

	req_info->path_info = lpECB->lpszPathInfo;
	req_info->path_translated = lpECB->lpszPathTranslated;
	req_info->request_method = lpECB->lpszMethod;
	req_info->query_string = lpECB->lpszQueryString;
	req_info->content_length = lpECB->cbTotalBytes;
	req_info->content_type = lpECB->lpszContentType;
	req_info->bindPath = procmgr->GetBindingForPath(req_info->path_translated);
	req_info->post_file = NULL;
	req_info->post_content = NULL;
	req_info->post_available = 0;
	req_info->post_read = 0;
	req_info->env = NULL;
	req_info->keepalive = 0;

	if (!req_info->bindPath) {
		return req_info;
	}

	_prepare_environment(req_info);
	return req_info;
}

inline void _free_request_info(sapi_request_info *req_info) 
{
	if (req_info->post_file) {
		fclose(req_info->post_file);
		DeleteFile(req_info->post_tmp_file);
	}
	/* free up some memory */
	if (req_info->env) {
		DWORD i = 0;
		while (req_info->env[i] != NULL) free(req_info->env[i++]);
	}
	free(req_info->env);
	free(req_info);
}

inline DWORD HandleRequest(sapi_request_info *req_info)
{
	DWORD dwState = HSE_STATUS_SUCCESS;

	/* we want to set the security token now */
	if (req_info->security_token)
		ImpersonateLoggedOnUser(req_info->security_token);

	// handle the request now
	// fastcgi specific
	if (procmgr->isFilter(req_info->bindPath)) {
		FCGI_Filter(req_info);
	} else {
		FCGI_Responder(req_info);
	}
	if (req_info->exitStatus != FCGI_SUCCESS) {
		LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK)req_info->server_context;
		SendError(lpECB,"503 Server too busy","Server Error, unable to connect to fastcgi server.");
		lpECB->dwHttpStatusCode = 503;
		dwState = HSE_STATUS_ERROR;
	} else if (req_info->keepalive != 0) {
		dwState = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
	}
	_free_request_info(req_info);
	return dwState;
}

#ifdef USE_THREAD_POOL
DWORD __stdcall RequestThreadProc(void *p)
// Thread process for worker thread.
{
	HANDLE								IoPort	= (HANDLE*)p;
	unsigned long						pN1, pN2; 
	OVERLAPPED*							pOverLapped;

	//OutputDebugString("RequestThreadProc Started\n");
	while(GetQueuedCompletionStatus(IoPort, &pN1, &pN2, 
		&pOverLapped, INFINITE))
	{
		if(pN1 == 0xFFFFFFFF || pOverLapped == (OVERLAPPED*)0xFFFFFFFF) {
			//OutputDebugString("RequestThreadProc Exiting\n");
			break;
		} else {
			sapi_request_info *req_info = (sapi_request_info *)pN1;
			LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK) req_info->server_context;
			DWORD dwState = HandleRequest(req_info);
			// Let IIS know that this worker thread is done with this request.  This will allow
			// IIS to recycle the EXTENSION_CONTROL_BLOCK.  
			lpECB->ServerSupportFunction(lpECB->ConnID, 
				HSE_REQ_DONE_WITH_SESSION, &dwState, NULL, 0);

		}
	}
	
	return 0;
}
#endif

void _initConfiguration() {
	HKEY hkey;
	m_nThreadCount = THREAD_COUNT;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, BASE_REG_PATH, 0, KEY_READ, &hkey) == ERROR_SUCCESS) {
		DWORD dwType = REG_DWORD;
		DWORD dwBufSize = sizeof(DWORD);
		if (RegQueryValueEx(hkey, "MaxPostData", 0, &dwType, (unsigned char *)&m_nMaxPostSize, &dwBufSize) != ERROR_SUCCESS
			|| dwType != REG_DWORD) {
			m_nMaxPostSize = 0;
		}

		dwType = REG_DWORD;
		dwBufSize = sizeof(DWORD);
		if (RegQueryValueEx(hkey, "BypassAuth", 0, &dwType, (unsigned char *)&m_nBypassAuth, &dwBufSize) != ERROR_SUCCESS
			|| dwType != REG_DWORD) {
			m_nBypassAuth = 0;
		}
		
		
		dwType = REG_BINARY;
		dwBufSize = sizeof(m_szCustomVarsBuf);
		memset(&m_szCustomVarsBuf, 0, sizeof(m_szCustomVarsBuf));
		if (RegQueryValueEx(hkey, "CustomVars", 0, &dwType, (unsigned char *)&m_szCustomVarsBuf, &dwBufSize) != ERROR_SUCCESS
			|| dwType != REG_BINARY) {
			m_szCustomVars[0] = 0;
			m_szCustomVarsBuf[0] = 0;
		} else {
			/* prepare for usage */
			char *p, *b = m_szCustomVarsBuf;
			m_nCountCustomVars=0;
			while ((p = strchr(b, 13)) != NULL 
				&& m_nCountCustomVars < MAX_CUSTOM_VARS) {
				*p = 0;
				if (*(p-1) == 10) *(p-1)=0;
				m_szCustomVars[m_nCountCustomVars++] = b;
				b = p + 1;
			}
		}

#ifdef USE_THREAD_POOL
		dwType = REG_DWORD;
		dwBufSize = sizeof(DWORD);
		if (RegQueryValueEx(hkey, "ThreadPoolSize", 0, &dwType, (unsigned char *)&m_nThreadCount, &dwBufSize) != ERROR_SUCCESS
			|| dwType != REG_DWORD) {
			m_nThreadCount = THREAD_COUNT;
		}
#endif
		RegCloseKey(hkey);
	}
}

BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer)
{
	pVer->dwExtensionVersion = HSE_VERSION;
	lstrcpyn(pVer->lpszExtensionDesc, ISAPI_NAME, HSE_MAX_EXT_DLL_NAME_LEN);

	_initConfiguration();
	/* initialize the process manager */
	procmgr = new FCGIProccessManager();
	Sleep(1000); /* give processes a chance to start */
#ifdef USE_THREAD_POOL
	if (m_nThreadCount > 0) {
		/* start a pool of threads to handle requests */
		m_phThreads = new HANDLE[m_nThreadCount];

		m_hIoPort = CreateIoCompletionPort((HANDLE)INVALID_HANDLE_VALUE, NULL, 0, 0);

		for(long n = 0; n < m_nThreadCount; n++)
		{
			DWORD tid=0;
			HANDLE thread = CreateThread(NULL, 0, RequestThreadProc, m_hIoPort, 0, &tid);
			m_phThreads[n] = thread;
		}
	}
#endif
	return TRUE;
}

BOOL WINAPI TerminateExtension(DWORD dwFlags)
{
#ifdef USE_THREAD_POOL
	// extension is being terminated
	if (m_nThreadCount > 0) {
		for(long n = 0; n < m_nThreadCount; n++) {
			PostQueuedCompletionStatus(m_hIoPort, 0xFFFFFFFF, 0, NULL);
		}

		WaitForMultipleObjects(m_nThreadCount, m_phThreads, TRUE, 120000);
		CloseHandle(m_hIoPort);
		for(long i = 0; i < m_nThreadCount; i++) {
			CloseHandle(m_phThreads[i]);
		}
		delete [] m_phThreads;
	}
#endif
	/* shutdown the process manager */
	delete procmgr;
	return TRUE;
}

DWORD DoRequest(sapi_request_info *req_info)
{
#ifndef USE_THREAD_POOL
	return HandleRequest(req_info);
#else
	BOOL fRet;
	if (m_nThreadCount == 0) {
		return HandleRequest(req_info);
	} else {
		if(!PostQueuedCompletionStatus(m_hIoPort, (DWORD)req_info, 0, NULL))
		{
			LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK) req_info->server_context;
			char	szBuffer[1024] = { 0 };
			_snprintf(szBuffer, sizeof(szBuffer)-1, "<H1>Error posting to completion port! Win32 Error = %i</H1>", GetLastError());
			fRet = SendError(lpECB,"503 Server too busy",szBuffer);
			lpECB->dwHttpStatusCode = 503;
			return fRet ? HSE_STATUS_SUCCESS : HSE_STATUS_ERROR;
		}
		return HSE_STATUS_PENDING;
	}
#endif
}


#ifdef DO_ASYNC_READ
VOID WINAPI
AsyncReadClientIoCompletion(
            IN LPEXTENSION_CONTROL_BLOCK pECB,
            IN PVOID pContext,
            IN DWORD cbIO,
            IN DWORD dwError)
{
    BOOL    fReturn;
    BYTE    *pbData = NULL;
    DWORD   dwFlags;
    DWORD   cbTotalToRead;
    char	szBuffer[1024] = { 0 };
	DWORD   dwState = HSE_STATUS_SUCCESS;

    sapi_request_info *req_info = (sapi_request_info *) pContext;

    if (ERROR_SUCCESS == dwError) {
		if (cbIO > 0) {
			req_info->post_read += cbIO; 
			fwrite(req_info->post_content, 1, cbIO, req_info->post_file);
		}
        if (cbIO == 0 || req_info->post_read == pECB->cbTotalBytes ) { 
			/* continue with our request now */
			free(req_info->post_content);
			req_info->post_content = NULL;
			fseek(req_info->post_file, 0, SEEK_SET);
			/* we will be pending if we are using thread pools */
			dwState = DoRequest(req_info);
            if (dwState == HSE_STATUS_PENDING) 
				return;
        } else {

            // 
            // Still have more data to read... 
            // 
            // We need to first check if the size of the remaining chunk 
            // is less than MAX_BUF_SIZE, if so just read what is available, 
            // otherwise read MAX_BUF_SIZE bytes of data.
            // 

            cbTotalToRead = pECB->cbTotalBytes - req_info->post_read;
            if ( cbTotalToRead > MAX_BUF_SIZE )  {
                cbTotalToRead = MAX_BUF_SIZE;
            }
            
            // 
            // Fire off another call to perform an asynchronus read from the client. 
            //

            dwFlags = HSE_IO_ASYNC;
            fReturn = 
            pECB->ServerSupportFunction(
                          pECB->ConnID
                        , HSE_REQ_ASYNC_READ_CLIENT
                        , req_info->post_content
                        , &cbTotalToRead
                        , &dwFlags);

			if (fReturn) return;

			_snprintf(szBuffer, sizeof(szBuffer)-1, "Problem occurred at ServerSupportFunction() sending HSE_REQ_ASYNC_READ_CLIENT request.");
			DWORD fRet = SendError(pECB,"500 Server very broken",szBuffer);
			pECB->dwHttpStatusCode = 500;
			dwState = HSE_STATUS_ERROR;
        }
    } else {
		_snprintf(szBuffer, sizeof(szBuffer)-1, "<H1>Error reading POST data! Win32 Error = %i</H1>", dwError);
		DWORD fRet = SendError(pECB,"500 Server very broken",szBuffer);
		pECB->dwHttpStatusCode = 500;
		dwState = HSE_STATUS_ERROR;
    }
	/* if we got here, we either failed, or we are done */
	pECB->ServerSupportFunction( pECB->ConnID,
                        HSE_REQ_DONE_WITH_SESSION,
                        &dwState,
                        NULL,
                        0);
} // AsyncReadClientIoCompletion
#endif


DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB)
{
	sapi_request_info *req_info = _init_request_info(lpECB);
	if (!req_info) {
		char	szBuffer[1024] = { 0 };
		_snprintf(szBuffer, sizeof(szBuffer)-1, "<H1>Error allocating memory! Win32 Error = %i</H1>", GetLastError());
		DWORD fRet = SendError(lpECB,"500 Server very broken",szBuffer);
		lpECB->dwHttpStatusCode = 500;
		return fRet ? HSE_STATUS_SUCCESS : HSE_STATUS_ERROR;
	}
	if (req_info->bindPath[0] == 0) {
		char	szBuffer[1024] = { 0 };
		_snprintf(szBuffer, sizeof(szBuffer)-1, "<H1>Error initializing bind path for %s</H1>", req_info->path_translated);
		DWORD fRet = SendError(lpECB,"500 Server very broken",szBuffer);
		lpECB->dwHttpStatusCode = 500;
		free(req_info);
		return fRet ? HSE_STATUS_SUCCESS : HSE_STATUS_ERROR;
	}

	/* if status is PENDING, we're reading POST data larger than 48K */
	DWORD status = _prepare_postdata(req_info);

	/* if we have SUCCESS, post handling is done, we did not
	   do async reading for post data */
	if (status == HSE_STATUS_SUCCESS) 
		return DoRequest(req_info);

	/* either there is an error, or we are in async read mode now */
	return status;
}

BOOL WINAPI GetFilterVersion(PHTTP_FILTER_VERSION pFilterVersion)
{
	bFilterLoaded = 1;
	pFilterVersion->dwFilterVersion = HTTP_FILTER_REVISION;
	strcpy(pFilterVersion->lpszFilterDesc, ISAPI_NAME);
	pFilterVersion->dwFlags= (SF_NOTIFY_AUTHENTICATION);
	return TRUE;
}

DWORD WINAPI HttpFilterProc(PHTTP_FILTER_CONTEXT pfc, DWORD notificationType, LPVOID pvNotification)
{
	switch (notificationType) {
		case SF_NOTIFY_AUTHENTICATION: {
				HTTP_FILTER_AUTHENT *auth = (HTTP_FILTER_AUTHENT *) pvNotification;

				if (m_nBypassAuth) {
					auth->pszUser[0] = 0;
					auth->pszPassword[0] = 0;
					return SF_STATUS_REQ_HANDLED_NOTIFICATION;
				}
				/*
				// fastcgi specific
				FCGIAppServer app;
				app.Authorizer(pfc);
				if (app.Status() == FCGI_SUCCESS)
					return SF_STATUS_REQ_HANDLED_NOTIFICATION;
				*/
				
			}
			break;
	}
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

static int isapi_write(sapi_request_info *req_info, const unsigned char *str, unsigned long str_length)
{
	DWORD num_bytes = str_length;
	LPEXTENSION_CONTROL_BLOCK ecb = (LPEXTENSION_CONTROL_BLOCK) req_info->server_context;
	
	if (ecb->WriteClient(ecb->ConnID, (char *) str, &num_bytes, HSE_IO_SYNC ) == FALSE) {
		/* XXX
		handle_aborted_connection();
		*/
		return -1;
	}
	return num_bytes;
}

static int isapi_getenv(sapi_request_info *req_info, const char *name, char *buf, unsigned long *bufsz)
{
	LPEXTENSION_CONTROL_BLOCK ecb = (LPEXTENSION_CONTROL_BLOCK) req_info->server_context;
	if (!name)
		return 0;

	if (ecb->GetServerVariable(ecb->ConnID, (char *)name, buf, bufsz)) {
		return SAPI_SUCCESS;
	}
	return GetLastError();
}

void isapi_sendheaders(sapi_request_info *req_info, char *statusline, char *sapi_headers)
{
	LPEXTENSION_CONTROL_BLOCK ecb = (LPEXTENSION_CONTROL_BLOCK) req_info->server_context;
	HSE_SEND_HEADER_EX_INFO header_info;
	header_info.pszStatus = statusline;
	header_info.cchStatus = strlen(header_info.pszStatus);
	header_info.pszHeader = sapi_headers;
	header_info.cchHeader = strlen(header_info.pszHeader);
	header_info.fKeepConn = req_info->keepalive != 0;
	ecb->dwHttpStatusCode = atoi(statusline);
	ecb->ServerSupportFunction(ecb->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &header_info, NULL, NULL);
}

struct _sapi_module_struct sapi_module = {
	isapi_write,			/* unbuffered write */
	isapi_getenv,			/* getenv */
	isapi_sendheaders		/* send header handler */
};

__declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
          //
          // Prevent the system from calling DllMain
          // when threads are created or destroyed.
          //
          DisableThreadLibraryCalls( hinstDLL);
		  putenv("FCGI_INTERFACE=ISAPI");
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}
