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

#ifndef _MG_CONTROLLER_H_
#define _MG_CONTROLLER_H_

/// \cond INTERNAL
//////////////////////////////////////////////////////////////////
/// \brief
/// This class contains methods to respond to all of the standard
/// web-application requests that are supported by both ADV and ZCV.
/// \remarks
/// Additional work is required to fill in the details of this class.
class MG_MAPGUIDE_API MgController : public MgDisposable
{
    DECLARE_CLASSNAME(MgController)

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgController object
    ///
    /// \return
    /// Nothing
    ///
    MgController(MgSiteConnection* siteConn = NULL);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct a MgController object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgController();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId() { return m_cls_id; }

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose()
    {
        delete this;
    }

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get an instance of the specified service.
    ///
    /// \return
    /// Pointer to an instance of the specified service
    ///
    MgService* GetService(INT32 serviceType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Apply the specified set of commands to a map view.
    ///
    virtual void ApplyMapViewCommands(MgMap* map, MgPropertyCollection* mapViewCommands);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Show or Hide a set of layers in the specified map.
    ///
    void ShowLayers(MgMap* map, CREFSTRING strLayers, bool show);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Show or Hide a set of groups in the specified map.
    ///
    void ShowGroups(MgMap* map, CREFSTRING strGroups, bool show);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Create an MgEnvelope instance from a string representation.
    ///
    /// \todo
    /// Will be a static method of MgEnvelope
    MgEnvelope* ParseEnvelope(CREFSTRING strEnvelope);

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_MapLayer_Controller;

protected:
    static const STRING     m_mapCmdSetViewCenterX;
    static const STRING     m_mapCmdSetViewCenterY;
    static const STRING     m_mapCmdSetViewScale;
    static const STRING     m_mapCmdSetDataExtent;
    static const STRING     m_mapCmdSetDisplayDpi;
    static const STRING     m_mapCmdSetDisplayWidth;
    static const STRING     m_mapCmdSetDisplayHeight;
    static const STRING     m_mapCmdShowLayers;
    static const STRING     m_mapCmdHideLayers;
    static const STRING     m_mapCmdShowGroups;
    static const STRING     m_mapCmdHideGroups;
    static const STRING     m_mapCmdRefreshLayers;
    Ptr<MgSiteConnection> m_siteConn;

};

#include "DwfController.h"
#include "HtmlController.h"

#endif

/// \endcond
