#ifndef _REGISTRY_H
#define _REGISTRY_H

#ifdef _WIN32

#define WIN32_REG_BUFSIZE 1024

typedef struct {
	HKEY hkey;
	DWORD	iSubKey;
	DWORD	iSubValue;
	CHAR	ClassName[MAX_PATH]; // Buffer for class name.
	DWORD	dwcClassLen;   // Length of class string.
	DWORD	dwcSubKeys;               // Number of sub keys.
	DWORD	dwcMaxSubKey;             // Longest sub key size.
	DWORD	dwcMaxClass;              // Longest class string.
	DWORD	dwcValues;                // Number of values for this key.
	DWORD	dwcMaxValueName;          // Longest Value name.
	DWORD	dwcMaxValueData;          // Longest Value data.
	DWORD	dwcSecDesc;               // Security descriptor.
	FILETIME ftLastWriteTime;          // Last write time.
} RegKey;

extern BYTE *_win32_reg_read(char *value_name, DWORD *dwType, DWORD *dwBufSize);
extern RegKey *_win32_reg_open(char *value_name);
static BYTE *_win32_reg_read_key(RegKey *pRegKey, char *rbuf, DWORD rsize);
extern int _win32_reg_read_value(RegKey *pRegKey, BYTE *name, DWORD *namesize, DWORD *dwType, BYTE *pvData, DWORD *dwDataSize);
extern int _win32_reg_rewind(RegKey *pRegKey);
extern int _win32_reg_close(RegKey *pRegKey);

#endif /* WIN32|WINNT */

#endif /* _REGISTRY_H */