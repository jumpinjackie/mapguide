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

#ifndef MG_GEOMETRY_DATA_READER_CREATOR_H
#define MG_GEOMETRY_DATA_READER_CREATOR_H

using namespace std;

#include <vector>

class MgGeometryDataReaderCreator : public MgDataReaderCreator<MgGeometry*>
{
    DECLARE_CLASSNAME(MgGeometryDataReaderCreator)

public:
    MgGeometryDataReaderCreator()
    {
    }

    MgGeometryDataReaderCreator(CREFSTRING propertyAlias)
    {
        m_propertyAlias = propertyAlias;
        m_propType = MgPropertyType::Geometry;
    }

    ~MgGeometryDataReaderCreator()
    {
    }

    //
    MgProperty* GetProperty(MgGeometry* val)
    {
        // Convert Geometry to AGF
        MgAgfReaderWriter agfWriter;
        Ptr<MgByteReader> reader = agfWriter.Write(val);

        // Create Geometry property
        return new MgGeometryProperty(m_propertyAlias, reader);
    }

    virtual void Dispose()
    {
        delete this;
    }
};

#endif
