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

#ifndef IOMAPVIEW_H_
#define IOMAPVIEW_H_

#include "../SAX2ElementHandler.h"
#include "../MdfModel/PrintLayout/MapView.h"

BEGIN_NAMESPACE_MDFPARSER

class IOMapView : public SAX2ElementHandler
{
public:
    IOMapView(MapView* mapView, Version& version);
    virtual ~IOMapView();

    virtual void StartElement(const wchar_t* name, HandlerStack* handlerStack);
    virtual void ElementChars(const wchar_t* ch);
    virtual void EndElement(const wchar_t* name, HandlerStack* handlerStack);

    static void Write(MdfStream& fd, MapView* mapView, Version* version, MgTab& tab);

private:
    // Hidden default/copy constructors and assignment operator.
    IOMapView();
    IOMapView(const IOMapView&);
    IOMapView& operator=(const IOMapView&);

    MapView* m_mapView;
};

END_NAMESPACE_MDFPARSER
#endif // IOMAPVIEW_H_
