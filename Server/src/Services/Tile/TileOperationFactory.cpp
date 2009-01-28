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
#include "TileOperationFactory.h"
#include "TileOperation.h"


///////////////////////////////////////////////////////////////////////////
/// <summary>
/// The default constructor for an MgTileOperationFactory object.  However, since
/// this function is protected, this object should never really be
/// constructed.  Rather, it is merely a wrapper class for other static
/// functions.
/// </summary>
MgTileOperationFactory::MgTileOperationFactory()
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
IMgOperationHandler* MgTileOperationFactory::GetOperation(
    ACE_UINT32 operationId, ACE_UINT32 operationVersion)
{
    auto_ptr<IMgOperationHandler> handler;

    MG_TRY()

    switch (operationId)
    {
    case MgTileServiceOpId::GetTile:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
        case VERSION_SUPPORTED(1,2):
            handler.reset(new MgOpGetTile());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgTileOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgTileServiceOpId::SetTile:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpSetTile());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgTileOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgTileServiceOpId::ClearCache:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpClearCache());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgTileOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgTileServiceOpId::GetDefaultTileSizeX:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,2):
            handler.reset(new MgOpGetDefaultTileSizeX());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgTileOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgTileServiceOpId::GetDefaultTileSizeY:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,2):
            handler.reset(new MgOpGetDefaultTileSizeY());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgTileOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    default:
        throw new MgInvalidOperationException(
            L"MgTileOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgTileOperationFactory.GetOperation")

    return handler.release();
}
