//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

///////////////////////////////////////////////////////////////////////////////
//
// Includes
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GwsQueryEngineImp.h"


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsFdoUnlockCommand
//
///////////////////////////////////////////////////////////////////////////////

CGwsFdoUnlockCommand::CGwsFdoUnlockCommand (
    FdoIConnection            *session,
    const GWSQualifiedName    &classname
)
:CGwsFdoLockingCommand (session, classname)
{
}


///////////////////////////////////////////////////////////////////////////////
CGwsFdoUnlockCommand::~CGwsFdoUnlockCommand ()
{
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoUnlockCommand::Init (const wchar_t* pFDOCommandClass /*NULL*/)
{
    EGwsStatus fdoes = eGwsOk;

    try {
        eGwsOkThrow (CGwsFdoLockingCommand::Init ());

        m_pCommand = m_connection->CreateCommand(FdoCommandType_ReleaseLock);
        assert (m_pCommand != NULL);

        // Set class name
        WSTR name;
        if(pFDOCommandClass)
            name = pFDOCommandClass;
        else
            name = QualifiedClassName();
        ((FdoIReleaseLock *)m_pCommand.p)->SetFeatureClassName(name.c_str());

        if ( ! m_owner.empty())
            ((FdoIReleaseLock *)m_pCommand.p)->SetLockOwner(m_owner.c_str());

    } catch (EGwsStatus es) {
        PushStatus (es);
        fdoes = es;

    } catch(FdoException *e) {
        PushFdoException (eGwsFailedToPrepareCommand, e);
        e->Release ();
        fdoes = eGwsFailedToPrepareCommand;
    }
    return fdoes;
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoUnlockCommand::SetFilterInternal (FdoFilter * filter)
{
    assert (m_pCommand);

    ((FdoIReleaseLock *) m_pCommand.p)->SetFilter (filter);
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoUnlockCommand::ExecuteFilter (FdoFilter *pFilter)
{
    EGwsStatus fdoes = eGwsOk;

    try {
        FdoIReleaseLock             * pUnlockCommand = (FdoIReleaseLock *) m_pCommand.p;
        GwsFailedStatus               failed;

        pUnlockCommand->SetFilter(pFilter);

        PrepareFilter (pFilter);
        // do I need to set filter after converting it?

        FdoPtr<FdoILockConflictReader> pReader = pUnlockCommand->Execute();
        if (ProcessLockConflicts (pReader, failed) == eGwsFdoLockConflict) {
            m_rejected.insert (failed.begin (), failed.end ());
            eGwsOkThrow (eGwsFdoLockConflict);
        }

    } catch (EGwsStatus es) {
        PushStatus (es);
        fdoes = es;

    } catch(FdoException *e) {
        PushFdoException (eGwsFailedToExecuteCommand, e);
        e->Release ();
        fdoes = eGwsFailedToPrepareCommand;
    }
    return fdoes;
}
