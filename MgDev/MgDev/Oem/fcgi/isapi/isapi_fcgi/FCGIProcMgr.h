#ifndef __FCGIProcMgr_h__
#define __FCGIProcMgr_h__

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <winsock2.h>
#include <winreg.h>
# include <process.h>
#include <assert.h>

extern "C" {
#include "fcgi_config.h"
#include "fcgimisc.h"
#include "fcgiapp.h"
#include "fcgiappmisc.h"
#include "fastcgi.h"
#include "fcgios.h"
#include "fcgi_server.h"
#include <time.h>
}

#define SIZE_EXT 32
#define BASE_REG_PATH "SOFTWARE\\FASTCGI"
#define DEFAULT_MAX_SERVERS 25

class Environment {
public:
	Environment();
	~Environment();
	char *getEnv(char *name);
	void putEnv(char *name, char *value);
	void putEnv(char *var);
	char *getEnviron() { return env; };
private:
	char *env;
	HANDLE mutex;
};

typedef struct _node {
	struct _node *pNext;
	struct _node *pPrev;
	void *pData;
} CListNode;

class CList {
public:
	CList() {
		pTail = NULL;
		pHead = NULL;
		lmutex = CreateMutex(NULL, FALSE, NULL);
	}
	~CList() {
		CloseHandle(lmutex);
	}
	BOOL Append(CListNode *pNode)
	{
		if (WaitForSingleObject(lmutex, INFINITE) != WAIT_OBJECT_0) {
			return FALSE;
		}
		
		if (pHead == NULL) {
			pHead = pNode;
			pNode->pPrev = NULL;
		}
		else {
			pTail->pNext = pNode;
			pNode->pPrev = pTail;
		}
		pTail = pNode;
		pNode->pNext = NULL;
		ReleaseMutex(lmutex);
		return TRUE;
	}

	BOOL Remove(CListNode *pNode)
	{
		if (WaitForSingleObject(lmutex, INFINITE) != WAIT_OBJECT_0) {
			return FALSE;
		}
		if (pNode->pPrev == NULL)
			pHead = pNode->pNext;
		else
			pNode->pPrev->pNext = pNode->pNext;
		if (pNode->pNext == NULL)
			pTail = pNode->pPrev;
		else
			pNode->pNext->pPrev = pNode->pPrev;
		ReleaseMutex(lmutex);
		return TRUE;
	}

	CListNode *GetHead() { return pHead; }
private:
    HANDLE lmutex;
	CListNode *pHead;
	CListNode *pTail;
};

typedef struct _FCGIProccessInfo {
	PROCESS_INFORMATION pInfo;
	HANDLE terminationEvent;
	HANDLE listenMutex;
	unsigned int ltime;
} FCGIProccessInfo;

class FCGIProcess: public CList {
public:
	FCGIProcess() {
		startServers = 0;
		incServers = 0;
		numServers = 0;
		maxServers = DEFAULT_MAX_SERVERS;
		memset(ext,0,sizeof(ext));
		memset(bindPath,0,sizeof(bindPath));
		memset(appPath,0,sizeof(appPath));
		memset(args,0,sizeof(args));
		pmutex = CreateMutex(NULL, FALSE, NULL);
	}
	~FCGIProcess() {
		SignalShutdown();
		Shutdown();
		CloseHandle(pmutex);
	}
	char ext[SIZE_EXT];
	char bindPath[MAX_PATH];
	char appPath[MAX_PATH];
	char args[MAX_PATH];
	unsigned int isFilter;
	unsigned int startServers;
	unsigned int incServers;
	unsigned int maxServers;
	unsigned int numServers;
	unsigned int timeout;
	Environment env;

	void Add(FCGIProccessInfo *data)
	{
		CListNode *pNode = new CListNode;
		pNode->pData = data;
		Append(pNode);
		++numServers;
	}
	
	void SignalShutdown(void);
	void Cleanup(void);
	void Shutdown(void);
	HANDLE pmutex;
};

class FCGIProcessList: public CList {
public:
	FCGIProcessList () {
		pCurrent = NULL;
	}
	~FCGIProcessList() {
		CListNode *node = GetHead();
		while (node) {
			Remove(node);
			FCGIProcess * proc = (FCGIProcess *)node->pData;
			if (proc) delete proc;
			delete node;
			node = GetHead();
		}
	}
	void Add(FCGIProcess *pData) {
		CListNode *node = new CListNode;
		node->pData = pData;
		Append(node);
	}
	FCGIProcess *GetFirst() {
		pCurrent = GetHead();
		return pCurrent?(FCGIProcess *)pCurrent->pData:NULL;
	}
	FCGIProcess *GetNext() {
		if (pCurrent && pCurrent->pNext) {
			pCurrent = pCurrent->pNext;
			return (FCGIProcess *)pCurrent->pData;
		}
		return NULL;
	};
private:
	CListNode *pCurrent;
};

class FCGIProccessManager
{
public:
	FCGIProccessManager();
	~FCGIProccessManager();
	void ConfigureServers();
	FCGIProcess *ConfigureDynamicServers(char *bindpath,FCGIProcess *parent, char *path);
	void InitiateServers(char *extension);
	void CheckServers(char *bindPath);
	void CleanupServers();
	int StartServers(FCGIProcess *proc);
	void StopServers();
	char *GetBindingForPath(char *path_translated);
	BOOL isFilter(char *bindpath);
	HANDLE pm_shutdown;
	HANDLE shutdownEvent;
private:
	HANDLE pm_thread;
	DWORD pm_tid;
	FCGIProcessList processList;
};

extern FCGIProccessManager *procmgr;
extern DWORD dwServerImpersonate;
#endif /* __FCGIProcMgr_h__ */
