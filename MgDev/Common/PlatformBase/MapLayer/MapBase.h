//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _MG_MAP_BASE_H_
#define _MG_MAP_BASE_H_

/// \defgroup MgMapBase MgMapBase
/// \ingroup Maps_and_Layers_Module
/// \{

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

BEGIN_NAMESPACE_MDFMODEL
class MapDefinition;
END_NAMESPACE_MDFMODEL

class MgPoint;
class MgMapBase;
template class MG_PLATFORMBASE_API Ptr<MgMapBase>;

#ifdef _WIN32
#undef CreateService
#endif

#define LAYER_ZORDER_TOP        100.0
#define LAYER_ZORDER_INCR       100.0

typedef vector<double> FINITESCALES;
typedef map<double, double, less<double> > SORTEDSCALES;

/////////////////////////////////////////////////////////////////
/// \brief
/// Defines the runtime state of a map.
///
/// \note This is a base class. In most cases it is best to use a class
    /// derived from this class. See \ref Maps_and_Layers_Module "Maps and Layers" for details.
///
class MG_PLATFORMBASE_API MgMapBase : public MgResource
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMapBase)

PUBLISHED_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an empty un-initialized MgMapBase object.
    ///
    /// \note This is a base class. In most cases it is best to use a class
    /// derived from this class. See \ref Maps_and_Layers_Module "Maps and Layers" for details.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgMapBase();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgMapBase();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgMapBase();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgMapBase();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of this map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the name of the map as a string.
    ///
    virtual STRING GetName();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the identifier of the session associated with this map.
    ///
    /// \note1
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetSessionId();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetSessionId();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetSessionId();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the session id of the map as a string.
    ///
    STRING GetSessionId();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets an identifier that can be used to uniquely identify this
    /// map.
    ///
    /// \note1
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetObjectId();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetObjectId();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetObjectId();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the unique identifier of this map as a string.
    ///
    virtual STRING GetObjectId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the resource identifier that specifies the location
    /// of the map definition that was used to create this map.
    ///
    /// \note1
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgResourceIdentifier GetMapDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgResourceIdentifier GetMapDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgResourceIdentifier GetMapDefinition();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgResourceIdentifier that specifies the location
    /// of the map definition.
    ///
    virtual MgResourceIdentifier* GetMapDefinition();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the spatial reference system used to display this map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetMapSRS();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetMapSRS();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetMapSRS();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the spatial reference system in the OpenGIS \link WKT WKT \endlink
    /// (Well Known Text) format as a string.
    ///
    virtual STRING GetMapSRS();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the the overall extent of the map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgEnvelope GetMapExtent();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgEnvelope GetMapExtent();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgEnvelope GetMapExtent();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgEnvelope that defines the overall extent of the
    /// map.
    ///
    virtual MgEnvelope* GetMapExtent();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns this map's layers.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayerCollection GetLayers();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayerCollection GetLayers();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayerCollection GetLayers();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgLayerCollection that contains the map layers.
    ///
    virtual MgLayerCollection* GetLayers();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns this map's layer groups.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayerGroupCollection GetLayerGroups();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayerGroupCollection GetLayerGroups();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayerGroupCollection GetLayerGroups();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgLayerGroupCollection that contains the map
    /// layer groups.
    ///
    virtual MgLayerGroupCollection* GetLayerGroups();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Initializes a new MgMapBase object given a resource service, map
    /// definition, and a name for the map. This method is used for
    /// MapGuide Viewers or for offline map production.
    ///
    /// \note1
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
    /// \param resourceService
    /// An MgResourceService that can be used to
    /// retrieve the map definition.
    /// \param mapDefinition
    /// An MgResourceIdentifier that specifies the
    /// location of the map definition in a resource
    /// repository.
    /// \param mapName
    /// A string that specifies the name of the map.
    ///
    virtual void Create(MgResourceService* resourceService, MgResourceIdentifier* mapDefinition, CREFSTRING mapName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Initializes a new MgMapBase object given a spatial reference system,
    /// spatial extent of the map, and a name for the map. This method
    /// is used for the WMS service implementation and creates a map
    /// without any layers.
    ///
    /// \note1
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
    /// \param mapSRS
    /// A string specifying the spatial reference system in OpenGIS WKT
    /// format.
    /// \param mapExtent
    /// An MgEnvelope defining the overall extent of the map.
    /// \param mapName
    /// A string that specifies the name of the map.
    ///
    virtual void Create(CREFSTRING mapSRS, MgEnvelope* mapExtent, CREFSTRING mapName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Loads the map object from a session repository.
    ///
    /// \note1
    ///
    /// \remarks
    /// For more information, see \ref Maps_and_Layers_Module "Maps and Layers".
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
    /// \param resourceService
    /// An MgResourceService that can be used to retrieve
    /// the map.
    /// \param mapName
    /// A string that specifies the name of the map. This
    /// is the name that was specified when \link MgMapBase::Create Create \endlink
    /// was called to create the map object.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \todo
    ///   * [[Job for Philip: If I move that overview to the
    ///     Developer's Guide, update the xref here.]]
    ///
    virtual void Open(MgResourceService* resourceService, CREFSTRING mapName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the current center point of the map, in the spatial
    /// reference system of the map.
    ///
    /// \note1
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPoint GetViewCenter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPoint GetViewCenter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPoint GetViewCenter();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgPoint that specifies the center of the map.
    ///
    virtual MgPoint* GetViewCenter();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the current scale of the map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// double GetViewScale();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// double GetViewScale();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// double GetViewScale();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a double that specifies the scale of the map.
    ///
    virtual double GetViewScale();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the extent of the data to be generated for the map
    /// in the spatial reference system of the map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgEnvelope GetDataExtent();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgEnvelope GetDataExtent();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgEnvelope GetDataExtent();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgEnvelope that defines the current data extent.
    ///
    virtual MgEnvelope* GetDataExtent();   /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the number of dots per inch of the map display.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetDisplayDpi();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetDisplayDpi();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetDisplayDpi();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the number of dots per inch of the map display.
    ///
    virtual INT32 GetDisplayDpi();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the current width of the map display in pixels.
    ///
    /// \remarks
    /// The width must be set by the client using the SETDISPLAYWIDTH
    /// command as part of a GetMap or GetMapUpdate HTTP request.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetDisplayWidth();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetDisplayWidth();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetDisplayWidth();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the width of the map display in pixels.
    ///
    virtual INT32 GetDisplayWidth();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the current height of the map display in pixels.
    ///
    /// \remarks
    /// The height must be set by the client using the
    /// SETDISPLAYHEIGHT command as part of a GetMap or GetMapUpdate
    /// HTTP request.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetDisplayHeight();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetDisplayHeight();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetDisplayHeight();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the height of the map display in pixels.
    ///
    virtual INT32 GetDisplayHeight();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of finite display scales in the map.
    ///
    /// \note1
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetFiniteDisplayScaleCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetFiniteDisplayScaleCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetFiniteDisplayScaleCount();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the number of finite display scales in the map.
    ///
    virtual INT32 GetFiniteDisplayScaleCount();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the finite display scale at the specified index.
    ///
    /// \note1
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetFiniteDisplayScaleAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetFiniteDisplayScaleAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetFiniteDisplayScaleAt(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index
    /// Index of the finite display scale to get.
    ///
    /// \return
    /// Returns the finite display scale.
    ///
    /// \exception MgOutOfRangeException if the index is invalid.
    ///
    virtual double GetFiniteDisplayScaleAt(INT32 index);

EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the background color for the map
    ///
    /// \return
    /// Background color.
    ///
    virtual STRING GetBackgroundColor();

INTERNAL_API:

    static MdfModel::MapDefinition* GetMapDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId);
    static MdfModel::MapDefinition* GetMapDefinition(CREFSTRING resourceContent);

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
    /// Does it allow to set the name
    ///
    /// \return
    /// true - allows to set the name
    /// false - can not set the name
    ///
    virtual bool CanSetName();


    enum LayerRefreshMode
    {
        unspecified,
        refreshAll,
        refreshNone,
    };

    //////////////////////////////////////////////////////////////////
    /// Set the refresh mode for all layers in this map
    virtual void SetLayerRefreshMode(LayerRefreshMode refreshMode);

    //////////////////////////////////////////////////////////////////
    /// Get the refresh mode for all layers in this map
    virtual LayerRefreshMode GetLayerRefreshMode();

    /// \brief
    /// Destruct a MgMapBase object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgMapBase();

    /// \brief
    /// Sets the current center point of the map, in the spatial reference system of the map
    ///
    /// \note1
    ///
    /// \param center
    /// Center of the map
    ///
    virtual void SetViewCenter(MgPoint* center);

    /// \brief
    /// Sets the current scale of the map
    ///
    /// \param scale
    /// Scale of the map
    ///
    virtual void SetViewScale(double scale);

    /// \brief
    /// Sets the extent of the data to be generated for the map in the spatial reference system of the map.
    /// This method is reserved for use by ADV and should not be called directly
    ///
    /// \param extent
    /// Data extent
    ///
    virtual void SetDataExtent(MgEnvelope* extent);

    /// \brief
    /// Sets the number of dots per inch of the map display
    /// This method is for use by the viewers and should not be called directly
    ///
    /// \param dpi
    /// Number of dots per inch of the map display
    ///
    virtual void SetDisplayDpi(INT32 dpi);

    /// \brief
    /// Sets the current width of the map display in pixels
    /// This method is for use by the viewers and should not be called directly
    ///
    /// \param width
    /// Width of the map display in pixels
    ///
    virtual void SetDisplayWidth(INT32 width);

    /// \brief
    /// Sets the current height of the map display in pixels
    /// This method is for use by the viewers and should not be called directly
    ///
    /// \param height
    /// Height of the map display in pixels
    ///
    virtual void SetDisplayHeight(INT32 height);

    /// \brief
    /// Gets the meters per unit for the map coordinate system.  This is calculated
    /// when the map is created.
    ///
    /// \return
    /// Number of meters for each coordsys unit
    ///
    virtual double GetMetersPerUnit();

    /// \brief
    /// Searches the list of finite display scales for the scale nearest the
    /// supplied scale, and returns the index of that scale.
    ///
    /// \param scale
    /// The scale to use when searching.
    ///
    /// \return
    /// The index of the finite display scale nearest the supplied scale, or -1
    /// if there are no scales.
    ///
    INT32 FindNearestFiniteDisplayScaleIndex(double scale);

    /// \brief
    /// Callback method invoked when a group is removed from the group collection
    ///
    virtual void OnGroupRemoved(MgLayerGroup* group);

    /// \brief
    /// Callback method invoked when a group is added to the group collection
    ///
    virtual void OnGroupAdded(MgLayerGroup* group);

    /// \brief
    /// Callback method invoked when a group visibility has changed
    ///
    virtual void OnGroupVisibilityChanged(MgLayerGroup* group, CREFSTRING visibility);

    /// \brief
    /// Callback method invoked when the display in legend state of a group has changed
    ///
    virtual void OnGroupDisplayInLegendChanged(MgLayerGroup* group, CREFSTRING displayInLegendState);

    /// \brief
    /// Callback method invoked when the legend label of a group has changed
    ///
    virtual void OnGroupLegendLabelChanged(MgLayerGroup* group, CREFSTRING legendLabel);

    /// \brief
    /// Callback method invoked when the parent of a group has changed
    ///
    virtual void OnGroupParentChanged(MgLayerGroup* group, CREFSTRING parentId);

    /// \brief
    /// Callback method invoked when a layer is removed from the layer collection
    ///
    virtual void OnLayerRemoved(MgLayerBase* layer);

    /// \brief
    /// Callback method invoked when a layer is added to the group collection
    ///
    virtual void OnLayerAdded(MgLayerBase* layer);

    /// \brief
    /// Callback method invoked when a layer visibility has changed
    ///
    virtual void OnLayerVisibilityChanged(MgLayerBase* layer, CREFSTRING visibility);

    /// \brief
    /// Callback method invoked when the display in legend state of a layer has changed
    ///
    virtual void OnLayerDisplayInLegendChanged(MgLayerBase* layer, CREFSTRING displayInLegendState);

    /// \brief
    /// Callback method invoked when the legend label of a layer has changed
    ///
    virtual void OnLayerLegendLabelChanged(MgLayerBase* layer, CREFSTRING legendLabel);

    /// \brief
    /// Callback method invoked when the parent of a layer has changed
    ///
    virtual void OnLayerParentChanged(MgLayerBase* layer, CREFSTRING parentId);

    /// \brief
    /// Callback method invoked when the selectability of a layer has changed
    ///
    virtual void OnLayerSelectabilityChanged(MgLayerBase* layer, CREFSTRING selectability);

    /// \brief
    /// Callback method invoked when the selectability of a layer has changed
    ///
    virtual void OnLayerDefinitionChanged(MgLayerBase* layer);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the changelists. Returns an empty collection of no changes were tracked
    ///
    virtual MgNamedCollection* GetChangeLists();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Clear any changes that have been made to layer or groups of this map
    ///
    virtual void ClearChanges();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Reset refresh flag for all the layers of this map
    ///
    virtual void ResetLayersRefresh();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an instance of the specified service.
    ///
    virtual MgService* GetService(INT32 serviceType);

protected:

    void TrackChange(CREFSTRING objectId, bool layer, MgObjectChange::ChangeType type, CREFSTRING param);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Unpacks Layers and groups from Memory stream (lazy initialization)
    /// This is a stub method to be overloaded by application specific code
    ///
    virtual void UnpackLayersAndGroups();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Packs Layers and groups to a Memory stream (lazy initialization)
    /// This is a stub method to be overloaded by application specific code
    ///
    virtual MgMemoryStreamHelper* PackLayersAndGroups();

    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
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

    virtual const char* GetResourceTypeName();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_MapLayer_MapBase;

protected:
    STRING                    m_name;
    STRING                    m_objectId;
    Ptr<MgResourceIdentifier> m_mapDefinitionId;
    STRING                    m_srs;
    Ptr<MgLayerCollection>    m_layers;
    Ptr<MgLayerGroupCollection> m_groups;
    Ptr<MgPoint>              m_center;
    double                    m_scale;
    Ptr<MgEnvelope>           m_mapExtent;
    Ptr<MgEnvelope>           m_dataExtent;
    INT32                     m_displayDpi;
    INT32                     m_displayWidth;
    INT32                     m_displayHeight;
    Ptr<MgNamedCollection>    m_changeLists;
    bool                      m_trackChangesDisabled;
    LayerRefreshMode          m_layerRefreshMode;
    STRING                    m_backColor;
    double                    m_metersPerUnit;
    FINITESCALES              m_finiteDisplayScales;
};
/// \}

#endif
