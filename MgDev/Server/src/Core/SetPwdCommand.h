//
//  Copyright (C) 2004-2017 by Autodesk, Inc.
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

#ifndef SET_PWD_COMMAND_H
#define SET_PWD_COMMAND_H

#include "MapGuideCommon.h"
#include "ServerInteractiveCommand.h"

class MgSetPwdCommand : public MgServerInteractiveCommand
{
public:
    MgSetPwdCommand(CREFSTRING locale, CREFSTRING userName, CREFSTRING password);
    virtual ~MgSetPwdCommand();

    virtual INT32 Execute();

private:
    STRING m_locale;
    STRING m_strUser;
    STRING m_strPassword;
};

#endif