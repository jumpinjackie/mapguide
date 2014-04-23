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

#ifndef MG_DATETIME_DATA_READER_CREATOR_H
#define MG_DATETIME_DATA_READER_CREATOR_H

using namespace std;

#include <vector>

/// \cond INTERNAL
class MgdDateTimeDataReaderCreator : public MgdDataReaderCreator<double>
{
    DECLARE_CLASSNAME(MgdDateTimeDataReaderCreator)

public:
    MgdDateTimeDataReaderCreator()
    {
    }

    MgdDateTimeDataReaderCreator(CREFSTRING propertyAlias)
    {
        m_propertyAlias = propertyAlias;
        m_propType = MgPropertyType::DateTime;
    }

    ~MgdDateTimeDataReaderCreator()
    {
    }

    //
    MgProperty* GetProperty(double val)
    {
        Ptr<MgDateTime> dateTime = new MgDateTime(val);
        return new MgDateTimeProperty(m_propertyAlias, dateTime);
    }

    virtual void Dispose()
    {
        delete this;
    }

};
/// \endcond
#endif
