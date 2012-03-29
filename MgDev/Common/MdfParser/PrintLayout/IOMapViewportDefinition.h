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

#ifndef IOMAPVIEWPORTDEFINITION_H_
#define IOMAPVIEWPORTDEFINITION_H_

#include "IOPrintLayoutElementDefinition.h"
#include "../MdfModel/PrintLayout/MapViewportDefinition.h"

BEGIN_NAMESPACE_MDFPARSER

class IOMapViewportDefinition : public IOPrintLayoutElementDefinition
{
public:
    IOMapViewportDefinition(MapViewportDefinition* mapViewportDef, Version& version);
    virtual ~IOMapViewportDefinition();

    MapViewportDefinition* GetMapViewportDefinition();

    virtual void StartElement(const wchar_t* name, HandlerStack* handlerStack);
    virtual void ElementChars(const wchar_t* ch);

    static void Write(MdfStream& fd, MapViewportDefinition* mapViewportDef, Version* version, MgTab& tab);

private:
    // Hidden default/copy constructors and assignment operator.
    IOMapViewportDefinition();
    IOMapViewportDefinition(const IOMapViewportDefinition&);
    IOMapViewportDefinition& operator=(const IOMapViewportDefinition&);
};

// Inline Methods
inline MapViewportDefinition* IOMapViewportDefinition::GetMapViewportDefinition()
{
    MapViewportDefinition* mapViewportDef = dynamic_cast<MapViewportDefinition*>(m_layoutElemDef);
    _ASSERT(NULL != mapViewportDef);
    return mapViewportDef;
}

END_NAMESPACE_MDFPARSER
#endif // IOMAPVIEWPORTDEFINITION_H_
