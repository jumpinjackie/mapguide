#ifndef TESTER_WINDOWS_H
#define TESTER_WINDOWS_H

#include <tchar.h>
#include <windows.h>

#define TESTRESULTSPATH L"..\\..\\TestResults\\"
#define GETUSER char userName[30];\
    DWORD nUserName = sizeof(userName);\
    if (GetUserName(userName, &nUserName) == 0) {\
        fprintf(stderr, "Failed to lookup username.");\
        exit(1);\
    }

#endif
