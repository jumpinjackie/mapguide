#include "FCGIAppServer.h"
#include "FCGIProcMgr.h"

/* Variables available in IIS per request */
static char * aszServerVariables[] =
	    {"APPL_MD_PATH", "APPL_PHYSICAL_PATH", "AUTH_PASSWORD",
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

FCGIAppServer::FCGIAppServer() 
{
	appServerSock = -1;
	exitStatus = FCGI_ERROR;
	exitStatusSet = FALSE;
}

FCGIAppServer::~FCGIAppServer()
{
}

BOOL FCGIAppServer::Responder(LPEXTENSION_CONTROL_BLOCK lpecb)
{
	lpECB = lpecb;

	exitStatus = FCGI_ERROR;
	DWORD bytesToRead = lpECB->cbTotalBytes;
    requestId = (DWORD)lpECB;
	char *bind = procmgr->GetBindingForPath(lpECB->lpszPathTranslated);
	if (Connect(bind)) {
		if (!BeginRequest(requestId, FCGI_RESPONDER, FALSE)) 
			return FALSE;
		if (SendEnvironment()) {
			if (SendPOSTData()) {
				HandleResponse();
				OS_Close(appServerSock);
				exitStatus = FCGI_SUCCESS;
				return TRUE;
			}
		}
		OS_Close(appServerSock);
	}
	return FALSE;
}

BOOL FCGIAppServer::Authorizer(PHTTP_FILTER_CONTEXT pfc)
{
/*	lpFC = pfc;
	lpFC->pFilterContext = this;

	exitStatus = FCGI_SUCCESS;
	DWORD bytesToRead = 0;
    requestId = (DWORD)lpFC;
	char *bind = pm.GetBindingForPath(lpECB->lpszPathTranslated);
	if (Connect(bind)) {
		if (!BeginRequest(requestId, FCGI_AUTHORIZER, FALSE)) return FALSE;
		if (!SendEnvironment()) return FALSE;
		if (!HandleResponse()) return FALSE;
		return TRUE;
	}*/
	exitStatus = FCGI_ERROR;
	return FALSE;
}

/* filter same as responder, except we send the 
   file we want processed to the fastcgi app */
BOOL FCGIAppServer::Filter(LPEXTENSION_CONTROL_BLOCK lpecb, DWORD notificationType)
{
	lpECB = lpecb;
	exitStatus = FCGI_ERROR;
	DWORD bytesToRead = lpECB->cbTotalBytes;
    requestId = (DWORD)lpECB;
	char *bind = procmgr->GetBindingForPath(lpECB->lpszPathTranslated);
	if (Connect(bind)) {
		if (!BeginRequest(requestId, FCGI_RESPONDER, FALSE)) return FALSE;
		if (!SendEnvironment()) return FALSE;
		if (!SendPOSTData()) return FALSE;
		if (!SendFile()) return FALSE;
		if (!HandleResponse()) return FALSE;
		OS_Close(appServerSock);
		exitStatus = FCGI_SUCCESS;
		return TRUE;
	}
	return FALSE;
}

/*
 *----------------------------------------------------------------------
 *
 * MakeBeginRequestBody --
 *
 *      Constructs an FCGI_BeginRequestBody record.
 *
 *----------------------------------------------------------------------
 */
void FCGIAppServer::MakeBeginRequestBody(
		FCGI_BeginRequestBody *body,
		int role,
        int keepConnection)
{
    ASSERT((role >> 16) == 0);
    body->roleB1 = (unsigned char) ((role >>  8) & 0xff);
    body->roleB0 = (unsigned char) (role         & 0xff);
    body->flags  = (unsigned char) ((keepConnection) ? FCGI_KEEP_CONN : 0);
    memset(body->reserved, 0, sizeof(body->reserved));
}

/*
 *----------------------------------------------------------------------
 *
 * MakeHeader --
 *
 *      Constructs an FCGI_Header struct.
 *
 *----------------------------------------------------------------------
 */
void FCGIAppServer::MakeHeader(
		FCGI_Header *header,
        int type,
        int requestId,
        int contentLength,
        int paddingLength)
{
    ASSERT(contentLength >= 0 && contentLength <= FCGI_MAX_LENGTH);
    ASSERT(paddingLength >= 0 && paddingLength <= 0xff);
    header->version = FCGI_VERSION_1;
    header->type             = (unsigned char) type;
    header->requestIdB1      = (unsigned char) ((requestId     >> 8) & 0xff);
    header->requestIdB0      = (unsigned char) ((requestId         ) & 0xff);
    header->contentLengthB1  = (unsigned char) ((contentLength >> 8) & 0xff);
    header->contentLengthB0  = (unsigned char) ((contentLength     ) & 0xff);
    header->paddingLength    = (unsigned char) paddingLength;
    header->reserved         =  0;
}

/*
 *----------------------------------------------------------------------
 *
 * BuildNameValueHeader --
 *
 *      Builds a name-value pair header from the name length
 *      and the value length.  Stores the header into *headerBuffPtr,
 *      and stores the length of the header into *headerLenPtr.
 *
 * Side effects:
 *      Stores header's length (at most 8) into *headerLenPtr,
 *      and stores the header itself into
 *      headerBuffPtr[0 .. *headerLenPtr - 1].
 *
 *----------------------------------------------------------------------
 */
void FCGIAppServer::BuildNameValueHeader(
        int nameLen,
        int valueLen,
        unsigned char *headerBuffPtr,
        int *headerLenPtr) {
    unsigned char *startHeaderBuffPtr = headerBuffPtr;

    ASSERT(nameLen >= 0);
    if (nameLen < 0x80) {
        *headerBuffPtr++ = (unsigned char) nameLen;
    } else {
        *headerBuffPtr++ = (unsigned char) ((nameLen >> 24) | 0x80);
        *headerBuffPtr++ = (unsigned char) (nameLen >> 16);
        *headerBuffPtr++ = (unsigned char) (nameLen >> 8);
        *headerBuffPtr++ = (unsigned char) nameLen;
    }
    ASSERT(valueLen >= 0);
    if (valueLen < 0x80) {
        *headerBuffPtr++ = (unsigned char) valueLen;
    } else {
        *headerBuffPtr++ = (unsigned char) ((valueLen >> 24) | 0x80);
        *headerBuffPtr++ = (unsigned char) (valueLen >> 16);
        *headerBuffPtr++ = (unsigned char) (valueLen >> 8);
        *headerBuffPtr++ = (unsigned char) valueLen;
    }
    *headerLenPtr = headerBuffPtr - startHeaderBuffPtr;
}


BOOL FCGIAppServer::Connect(char *bindPath)
{
    appServerSock = OS_FcgiConnect(bindPath);
    if(appServerSock < 0) {
		/*if (GetLastError() == ERROR_PIPE_BUSY) {
			Sleep(75);
			return Connect(bindPath);
		}*/
		DWORD tid = GetCurrentThreadId();
        fprintf(stderr, "tid %d Could not connect to %s\n", tid, bindPath);
		exitStatus = OS_Errno;
		return FALSE;
    }
	return TRUE;
}

BOOL FCGIAppServer::BeginRequest(
		int requestId,
		int role,
        int keepConnection)
{
	DWORD count = 0;
	FCGI_BeginRequestRecord beginRecord;
    MakeHeader(&beginRecord.header,FCGI_BEGIN_REQUEST, requestId,
            sizeof(beginRecord.body), 0);
    MakeBeginRequestBody(&beginRecord.body, role, keepConnection);
    count = OS_Write(appServerSock, (char *)&beginRecord, sizeof(beginRecord));
    if(count != sizeof(beginRecord)) {
        exitStatus = OS_Errno;
		return FALSE;
    }
	return TRUE;
}

BOOL FCGIAppServer::SendEnvironmentVariable(
		FCGX_Stream *paramsStream,
		const char *envName,
		const char *envVal)
{
    unsigned char headerBuff[8];
	int headerLen;
	DWORD valueLen = strlen(envVal);
	DWORD nameLen = strlen(envName);
	BuildNameValueHeader(
			nameLen,
			valueLen,
			&headerBuff[0],
			&headerLen);
	//fprintf(stderr,"sending %s=%s\n",envName,envVal);
	if(FCGX_PutStr((char *) &headerBuff[0], headerLen, paramsStream) < 0
	  || FCGX_PutStr(envName, nameLen, paramsStream) < 0
	  || FCGX_PutStr(envVal, valueLen, paramsStream) < 0) {
		exitStatus = FCGX_GetError(paramsStream);
		return FALSE;
	}
	return TRUE;
}

/*
 * SendEnvironment
 * read environment from IIS and send to fcgi app
 */
BOOL FCGIAppServer::SendEnvironment()
{
	char buf[MAX_PATH];
    FCGX_Stream *paramsStream = CreateWriter(appServerSock, requestId, 8192, FCGI_PARAMS);
	DWORD dwNumVars = ( sizeof aszServerVariables )/( sizeof aszServerVariables[0] );

	if (lpECB->lpszMethod[0] != 0)
		SendEnvironmentVariable(paramsStream,"REQUEST_METHOD", lpECB->lpszMethod);
	if (lpECB->lpszQueryString[0] != 0)
		SendEnvironmentVariable(paramsStream,"QUERY_STRING", lpECB->lpszQueryString);
	if (lpECB->lpszPathInfo[0] != 0)
		SendEnvironmentVariable(paramsStream,"PATH_INFO", lpECB->lpszPathInfo);
	if (lpECB->lpszPathTranslated[0] != 0)
		SendEnvironmentVariable(paramsStream,"PATH_TRANSLATED", lpECB->lpszPathTranslated);
	_snprintf(buf, sizeof(buf)-1,"%d",lpECB->cbTotalBytes);
	SendEnvironmentVariable(paramsStream,"CONTENT_LENGTH", buf);
	if (lpECB->lpszContentType[0] != 0)
		SendEnvironmentVariable(paramsStream,"CONTENT_TYPE", lpECB->lpszContentType);

	for (DWORD x = 0; x < dwNumVars; x++ ) {
		DWORD dwBuffSize = 1024;
		char szValue[1024];
        szValue[0] = '\0';
		char *envname = aszServerVariables[x];
		if (!lpECB->GetServerVariable( 
                lpECB->ConnID, 
                envname, 
                szValue, 
                &dwBuffSize) && szValue[0] != 0) {
			SendEnvironmentVariable(paramsStream,envname, szValue);
		}
    }
    FCGX_FClose(paramsStream);
    FreeStream(&paramsStream);
	return TRUE;
}

BOOL FCGIAppServer::SendPOSTData()
{
    FCGI_Header header;
	DWORD headerLen;
	DWORD bufSize = sizeof(work_buffer);
	DWORD bytesRead=0, bytesToRead = bufSize;
	DWORD err = 0;

	/* create stream to write to client */
    FCGX_Stream *paramsStream = CreateWriter(appServerSock, requestId, 8192, FCGI_STDIN);
	/* send currently available data to client */
	if (lpECB->lpbData && lpECB->cbAvailable) {
		if (lpECB->cbAvailable <= FCGI_MAX_LENGTH) {
			MakeHeader(&header, FCGI_STDIN, requestId, lpECB->cbAvailable, 0);
			headerLen = sizeof(header);
			if(FCGX_PutStr((char *) &header, headerLen, paramsStream) < 0
				|| FCGX_PutStr((char *)lpECB->lpbData, lpECB->cbAvailable, paramsStream)) {
				err = FCGX_GetError(paramsStream);
			}
		} else {
			/* we have to loop through the available data and send it in chunks */
			unsigned char *dptr = lpECB->lpbData;
			
			while (bytesToRead > 0) {
				memcpy(work_buffer, dptr, bytesToRead);
				bytesRead += bytesToRead;
				MakeHeader(&header, FCGI_STDIN, requestId, bytesToRead, 0);
				headerLen = sizeof(header);
				if(FCGX_PutStr((char *) &header, headerLen, paramsStream) < 0
					|| FCGX_PutStr((char *)work_buffer, bytesToRead, paramsStream)) {
					err = FCGX_GetError(paramsStream);
				}
				dptr += bytesToRead;
				// how many more bytes to read
				bytesToRead = bufSize;
				if (bytesToRead > (lpECB->cbAvailable - bytesRead)) 
					bytesToRead = (lpECB->cbAvailable - bytesRead);
			}
		}
		/* reset in case we have additional info to read */
		bytesToRead = bufSize;
		bytesRead = 0;
	}

	/* send any additional data to client */
	DWORD extraBytes = lpECB->cbTotalBytes - lpECB->cbAvailable;
	if (extraBytes < bytesToRead) bytesToRead = extraBytes;
	while (bytesToRead &&
		lpECB->ReadClient(lpECB, work_buffer, &bytesToRead)) {

		bytesRead+=bytesToRead;
		MakeHeader(&header, FCGI_STDIN, requestId, bytesToRead, 0);
		headerLen = sizeof(header);
		if(FCGX_PutStr((char *) &header, headerLen, paramsStream) < 0
			|| FCGX_PutStr((char *) work_buffer, bytesToRead, paramsStream)) {
			err = FCGX_GetError(paramsStream);
		}

		// how many more bytes to read
		bytesToRead = bufSize;
		if (bytesToRead > (extraBytes - bytesRead)) 
			bytesToRead = (extraBytes - bytesRead);

    }
	/* send eof on stdin */
	MakeHeader(&header, FCGI_STDIN, requestId, 0, 0);
	headerLen = sizeof(header);
	if(FCGX_PutStr((char *) &header, headerLen, paramsStream) < 0) {
		err = FCGX_GetError(paramsStream);
	}

    FCGX_FClose(paramsStream);
    FreeStream(&paramsStream);

	if (err) {
		exitStatus = err;
		return FALSE;
	}
	return TRUE;
}

BOOL FCGIAppServer::SendFile()
{
    FCGI_Header header;
	DWORD headerLen;
	DWORD bufSize = sizeof(work_buffer);
	char szPathTranslated[1024];
	DWORD dwBuffSize = sizeof(szPathTranslated);
	DWORD bytesRead;

    szPathTranslated[0] = '\0';
	// get the path_translated
	if (!lpECB->GetServerVariable( 
            lpECB->ConnID, 
            "PATH_TRANSALTED", 
            szPathTranslated, 
            &dwBuffSize) && szPathTranslated[0] != 0) {
		FCGX_Stream *paramsStream = CreateWriter(appServerSock, requestId, 8192, FCGI_DATA);
		FILE *f = fopen(szPathTranslated, "rb");
		if (f) {
			while ((bytesRead = fread(work_buffer, bufSize, 1, f)) > 0) {
				MakeHeader(&header, FCGI_DATA, requestId, bytesRead, 0);
				headerLen = sizeof(header);
				if(FCGX_PutStr((char *) &header, headerLen, paramsStream) < 0
					|| FCGX_PutStr((char *) work_buffer, bytesRead, paramsStream)) {
					exitStatus = FCGX_GetError(paramsStream);
				}
			}
			fclose(f);
		}
		FCGX_FClose(paramsStream);
		FreeStream(&paramsStream);
	}
	// send the file we want to process to the application server
	if (exitStatus) return FALSE;
	return TRUE;
}

BOOL FCGIAppServer::HandleResponse()
{
	FCGI_Header	Header;
	DWORD rcvd = 0;
	BOOL headersDone = FALSE;
	// read the response data from the application
	BOOL InProgress = TRUE;
	DWORD writelen = 0;
	char *writebuf = NULL;
	while(InProgress &&
		(rcvd = OS_Read(appServerSock, (char *)&Header, sizeof(Header))) > 0) {
		DWORD FcgiLength = 0;
		switch( Header.type ) {

			case FCGI_END_REQUEST:
				FcgiLength = (Header.contentLengthB1 << 8 ) + Header.contentLengthB0 + Header.paddingLength;
				rcvd = OS_Read(appServerSock,(char *)&work_buffer, FcgiLength);
				InProgress = FALSE;
				break;
			
			case FCGI_STDOUT:
				FcgiLength = (Header.contentLengthB1 << 8 ) + Header.contentLengthB0 + Header.paddingLength;
				rcvd = OS_Read(appServerSock,(char *)&work_buffer, FcgiLength);
				writebuf = (char *)work_buffer;
				writelen = FcgiLength;
				if (!headersDone) {
					// find the end of the headers
					char *p = NULL;
					if ((p = strstr(writebuf, "\015\012\015\012")) != NULL) {
						// just another check that these are headers
						char *c = strchr(writebuf, ':');
						if (c && c < p) {
							// FIXME quick hack 
							DWORD len = p-writebuf+4;
							char *b = (char *)malloc(len+1);
							memset(b,0,len+1);
							memcpy(b, work_buffer, len);

							// headers done, send them all now
							HSE_SEND_HEADER_EX_INFO header_info;
							header_info.pszStatus = "200 OK";
							header_info.cchStatus = strlen(header_info.pszStatus);
							header_info.pszHeader = b;
							header_info.cchHeader = len;
							lpECB->dwHttpStatusCode = 200;
							lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &header_info, NULL, NULL);
							free(b);

							writebuf += len;
							writelen -= len;
						}
					}
					
					headersDone = TRUE;
				}
				// write the data to IIS
				if (writelen) {
					lpECB->WriteClient(lpECB->ConnID,writebuf,&writelen,0);
				}
				break;
					
			default:	
				InProgress = FALSE;
				exitStatus = FCGI_UNKNOWN_TYPE;
				break;
		}
	}
	if (exitStatus) return FALSE;
	return TRUE;
}

BOOL FCGIAppServer::Abort()
{
	DWORD count = 0;
	FCGI_Header	header; 
	MakeHeader(&header, FCGI_ABORT_REQUEST, requestId,
            sizeof(header), 0);
    count = OS_Write(appServerSock, (char *)&header, sizeof(header));
    if(count != sizeof(header)) {
        exitStatus = OS_Errno;
		return FALSE;
    }
	return TRUE;
}

