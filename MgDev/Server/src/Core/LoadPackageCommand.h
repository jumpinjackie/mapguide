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

#ifndef LOAD_PACKAGE_COMMAND_H
#define LOAD_PACKAGE_COMMAND_H

#include "MapGuideCommon.h"
#include "ServerInteractiveCommand.h"

class MgLoadPackageCommand : public MgServerInteractiveCommand
{
public:
    MgLoadPackageCommand(CREFSTRING locale, CREFSTRING path);
    virtual ~MgLoadPackageCommand();
    virtual INT32 Execute();

private:
    STRING m_locale;
    STRING m_path;
};

#endif
