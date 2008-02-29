//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef MGPACKAGELOGREADER_H_
#define MGPACKAGELOGREADER_H_

#include "PackageLogHandler.h"

class MG_SERVER_MANAGER_API MgPackageLogReader : public MgPackageLogHandler
{
    DECLARE_CLASSNAME(MgPackageLogReader)

/// Constructors/Destructor

public:

    explicit MgPackageLogReader(CREFSTRING packagePathname);
    virtual ~MgPackageLogReader();

private:

    // Unimplemented Constructors/Methods

    MgPackageLogReader();
    MgPackageLogReader(const MgPackageLogReader&);
    MgPackageLogReader& operator=(const MgPackageLogReader&);

/// Methods

public:

    MgPackageStatusInformation* GetStatus();
    MgByteReader* GetLog();

private:

    bool SetEntry(CREFSTRING name, CREFSTRING value);
    void ReadStatus();

/// Data Members

private:

};

/// Inline Methods

#endif
