//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "WebApp.h"

///////////////////////////////////////////////////////////////////////////
// Tells if the legend band is shown
//
bool MgWebInformationPane::IsLegendBandVisible()
{
    return m_legendVisible;
}

///////////////////////////////////////////////////////////////////////////
// Tells if the properties band is shown
//
bool MgWebInformationPane::IsPropertiesBandVisible()
{
    return m_propertiesVisible;
}

///////////////////////////////////////////////////////////////////////////
// ctor
//
MgWebInformationPane::MgWebInformationPane()
: m_legendVisible(true),
  m_propertiesVisible(true)
{

}

///////////////////////////////////////////////////////////////////////////
// Set the legend band visibility
//
void MgWebInformationPane::SetLegendBandVisible(bool visible)
{
    m_legendVisible = visible;
}

///////////////////////////////////////////////////////////////////////////
// Set the properties band visibility
//
void MgWebInformationPane::SetPropertiesBandVisible(bool visible)
{
    m_propertiesVisible = visible;
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebInformationPane::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebInformationPane::Dispose()
{
    delete this;
}
