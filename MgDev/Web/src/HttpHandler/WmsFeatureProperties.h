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

#ifndef _MgWmsFeatureProperties_h
#define _MgWmsFeatureProperties_h

#include "XmlParser.h"
#include "Dictionary.h"

class MgWmsFeatureGeometry;

class MgWmsFeatureProperties: public IOgcResourceEnumerator, public MgDisposable
{
public:
    MgWmsFeatureProperties(MgPropertyCollection* propertyCollection);

    //Default constructor to keep Ptr<> happy
    MgWmsFeatureProperties(){};
    virtual ~MgWmsFeatureProperties();

    void SetFormat(CREFSTRING format);

    bool Next();
    void GenerateDefinitions(MgUtilDictionary& Dictionary);
    MgWmsFeatureGeometry* GetGeometry();
    STRING GetLayerName();

protected:
    virtual void Dispose()
    {
        delete this;
    }

private:
    static bool IsSpecialProperty(MgProperty* prop);
    Ptr<MgPropertyCollection> m_propertyCollection;
    int m_index;
    STRING m_format;
};

#endif//_MgWmsFeatureProperties_h
