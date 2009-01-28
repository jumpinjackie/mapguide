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

///////////////////////////////////////////////////////////////////////////////
//
// Includes
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GwsQueryEngineImp.h"


//////////////////////////////////////////////////////////////////////////////
//
// class CGwsFdoDeleteCommand
//
///////////////////////////////////////////////////////////////////////////////

CGwsFdoDeleteCommand::CGwsFdoDeleteCommand (
    FdoIConnection             *session,
    const GWSQualifiedName     &classname
)
:CGwsFdoCommand (session, classname)
{
}


///////////////////////////////////////////////////////////////////////////////
CGwsFdoDeleteCommand::~CGwsFdoDeleteCommand ()
{
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoDeleteCommand::Init (const wchar_t* pFDOCommandClass /*NULL*/)
{
    EGwsStatus fdoes = eGwsOk;

    try {
        eGwsOkThrow (CGwsFdoCommand::Init ());

        m_pCommand = m_connection->CreateCommand(FdoCommandType_Delete);
        assert (m_pCommand != NULL);

        WSTR name;
        if(pFDOCommandClass)
            name = pFDOCommandClass;
        else
            name = QualifiedClassName();

        ((FdoIDelete *)m_pCommand.p)->SetFeatureClassName(name.c_str());

    } catch(FdoException *e) {
        PushFdoException (eGwsFailedToPrepareCommand, e);
        e->Release();
        fdoes = eGwsFailedToPrepareCommand;

    } catch(EGwsStatus es) {
        PushStatus (es);
        fdoes = es;
    }
    return fdoes;
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoDeleteCommand::PrepareInternal ()
{
    PrepareNonKeyProperties ();
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoDeleteCommand::SetFilterInternal (FdoFilter * filter)
{
    assert (m_pCommand);

    ((FdoIDelete *) m_pCommand.p)->SetFilter (filter);
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoDeleteCommand::Execute (const GWSFeatureId & featid)
{
    Clear ();

    EGwsStatus fdoes = eGwsOk;

    if(!m_bIsPrepared) {
        PushStatus (eGwsFdoCommandIsNotPrepared);
        return eGwsFdoCommandIsNotPrepared;
    }

    try {
        FdoPtr<FdoFilter>      filter;
        GwsFailedStatus        failed;

        eGwsOkThrow (BuildFilter (featid, filter.p));

        ((FdoIDelete*)m_pCommand.p)->SetFilter (filter);
        if (((FdoIDelete*)m_pCommand.p)->Execute() < 1) {
            if (m_bSupportLocking) {
                FdoPtr<FdoILockConflictReader> reader = ((FdoIDelete*)m_pCommand.p)->GetLockConflicts ();
                eGwsOkThrow (ProcessLockConflicts (reader, failed));
            }
        }
    } catch(FdoException *e) {
        PushFdoException (eGwsFailedToPrepareCommand, e);
        e->Release();
        fdoes = eGwsFailedToPrepareCommand;

    } catch(EGwsStatus es) {
        // in case of AcMapFdoError::kMapFdoLockConflict error stack was populated by
        // lock conflict processor
        if (es != eGwsFdoLockConflict)
            PushStatus (es);
        fdoes = es;
    }
    return fdoes;
}
