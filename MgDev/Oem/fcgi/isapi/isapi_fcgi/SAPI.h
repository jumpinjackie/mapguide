#ifndef SAPI_H
#define SAPI_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef struct {
	void *server_context;
	void *security_token;

	char *request_method;
	char *query_string;
	char *path_translated;
	char *path_info;
	unsigned long content_length;
	char *content_type;
	int keepalive;
	unsigned long post_available;
	unsigned char *post_content;
	unsigned long post_read;
	char post_tmp_file[MAX_PATH];
	FILE *post_file;

	char **env;

	/* fast cgi information */
	int role;
	int keepConnection;
	char *bindPath;
	int appServerSock;  /* Socket connected to FastCGI application,
                                 * used by AppServerReadHandler and
                                 * AppServerWriteHandler. */
	int exitStatus;
} sapi_request_info;


struct _sapi_module_struct {
	int (*write)(sapi_request_info *server_context, const unsigned char *str, unsigned long str_length);
	int (*getenv)(sapi_request_info *server_context, const char *name, char *buf, unsigned long *bufsz);
	void (*send_headers)(sapi_request_info *server_context, char *statusline, char *sapi_headers);
};

extern struct _sapi_module_struct sapi_module;

#define SAPI_SUCCESS 0
#define SAPI_FAILURE 1
/* 
	defines for getenv 
	these are already defined for ISAPI, other interfaces will need to return them
	appropriately, see GetServerVariable documentation
*/
#ifndef ERROR_INVALID_PARAMETER
#define ERROR_INVALID_PARAMETER   1 /* Bad connection handle, in either lpszVariableName or lpdwSizeOfBuffer. */
#define ERROR_INVALID_INDEX		  2 /* Bad or unsupported variable identifier. */
#define ERROR_INSUFFICIENT_BUFFER 3 /* Buffer too small. The required buffer size is *lpdwSizeofBuffer. */
#define ERROR_NO_DATA			  4 /* The data requested is not available.  */
#endif

#ifdef __cplusplus
} /* end extern C */
#endif

#endif

