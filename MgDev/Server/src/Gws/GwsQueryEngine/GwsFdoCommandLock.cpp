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
// Defines
//
///////////////////////////////////////////////////////////////////////////////

#define LOCK_BATCH_SIZE 150     // batch size for locking


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsFdoLockingCommand
//
///////////////////////////////////////////////////////////////////////////////

CGwsFdoLockingCommand::CGwsFdoLockingCommand (
    FdoIConnection           *session,
    const GWSQualifiedName   &classname
)
:CGwsFdoCommand (session, classname)
{
}


///////////////////////////////////////////////////////////////////////////////
CGwsFdoLockingCommand::~CGwsFdoLockingCommand ()
{
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoLockingCommand::PrepareInternal ()
{
    PrepareKeyProperties ();
    m_rejected.clear ();
    m_processed.clear ();
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoLockingCommand::Execute (CGwsMutableFeature & feature)
{
    // this should not be exteneded feature
    return Execute (feature.GetFeatureId ());
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoLockingCommand::Execute (const GWSFeatureId & featid)
{
    Clear ();
    EGwsStatus          fdoes = eGwsOk;
    FdoPtr<FdoFilter>   filter;
    if(!m_bIsPrepared) {
        PushStatus (eGwsFdoQueryIsNotPrepared);
        return eGwsFdoQueryIsNotPrepared;
    }

    try {
        fdoes = BuildFilter (featid, filter.p);   // build filter
        if (! IsError (fdoes))
            fdoes = ExecuteFilter (filter);
        if (fdoes != eGwsFdoLockConflict) {
            if (IsError (fdoes)) {
                wchar_t wbuff [256];
                GWSExtendedFeatureId efid (ClassName (), featid);
                efid.ToString (wbuff, 256);

                CGwsStatus stat (eGwsFdoLockConflict);
                stat.SetParameter (L"FeatureId", wbuff);
                PushStatus (stat);
                m_rejected.insert (GwsFailedStatus::value_type (featid,stat));
            } else {
                m_processed.insert (featid);
            }
        }

    } catch (FdoException * e) {
        PushFdoException (eGwsFailed, e);
        e->Release ();
        fdoes = eGwsFailed;
    }

    return fdoes;
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoLockingCommand::Execute (
    const GwsFeaturesIdVector    & featids,
    int                            lbound,
    int                            ubound
)
{
    Clear ();
    int          idx;
    EGwsStatus   fdoes = eGwsOk;

    try {
        int fcount = (int)
                     (featids.size() / LOCK_BATCH_SIZE) +
                     ((featids.size () % LOCK_BATCH_SIZE) == 0 ? 0 : 1);

        for (idx = 0; idx < fcount; idx ++) {
            FdoPtr<FdoFilter>   filter;

            BuildFilter (featids,
                         idx * LOCK_BATCH_SIZE,
                         gws_min (((idx + 1) * LOCK_BATCH_SIZE), (int)featids.size ()),
                         filter.p);
            fdoes = ExecuteFilter (filter); // this will append to rejected list
            if (fdoes != eGwsFdoLockConflict && IsError (fdoes))
            {
                // in case of failure, all features are rejected
                for (int i = idx * LOCK_BATCH_SIZE;
                     i < gws_min (((idx + 1) * LOCK_BATCH_SIZE), (int)featids.size ());
                     i ++)
                {

                    wchar_t wbuff [256];
                    GWSExtendedFeatureId efid (ClassName (), featids[i]);
                    efid.ToString (wbuff, 256);

                    CGwsStatus stat (fdoes);
                    stat.SetParameter (L"FeatureId", wbuff);
                    PushStatus (stat);
                    m_rejected.insert (GwsFailedStatus::value_type (featids[i],stat));
                }
            } else
                fdoes = eGwsOk;
        }
        for (idx = 0; idx < (int)featids.size (); idx ++) {
            if (m_rejected.find (featids[idx]) == m_rejected.end ()) {
                m_processed.insert (featids[idx]);
            }
        }

        if (m_processed.size () == 0) {
            PushStatus (eGwsFdoLockConflict);
            fdoes = eGwsFdoLockConflict;
        }
    } catch (FdoException * e) {
        PushFdoException (eGwsFailedToExecuteCommand, e);
        e->Release ();
        fdoes = eGwsFailedToExecuteCommand;
    }
    return fdoes;
}


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsFdoLockCommand
//
///////////////////////////////////////////////////////////////////////////////

CGwsFdoLockCommand::CGwsFdoLockCommand (
//    CGwsFeatureSourceSession *session,
    FdoIConnection           *session,
    const GWSQualifiedName   &classname
)
:CGwsFdoLockingCommand (session, classname)
{
    m_lockType = FdoLockType_None;
}


///////////////////////////////////////////////////////////////////////////////
CGwsFdoLockCommand::~CGwsFdoLockCommand ()
{
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoLockCommand::Init (const wchar_t* pFDOCommandClass /*NULL*/)
{
    EGwsStatus             fdoes = eGwsOk;

    try {
        fdoes = CGwsFdoCommand::Init ();
        if (IsError (fdoes))
            return fdoes;

        m_pCommand = m_connection->CreateCommand(FdoCommandType_AcquireLock);
        assert (m_pCommand != NULL);

        // Set class name
        WSTR name;
        if (pFDOCommandClass)
            name = pFDOCommandClass;
        else
            name = QualifiedClassName();
        ((FdoIAcquireLock *)m_pCommand.p)->SetFeatureClassName(name.c_str());

    } catch(FdoException *e) {
        PushFdoException (eGwsFailedToPrepareCommand, e);
        e->Release ();
        fdoes = eGwsFailedToPrepareCommand;
    }
    return fdoes;
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoLockCommand::PrepareInternal()
{
    CGwsFdoLockingCommand::PrepareInternal ();

    try {
        FdoPtr<FdoClassCapabilities> capabilities;
        FdoInt32                     size = 0;

        // discover class locking capabilities
        capabilities = m_classDef->GetCapabilities ();
        if (capabilities == NULL)
            return;

        m_lockType = FdoLockType_None;

        if (capabilities->SupportsLocking ()) {

            FdoLockType * locktypes = capabilities->GetLockTypes (size);
            if (locktypes == NULL)
                return;

            // try to get persistent lock type in the following order
            // (always try to get the most restrictive)
            // FdoLockType_AllLongTransactionExclusive,
            // FdoLockType_Exclusive,
            // FdoLockType_LongTransactionExclusive

            for (int i = 0; i < size; i ++) {
                if (locktypes [i] == FdoLockType_AllLongTransactionExclusive) {
                    m_lockType = FdoLockType_AllLongTransactionExclusive;
                    break;
                }
                if (locktypes [i] == FdoLockType_Exclusive) {
                    m_lockType = FdoLockType_Exclusive;
                    continue;
                }
                if (locktypes [i] == FdoLockType_LongTransactionExclusive &&
                    m_lockType != FdoLockType_Exclusive)
                {
                    m_lockType = FdoLockType_LongTransactionExclusive;
                    continue;
                }
            }
        }
    } catch (FdoException * fdoEx) {
        fdoEx->Release ();
    }
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoLockCommand::SetFilterInternal (FdoFilter * filter)
{
    assert (m_pCommand);

    ((FdoIAcquireLock *) m_pCommand.p)->SetFilter (filter);
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoLockCommand::ExecuteFilter (FdoFilter *pFilter)
{
    EGwsStatus fdoes = eGwsOk;

    try {
        FdoIAcquireLock             * pLockCommand = (FdoIAcquireLock *) m_pCommand.p;
        GwsFailedStatus               failed;

        pLockCommand->SetLockType(m_lockType);
        pLockCommand->SetLockStrategy(FdoLockStrategy_Partial);
        pLockCommand->SetFilter(pFilter);
        PrepareFilter (pFilter);  // do I need to set filter after converting it?

        FdoPtr<FdoILockConflictReader> pReader = pLockCommand->Execute();
        if (ProcessLockConflicts (pReader, failed) == eGwsFdoLockConflict) {
            m_rejected.insert (failed.begin (), failed.end ());
            return eGwsFdoLockConflict;
        }

    } catch (FdoException *e) {
        PushFdoException (eGwsFailedToExecuteCommand, e);
        e->Release ();
        fdoes = eGwsFailedToExecuteCommand;
    }
    return fdoes;
}
