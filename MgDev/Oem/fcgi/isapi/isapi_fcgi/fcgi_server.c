#include "fcgi_server.h"
#include <malloc.h>

char *stristr(char *haystack, char *needle);

int FCGI_Responder(sapi_request_info *req_info)
{
	req_info->exitStatus = FCGI_SUCCESS;
	req_info->role = FCGI_RESPONDER;
	req_info->keepConnection = 0;

	if (FCGI_Connect(req_info)) {
		if (FCGI_BeginRequest(req_info) && 
			FCGI_SendEnvironment(req_info) &&
			FCGI_SendPOSTData(req_info) &&
			FCGI_HandleResponse(req_info)) {
				OS_Close(req_info->appServerSock);
				return 1;
		}
		req_info->exitStatus = FCGI_ERROR;
		FCGI_Abort(req_info);
		OS_Close(req_info->appServerSock);
	}
	return 0;
}

int FCGI_Authorizer(sapi_request_info *req_info)
{
/*	lpFC = pfc;
	lpFC->pFilterContext = this;

	exitStatus = FCGI_SUCCESS;
	unsigned int bytesToRead = 0;
    (int)req_info->server_context = (unsigned int)lpFC;
	char *bind = pm.GetBindingForPath(lpECB->lpszPathTranslated);
	if (Connect(bind)) {
		if (!BeginRequest((int)req_info->server_context, FCGI_AUTHORIZER, FALSE)) return FALSE;
		if (!SendEnvironment()) return FALSE;
		if (!HandleResponse()) return FALSE;
		return TRUE;
	}*/
	req_info->exitStatus = FCGI_ERROR;
	return 0;
}

/* filter same as responder, except we send the 
   file we want processed to the fastcgi app */
int FCGI_Filter(sapi_request_info *req_info)
{
	req_info->exitStatus = FCGI_SUCCESS;
	req_info->role = FCGI_FILTER;
	req_info->keepConnection = 0;

	if (FCGI_Connect(req_info)) {
		if (FCGI_BeginRequest(req_info) && 
			FCGI_SendEnvironment(req_info) &&
			FCGI_SendPOSTData(req_info) &&
			FCGI_SendFile(req_info) &&
			FCGI_HandleResponse(req_info)) {
				OS_Close(req_info->appServerSock);
				return 1;
		}
		req_info->exitStatus = FCGI_ERROR;
		FCGI_Abort(req_info);
		OS_Close(req_info->appServerSock);
	}
	return 0;
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
void FCGI_MakeBeginRequestBody(
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
void FCGI_MakeHeader(
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
void FCGI_BuildNameValueHeader(
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


BOOL FCGI_Connect(sapi_request_info *req_info)
{
    req_info->appServerSock = OS_FcgiConnect(req_info->bindPath);
	CheckServers(req_info->bindPath);
    if(req_info->appServerSock <= 0) {
		if (GetLastError() == ERROR_PIPE_BUSY) {
			/* revert to the process security token first 
				we do this so that the pipes we use can define
				the security token for the external process */
			RevertToSelf();
			/* start some new servers */
			InitiateServers(req_info->bindPath);
			Sleep(1000);

			/* impersonate the http user now */
			if (req_info->security_token)
				ImpersonateLoggedOnUser(req_info->security_token);

			req_info->appServerSock = OS_FcgiConnect(req_info->bindPath);
			if (req_info->appServerSock > 0) return 1;
		}
        fprintf(stderr, "tid %d Could not connect to %s\n", GetCurrentThreadId(), req_info->bindPath);
		req_info->exitStatus = OS_Errno;
		return 0;
    }
	return 1;
}

int FCGI_BeginRequest(sapi_request_info *req_info)
{
	unsigned long count = 0;
	FCGI_BeginRequestRecord beginRecord;

    FCGI_MakeHeader(&beginRecord.header,FCGI_BEGIN_REQUEST, (int)req_info->server_context,
            sizeof(beginRecord.body), 0);
    FCGI_MakeBeginRequestBody(&beginRecord.body, req_info->role, req_info->keepConnection);
    count = OS_Write(req_info->appServerSock, (char *)&beginRecord, sizeof(beginRecord));
    if(count != sizeof(beginRecord)) {
        req_info->exitStatus = OS_Errno;
		return 0;
    }
	return 1;
}

BOOL FCGI_SendEnvironmentVariable(
		sapi_request_info *req_info,
		FCGX_Stream *paramsStream,
		const char *envName,
		const char *envVal)
{
    unsigned char headerBuff[8];
	int headerLen;
	unsigned int valueLen = strlen(envVal);
	unsigned int nameLen = strlen(envName);
	FCGI_BuildNameValueHeader(
			nameLen,
			valueLen,
			&headerBuff[0],
			&headerLen);
	//fprintf(stderr,"sending %s=%s\n",envName,envVal);
	if(FCGX_PutStr((char *) &headerBuff[0], headerLen, paramsStream) < 0
	  || FCGX_PutStr(envName, nameLen, paramsStream) < 0
	  || FCGX_PutStr(envVal, valueLen, paramsStream) < 0) {
		req_info->exitStatus = FCGX_GetError(paramsStream);
		return 0;
	}
	return 1;
}

/*
 * SendEnvironment
 */
BOOL FCGI_SendEnvironment(sapi_request_info *req_info)
{
    FCGX_Stream *paramsStream = FCGX_CreateWriter(req_info->appServerSock, (int)req_info->server_context, 8192, FCGI_PARAMS);
	unsigned int i = 0;
	while (req_info->env[i] != NULL) {
		char *eq = strstr(req_info->env[i], "=");
		if (eq) {
			*eq = 0;
			FCGI_SendEnvironmentVariable(req_info, paramsStream, req_info->env[i], eq+1);
		}
		++i;
	}
    FCGX_FClose(paramsStream);
    FCGX_FreeStream(&paramsStream);
	return 1;
}

int FCGI_SendPOSTData(sapi_request_info *req_info)
{
	char work_buffer[8192];
	unsigned int bufSize = sizeof(work_buffer);
	unsigned int bytesRead=0, bytesToRead = bufSize;
	unsigned int err = 0;
	unsigned int extraBytes = 0;
	FCGX_Stream *paramsStream = NULL;

	if (req_info->content_length == 0) return 1;
	/* create stream to write to client */
    paramsStream = FCGX_CreateWriter(req_info->appServerSock, (int)req_info->server_context, 8192, FCGI_STDIN);
	/* send currently available data to client */
	if (req_info->post_content && req_info->post_available) {
		if (req_info->post_available <= FCGI_MAX_LENGTH) {
			if(FCGX_PutStr((char *)req_info->post_content, req_info->post_available, paramsStream)  < 0) {
				err = FCGX_GetError(paramsStream);
			}
		} else {
			/* we have to loop through the available data and send it in chunks */
			unsigned char *dptr = req_info->post_content;
			
			while (bytesToRead > 0) {
				memcpy(work_buffer, dptr, bytesToRead);
				bytesRead += bytesToRead;
				if(FCGX_PutStr((char *)work_buffer, bytesToRead, paramsStream)  < 0) {
					err = FCGX_GetError(paramsStream);
					break;
				}
				dptr += bytesToRead;
				// how many more bytes to read
				bytesToRead = bufSize;
				if (bytesToRead > (req_info->post_available - bytesRead)) 
					bytesToRead = (req_info->post_available - bytesRead);
			}
		}
		/* reset in case we have additional info to read */
		bytesToRead = bufSize;
		bytesRead = 0;
	}

	/* send any additional data to client */
	extraBytes = req_info->content_length - req_info->post_available;
	bytesToRead = bufSize;
	while (req_info->post_file && bytesToRead &&
		(bytesToRead = fread(work_buffer, sizeof(char), bytesToRead, req_info->post_file)) > 0) {

		bytesRead+=bytesToRead;
		if(FCGX_PutStr((char *) work_buffer, bytesToRead, paramsStream) < 0) {
			err = FCGX_GetError(paramsStream);
			break;
		}

		// how many more bytes to read
		bytesToRead = bufSize;
		if (bytesToRead > (extraBytes - bytesRead)) 
			bytesToRead = (extraBytes - bytesRead);

    }

    FCGX_FClose(paramsStream);
    FCGX_FreeStream(&paramsStream);

	if (err) {
		req_info->exitStatus = err;
		return 0;
	}
	return 1;
}

BOOL FCGI_SendFile(sapi_request_info *req_info)
{
    //FCGI_Header header;
	char work_buffer[8192];
	unsigned int bufSize = sizeof(work_buffer);
	unsigned int bytesRead;

	if (req_info->path_translated) {
		FCGX_Stream *paramsStream = FCGX_CreateWriter(req_info->appServerSock, (int)req_info->server_context, 8192, FCGI_DATA);
		FILE *f = fopen(req_info->path_translated, "rb");
		if (f) {
			while ((bytesRead = fread(work_buffer, 1, bufSize, f)) > 0) {
				/*FCGI_MakeHeader(&header, FCGI_DATA, (int)req_info->server_context, bytesRead, 0);*/
				if(/*FCGX_PutStr((char *) &header, sizeof(header), paramsStream) < 0
					||*/ FCGX_PutStr((char *) work_buffer, bytesRead, paramsStream)) {
					req_info->exitStatus = FCGX_GetError(paramsStream);
					break;
				}
			}
			fclose(f);
		}
		FCGX_FClose(paramsStream);
		FCGX_FreeStream(&paramsStream);
	}
	// send the file we want to process to the application server
	if (req_info->exitStatus) return 0;
	return 1;
}

BOOL FCGI_HandleResponse(sapi_request_info *req_info)
{
	FCGI_Header Header;
	int rcvd = 0;
	char work_buffer[8192];
	char header_buffer[2048] = {0};
	int headersDone = 0;
	unsigned int writelen = 0;
	char *writebuf = NULL;
	unsigned int contentLength = 0;

	for (;;)
	{
        rcvd = OS_Read(req_info->appServerSock, 
				(char *) &Header, sizeof(Header));
        if (rcvd == SOCKET_ERROR || rcvd < 0) 
            return FALSE;

        switch( Header.type ) {

        case FCGI_END_REQUEST:
            contentLength = (Header.contentLengthB1 << 8 ) + 
				Header.contentLengthB0;// + Header.paddingLength;
            rcvd = OS_Read(req_info->appServerSock,
					(char *)&work_buffer, contentLength + Header.paddingLength);
            return !(rcvd==SOCKET_ERROR || rcvd < 0);
            break;
                    
        case FCGI_STDERR:
                /* for now, just pass through */
        case FCGI_STDOUT:
            contentLength =(Header.contentLengthB1 << 8 ) + 
				Header.contentLengthB0;// + Header.paddingLength;
            rcvd = OS_Read(req_info->appServerSock,
					(char *)&work_buffer, contentLength + Header.paddingLength);
			if (rcvd == SOCKET_ERROR || rcvd < 0) 
				return FALSE;
            writebuf = (char *)work_buffer;
            writelen = contentLength;

            if (!headersDone) {
				int hsz = strlen(header_buffer);
                // find the end of the headers
				int crlf = 4;
				char *p = strstr(writebuf, "\015\012\015\012");
				if (!p) {
					// handle lame apps that don't follow http spec
					p = strstr(writebuf, "\012\012");
					if (p) crlf = 2;
				}

                if (p) {
					unsigned int len = p-writebuf+crlf;
                    // just another check that these are headers
                    char *c = strchr(writebuf, ':');
					// do we have enough buffer to deal with the headers
					if (c && c < p && len < sizeof(header_buffer) - hsz -1) {
						char *status;
                        char *hp = header_buffer;
						char http_status[MAX_PATH];

						// copy the last bit into the header buffer
						strncpy(header_buffer + hsz, writebuf,len);
						header_buffer[hsz + len] = 0;

						// discover the status of reply
						if ((status = strstr(header_buffer, "HTTP/")) != NULL) {
							status += 9;
							hp = strchr(status, '\n') + 1;
						} else if ((status = strstr(header_buffer, "tatus:")) != NULL) {
							status += 7;
						}
						if (strstr(header_buffer, "ocation:") != NULL) {
							status = "302 Moved";
						}
						if (!status) {
							status = "200 OK";
						}
						strncpy(http_status, status, sizeof(http_status)-1);
						status = strchr(http_status, '\n');
						if (status) {
							*status = 0;
							if (*(status - 1) == '\r') *(status - 1) = 0;
						}

						// if we have content length, we can keep connections alive
						req_info->keepalive = stristr(header_buffer, "content-length:") != NULL;

                        /* headers done, send them all now */
						sapi_module.send_headers(req_info, http_status, hp);

                        writebuf += len;
                        writelen -= len;
                    } else { /* not headers after all */
						/* flush the header buffer */
						sapi_module.write(req_info,header_buffer, hsz);
					}
					headersDone = 1;
                } else {
					/* we may have more headers yet to read */
					if (contentLength < sizeof(header_buffer) - hsz -1) {
						strcpy(header_buffer + hsz, writebuf);
						writelen = 0;
					} else {
						/* not enough room for headers, or no headers */
						/* flush the header buffer */
						sapi_module.write(req_info,header_buffer, hsz);
						headersDone = 1;	
					}
				}
                
                
            }
            // write the data to the web server
            if (writelen) {
                sapi_module.write(req_info,writebuf, writelen);
            }
            break;
                            
		case FCGI_GET_VALUES_RESULT:
			/* XXX coming soon */
		case FCGI_UNKNOWN_TYPE:
			/* XXX coming soon */
        default:        
            req_info->exitStatus = FCGI_UNKNOWN_TYPE;
            return FALSE;
		}
    }
    return TRUE;
}



int FCGI_Abort(sapi_request_info *req_info)
{
	unsigned int count = 0;
	FCGI_Header	header; 

	FCGI_MakeHeader(&header, FCGI_ABORT_REQUEST, 
		(int)req_info->server_context, sizeof(header), 0);

    count = OS_Write(req_info->appServerSock, 
				(char *)&header, sizeof(header));

    if(count != sizeof(header)) {
        req_info->exitStatus = OS_Errno;
		return 0;
    }
	return 1;
}

/* XXX quick and dirty, needs to be vastely improved */
char *stristr(char *haystack, char *needle)
{
	int hl = strlen(haystack);
	int nl = strlen(needle);
	int i=0;
	char *hc = (char *)malloc(hl + 1);
	char *nc = (char *)malloc(nl + 1);
	char *p = NULL;
	memcpy(hc, haystack, hl + 1);
	memcpy(nc, needle, nl + 1);
	_strlwr(hc);
	_strlwr(nc);
	p = strstr(hc, nc);
	if (p) {
		p = (p - hc) + haystack;
	}
	free(hc);
	free(nc);
	return p;
}