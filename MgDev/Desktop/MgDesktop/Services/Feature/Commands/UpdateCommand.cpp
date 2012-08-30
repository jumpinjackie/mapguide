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
#include "UpdateCommand.h"
#include "Services/Feature/FeatureUtil.h"
#include "Services/FeatureReader.h"

MgdUpdateCommand::MgdUpdateCommand()
{
    m_srvrFeatConn = NULL;
    m_featCommand = NULL;
}

MgdUpdateCommand::MgdUpdateCommand(MgFeatureCommand* command, MgdFeatureConnection* connection, INT32 cmdId)
{
    CHECKNULL(command, L"MgdUpdateCommand.MgdUpdateCommand");
    CHECKNULL(connection, L"MgdUpdateCommand.MgdUpdateCommand");

    m_srvrFeatConn = SAFE_ADDREF((MgdFeatureConnection*)connection);
    m_featCommand = SAFE_ADDREF((MgUpdateFeatures*)command);
    m_cmdId = cmdId;
}

MgdUpdateCommand::~MgdUpdateCommand()
{
    m_srvrFeatConn = NULL;
}

MgProperty* MgdUpdateCommand::Execute()
{
    STRING clsName = m_featCommand->GetFeatureClassName();
    STRING filterText = m_featCommand->GetFilterText();
    Ptr<MgPropertyCollection> propCol = m_featCommand->GetPropertyValues();

    FdoPtr<FdoIConnection> fdoConn = m_srvrFeatConn->GetConnection();

    // Create the SQL command
    FdoPtr<FdoIUpdate> fdoCommand = (FdoIUpdate*)fdoConn->CreateCommand(FdoCommandType_Update);
    CHECKNULL((FdoIUpdate*)fdoCommand, L"MgdUpdateCommand.Execute");

    fdoCommand->SetFeatureClassName(clsName.c_str());
    fdoCommand->SetFilter(filterText.c_str());

    FdoPtr<FdoPropertyValueCollection> paramValCol = fdoCommand->GetPropertyValues();
    MgdFeatureUtil::FillFdoPropertyCollection(propCol, paramValCol);

    INT32 recordsUpdated = fdoCommand->Execute();

    char buff[32];
    sprintf(buff, "%d", m_cmdId);
    STRING str = MgUtil::MultiByteToWideChar(string(buff));

    return new MgInt32Property(str, recordsUpdated);
}
