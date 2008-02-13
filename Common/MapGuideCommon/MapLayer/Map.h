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

#ifndef _MG_MAP_H_
#define _MG_MAP_H_

/// \defgroup MgMap MgMap
/// \ingroup Maps_and_Layers_Module
/// \{

/*TSW remove
#include "Foundation.h"
#include <vector>
#include <list>
#include <map>
#include "LayerBase.h"
#include "LayerGroup.h"
#include "LayerCollection.h"
#include "LayerGroupCollection.h"
#include "MapCollection.h"
#include "ObjectChange.h"
#include "ChangeList.h"
#include "ReadOnlyLayerCollection.h"
#include "SelectionBase.h"
*/

class MgMap;
class MgSiteConnection;
template class MG_MAPGUIDE_API Ptr<MgMap>;

#ifdef _WIN32
#undef CreateService
#endif


/////////////////////////////////////////////////////////////////
/// \brief
/// Defines the runtime state of a map.
/// 
/// \remarks
/// This corresponds with the state of the map as seen by the
/// client. Note that this may differ from the MapDefinition
/// stored in the resource repository. For example, the user may
/// have altered the scale or hidden layers.
/// \n
/// You can use the Save and Open methods to store the runtime
/// state into the session repository and retrieve it from the
/// session repository.
/// 
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// This example shows information about a map:
/// \code
/// <?php
/// try
/// {
///     // Include constants like MgServiceType::ResourceService
///     include 'C:\Inetpub\wwwroot\PhpMapAgent\MgConstants.php';
///     // Initialize
///     MgInitializeWebTier('C:\Inetpub\wwwroot\PhpMapAgent\webconfig.ini');
///     // Establish a connection with a MapGuide site.
///     $user = new MgUserInformation('Administrator', 'admin');
///     $siteConnection = new MgSiteConnection();
///     $siteConnection->Open($user);
///     // Create a session repository
///     $site = $siteConnection->GetSite();
///     $sessionID = $site->CreateSession();
///     $user->SetMgSessionId($sessionID);
///     // Get an instance of the required services.
///     $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);
///     $mappingService = $siteConnection->CreateService(MgServiceType::MappingService);
///
///     // Get a runtime map from a map definition
///     $resourceID = new  MgResourceIdentifier('Library://Calgary/Maps/Calgary.MapDefinition');
///     $map = new MgMap();
///     $map->Create($resourceService, $resourceID, 'Calgary');
///
///     // Show information about the map
///     echo "Name of map:               '" . $map->GetName() . "'n";
///     echo "   Session ID of map:      " . $map->GetSessionId() . "n";
///     echo "   Object ID:              " . $map->GetObjectId() . "n";
///
///     $envelope = $map->GetDataExtent();
///     echo "   Envelope:               ";
///     PrintEnvelope($envelope);
///
///     $extent = $map->GetMapExtent();
///     echo "   lower left coordinate:  ";
///     PrintCoordinate( $extent->GetLowerLeftCoordinate() );
///     echo "   upper right coordinate: " ;
///     PrintCoordinate( $extent->GetUpperRightCoordinate() );
///
///     $point = $map->GetViewCenter();
///     echo "   view center:            ";
///     PrintPoint($point);
///
///     echo "   View scale:             " . $map->GetViewScale() . "n";
///     echo "   Display dpi:            " . $map->GetDisplayDpi() . "n";
///     echo "   Display width:          " . $map->GetDisplayWidth() . "n";
///     echo "   Display height:         " . $map->GetDisplayHeight() . "n";
///
///     $layerCollection = $map->GetLayers();
///     echo "   Layers: n";
///     PrintLayerCollection( $layerCollection );
///
///     $layerGroupCollection = $map->GetLayerGroups();
///     echo "   Layer groups: n";
///     PrintLayerGroupCollection( $layerGroupCollection );
///
///     echo "   Finite display scales: n";
///     PrintFiniteDisplayScales( $map );
///
///     echo "Done n";
/// }
/// catch (MgException $e)
/// {
///     echo "ERROR: " . $e->GetMessage() . "n";
///     echo $e->GetDetails() . "n";
///     echo $e->GetStackTrace() . "n";
/// }
///
/// /********************************************************************/
/// function PrintEnvelope($envelope)
/// {
///     echo "depth = " . $envelope->GetDepth() . ", height = " . $envelope->GetHeight() . ", width = " . $envelope->GetWidth() . "n";
/// }
///
/// /********************************************************************/
/// function PrintCoordinate($coordinate)
/// {
///     echo "(" . $coordinate->GetX() . ", " . $coordinate->GetY() . ", " . $coordinate->GetZ() . ") n";
/// }
///
/// /********************************************************************/
/// function PrintPoint($point)
/// {
///     PrintCoordinate( $point->GetCoordinate() );
/// }
///
/// /********************************************************************/
/// function PrintLayerCollection($layerCollection)
/// {
///     for ($i = 0; $i < $layerCollection->GetCount(); $i++)
///     {
///         $layer = $layerCollection->GetItem($i);
///         echo "      layer #" . ($i + 1) . ": n" ;
///         PrintLayer($layer);
///     }
/// }
///
/// /********************************************************************/
/// function PrintLayer($layer)
/// {
///     echo "      name:                '" . $layer->GetName() . "'n";
///     $layerDefinition = $layer->GetLayerDefinition();
///     echo "      layer definition:    '" . $layerDefinition->ToString()  . "'n";
///     echo "      legend label:        '" . $layer->GetLegendLabel()  . "'n";
///     echo "      display in legend:   " . ConvertBooleanToString($layer->GetDisplayInLegend()) . "n";
///     echo "      expand in legend:    " . ConvertBooleanToString($layer->GetExpandInLegend()) . "n";
///     echo "      selectable:          " . ConvertBooleanToString($layer->GetSelectable()) . "n";
///     echo "      potentially visible: " . ConvertBooleanToString($layer->GetVisible()) . "n";
///     echo "      actually visible:    " . ConvertBooleanToString($layer->IsVisible()) . "n";
///     echo "      needs refresh:       " . ConvertBooleanToString($layer->NeedsRefresh()) . "n";
/// }
///
/// /********************************************************************/
/// function PrintLayerGroupCollection($layerGroupCollection)
/// {
///     for ($i = 0; $i < $layerGroupCollection->GetCount(); $i++)
///     {
///         $layerGroup = $layerGroupCollection->GetItem($i);
///         echo "      layer group #" . ($i + 1) . ": " ;
///         PrintLayerGroup($layerGroup);
///     }
/// }
///
/// /********************************************************************/
/// function PrintLayerGroup($layerGroup)
/// {
///     echo "      layer group name        '" . $layerGroup->GetName() . "'n";
///     echo "      display in legend:      " . ConvertBooleanToString($layerGroup->GetDisplayInLegend()) . "n";
///     echo "      expand in legend:       " . ConvertBooleanToString($layerGroup->GetExpandInLegend()) . "n";
///     $parentGroup = $layerGroup->GetGroup();
///     echo "      group                   " . $parentGroup->GetName() . "n";
///     echo "      legend label            " . $layerGroup->GetLegendLabel() . "n";
///     echo "      object ID               " . $layerGroup->GetObjectId() . "n";
///     echo "      potentially visible:    " . ConvertBooleanToString($layerGroup->GetVisible()) . "n";
///     echo "      actually visible:       " . ConvertBooleanToString($layerGroup->IsVisible()) . "n";
/// }
///
/// /********************************************************************/
/// function PrintFiniteDisplayScales($map)
/// {
///     for ($i = 0; $i < $map->GetFiniteDisplayScaleCount(); $i++)
///     {
///         echo "      finite display scale #" . ($i + 1) . ": " . $map->GetFiniteDisplayScaleAt($i) . "'n";
///     }
/// }
///
/// /********************************************************************/
/// // Converts a boolean to "yes" or "no".
/// function ConvertBooleanToString($boolean)
/// {
///     if (is_bool($boolean))
///         return ($boolean ? "yes" : "no");
///     else
///         return "ERROR in ConvertBooleanToString.";
/// }
///
/// /********************************************************************/
/// ?>
/// \endcode
/// \htmlinclude ExampleBottom.html
/// 
class MG_MAPGUIDE_API MgMap : public MgMapBase
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMap)

PUBLISHED_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an empty un-initialized MgMap object.
    ///
    /// \remarks
    /// This method has been depreciated. Use the following method:
    /// MgMap(MgSiteConnection siteConnection)
    /// The instance of MgMap cannot be used until either the \link MgMapBase::Create Create \endlink
    /// or \link MgMapBase::Open Open \endlink
    /// method is called.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgMap();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgMap();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgMap();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase::Create Create \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    MgMap();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgMap object that takes an MgSiteConnection instance.
    ///
    /// \remarks
    /// The instance of MgMap cannot be used until either the \link MgMapBase::Create Create \endlink
    /// or \link MgMapBase::Open Open \endlink
    /// method is called.
    /// Session ID is required for this MgMap constructor.
    /// MgMap cannot exist in the Library repository.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgMap(MgSiteConnection siteConnection);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgMap(MgSiteConnection siteConnection);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgMap(MgSiteConnection siteConnection);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param siteConnection (MgSiteConnection)
    /// An MgSiteConnection instance the MgMap object can use to
    /// allocate service instances.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase::Create Create \endlink.
    /// \code
    /// $siteConn = new MgSiteConnection();
    /// $userInfo = new MgUserInformation();
    /// $userInfo->SetMgSessionId('abc123-xxx-yyy');
    /// $siteConn->Open($userInfo);
    /// $map = new MgMap($siteConn);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    MgMap(MgSiteConnection* siteConnection);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Initializes a new MgMap object given a resource service, map
    /// definition, and a name for the map. This method is used for
    /// MapGuide Viewers or for offline map production.
    ///
    /// \remarks
    /// This method has been depreciated. Use the following method:
    /// void Create(MgResourceIdentifier mapDefinition, string mapName)
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Create(MgResourceService resourceService, MgResourceIdentifier mapDefinition, string mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Create(MgResourceService resourceService, MgResourceIdentifier mapDefinition, String mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Create(MgResourceService resourceService, MgResourceIdentifier mapDefinition, string mapName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resourceService (MgResourceService)
    /// An MgResourceService that can be used to
    /// retrieve the map definition.
    /// \param mapDefinition (MgResourceIdentifier)
    /// An MgResourceIdentifier that specifies the
    /// location of the map definition in a resource
    /// repository.
    /// \param mapName (String/string)
    /// A string that specifies the name of the map.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the resource service has already been intialized
    /// $resourceID = new  MgResourceIdentifier('Library://Calgary/Maps/Calgary.MapDefinition');
    /// $map = new MgMap();
    /// $map->Create($resourceService, $resourceID, 'Calgary');
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual void Create(MgResourceService* resourceService, MgResourceIdentifier* mapDefinition, CREFSTRING mapName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Initializes a new MgMap object given a map definition
    /// and a name for the map. This method is used for
    /// MapGuide Viewers or for offline map production.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Create(MgResourceIdentifier mapDefinition, string mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Create(MgResourceIdentifier mapDefinition, String mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Create(MgResourceIdentifier mapDefinition, string mapName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param mapDefinition (MgResourceIdentifier)
    /// An MgResourceIdentifier that specifies the
    /// location of the map definition in a resource
    /// repository.
    /// \param mapName (String/string)
    /// A string that specifies the name of the map.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the site connection has already been intialized
    /// $resourceID = new  MgResourceIdentifier('Library://Calgary/Maps/Calgary.MapDefinition');
    /// $map = new MgMap($site);
    /// $map->Create($resourceID, 'Calgary');
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual void Create(MgResourceIdentifier* mapDefinition, CREFSTRING mapName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Initializes a new Map object given a spatial reference system,
    /// spatial extent of the map, and a name for the map. This method
    /// is used for the WMS service implementation and creates a map
    /// without any layers.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Create(string mapSRS, MgEnvelope mapExtent, string mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Create(String mapSRS, MgEnvelope mapExtent, String mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Create(string mapSRS, MgEnvelope mapExtent, string mapName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param mapSRS (String/string)
    /// A string specifying the spatial reference system in OpenGIS WKT
    /// format.
    /// \param mapExtent (MgEnvelope)
    /// An MgEnvelope defining the overall extent of the map.
    /// \param mapName (String/string)
    /// A string that specifies the name of the map.
    ///
    virtual void Create(CREFSTRING mapSRS, MgEnvelope* mapExtent, CREFSTRING mapName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Loads the map object from a session repository.
    ///
    /// \remarks
    /// This method has been depreciated. Use the following method:
    /// void Open(string mapName)
    /// For more information, see \link Mapping_Service_Module Mapping Service \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Open(MgResourceService resourceService, string mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Open(MgResourceService resourceService, String mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Open(MgResourceService resourceService, string mapName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resourceService (MgResourceService)
    /// An MgResourceService that can be used to retrieve
    /// the map.
    /// \param mapName (String/string)
    /// A string that specifies the name of the map. This
    /// is the name that was specified when \link MgMapBase::Create Create \endlink
    /// was called to create the map object.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the resource service has already been initialized
    /// $map = new MgMap();
    /// $map->Open($resourceService, 'Calgary');
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \todo
    ///   * [[Job for Philip: If I move that overview to the
    ///     Developer's Guide, update the xref here.]]
    ///
    virtual void Open(MgResourceService* resourceService, CREFSTRING mapName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Loads the map object from a session repository.
    ///
    /// \remarks
    /// For more information, see \link Mapping_Service_Module Mapping Service \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Open(string mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Open(String mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Open(string mapName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param mapName (String/string)
    /// A string that specifies the name of the map. This
    /// is the name that was specified when \link MgMapBase::Create Create \endlink
    /// was called to create the map object.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the site connection has already been intialized
    /// $map = new MgMap($site);
    /// $map->Open('Calgary');
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \todo
    ///   * [[Job for Philip: If I move that overview to the
    ///     Developer's Guide, update the xref here.]]
    ///
    virtual void Open(CREFSTRING mapName);

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Saves the Map using the specified resource service.
    ///
    /// \remarks
    /// This method has been depreciated. Use the following method:
    /// void Open(string mapName)
    /// This method assumes a valid resource identifier has already
    /// been established via either Open or Save.
    ///
    /// \param resourceService
    /// Resource service.
    ///
    void Save(MgResourceService* resourceService);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Saves the Map using the specified resource service and
    /// resource identifier.
    ///
    /// \param resourceService
    /// Resource service to use to save the Map.
    /// \param resourceId
    /// Resource identifier.
    ///
    void Save(MgResourceService* resourceService, MgResourceIdentifier* resourceId);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Saves the Map.
    ///
    /// \return
    /// Returns nothing.
    ///
    void Save();

INTERNAL_API:

    /// \brief
    /// Destruct a MgMap object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgMap();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to a TCP/IP stream.
    ///
    /// \param stream
    /// Stream.
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from a TCP/IP stream.
    ///
    /// \param stream
    /// Stream.
    ///
    virtual void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an instance of the specified service.
    ///
    virtual MgService* GetService(INT32 serviceType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets internal resource service references.  Used for Lazy loading
    ///
    /// \param resourceService
    /// Resource service.
    ///
    void SetDelayedLoadResourceService(MgResourceService* resourceService);

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Unpacks Layers and groups from Memory stream (lazy initialization)
    ///
    virtual void UnpackLayersAndGroups();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Packs Layers and groups to a Memory stream (lazy initialization)
    /// Pack will only occur if changes have been made to the layers and groups
    ///
    virtual MgMemoryStreamHelper* PackLayersAndGroups();

    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifier.
    ///
    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose();

private:

    void InitializeResourceService(MgResourceService* resourceService);

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_MapLayer_Map;

private:

    // Version for serialization
    static const int m_serializeVersion = (4<<16) + 0;
    static STRING m_layerGroupTag;

    Ptr<MgSiteConnection> m_siteConnection;
    Ptr<MgMemoryStreamHelper> m_layerGroupHelper;
    Ptr<MgResourceService> m_resourceService;
    bool m_inSave;
    bool m_unpackedLayersGroups;
};
/// \}

#endif
