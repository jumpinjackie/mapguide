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
#include "FeatureManipulationCommand.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FeatureUtil.h"
#include "InsertCommand.h"
#include "UpdateCommand.h"
#include "DeleteCommand.h"

MgFeatureManipulationCommand* MgFeatureManipulationCommand::CreateCommand(MgFeatureCommand* webCmd, MgFeatureConnection* connection, INT32 cmdId)
{
    CHECKNULL(webCmd, L"MgFeatureManipulationCommand.CreateCommand")
    CHECKNULL(connection, L"MgFeatureManipulationCommand.CreateCommand")

    INT32 cmdType = webCmd->GetCommandType();
    bool supports = false;

    Ptr<MgFeatureManipulationCommand> command;

    switch(cmdType)
    {
        case MgFeatureCommandType::InsertFeatures:
        {
            supports = connection->SupportsCommand(FdoCommandType_Insert);
            if (supports)
            {
                command = new MgServerInsertCommand(webCmd, connection, cmdId);
            }
            break;
        }
        case MgFeatureCommandType::UpdateFeatures:
        {
            supports = connection->SupportsCommand(FdoCommandType_Update);
            if (supports)
            {
                command = new MgServerUpdateCommand(webCmd, connection, cmdId);
            }
            break;
        }
        case MgFeatureCommandType::DeleteFeatures:
        {
            supports = connection->SupportsCommand(FdoCommandType_Delete);
            if (supports)
            {
                command = new MgServerDeleteCommand(webCmd, connection, cmdId);
            }
            break;
        }
    }

    if (!supports)
    {
        STRING message = MgFeatureUtil::GetMessage(L"MgCommandNotSupported");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgFeatureManipulationCommand.CreateCommand", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return command.Detach();
}
