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

#ifndef _MgWmsFeatureInfoGeometry_h
#define _MgWmsFeatureInfoGeometry_h

#include "XmlParser.h"
#include "Dictionary.h"

class MgWmsFeatureGeometry : public IOgcResourceEnumerator, public MgDisposable
{
public:
    MgWmsFeatureGeometry(MgGeometry* geom);
    //Default constructor to keep Ptr<> happy
    MgWmsFeatureGeometry() {};
    virtual ~MgWmsFeatureGeometry();
    
    void SetFormat(CREFSTRING format);

    bool Next();
    void GenerateDefinitions(MgUtilDictionary& Dictionary);

protected:
    virtual void Dispose()
    {
        delete this;
    }

private:
    Ptr<MgGeometry> m_geom;
    bool m_bRead;
    STRING m_format;
};

#endif