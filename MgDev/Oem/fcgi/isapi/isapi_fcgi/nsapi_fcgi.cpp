
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <winsock2.h>
# include <process.h>
#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>

/*
 * If neither XP_UNIX not XP_WIN32 is defined, try to guess which one.
 * Ideally, this should be done by the configure script.
 */
#if !defined(XP_UNIX) && !defined(XP_WIN32)
	#if defined(_WIN32)
		#define XP_WIN32
	#else
		#define XP_UNIX
	#endif
#endif

extern "C" {
/*
 * NSAPI includes
 */
#include "nsapi.h"
	/* these are depricated, but may be needed if 
	   compiling with an old netscape server
	#include "base/pblock.h"
	#include "base/session.h"
	#include "frame/req.h"
	#include "frame/protocol.h"  
	#include "base/util.h"       
	#include "frame/log.h"       
	*/
}

#include "SAPI.h"

#include "fcgi_server.h"
#include "FCGIProcMgr.h"

long	m_nMaxPostSize = 0;
#define MAX_CUSTOM_VARS 50
char    *m_szCustomVars[MAX_CUSTOM_VARS];
char    m_szCustomVarsBuf[1024];
DWORD    m_nCountCustomVars = 0;
inline void _free_request_info(sapi_request_info *req_info);
////////////////////////////////////////////////////////////////
//
//
#define NSAPI_NAME "NSAPI_FASTCGI"
#define NSAPI_READ_TIMEOUT 60 /* 60 seconds */
/*
 * Structure to encapsulate the NSAPI request in SAPI
 */
typedef struct nsapi_request_context {
	pblock	*pb;
	Session	*sn;
	Request	*rq;
	int	read_post_bytes;
} nsapi_request_context;

/*
 * Mappings between NSAPI names and environment variables. This
 * mapping was obtained from the sample programs at the iplanet
 * website.
 */
enum {
	headers,
	reqpb,
	vars,
	client,
	remote_host,
	https,
	server_name,
	server_port,
	server_software,
	server_url
} headerTypes;

typedef struct nsapi_equiv {
	const char *env_var;
	const char *nsapi_eq;
	int type;
} nsapi_equiv;

static nsapi_equiv nsapi_headers[] = {
	{ "AUTH_USER",				"auth-user" , vars},
	{ "AUTH_TYPE",				"auth-type" , vars},
	{ "CLIENT_CERT",			"auth-cert" , vars},
	{ "CONTENT_LENGTH",			"content-length" , headers},
	{ "CONTENT_TYPE",			"content-type" , headers},
	{ "HTTP_ACCEPT",			"accept" , headers},
	{ "HTTP_ACCEPT_ENCODING",	"accept-encoding" , headers},
	{ "HTTP_ACCEPT_LANGUAGE",	"accept-language" , headers},
	{ "HTTP_ACCEPT_CHARSET",	"accept-charset" , headers},
	{ "HTTP_AUTHORIZATION",		"authorization" , headers},
	{ "HTTP_COOKIE",			"cookie" , headers},
	{ "HTTP_IF_MODIFIED_SINCE",	"if-modified-since" , headers},
	{ "HTTP_REFERER",			"referer" , headers},
	{ "HTTP_USER_AGENT",		"user-agent" , headers},
	{ "HTTP_USER_DEFINED",		"user-defined" , headers},
	{ "HTTPS",					NULL , https},
	{ "HTTPS_KEYSIZE",			"keysize" , client},
	{ "HTTPS_SECRETSIZE",		"secret-keysize" , client},
	{ "PATH_INFO",				"path-info" , vars},
	{ "PATH_TRANSLATED",		"path" , vars},
	{ "QUERY_STRING",			"query" , reqpb},
	{ "REMOTE_ADDR",			"ip" , client},
	{ "REMOTE_HOST",			NULL , remote_host},
	{ "REMOTE_IDENT",			"from" , headers},
	{ "REMOTE_USER",			"auth-user" , vars},
	{ "REQUEST_METHOD",			"method" , reqpb},
	{ "REQUEST_LINE",			"clf-request" , reqpb},
	{ "SCRIPT_NAME",			"uri" , reqpb},
	{ "SERVER_NAME",			NULL , server_name},
	{ "SERVER_PORT",			NULL , server_port},
	{ "SERVER_PROTOCOL",		"protocol" , reqpb},
	{ "SERVER_SOFTWARE",		NULL , server_software},
	{ "SERVER_URL",				NULL , server_url}
};
static size_t nsapi_translate_size = sizeof(nsapi_headers)/sizeof(nsapi_headers[0]);

char *_getEnvironmentVariable(sapi_request_info *req_info, char *name, char *value, unsigned int vsize)
{
	char *val = NULL;
	nsapi_request_context *ctx = (nsapi_request_context *) req_info->server_context;
	char buf[50];
	char *p = buf;

	/* mangle the requested name into a iplanet style name */
	strncpy(buf, name, sizeof(buf)-1);
	while (*p) {
		if (*p == '_') *p = '-';
		++p;
	}
	_strlwr(buf);

	if (_strnicmp(name, "http_", 5) == 0) {
		val = pblock_findval(buf, ctx->rq->headers);
		/* no value, try chopping off the http_ */
		if (!val) 
			val = pblock_findval(&buf[6], ctx->rq->headers);
	}
	/* look everywhere for it! */
	if (!val) {
		if ((val = pblock_findval(name, ctx->rq->headers))==NULL) {
			if ((val = pblock_findval(name, ctx->rq->reqpb))==NULL) {
				if ((val = pblock_findval(name, ctx->rq->vars))==NULL) {
					val = pblock_findval(name, ctx->sn->client);
				}
			}
		}
	}

	if (val) strncpy(value, val, vsize-1);
	return value;
}

/* create an environment for fastcgi to send */
inline void _prepare_environment(sapi_request_info *req_info)
{
	char buf[100];
	char envvar[2048];
	nsapi_request_context *ctx = (nsapi_request_context *) req_info->server_context;
	DWORD envSize = nsapi_translate_size + m_nCountCustomVars;
	DWORD count = 0, envcount = 0;
	req_info->env = (char **)calloc(envSize, sizeof(char *));

	for(count=0; count < nsapi_translate_size; count++) {
		char *val = NULL;
		switch(nsapi_headers[count].type) {
		case headers:
			val = pblock_findval(nsapi_headers[count].nsapi_eq, ctx->rq->headers);
			break;
		case reqpb:
			val = pblock_findval(nsapi_headers[count].nsapi_eq, ctx->rq->reqpb);
			break;
		case vars:
			val = pblock_findval(nsapi_headers[count].nsapi_eq, ctx->rq->vars);
			break;
		case client:
			val = pblock_findval(nsapi_headers[count].nsapi_eq, ctx->sn->client);
			break;
		case remote_host:
			val = session_dns(ctx->sn) ? session_dns(ctx->sn) : pblock_findval("ip", ctx->sn->client);  
			break;
		case https:
			val = security_active ? "ON" : "OFF"; 
			break;
		case server_name:
			val = conf_getglobals()->Vserver_hostname;
			break;
		case server_port:
			_snprintf(buf, sizeof(buf)-1, "%d", conf_getglobals()->Vport);
			val = buf;
			break;
		case server_software:
			val = system_version();
			break;
		case server_url:
			val = http_uri2url_dynamic("","", ctx->sn, ctx->rq);  
			break;
		default:
			break;
		}
		if (val) {
			_snprintf(envvar, sizeof(envvar)-1, "%s=%s",nsapi_headers[count].env_var,val);
			req_info->env[envcount++] = strdup(envvar);
		}
	}

	/* handle custom vars */
	for (DWORD x = 0; x < m_nCountCustomVars; x++ ) {
		char szValue[1024];
        szValue[0] = '\0';
		char *envname = m_szCustomVars[x];
		if (_getEnvironmentVariable( 
                req_info, 
                envname, 
                szValue, 
                sizeof(szValue)) && szValue[0] != 0) {
			_snprintf(buf, sizeof(buf)-1,"%s=%s",envname,szValue);
			req_info->env[count++] = strdup(buf);
		}
	}

	req_info->env[envcount] = NULL;
}

/* pre-read post data into a temp file 
	TODO: this could use async reading to lower cpu usage and improve performance
*/
inline int _prepare_postdata(sapi_request_info *req_info)
{
	nsapi_request_context *ctx = (nsapi_request_context *) req_info->server_context;
	netbuf *nbuf = ctx->sn->inbuf;
	unsigned int length = 0;
	if (req_info->content_length <= 0) return 1;

	if (req_info->content_length < 48*1024) {
		/* it's a small post, just read it into a buffer */
		unsigned int read = nbuf->cursize;
		req_info->post_content = (unsigned char *)malloc(req_info->content_length);
		if (read > 0) {
			memcpy(req_info->post_content, nbuf->inbuf, read);
		}

		length = net_read(ctx->sn->csd, (char *)(req_info->post_content + read), 
			req_info->content_length - read, NSAPI_READ_TIMEOUT);
		if (length == IO_ERROR) {
			free(req_info->post_content);
			return 0;
		}

		return 1;
	} else {
		/* get a temp file */
		char filename[MAX_PATH];
		long bytesRead = 0;

		GetTempFileName(getenv("TEMP"), "fcgi", 0, filename);
		req_info->post_file = fopen(filename, "wb+");
		if (!req_info->post_file) return 0;

		/* write what IIS has already retreived */
		if (nbuf->cursize > 0) {
			bytesRead = fwrite(nbuf->inbuf, nbuf->cursize, 1, req_info->post_file);
			if (bytesRead != nbuf->cursize) {
				fclose(req_info->post_file);
				return 0;
			}
		}

		/* send any additional data to client */
		unsigned int totalBytes = req_info->content_length;
		char work_buffer[8192];
		unsigned int bytesToRead = sizeof(work_buffer);

		while (bytesRead < req_info->content_length) {
			unsigned int dwWritten = 0;
			length = net_read(ctx->sn->csd, work_buffer, bytesToRead, NSAPI_READ_TIMEOUT);
			if (length == IO_ERROR) {
				fclose(req_info->post_file);
				return 0;
			}
			if (length == IO_EOF) {
				break;
			}

			bytesRead+=length;
			
			if ((dwWritten = fwrite(work_buffer, bytesToRead, 1, req_info->post_file)) != length) {
				fclose(req_info->post_file);
				return 0;
			}

			// how many more bytes to read
			DWORD bytesToRead = sizeof(work_buffer);
			if (bytesToRead > (totalBytes - bytesRead)) 
				bytesToRead = (totalBytes - bytesRead);

			if (m_nMaxPostSize > 0 && bytesRead > m_nMaxPostSize) {
				fclose(req_info->post_file);
				return 0;
			}
		}
		/* reset to beginning of file */
		fseek(req_info->post_file, 0, SEEK_SET);
	}
	return 1;
}

inline sapi_request_info *_init_request_info(pblock *pb, Session *sn, Request *rq)
{
	sapi_request_info *req_info = (sapi_request_info *)malloc(sizeof(sapi_request_info));
	if (!req_info) return NULL;
	
	req_info->server_context = (nsapi_request_context *)malloc(sizeof(nsapi_request_context));
	if (!req_info->server_context) {
		free(req_info);
		return NULL;
	}
	nsapi_request_context *ctx = (nsapi_request_context *) req_info->server_context;

	ctx->pb = pb;
	ctx->sn = sn;
	ctx->rq = rq;

	char *query_string = pblock_findval("query", rq->reqpb);
	char *uri = pblock_findval("uri", rq->reqpb);
	char *path_info = pblock_findval("path-info", rq->vars);
	char *path_translated = pblock_findval("path", rq->vars);
	char *request_method = pblock_findval("method", rq->reqpb);
	char *content_type = pblock_findval("content-type", rq->headers);
	char *content_length = pblock_findval("content-length", rq->headers);

	if ((path_translated == NULL) && (uri != NULL))
		path_translated = request_translate_uri(uri, sn);

#if defined(NSAPI_DEBUG)
	log_error(LOG_INFORM, "nsapi_request_ctor", sn, rq,
		"query_string = %s, "
		"uri = %s, "
		"path_info = %s, "
		"path_translated = %s, "
		"request_method = %s, "
		"content_type = %s, "
		"content_length = %s",
		query_string,
		uri,
		path_info,
		path_translated,
		request_method,
		content_type,
		content_length);
#endif

	req_info->path_info = strdup(path_info);
	req_info->path_translated = strdup(path_translated);
	req_info->request_method = strdup(request_method);
	req_info->query_string = strdup(query_string);
	req_info->content_length = (content_length == NULL) ? 0 : strtoul(content_length, 0, 0);
	req_info->content_type = strdup(content_type);
	req_info->bindPath = procmgr->GetBindingForPath(req_info->path_translated);
	req_info->post_file = NULL;
	req_info->post_content = NULL;
	req_info->post_available = 0;
	req_info->env = NULL;

	if (!req_info->bindPath) {
		_free_request_info(req_info);
		return NULL;
	}

	_prepare_environment(req_info);
	_prepare_postdata(req_info);
	return req_info;
}

inline void _free_request_info(sapi_request_info *req_info) 
{
	if (req_info->post_file) fclose(req_info->post_file);
	/* free up some memory */
	if (req_info->env) {
		DWORD i = 0;
		while (req_info->env[i] != NULL) free(req_info->env[i++]);
	}
	free(req_info->env);
	free(req_info->path_info);
	free(req_info->path_translated);
	free(req_info->request_method);
	free(req_info->query_string);
	free(req_info->content_type);
	free(req_info->server_context);
	free(req_info);
}

int NSAPI_PUBLIC
FCGIServiceHandler(pblock *pb, Session *sn, Request *rq)
{
	int retval = REQ_PROCEED;

	sapi_request_info *req_info = _init_request_info(pb,sn,rq);
	if (!req_info) return REQ_EXIT;
	FCGI_Responder(req_info);
	if (req_info->exitStatus != FCGI_SUCCESS)
		retval = REQ_EXIT;
	_free_request_info(req_info);

	return retval;
}

int NSAPI_PUBLIC 
FCGIAuthTransHandler(pblock * pb, Session * sn, Request * rq)
{
/*	switch (notificationType) {
		case SF_NOTIFY_AUTHENTICATION: {
				HTTP_FILTER_AUTHENT *auth = (HTTP_FILTER_AUTHENT *) pvNotification;
				// fastcgi specific
				FCGIAppServer app;
				app.Authorizer(pfc);
				if (app.Status() == FCGI_SUCCESS)
					return SF_STATUS_REQ_HANDLED_NOTIFICATION;
			}
			break;
	}
*/	return REQ_PROCEED;
}

static int nsapi_write(sapi_request_info *req_data, const unsigned char *str, unsigned long str_length)
{
	int retval;
	nsapi_request_context *rc = (nsapi_request_context *)req_data->server_context;

	retval = net_write(rc->sn->csd, (char *)str, str_length);
	//if (retval == IO_ERROR /*-1*/ || retval == IO_EOF /*0*/)
	//	php_handle_aborted_connection();
	return retval;
}

static int nsapi_getenv(sapi_request_info *req_data, const char *name, char *buf, unsigned long *bufsz)
{
	if (_getEnvironmentVariable(req_data, (char *)name, buf, *bufsz)) return 1;
	return 0;
}

void nsapi_sendheaders(sapi_request_info *req_data, char *statusline, char *sapi_headers)
{
	nsapi_request_context *rc = (nsapi_request_context *)req_data->server_context;
	/* we have a text block we must parse and add our headers */
	char *p = sapi_headers;
	char *e = NULL;
	int http_code = atoi(statusline);

	param_free(pblock_remove("content-type", rc->rq->srvhdrs));
	pblock_nvinsert("content-type", "text/html", rc->rq->srvhdrs);
	
	while ((e = strchr(p, '\n')) != NULL) {
		*e = 0;
		if (*(e-1)=='\r') *(e-1)=0;
		char *c = strchr(p, ':');
		if (c) {
			*c = 0;
			++c;
			while (*c == ' ') ++c;
			
			if (stricmp(p, "content-type") == 0) {
				param_free(pblock_remove("content-type", rc->rq->srvhdrs));
				pblock_nvinsert("content-type", c, rc->rq->srvhdrs);
			} else if (stricmp(p, "set-cookie") == 0) {
				pblock_nvinsert("set-cookie", c, rc->rq->srvhdrs);
			} else if (stricmp(p, "status") == 0) {
				http_code = atoi(c);
			} else {
				param_free(pblock_remove(p, rc->rq->srvhdrs));
				pblock_nvinsert(p, c, rc->rq->srvhdrs);
			}
		}
		p = e + 1;
	}
	protocol_status(rc->sn, rc->rq, http_code, NULL);
	protocol_start_response(rc->sn, rc->rq);
}

struct _sapi_module_struct sapi_module = {
	nsapi_write,			/* unbuffered write */
	nsapi_getenv,			/* getenv */
	nsapi_sendheaders		/* send header handler */
};

void _initConfiguration() {
	HKEY hkey;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, BASE_REG_PATH, 0, KEY_READ, &hkey) == ERROR_SUCCESS) {
		DWORD dwType = REG_DWORD;
		DWORD dwBufSize = sizeof(DWORD);
		if (RegQueryValueEx(hkey, "MaxPostData", 0, &dwType, (unsigned char *)&m_nMaxPostSize, &dwBufSize) != ERROR_SUCCESS
			|| dwType != REG_DWORD) {
			m_nMaxPostSize = 0;
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
			while ((p = strchr(b, '\n')) != NULL && m_nCountCustomVars < MAX_CUSTOM_VARS) {
				*p = 0;
				if (*(p-1) == '\r') *(p-1)=0;
				m_szCustomVars[m_nCountCustomVars++] = b;
				b = p + 1;
			}
		}

		RegCloseKey(hkey);
	}
}

__declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
          //
          // Prevent the system from calling DllMain
          // when threads are created or destroyed.
          //
          DisableThreadLibraryCalls( hinstDLL);
		  putenv("FCGI_INTERFACE=NSAPI");
		  _initConfiguration();
		  procmgr = new FCGIProccessManager();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			delete procmgr;
			break;
	}
	return TRUE;
}
