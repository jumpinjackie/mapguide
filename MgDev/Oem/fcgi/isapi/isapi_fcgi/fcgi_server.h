#ifndef __FCGI_SERVER_h__
#define __FCGI_SERVER_h__

#ifdef __cplusplus
extern "C" {
#endif

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <winsock2.h>
# include <process.h>
#include <assert.h>

#include "SAPI.h"

#include "fcgi_config.h"
#include "fcgimisc.h"
#include "fcgiapp.h"
#include "fcgiappmisc.h"
#include "fastcgi.h"
#include "fcgios.h"

#define FCGI_SUCCESS 0
#define FCGI_ERROR   1
#define FCGI_SERVER_VERSION "2.2.2 0.5.2 beta"
#define FCGI_ENVIRON_VER "FCGI_SERVER_VERSION=" FCGI_SERVER_VERSION

extern void InitiateServers(char *szExtension);
extern void CheckServers(char *szExtension);

int FCGI_Responder(sapi_request_info *req_info);
int FCGI_Authorizer(sapi_request_info *req_info);
int FCGI_Filter(sapi_request_info *req_info);

/* request process functions */
int FCGI_Connect(sapi_request_info *req_info);
int FCGI_BeginRequest(sapi_request_info *req_info);
int FCGI_SendEnvironment(sapi_request_info *req_info);
int FCGI_SendPOSTData(sapi_request_info *req_info);
int FCGI_SendFile(sapi_request_info *req_info);
int FCGI_HandleResponse(sapi_request_info *req_info);
int FCGI_Abort(sapi_request_info *req_info);
int FCGI_WriteToServer(sapi_request_info *req_info, char *data, unsigned int dataLen);

/* header support functions */
void FCGI_MakeHeader(
	FCGI_Header *header, 
	int type,
    int requestId,
    int contentLength,
    int paddingLength);
void FCGI_MakeBeginRequestBody(
	FCGI_BeginRequestBody *body,
    int role,
    int keepConnection);
void FCGI_BuildNameValueHeader(
    int nameLen,
    int valueLen,
    unsigned char *headerBuffPtr,
    int *headerLenPtr);
int FCGI_SendEnvironmentVariable(
	sapi_request_info *req_info,
	FCGX_Stream *paramsStream,
	const char *envName,
	const char *envVal);

#ifdef __cplusplus
} /* end extern C */
#endif

#endif



