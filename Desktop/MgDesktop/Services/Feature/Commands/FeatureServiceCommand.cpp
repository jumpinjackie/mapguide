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

#include "Services/Feature/FeatureDefs.h"
#include "MgDesktop.h"
#include "Services/FeatureService.h"
#include "FeatureServiceCommand.h"
#include "ExtendedSelectCommand.h"
#include "SelectCommand.h"
#include "SelectAggregateCommand.h"

MgdFeatureServiceCommand* MgdFeatureServiceCommand::CreateCommand(MgResourceIdentifier* resource, FdoCommandType commandType)
{
    Ptr<MgdFeatureServiceCommand> command;
    switch(commandType)
    {
        case FdoCommandType_Select:
        {
            command = new MgdSelectCommand(resource);
            break;
        }
        case FdoCommandType_SelectAggregates:
        {
            command = new MgdSelectAggregateCommand(resource);
            break;
        }
        case FdoCommandType_ExtendedSelect:
        {
            command = new MgdExtendedSelectCommand(resource);
            break;
        }
    }
    return command.Detach();
}

bool MgdFeatureServiceCommand::IsFdoSupportedFunction(FdoIConnection* connection, FdoFunction* fdoFunc)
{
    CHECKNULL(connection, L"MgServerSelectFeatures.SupportsFunction");

    FdoPtr<FdoIExpressionCapabilities> fec = connection->GetExpressionCapabilities();
    CHECKNULL((FdoIExpressionCapabilities*)fec, L"MgServerSelectFeatures.SupportsFunction");

    bool supports = false;

    FdoPtr<FdoFunctionDefinitionCollection> ffdc = fec->GetFunctions();
    if (NULL != (FdoFunctionDefinitionCollection*)ffdc)
    {
        FdoInt32 funcCnt = ffdc->GetCount();
        for (FdoInt32 i=0; i < funcCnt; i++)
        {
            FdoPtr<FdoFunctionDefinition> ffd = ffdc->GetItem(i);
            CHECKNULL((FdoFunctionDefinition*)ffd, L"MgServerSelectFeatures.SupportsFunction");

            // TODO: Just comparing name is enough?
            // TODO: I think, NOT, because there can be overloaded functions like one function
            // with multiple arguments, differnet datatypes etc.
            //
            // Comparing argument count is not sufficient because, there can be optional arguments
            // as well. Therefore, we should just restrict to name comparision only
            FdoString* funcNameAllowed = ffd->GetName();
            FdoString* funcNameSupplied = fdoFunc->GetName();
            size_t cmp = _wcsicmp(funcNameAllowed, funcNameSupplied);
            if (cmp == 0)
            {
                supports = true;
                break;
            }
        }
    }
    return supports;
}

bool MgdFeatureServiceCommand::SupportsSelectDistinct(FdoIConnection* connection)
{
    CHECKNULL((FdoIConnection*)connection, L"MgdFeatureServiceCommand.SupportsSelectDistinct");

    FdoPtr<FdoICommandCapabilities> fcc = connection->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)fcc, L"MgdFeatureServiceCommand.SupportsSelectDistinct");

    bool supports = fcc->SupportsSelectDistinct();

    return supports;
}


bool MgdFeatureServiceCommand::SupportsSelectOrdering(FdoIConnection* connection)
{
    CHECKNULL((FdoIConnection*)connection, L"MgdFeatureServiceCommand.SupportsSelectOrdering");

    FdoPtr<FdoICommandCapabilities> fcc = connection->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)fcc, L"MgdFeatureServiceCommand.SupportsSelectOrdering");

    bool supports = fcc->SupportsSelectOrdering();

    return supports;
}

bool MgdFeatureServiceCommand::SupportsSelectGrouping(FdoIConnection* connection)
{
    CHECKNULL((FdoIConnection*)connection, L"MgdFeatureServiceCommand.SupportsSelectGrouping");

    FdoPtr<FdoICommandCapabilities> fcc = connection->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)fcc, L"MgdFeatureServiceCommand.SupportsSelectGrouping");

    bool supports = fcc->SupportsSelectGrouping();

    return supports;
}
