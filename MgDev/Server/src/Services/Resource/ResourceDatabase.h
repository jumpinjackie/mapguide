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

#ifndef MGRESOURCEDATABASE_H_
#define MGRESOURCEDATABASE_H_

#include "Database.h"

class MgResourceDatabase : public MgDatabase
{
/// Constructors/Destructor

    public:

        MgResourceDatabase(MgDbEnvironment& environment,
            const string& fileName);
        virtual ~MgResourceDatabase();

    private:

        // Unimplemented Constructors/Methods

        MgResourceDatabase();
        MgResourceDatabase(const MgResourceDatabase&);
        MgResourceDatabase& operator=(const MgResourceDatabase&);

/// Methods

    public:

        virtual string GetName();
        Db& GetDb();

/// Data Members

    private:

        Db m_db;
};

/// Inline Methods

inline Db& MgResourceDatabase::GetDb()
{
    return m_db;
}

#endif
