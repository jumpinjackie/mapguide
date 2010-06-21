//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef MGPACKAGELOGWRITER_H_
#define MGPACKAGELOGWRITER_H_

#include "PackageLogHandler.h"

class MG_SERVER_MANAGER_API MgPackageLogWriter : public MgPackageLogHandler
{
    DECLARE_CLASSNAME(MgPackageLogWriter)

/// Constructors/Destructor

public:

    MgPackageLogWriter(CREFSTRING packageApiName, CREFSTRING packagePathname);
    virtual ~MgPackageLogWriter();

private:

    // Unimplemented Constructors/Methods

    MgPackageLogWriter();
    MgPackageLogWriter(const MgPackageLogWriter&);
    MgPackageLogWriter& operator=(const MgPackageLogWriter&);

/// Methods

public:

    void SetError(MgException* except);
    void AddOperation(CREFSTRING operation);
    void UpdateLog();

private:

    void WriteEntry(FILE* file, CREFSTRING name, CREFSTRING value,
        bool replaceReservedCharacters = false);
    void WriteEntry(FILE* file, CREFSTRING name, INT32 value);
    void WriteEntry(FILE* file, CREFSTRING name, INT64 value);
    void WriteEntry(FILE* file, CREFSTRING name, double value);
    void WriteEntry(FILE* file, CREFSTRING name, MgDateTime* value);
    void WriteLine(FILE* file);

/// Data Members

private:

    vector<STRING> m_operations;
};

/// Inline Methods

#endif
