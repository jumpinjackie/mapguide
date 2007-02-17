//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef _MG_SERVER_UPDATE_COMMAND_H_
#define _MG_SERVER_UPDATE_COMMAND_H_

class MgServerFeatureConnection;

class MgServerUpdateCommand : public MgFeatureManipulationCommand
{
    DECLARE_CLASSNAME(MgServerUpdateCommand)

public:
    MgServerUpdateCommand(MgFeatureCommand* command, MgServerFeatureConnection* connection, INT32 cmdId);
    virtual MgProperty* Execute();
protected:
    MgServerUpdateCommand();
    ~MgServerUpdateCommand();
private:
    Ptr<MgUpdateFeatures> m_featCommand;
    Ptr<MgServerFeatureConnection> m_srvrFeatConn;
    INT32 m_cmdId;
};

#endif
