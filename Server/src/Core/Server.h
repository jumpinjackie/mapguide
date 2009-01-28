//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef MGSERVER_H_
#define MGSERVER_H_

#include "MapGuideCommon.h"
#include "EventTimerManager.h"

#ifdef _WIN32
#include "ace/NT_Service.h"
#endif

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#ifndef DEBUG_LOG
#ifdef _DEBUG
#define DEBUG_LOG(X) DebugOutput X;
extern void DebugOutput(const ACE_TCHAR* format, ...);
#else
#define DEBUG_LOG(X)
#endif
#endif

#ifdef _WIN32
// This macro has been slightly modified from the ACE macro that is provided to
// do this job.  See the ACE_NT_SERVICE_DEFINE macro
#define MG_NT_SERVICE_DEFINE(SVCNAME, SVCCLASS, SVCDESC)                        \
    ACE_NT_Service * _ace_nt_svc_obj_##SVCNAME = 0;                             \
    VOID WINAPI ace_nt_svc_handler_##SVCNAME (DWORD fdwControl)                 \
    {                                                                           \
        _ace_nt_svc_obj_##SVCNAME->handle_control(fdwControl);                  \
    }                                                                           \
    VOID WINAPI ace_nt_svc_main_##SVCNAME (DWORD dwArgc, ACE_TCHAR **lpszArgv)  \
    {                                                                           \
        int delete_svc_obj = 0;                                                 \
        if (_ace_nt_svc_obj_##SVCNAME == 0)                                     \
        {                                                                       \
            ACE_NEW (_ace_nt_svc_obj_##SVCNAME, SVCCLASS);                      \
            if (_ace_nt_svc_obj_##SVCNAME == 0)                                 \
                return;                                                         \
            delete_svc_obj = 1;                                                 \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            _ace_nt_svc_obj_##SVCNAME->inherit_log_msg_attributes ();           \
        }                                                                       \
                                                                                \
        int result = _ace_nt_svc_obj_##SVCNAME->init(dwArgc, lpszArgv);         \
        _ace_nt_svc_obj_##SVCNAME->svc_handle(                                  \
            ACE_TEXT_RegisterServiceCtrlHandler(SVCDESC,                        \
                &ace_nt_svc_handler_##SVCNAME));                                \
                                                                                \
        if (0 != result)                                                        \
        {                                                                       \
            _ace_nt_svc_obj_##SVCNAME->fini();                                  \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            _ace_nt_svc_obj_##SVCNAME->open();                                  \
            _ace_nt_svc_obj_##SVCNAME->wait();                                  \
            _ace_nt_svc_obj_##SVCNAME->fini();                                  \
        }                                                                       \
        if (delete_svc_obj)                                                     \
        {                                                                       \
            delete _ace_nt_svc_obj_##SVCNAME;                                   \
            _ace_nt_svc_obj_##SVCNAME = 0;                                      \
        }                                                                       \
        return;                                                                 \
    }
#endif

#ifdef _WIN32
class MgServer : public ACE_NT_Service
#else
class MgServer : public ACE_Service_Object
#endif
{
    ///////////////////////////////////////////////////////
    /// Methods
public:
    MgServer();
    ~MgServer();


    // ACE_Service_Object methods
    virtual int init(int argc, ACE_TCHAR *argv[]);
    virtual int fini();

    virtual int open(void *args = 0);
    virtual int svc();

#ifdef _WIN32
    virtual void handle_control(DWORD controlCode);
    virtual void stop_requested (DWORD controlCode);
#endif

private:

    void ParseArgs(INT32 argc, ACE_TCHAR *argv[]);
    void AddFontManagerFontAliases();
    static STRING LocaleCallback();

    ///////////////////////////////////////////////////////
    /// Member data
private:
    bool m_bTestMode;
    bool m_bTestFdo;
    STRING m_strTestFileName;
    STRING m_strTestName;

#ifdef _DEBUG
    INT32 m_nClientRequestLimit;        // DEBUG ONLY
#endif

    MgEventTimerManager m_eventTimerManager;
};

typedef ACE_Singleton<MgServer, ACE_SYNCH_MUTEX> SERVER;

#endif
