#include "FCGIProcMgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "md5.h"

DWORD dwServerImpersonate = 0;
DWORD gStartServers,gIncServers,gMaxServers,gTimeout;

Environment::Environment()
{
	/* duplicate the current environment */
	char *tenv = GetEnvironmentStrings();
	char *p = tenv;
	while (p && *p) {
		p += strlen(p) + 1;
	}
	int sizeenv = p - tenv + 1;
	env = (char *)calloc(sizeenv, 1);
	memcpy(env, tenv, sizeenv);
	FreeEnvironmentStrings(tenv);
	mutex = CreateMutex(NULL, FALSE, NULL);
}

Environment::~Environment()
{
	if (env) free(env);
	CloseHandle(mutex);
}

char *Environment::getEnv(char *name)
{
	/* could be optimized by using read/write muticies */
	WaitForSingleObject(mutex, INFINITE);
	char *p = env;
	char *ret = NULL;
	while (p && *p) {
		char *e = strchr(p, '=');
		if (e) {
			if (strnicmp(name, p, e-p) == 0) {
				ret = e + 1;
				break;
			}
		}
		p += strlen(p) +1;
	}
	ReleaseMutex(mutex);
	return ret;
}

void Environment::putEnv(char *var)
{
	if (!var) return;
	char *v = strdup(var);
	if (!v) return;
	char *e = strchr(v, '=');
	char *val = NULL;
	if (e) {
		if (strlen(v) > e-v) val = e+1;
		*e = 0;
	}

	putEnv(v,val);
	free(v);
}

void Environment::putEnv(char *name, char *value)
{
	WaitForSingleObject(mutex, INFINITE);
	/* do we have it already? */
	char *p = env;
	char *locs = NULL;
	char *loce = NULL;
	while (p && *p) {
		char *e = strchr(p, '=');
		if (e) {
			if (strnicmp(name, p, e-p) == 0) {
				locs = p;
				loce = p + strlen(p) + 1;
			}
		}
		p += strlen(p) +1;
	}

	int newvarsize = strlen(name) + strlen(value) + 2;
	int sizeenv = locs?
		(locs - env) + (p - loce) + newvarsize + 1
		: p - env + newvarsize + 1;

	char *buf = (char *)calloc(sizeenv, 1);
	if (locs) {
		memcpy(buf, env, locs-env);
		sprintf(buf + (locs-env), "%s=%s", name, value);
	} else {
		memcpy(buf, env, p-env);
		sprintf(buf + (p-env), "%s=%s", name, value);
	}
	if (loce) {
		memcpy(buf + (locs-env) + newvarsize, loce, p-loce);
	}
	buf[sizeenv - 2] = 0;
	buf[sizeenv - 1] = 0;

	free(env);
	env = buf;
	ReleaseMutex(mutex);
}


void FCGIProcess::Cleanup(void)
{
	CListNode *pNode = GetHead();
	FCGIProccessInfo *entry = NULL;
	numServers = 0;
	while (pNode) {
		DWORD exit;
		entry = (FCGIProccessInfo *)pNode->pData;
		GetExitCodeProcess(entry->pInfo.hProcess,  &exit);
		if (exit != STILL_ACTIVE) {
			/* clear the entry */
			CloseHandle(entry->terminationEvent);
			CloseHandle(entry->pInfo.hThread);
			CloseHandle(entry->pInfo.hProcess);
			CloseHandle(entry->listenMutex);

			Remove(pNode);
			delete pNode;
			delete entry;
			pNode = GetHead();
		} else {
			pNode = pNode->pNext;
			++numServers;
		}
	}

	pNode = GetHead();
	DWORD currentServers = numServers;
	time_t now;
	time(&now);
	while (pNode && currentServers > startServers) {
		if (entry->ltime && timeout &&
				entry->ltime + timeout < now) {
			SetEvent(entry->terminationEvent);
			currentServers--;
		} else if (!entry->ltime) {
			entry->ltime = now;
		}
		pNode = pNode->pNext;
	}
}

void FCGIProcess::SignalShutdown(void)
{
	CListNode *pNode = GetHead();
	while (pNode) {
		FCGIProccessInfo *entry = (FCGIProccessInfo *)pNode->pData;
		SetEvent(entry->terminationEvent);
		pNode = pNode->pNext;
	}
}

void FCGIProcess::Shutdown(void)
{
	CListNode *pNode = GetHead();
	while (pNode) {
		DWORD exit;
		FCGIProccessInfo *entry = (FCGIProccessInfo *)pNode->pData;
		GetExitCodeProcess(entry->pInfo.hProcess,  &exit);
		if (exit == STILL_ACTIVE) {
			fprintf(stderr, "Killing process [%d] [%s]\n",entry->pInfo.dwProcessId,bindPath);
			TerminateProcess(entry->pInfo.hProcess, 1);
		}
		CloseHandle(entry->terminationEvent);
		CloseHandle(entry->pInfo.hThread);
		CloseHandle(entry->pInfo.hProcess);
		CloseHandle(entry->listenMutex);

		Remove(pNode);
		delete pNode;
		delete entry;
		pNode = GetHead();
	}
}

DWORD __stdcall ProcessTimeoutManager(void *p)
{
	FCGIProccessManager *prcman = (FCGIProccessManager*)p;
	do {
		prcman->CleanupServers();
	} while (WaitForSingleObject(prcman->shutdownEvent,10000) == WAIT_TIMEOUT);
	CloseHandle(prcman->shutdownEvent);
	return 0;
}

FCGIProccessManager *procmgr;

FCGIProccessManager::FCGIProccessManager() 
{
	DWORD tid;
	OS_LibInit(NULL);
	shutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE thread = CreateThread(NULL, 0, ProcessTimeoutManager, this, 0, &tid);
	// quick hack to start some servers
	ConfigureServers();
}

FCGIProccessManager::~FCGIProccessManager() 
{
	SetEvent(shutdownEvent);
	StopServers();
	OS_LibShutdown();
}

void FCGIProccessManager::ConfigureServers() 
{
	HKEY hkey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, BASE_REG_PATH, 0, KEY_READ, &hkey) == ERROR_SUCCESS) {
		/* enumerate keys */
		DWORD dwIndex = 0;
		DWORD cbExtension = SIZE_EXT;
		FILETIME ftLastWriteTime;
		DWORD err = 0;

		/* do we do impersonation? */
		DWORD dwBufSize = sizeof(DWORD);
		DWORD dwType = REG_DWORD;
		if (RegQueryValueEx(hkey, "Impersonate", 0, &dwType, (unsigned char *)&dwServerImpersonate, &dwBufSize) != ERROR_SUCCESS
			|| dwType != REG_DWORD) {
			dwServerImpersonate = 1;
		}
		OS_SetImpersonate(dwServerImpersonate);

		/* get the number of servers to start */
		dwBufSize = sizeof(DWORD);
		dwType = REG_DWORD;
		if (RegQueryValueEx(hkey, "StartServers", 0, &dwType, (unsigned char *)&gStartServers, &dwBufSize) != ERROR_SUCCESS
			|| dwType != REG_DWORD) {
			gStartServers = 1;
		}

		/* get the number of servers to start */
		dwBufSize = sizeof(DWORD);
		dwType = REG_DWORD;
		if (RegQueryValueEx(hkey, "IncrementServers", 0, &dwType, (unsigned char *)&gIncServers, &dwBufSize) != ERROR_SUCCESS
			|| dwType != REG_DWORD || gIncServers == 0) {
			gIncServers = (int)gIncServers / 2;
			if (gIncServers < 2) gIncServers = 2;
		}

		/* get the max number of servers to start */
		dwBufSize = sizeof(DWORD);
		dwType = REG_DWORD;
		if (RegQueryValueEx(hkey, "MaxServers", 0, &dwType, (unsigned char *)&gMaxServers, &dwBufSize) != ERROR_SUCCESS
			|| dwType != REG_DWORD || gMaxServers == 0) {
			gMaxServers = DEFAULT_MAX_SERVERS;
		}

		/* get the idle timeout */
		dwBufSize = sizeof(DWORD);
		dwType = REG_DWORD;
		if (RegQueryValueEx(hkey, "Timeout", 0, &dwType, (unsigned char *)&gTimeout, &dwBufSize) != ERROR_SUCCESS
			|| dwType != REG_DWORD || gTimeout == 0) {
			gTimeout = 600; // default 5 minutes
		}


		FCGIProcess *proc = new FCGIProcess();
		while ((err = RegEnumKeyEx(hkey, dwIndex, proc->ext, &cbExtension, NULL, NULL, NULL, &ftLastWriteTime) == ERROR_SUCCESS)) {
			cbExtension = SIZE_EXT; //reset size for next loop
			HKEY hkey2;
			char regPath[MAX_PATH];
			_snprintf(regPath, sizeof(regPath)-1, BASE_REG_PATH "\\%s",proc->ext);
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ, &hkey2) == ERROR_SUCCESS) {
				DWORD dwType = REG_SZ;

				/* get the command line */
				dwBufSize = MAX_PATH;
				dwType = REG_SZ;
				if (RegQueryValueEx(hkey2, "AppPath", 0, &dwType, (unsigned char *)proc->appPath, &dwBufSize) != ERROR_SUCCESS
					|| dwType != REG_SZ) {
					dwIndex++;
					continue;
				}
				
				/* get the bind path, must have at least that */
				DWORD dwBufSize = MAX_PATH;
				if (RegQueryValueEx(hkey2, "BindPath", 0, &dwType, (unsigned char *)proc->bindPath, &dwBufSize) != ERROR_SUCCESS
					|| dwType != REG_SZ) {
					// this is a dynamic server
					proc->bindPath[0] = 0;
				}

				/* get the command line arguments */
				dwBufSize = MAX_PATH;
				dwType = REG_SZ;
				if (RegQueryValueEx(hkey2, "Args", 0, &dwType, (unsigned char *)proc->args, &dwBufSize) != ERROR_SUCCESS
					|| dwType != REG_SZ) {
					proc->args[0] = 0;
				}

				/* read the extra environment variables */
				char ExtraEnvBuf[1024];
				dwBufSize = sizeof(ExtraEnvBuf);
				dwType = REG_BINARY;
				if (RegQueryValueEx(hkey2, "Environment", 0, &dwType, (unsigned char *)ExtraEnvBuf, &dwBufSize) != ERROR_SUCCESS
					|| dwType != REG_BINARY) {
					// do nothing
				} else {
					/* prepare the env section */
					char *p, *b = ExtraEnvBuf;
					int i=0;
					while ((p = strchr(b, 13)) != NULL) {
						*p = 0;
						if (*(p-1) == 10) *(p-1)=0;
						proc->env.putEnv(b);
						b = p + 1;
					}
				}
				proc->env.putEnv(FCGI_ENVIRON_VER);
				if (dwServerImpersonate) {
					proc->env.putEnv("_FCGI_NTAUTH_IMPERSONATE_=1");
				}

				/* get the number of servers to start */
				dwBufSize = sizeof(DWORD);
				dwType = REG_DWORD;
				if (RegQueryValueEx(hkey2, "StartServers", 0, &dwType, (unsigned char *)&proc->startServers, &dwBufSize) != ERROR_SUCCESS
					|| dwType != REG_DWORD) {
					proc->startServers = gStartServers;
				}

				/* get the number of servers to start */
				dwBufSize = sizeof(DWORD);
				dwType = REG_DWORD;
				if (RegQueryValueEx(hkey2, "IncrementServers", 0, &dwType, (unsigned char *)&proc->incServers, &dwBufSize) != ERROR_SUCCESS
					|| dwType != REG_DWORD || proc->incServers == 0) {
					proc->incServers = (int)proc->startServers / 2;
					if (!proc->incServers) proc->incServers = gIncServers;
				}

				/* get the max number of servers to start */
				dwBufSize = sizeof(DWORD);
				dwType = REG_DWORD;
				if (RegQueryValueEx(hkey2, "MaxServers", 0, &dwType, (unsigned char *)&proc->maxServers, &dwBufSize) != ERROR_SUCCESS
					|| dwType != REG_DWORD || proc->maxServers == 0) {
					proc->maxServers = gMaxServers;
				}

				/* get the idle timeout */
				dwBufSize = sizeof(DWORD);
				dwType = REG_DWORD;
				if (RegQueryValueEx(hkey2, "Timeout", 0, &dwType, (unsigned char *)&proc->timeout, &dwBufSize) != ERROR_SUCCESS
					|| dwType != REG_DWORD || proc->timeout == 0) {
					proc->timeout = gTimeout; // default 5 minutes
				}

				/* is this a filter or a responder? */
				dwBufSize = sizeof(DWORD);
				dwType = REG_DWORD;
				if (RegQueryValueEx(hkey2, "Filter", 0, &dwType, (unsigned char *)&proc->isFilter, &dwBufSize) != ERROR_SUCCESS
					|| dwType != REG_DWORD) {
					proc->isFilter = 0;
				}

				RegCloseKey(hkey2);

				processList.Add(proc);
				if (proc->bindPath[0] != 0) {
					StartServers(proc);
				}
				proc = new FCGIProcess();
			}
			dwIndex++;
		}
		delete proc;

		RegCloseKey(hkey);
	}
}

FCGIProcess *FCGIProccessManager::ConfigureDynamicServers(char *bindpath, FCGIProcess *parent, char *path) 
{
	FCGIProcess *proc = new FCGIProcess();
	strncpy(proc->appPath, parent->appPath, sizeof(proc->appPath)-1);
	proc->startServers = parent->startServers;
	proc->incServers = parent->incServers;
	proc->maxServers = parent->maxServers;
	proc->timeout = parent->timeout;
	if (path) strncpy(proc->args, path, sizeof(proc->args)-1);
	strncpy(proc->bindPath, bindpath, sizeof(proc->bindPath)-1);
	if (dwServerImpersonate) {
		proc->env.putEnv("_FCGI_NTAUTH_IMPERSONATE_=1");
	}
	processList.Add(proc);
	StartServers(proc);
	return proc;
}

/* clear processes that have died */
void FCGIProccessManager::CleanupServers() 
{
	FCGIProcess *proc = processList.GetFirst();
	while(proc) {
		proc->Cleanup();
		proc = processList.GetNext();
	}
}

void FCGIProccessManager::InitiateServers(char *bindPath) 
{
	FCGIProcess *proc = processList.GetFirst();
	while(proc) {
		if (stricmp(proc->bindPath, bindPath)==0) {
			if (WaitForSingleObject(proc->pmutex,0) == WAIT_OBJECT_0) {
				proc->Cleanup();
				StartServers(proc);
				ReleaseMutex(proc->pmutex);
			}
			break;
		}
		proc = processList.GetNext();
	}
}


int FCGIProccessManager::StartServers(FCGIProcess *proc)
{
	// start some servers
    int listenFd;
	unsigned int i;

    if(access(proc->appPath, 0) == -1) {
		fprintf(stderr, "%s is not executable\n", proc->appPath);
		return 1;
    }

    /*
     * Create the server processes
     */
	unsigned int numsrv = proc->numServers?proc->incServers:proc->startServers;

    for(i = 0; i < numsrv && proc->numServers < proc->maxServers; i++) {
		/* @@@ Should be able to pick up the backlog as an arg */
		if((listenFd = OS_CreateLocalIpcFd(proc->bindPath, 5, 0)) < 0) {
			fprintf(stderr, "Could not get ipc\n");
			continue;
		}

		FCGIProccessInfo *iproc = new FCGIProccessInfo;
		iproc->ltime = 0;
		memset(iproc,0,sizeof(FCGIProccessInfo));

		/* create the termination event */
		iproc->terminationEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		SetHandleInformation(iproc->terminationEvent, HANDLE_FLAG_INHERIT, TRUE);
		/* create the listener mutex */
		iproc->listenMutex = CreateMutex(NULL, FALSE, NULL);
		SetHandleInformation(iproc->listenMutex, HANDLE_FLAG_INHERIT, TRUE);

		/* create an environment for this */

		/* XXX not the best way to handle this, but deal with it later */
		char buf[MAX_PATH];
		_snprintf(buf, sizeof(buf)-1,"_FCGI_SHUTDOWN_EVENT_=%ld", iproc->terminationEvent);
		proc->env.putEnv(buf);
		_snprintf(buf, sizeof(buf)-1, "_FCGI_MUTEX_=%ld", iproc->listenMutex);
		proc->env.putEnv(buf);

		_snprintf(buf, sizeof(buf)-1,"%s %s",proc->appPath,proc->args);
        if (OS_SpawnChild(buf, listenFd, &iproc->pInfo, proc->env.getEnviron()) == 0) {
			proc->Add(iproc);
		} else {
			CloseHandle(iproc->terminationEvent);
			delete iproc;
		}
		OS_Close(listenFd);
    }
	//Sleep(1000); // wait for them to start!
	return 0;
}

void FCGIProccessManager::StopServers() 
{
	FCGIProcess *proc = processList.GetFirst();
	while (proc) {
		proc->SignalShutdown();
		proc = processList.GetNext();
	}
	Sleep(5000);
	proc = processList.GetFirst();
	while (proc) {
		proc->Shutdown();
		proc = processList.GetNext();
	}
}

BOOL FCGIProccessManager::isFilter(char *bindpath)
{
	FCGIProcess *proc = processList.GetFirst();
	while(proc) {
		if (proc->bindPath[0] != 0
			&& stricmp(proc->bindPath, bindpath)==0) {
			return proc->isFilter;
		}
		proc = processList.GetNext();
	}
	return FALSE;
}

char *FCGIProccessManager::GetBindingForPath(char *path_translated)
{
	// figure out which process group to call on for a given URI
	char *ext = strrchr(path_translated, '.');
	char *exec= NULL;
	char *end = strchr(ext,'/');
	char dbindPath[33];

	if (!end) end = strchr(ext,'\\');
	if (end) *end = 0;
	exec = strrchr(path_translated, '/');
	if (!exec) exec = strrchr(path_translated,'\\');
	if (exec) exec++;

	md5digest(path_translated, dbindPath);

	FCGIProcess *proc = processList.GetFirst();
	// first look for existing dynamic servers
	while(proc) {
		if (proc->ext[0]==0 && proc->bindPath[0] != 0 && 
			(stricmp(proc->bindPath, (const char *)dbindPath)==0)) {
			if (end) *end = '/';
			return proc->bindPath;
		}
		proc = processList.GetNext();
	}

	proc = processList.GetFirst();
	// now look for static servers, or start dynamic servers
	while(proc) {
		if (proc->ext[0] == '.') {
			if (stricmp(proc->ext, ext)==0) {
				if (proc->bindPath[0] != 0)	{
					if (end) *end = '/';
					return proc->bindPath;
				} else {
					// build a dynamic fcgiprocess
					FCGIProcess *dproc = ConfigureDynamicServers(dbindPath,proc,path_translated);
					if (end) *end = '/';
					return dproc->bindPath;
				}
				break;
			}
		} else if (stricmp(proc->ext, exec)==0) {
			// compiled executable
			if (proc->bindPath[0] != 0)	{
				if (end) *end = '/';
				return proc->bindPath;
			} else {
				// build a dynamic fcgiprocess
				FCGIProcess *dproc = ConfigureDynamicServers(dbindPath,proc,NULL);
				if (end) *end = '/';
				return dproc->bindPath;
			}
			break;
		}
		proc = processList.GetNext();
	}
	if (end) *end = '/';
	return NULL;
}

void FCGIProccessManager::CheckServers(char *bindPath)
{
	// figure out which process group to call on for a given URI
	FCGIProcess *proc = processList.GetFirst();
	while(proc) {
		if (stricmp(proc->bindPath, bindPath)==0) {
			if (WaitForSingleObject(proc->pmutex,0) == WAIT_OBJECT_0) {
				proc->Cleanup();
				if (proc->numServers < proc->startServers) {
					StartServers(proc);
				}
				ReleaseMutex(proc->pmutex);
			}
			return;
		}
		proc = processList.GetNext();
	}
}

extern "C" {
void InitiateServers(char *bindPath) {
	procmgr->InitiateServers(bindPath);
}
void CheckServers(char *bindPath) {
	procmgr->CheckServers(bindPath);
}
}
