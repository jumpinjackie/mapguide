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

#ifndef IOMAPVIEWPORT_H_
#define IOMAPVIEWPORT_H_

#include "IOPrintLayoutElement.h"
#include "MapViewport.h"

BEGIN_NAMESPACE_MDFPARSER

class IOMapViewport : public IOPrintLayoutElement
{
public:
    IOMapViewport(MapViewport* mapViewport, Version& version);
    virtual ~IOMapViewport();

    MapViewport* GetMapViewport();

    virtual void StartElement(const wchar_t* name, HandlerStack* handlerStack);
    virtual void ElementChars(const wchar_t* ch);

    static void Write(MdfStream& fd, MapViewport* mapViewport, Version* version);

private:
    // Hidden default/copy constructors and assignment operator.
    IOMapViewport();
    IOMapViewport(const IOMapViewport&);
    IOMapViewport& operator=(const IOMapViewport&);
};

// Inline Methods
inline MapViewport* IOMapViewport::GetMapViewport()
{
    MapViewport* mapViewport = dynamic_cast<MapViewport*>(m_printLayoutElement);
    _ASSERT(NULL != mapViewport);
    return mapViewport;
}

END_NAMESPACE_MDFPARSER
#endif // IOMAPVIEWPORT_H_
