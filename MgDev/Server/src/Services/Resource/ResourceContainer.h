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

#ifndef MGRESOURCECONTAINER_H_
#define MGRESOURCECONTAINER_H_

#include "Database.h"
#include "ResourceInfo.h"

class MgResourceContainer : public MgDatabase
{
/// Constructors/Destructor

    public:

        MgResourceContainer(MgDbEnvironment& environment,
            const string& fileName);
        virtual ~MgResourceContainer();

    private:

        // Unimplemented Constructors/Methods

        MgResourceContainer();
        MgResourceContainer(const MgResourceContainer&);
        MgResourceContainer& operator=(const MgResourceContainer&);

/// Methods

    public:

        virtual string GetName();
        XmlContainer& GetXmlContainer();

        void AddIndex(const string& uri, const string& name,
            const string& index);
        void DeleteIndex(const string& uri, const string& name,
            const string& index);

/// Data Members

    private:

        auto_ptr<XmlContainer> m_xmlContainer;
};

/// Inline Methods

inline XmlContainer& MgResourceContainer::GetXmlContainer()
{
    assert(NULL != m_xmlContainer.get());
    return *m_xmlContainer.get();
}

#endif
