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
#include "Services/Feature/FeatureConnection.h"
#include "FeatureManipulationCommand.h"
#include "DeleteCommand.h"
#include "Services/Feature/FeatureUtil.h"
#include "Services/FeatureReader.h"

MgdDeleteCommand::MgdDeleteCommand()
{
    m_srvrFeatConn = NULL;
    m_featCommand = NULL;
}

MgdDeleteCommand::MgdDeleteCommand(MgFeatureCommand* command, MgdFeatureConnection* connection, INT32 cmdId)
{
    CHECKNULL(command, L"MgdDeleteCommand.MgdDeleteCommand");
    CHECKNULL(connection, L"MgdDeleteCommand.MgdDeleteCommand");

    m_srvrFeatConn = SAFE_ADDREF((MgdFeatureConnection*)connection);
    m_featCommand = SAFE_ADDREF((MgDeleteFeatures*)command);
    m_cmdId = cmdId;
}

MgdDeleteCommand::~MgdDeleteCommand()
{
    m_srvrFeatConn = NULL;
}

MgProperty* MgdDeleteCommand::Execute()
{
    STRING clsName = m_featCommand->GetFeatureClassName();
    STRING filterText = m_featCommand->GetFilterText();

    FdoPtr<FdoIConnection> fdoConn = m_srvrFeatConn->GetConnection();

    // Create the SQL command
    FdoPtr<FdoIDelete> fdoCommand = (FdoIDelete*)fdoConn->CreateCommand(FdoCommandType_Delete);
    CHECKNULL((FdoIDelete*)fdoCommand, L"MgdDeleteCommand.Execute");

    fdoCommand->SetFeatureClassName(clsName.c_str());
    fdoCommand->SetFilter(filterText.c_str());

    INT32 recordsDeleted = fdoCommand->Execute();

    char buff[32];
    sprintf(buff, "%d", m_cmdId);
    STRING str = MgUtil::MultiByteToWideChar(string(buff));

    return new MgInt32Property(str, recordsDeleted);
}
