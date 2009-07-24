// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

int Register(_TCHAR* name, _TCHAR* displayName, _TCHAR* description, _TCHAR* version, _TCHAR* fdoVersion, _TCHAR* path);
int Unregister(_TCHAR* name);
void Usage();

// TODO: reference additional headers your program requires here
#include "Fdo.h"