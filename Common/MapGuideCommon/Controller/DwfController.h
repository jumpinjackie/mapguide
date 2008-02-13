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

#ifndef _MG_ADV_CONTROLLER_H_
#define _MG_ADV_CONTROLLER_H_

/// \cond INTERNAL
//Temporary:  page units will be defined in mapping services
class MG_MAPGUIDE_API MgPageSizeUnits
{
};
/// \endcond


/// \cond INTERNAL
//////////////////////////////////////////////////////////////////
/// \brief
/// This class provides functionality for responding to
/// ADV requests and for generating ADV update scripts
/// THIS IS INTERNAL API ONLY.  DO NOT DOCUMENT
class MG_MAPGUIDE_API MgDwfController : public MgController
{
    DECLARE_CLASSNAME(MgDwfController)

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgDwfController object
    ///
    MgDwfController();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgDwfController object
    ///
    MgDwfController(MgSiteConnection* siteConn, CREFSTRING mapAgentUri);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct a MgDwfController object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgDwfController();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Processes a GetMap request from the Viewer and returns
    /// a DWF containing an eMap that fully describes the map
    /// but does not yet contain any layer graphics
    ///
    /// \param mapDefinition
    /// Id of a map definition in a resource repository
    /// \param dwfVersion
    /// DWF version
    /// \param eMapVersion
    /// eMap version
    /// \param language
    /// Language code
    /// \param mapViewCommands
    /// Set of commands to be applied
    ///
    /// \return
    /// A byte reader embedding a DWF containing an eMap
    ///
    MgByteReader* GetMap(
        MgResourceIdentifier* mapDefinition,
        CREFSTRING dwfVersion,
        CREFSTRING eMapVersion,
        MgPropertyCollection* mapViewCommands);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Processes a GetMapUpdate request from the Viewer and
    /// returns an eMapTransaction DWF containing updated
    /// content for the specified map
    ///
    /// \param mapName
    /// Name of the map
    /// \param seqNo
    /// Sequence number.
    /// \param dwfVersion
    /// DWF version
    /// \param eMapVersion
    /// eMap version
    /// \param mapViewCommands
    /// Set of commands to be applied
    ///
    /// \return
    /// A byte reader embedding a DWF containing updated
    /// content for the specified map
    ///
    MgByteReader* GetMapUpdate(CREFSTRING mapName,
        int seqNo,
        CREFSTRING dwfVersion,
        CREFSTRING eMapVersion,
        MgPropertyCollection* mapViewCommands);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Processes a GetPlot request from the Viewer and returns either an EPlot for print to scale functionality,
    /// an EPlot DWF for offline use, or an EPlot generated using a specified print layout
    ///
    /// \param map
    /// Map object to use
    /// \param plotSpec
    /// Plot specification to use
    /// \param layout
    /// Optional layout for plot
    /// \param dwfVersion
    /// DWF version
    /// \param ePlotVersion
    /// EPlot version
    /// \param mapViewCommands
    /// Set of commands to be applied
    ///
    /// \return
    /// A byte reader embedding an EPlot DWF
    ///
    MgByteReader* GetPlot(
        MgMap* map,
        MgPlotSpecification* plotSpec,
        MgLayout* layout,
        CREFSTRING dwfVersion,
        CREFSTRING ePlotVersion,
        MgPropertyCollection* mapViewCommands);

protected:
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
    /// Apply the specified set of commands to a map view.
    ///
    virtual void ApplyMapViewCommands(MgMap* map, MgPropertyCollection* mapViewCommands);

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_MapLayer_AdvController;

private:
    typedef enum OperationType
    {
        unknown,
        get,
        update,
        plot
    } OperationType;

     STRING         m_mapAgentUri;
     OperationType  m_operation;
};

/// \endcond

#endif
