//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
// class CGwsFdoUpdateCommand
//
///////////////////////////////////////////////////////////////////////////////

CGwsFdoUpdateCommand::CGwsFdoUpdateCommand (
    FdoIConnection           * session,
    const GWSQualifiedName   & classname
)
:CGwsFdoCommand (session, classname)
{
}


///////////////////////////////////////////////////////////////////////////////
CGwsFdoUpdateCommand::~CGwsFdoUpdateCommand ()
{
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoUpdateCommand::Init (const wchar_t* pFDOCommandClass /*NULL*/)
{
    EGwsStatus             fdoes = eGwsOk;

    try {
        eGwsOkThrow (CGwsFdoCommand::Init ());

        m_pCommand = m_connection->CreateCommand(FdoCommandType_Update);
        assert (m_pCommand != NULL);

        // Set class name
        std::wstring name;
        if(pFDOCommandClass)
            name = pFDOCommandClass;
        else
            name = QualifiedClassName();
        ((FdoIUpdate *)m_pCommand.p)->SetFeatureClassName(name.c_str());

    } catch(FdoException *e) {
        PushFdoException (eGwsFailedToPrepareCommand, e);
        e->Release();
        fdoes = eGwsFailedToPrepareCommand;

    } catch(EGwsStatus es) {
        fdoes = es;
    }

    return fdoes;
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoUpdateCommand::PrepareInternal ()
{
    PrepareNonKeyProperties ();
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoUpdateCommand::SetFilterInternal (FdoFilter * filter)
{
    assert (m_pCommand);

    ((FdoIUpdate *) m_pCommand.p)->SetFilter (filter);
}


///////////////////////////////////////////////////////////////////////////////
FdoPropertyValueCollection * CGwsFdoUpdateCommand::GetPropertyValues ()
{
    if (m_pProperties == NULL) {
        m_pProperties = ((FdoIUpdate*)m_pCommand.p)->GetPropertyValues();
    }

    return m_pProperties;
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoUpdateCommand::Execute (CGwsMutableFeature & feat)
{
    Clear ();

    EGwsStatus fdoes = eGwsOk;
    bool       bRet  = false;

    try {
        GetPropertyValues ();   // initialize property value collection
        eGwsOkThrow(SetProperties (feat, false));
        FdoPtr<FdoFilter>           filter;
        GwsFailedStatus             failed;

        eGwsOkThrow (BuildFilter (feat.GetFeatureId (), filter.p));

        ((FdoIUpdate*)m_pCommand.p)->SetFilter (filter);
        if (((FdoIUpdate*)m_pCommand.p)->Execute() < 1) {
            if (m_bSupportLocking) {
                FdoPtr<FdoILockConflictReader> reader = ((FdoIUpdate*)m_pCommand.p)->GetLockConflicts ();
                eGwsOkThrow (ProcessLockConflicts (reader, failed));
            }
        }
    } catch (FdoException * ex) {
        PushFdoException (eGwsFailedToExecuteCommand, ex);
        ex->Release ();
        fdoes = eGwsFailedToExecuteCommand;

    } catch (EGwsStatus e) {
        // in case of AcMapFdoError::kMapFdoLockConflict error stack was populated by
        // lock conflict processor
        if (e != eGwsFdoLockConflict)
            PushStatus (e);
        fdoes = e;
    }
    ReleasePropertyValues ();   // Releases property value collection
    return fdoes;
}
