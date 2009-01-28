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

#ifndef MGRESOURCEPACKAGEHANDLER_H_
#define MGRESOURCEPACKAGEHANDLER_H_

#include "LibraryRepositoryManager.h"
#include "PackageLogWriter.h"

class MgResourcePackageHandler
{
/// Constructors/Destructor

public:

    explicit MgResourcePackageHandler(MgLibraryRepositoryManager& repositoryManager);
    virtual ~MgResourcePackageHandler();

private:

    // Unimplemented Methods

    MgResourcePackageHandler();
    MgResourcePackageHandler(const MgResourcePackageHandler&);
    MgResourcePackageHandler& operator=(const MgResourcePackageHandler&);

/// Methods

public:

    virtual void End(MgException* except) = 0;

protected:

    void InitializeStatus(CREFSTRING packageApiName,
        CREFSTRING packagePathname, bool logActivities);
    void UpdateStatus(MgException* except);

/// Data Members

protected:

    MgLibraryRepositoryManager& m_repositoryManager;
    Ptr<MgPackageLogWriter> m_packageLogWriter;

    STRING m_packagePathname;

    INT32 m_opsSucceeded;
    INT32 m_opsReceived;
};

/// Inline Methods

#endif
