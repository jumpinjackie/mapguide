#include <windows.h>
#include "registry.h"
#include <stdio.h>

#define FASTCGI_REG_KEY "HKEY_LOCAL_MACHINE\\Software\\FASTCGI\\"


static struct {
	char * name;
	HKEY hkey;
} root_keys[] = {
	{ "HKEY_LOCAL_MACHINE",		HKEY_LOCAL_MACHINE },
	{ "HKEY_CLASSES_ROOT",		HKEY_CLASSES_ROOT },
	{ "HKEY_CURRENT_USER",		HKEY_CURRENT_USER },
	{ "HKEY_USERS",				HKEY_USERS }
};

static BYTE *_win32_reg_read(char *value_name,DWORD *dwType, DWORD *dwBufSize){
	int i,found;
	char *root, *path, *key_name, *p;
	HKEY hkey;
	LONG status;
	BYTE *pbuffer;

	/*
	 * From a path on the form "HKEY_LOCAL_MACHINE\\Software\\FASTCGI\\keyname", extract the following parts:
	 * root:	 "HKEY_LOCAL_MACHINE"
	 * path:	 "\Software\FASTCGI\"
	 * key_name: "keyname"
	 */
	root = strdup(value_name);
	root = strtok(root, "\\");

	path = strdup(value_name+strlen(root)+1);
	p = strrchr(path, '\\');
	if (!p) {
		/*error("invalid registry key name '%s'", value_name);*/
		free(root);
		free(path);
		return NULL;
	}
    key_name = strdup(p+1);
	*p = 0;

	/* Find the predefined handle value corresponding to 'root' */
	found = 0;
	for (i = 0; root_keys[i].name; ++i)
		if (!stricmp(root_keys[i].name, root))
		{
			found = 1;
			break;
		}
	
	if (!found) {
		free(root);
		free(path);
		free(key_name);
		return NULL;
	}

	status = RegOpenKeyEx(root_keys[i].hkey, path, 0, KEY_READ, &hkey);
	if (status != ERROR_SUCCESS) {
		free(root);
		free(path);
		free(key_name);
		return NULL;
	}

	/* Determine necessary size of buffer */
	status = RegQueryValueEx(hkey, key_name, 0, dwType, NULL, dwBufSize);
	if (status != ERROR_SUCCESS) {
		free(root);
		free(path);
		free(key_name);
		return 0;
	}
	if (*dwBufSize < 1)
		*dwBufSize = 1;

	pbuffer = malloc(*dwBufSize);
	status = RegQueryValueEx(hkey, key_name, 0, dwType, pbuffer, dwBufSize);
	RegCloseKey(hkey);
	
	free(root);
	free(path);
	free(key_name);
	return pbuffer;
}


static RegKey *_win32_reg_open(char *value_name){
	char *root, *path;
	HKEY hkey;
	LONG status;
	RegKey *pRegKey;
	
	int i,found;
	/*
	 * From a path on the form "HKEY_LOCAL_MACHINE\\Software\\FASTCGI\\", extract the following parts:
	 * root:	 "HKEY_LOCAL_MACHINE"
	 * path:	 "\Software\FASTCGI\"
	 */
	root = strdup(value_name);
	root = strtok(root, "\\");

	path = strdup(value_name+strlen(root)+1);

	/* Find the predefined handle value corresponding to 'root' */
	found = 0;
	for (i = 0; root_keys[i].name; ++i)
		if (!stricmp(root_keys[i].name, root))
		{
			found = 1;
			break;
		}
	
	if (!found) {
		free(root);
		free(path);
		return NULL;
	}

	status = RegOpenKeyEx(root_keys[i].hkey, path, 0, KEY_READ, &hkey);
	if (status != ERROR_SUCCESS) {
		free(root);
		free(path);
		return NULL;
	}

	free(root);
	free(path);

	pRegKey = malloc(sizeof(RegKey));
	pRegKey->hkey = hkey;
	pRegKey->iSubKey = 0;
	pRegKey->iSubValue = 0;
	pRegKey->ClassName[0] = '\0';
	pRegKey->dwcClassLen = MAX_PATH;

	RegQueryInfoKey (pRegKey->hkey,              // Key handle.
                   pRegKey->ClassName,         // Buffer for class name.
                   &pRegKey->dwcClassLen,      // Length of class string.
                   NULL,					  // Reserved.
                   &pRegKey->dwcSubKeys,       // Number of sub keys.
                   &pRegKey->dwcMaxSubKey,     // Longest sub key size.
                   &pRegKey->dwcMaxClass,      // Longest class string.
                   &pRegKey->dwcValues,        // Number of values for this key.
                   &pRegKey->dwcMaxValueName,  // Longest Value name.
                   &pRegKey->dwcMaxValueData,  // Longest Value data.
                   &pRegKey->dwcSecDesc,       // Security descriptor.
                   &pRegKey->ftLastWriteTime); // Last write time.

	return pRegKey;
}


static BYTE *_win32_reg_read_key(RegKey *pRegKey, char *rbuf, DWORD rsize){
	DWORD dwBufSize;
	LONG status;
	FILETIME filetime;
	BYTE buffer[WIN32_REG_BUFSIZE+1];

	dwBufSize = WIN32_REG_BUFSIZE;
	status = RegEnumKeyEx(pRegKey->hkey, pRegKey->iSubKey, buffer, &dwBufSize, NULL, NULL, NULL, &filetime);
	if (status != ERROR_SUCCESS) {
		return NULL;
	}
	++pRegKey->iSubKey;
	strncpy(rbuf, buffer, rsize);
	return rbuf;
}


static int _win32_reg_read_value(RegKey *pRegKey, BYTE *buffer, DWORD *dwBufSize, DWORD *dwType, BYTE *pvData, DWORD *dwDataSize){
	LONG status;
	
	status = RegEnumValue(pRegKey->hkey, pRegKey->iSubValue, buffer, dwBufSize, NULL, dwType, pvData, dwDataSize);
	if (status != ERROR_SUCCESS) {
		return status;
	}
	++pRegKey->iSubValue;
	return 1;
}


static int _win32_reg_rewind(RegKey *pRegKey){

	pRegKey->iSubKey = 0;
	pRegKey->iSubValue = 0;
	return TRUE;
}


static int _win32_reg_close(RegKey *pRegKey){
	LONG status;

	status = RegCloseKey(pRegKey->hkey);
	free(pRegKey);
	if(status != ERROR_SUCCESS)return FALSE;
	return TRUE;
}

