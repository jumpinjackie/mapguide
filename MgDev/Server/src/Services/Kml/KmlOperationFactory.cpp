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

#include "MapGuideCommon.h"
#include "Services/KmlDefs.h"
#include "KmlOperationFactory.h"
#include "KmlOperation.h"


///////////////////////////////////////////////////////////////////////////
/// <summary>
/// The default constructor for an MgKmlOperationFactory object.  However, since
/// this function is protected, this object should never really be
/// constructed.  Rather, it is merely a wrapper class for other static
/// functions.
/// </summary>
MgKmlOperationFactory::MgKmlOperationFactory()
{
}


///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// This static method returns the IMgOperationHandler object that corresponds
/// to the given ID and Version parameters.
/// </summary>
/// <param name="operationId">
/// The ID of the requested operation.
/// </param>
/// <param name="operationVersion">
/// The version of the requested operation.
/// </param>
/// <returns>
/// Returns an IMgOperationHandler object corresponding to the given parameters.
/// Returns NULL if one cannot be found.
/// </returns>
/// <exceptions>
/// An MgException is thrown on failure.
/// </exceptions>
/// TODO:   handle different versions
/// TODO:   set up ids and whatnot in a hash or map instead of hardcoding the ids here
IMgOperationHandler* MgKmlOperationFactory::GetOperation(
    ACE_UINT32 operationId, ACE_UINT32 operationVersion)
{
    auto_ptr<IMgOperationHandler> handler;

    MG_TRY()

    switch (operationId)
    {
    case MgKmlServiceOpId::GetMapKml:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetMapKml());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgKmlOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgKmlServiceOpId::GetLayerKml:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetLayerKml());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgKmlOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgKmlServiceOpId::GetFeaturesKml:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetFeaturesKml());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgKmlOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    default:
        throw new MgInvalidOperationException(
            L"MgKmlOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgKmlOperationFactory.GetOperation")

    return handler.release();
}
