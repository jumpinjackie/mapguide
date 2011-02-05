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

#ifndef _MGWEBINFORMATIONPANE_
#define _MGWEBINFORMATIONPANE_

class MgWebWidgetCollection;

class MgWebInformationPane;
template class MG_WEBAPP_API Ptr<MgWebInformationPane>;

class MG_WEBAPP_API MgWebInformationPane : public MgWebUiSizablePane
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Tells if the legend band is shown
    /// </summary>
    /// <returns>
    /// TRUE if the legend band is shown
    /// </returns>
    bool IsLegendBandVisible();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Tells if the properties band is shown
    /// </summary>
    /// <returns>
    /// TRUE if the properties band is shown
    /// </returns>
    bool IsPropertiesBandVisible();

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    // ctor
    //
    MgWebInformationPane();

    ///////////////////////////////////////////////////////////////////////////
    // Set the legend band visibility
    //
    void SetLegendBandVisible(bool visible);

    ///////////////////////////////////////////////////////////////////////////
    // Set the properties band visibility
    //
    void SetPropertiesBandVisible(bool visible);

protected:
    ///////////////////////////////////////////////////////////////////////////
    // Get the unique identifier for the class
    //
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    // Dispose this object
    //
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = WebApp_InformationPane;

private:
    bool    m_legendVisible;
    bool    m_propertiesVisible;
};

#endif // _MGWEBINFORMATIONPANE_
