#ifndef __FCGIAppServer_h__
#define __FCGIAppServer_h__

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <winsock2.h>
# include <process.h>
#include <assert.h>

#include "SAPI.h"

extern "C" {
#include "fcgi_config.h"
#include "fcgimisc.h"
#include "fcgiapp.h"
#include "fcgiappmisc.h"
#include "fastcgi.h"
#include "fcgios.h"
}

////////////////////////////////////////////////////////////////
//
// FCGIAppServer
// responsible for handline a single request

// error status for fcgi request
#define FCGI_SUCCESS 0
#define FCGI_ERROR   1

class FCGIAppServer
{
public:
	FCGIAppServer();
	~FCGIAppServer();
	BOOL Responder(sapi_request_info *req_info);
	BOOL Authorizer(sapi_request_info *req_info);
	BOOL Filter(sapi_request_info *req_info, DWORD notificationType);
	DWORD Status() { return req_info->exitStatus; }

	/* request process functions */
	BOOL Connect(char *bindPath);
	BOOL BeginRequest(int requestId, int role, int keepConnection);
	BOOL SendEnvironment();
	BOOL SendPOSTData();
	BOOL SendFile();
	BOOL HandleResponse();
	BOOL Abort();
	BOOL FCGIAppServer::WriteToServer(char *data, DWORD dataLen);

	/* header support functions */
	void MakeHeader(FCGI_Header *header, 
		int type,
        int requestId,
        int contentLength,
        int paddingLength);
	void MakeBeginRequestBody(
		FCGI_BeginRequestBody *body,
        int role,
        int keepConnection);
	void BuildNameValueHeader(
        int nameLen,
        int valueLen,
        unsigned char *headerBuffPtr,
        int *headerLenPtr);
	BOOL SendEnvironmentVariable(
		FCGX_Stream *paramsStream,
		const char *envName,
		const char *envVal);
private:
	sapi_request_info *req_info;
};

#endif