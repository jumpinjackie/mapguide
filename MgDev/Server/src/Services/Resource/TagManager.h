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

#ifndef MGTAGMANAGER_H_
#define MGTAGMANAGER_H_

#include "TagInfo.h"
#include "DataBindingInfo.h"

typedef std::map<STRING, MgTagInfo> MgTagMap;

class MgTagManager
{
// Constructors/Destructor

public:

    MgTagManager();
    MgTagManager(CREFSTRING tags);
    virtual ~MgTagManager();

// Methods

public:

    static bool IsReservedTag(CREFSTRING name);
    static void ValidateTag(CREFSTRING name, CREFSTRING type);

    MgTagMap& GetTagMap();

    void ParseTags(CREFSTRING tags);
    STRING GetTags() const;
    int SubstituteTags(const MgDataBindingInfo& dataBindingInfo,
        string& doc);

    bool GetTag(CREFSTRING name, MgTagInfo& tagInfo,
        bool strict = true) const;
    void SetTag(CREFSTRING name, CREFSTRING type, CREFSTRING value,
        CREFSTRING mimeType);
    void DeleteTag(CREFSTRING name, MgTagInfo& tagInfo);
    void RenameTag(CREFSTRING oldName, CREFSTRING newName);

protected:

    void AddTag(CREFSTRING name, const MgTagInfo& tagInfo);

private:

    void CheckTagString(CREFSTRING str);
    void CheckTagString(const string& doc, CREFSTRING tag);
    void CheckTagStrings(const string& doc);

// Data Members

private:

    MgTagMap m_tagMap;
};

// Inline Methods

inline MgTagMap& MgTagManager::GetTagMap()
{
    return m_tagMap;
}

#endif
