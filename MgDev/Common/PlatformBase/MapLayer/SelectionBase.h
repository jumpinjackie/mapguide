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

#ifndef _MG_SELECTION_BASE_H_
#define _MG_SELECTION_BASE_H_

/// \defgroup MgSelectionBase MgSelectionBase
/// \ingroup Maps_and_Layers_Module
/// \{

class MG_PLATFORMBASE_API MgMemoryStreamHelper;

/// Exposed API class for handling selection
/// Represents a set of selections for multiple layers and classes
class MG_PLATFORMBASE_API MgSelectionBase : public MgResource
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgSelectionBase)

PUBLISHED_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Initialize selection generation from a map
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgSelection(MgMap map);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgSelection(MgMap map);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgSelection(MgMap map);
    /// \htmlinclude SyntaxBottom.html
    ///
    MgSelectionBase(MgMapBase* map);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Initialize selection from a map and xml selection
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgSelection(MgMap map, string xmlSelectionString);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgSelection(MgMap map, String xmlSelectionString);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgSelection(MgMap map, string xmlSelectionString);
    /// \htmlinclude SyntaxBottom.html
    ///
    MgSelectionBase(MgMapBase* map, CREFSTRING xmlSelectionString);

    /// \brief
    /// Returns the number of selected features.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetSelectedFeaturesCount(MgLayerBase layer, string className);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetSelectedFeaturesCount(MgLayerBase layer, String className);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetSelectedFeaturesCount(MgLayerBase layer, string className);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param layer (MgLayerBase)
    /// Input layer.
    /// \param className (String/string)
    /// Input feature class name.
    ///
    /// \return
    /// Returns Count of all selected features.
    ///
    INT32 GetSelectedFeaturesCount(MgLayerBase* layer, CREFSTRING className);

    /// \brief
    /// Returns the selected feature data for the specified feature class.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgFeatureReader GetSelectedFeatures(MgLayerBase layer, string className, bool mappedOnly);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgFeatureReader GetSelectedFeatures(MgLayerBase layer, String className, boolean mappedOnly);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgFeatureReader GetSelectedFeatures(MgLayerBase layer, string className, bool mappedOnly);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param layer (MgLayerBase)
    /// Input layer.
    /// \param className (String/string)
    /// Input feature class name.
    /// \param mappedOnly (boolean/bool)
    /// Return mapped (true) or all (false) properties for selected features
    ///
    /// \return
    /// Returns a feature reader containing all the features for the given
    /// feature class in this selection.
    ///
    virtual MgFeatureReader* GetSelectedFeatures(MgLayerBase* layer, CREFSTRING className, bool mappedOnly);

    /// Returns the selected feature data for the specified feature class.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgFeatureReader GetSelectedFeatures(MgLayerBase layer, string className, MgStringCollection propertyNames);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgFeatureReader GetSelectedFeatures(MgLayerBase layer, String className, MgStringCollection propertyNames);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgFeatureReader GetSelectedFeatures(MgLayerBase layer, string className, MgStringCollection propertyNames);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param layer (MgLayerBase)
    /// Input layer.
    /// \param className (String/string)
    /// Input feature class name.
    /// \param propertyNames (MgStringCollection)
    /// Properties to return for selected features.
    ///
    /// \return
    /// Returns a feature reader containing all the features for the given
    /// feature class in this selection.
    ///
    virtual MgFeatureReader* GetSelectedFeatures(MgLayerBase* layer, CREFSTRING className, MgStringCollection* propertyNames);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Read selection from XML document.  Previous selection is cleared.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void FromXml(string xmlSelectionString);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void FromXml(String xmlSelectionString);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void FromXml(string xmlSelectionString);
    /// \htmlinclude SyntaxBottom.html
    ///
    void FromXml(CREFSTRING xmlSelectionString);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Write selection as XML document.  This document will typically
    /// be passed back to a viewer to select the desired features.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string ToXml();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String ToXml();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string ToXml();
    /// \htmlinclude SyntaxBottom.html
    ///
    STRING ToXml();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Add nFeatures to selection set from the FeatureReader.
    /// Specify nFeatures = 0 for all features in reader.
    /// Internal identity mapping for layer dictates the properties used
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void AddFeatures(MgLayer layer, MgFeatureReader features, int nFeatures);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void AddFeatures(MgLayer layer, MgFeatureReader features, int nFeatures);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void AddFeatures(MgLayer layer, MgFeatureReader features, int nFeatures);
    /// \htmlinclude SyntaxBottom.html
    ///
    void AddFeatures(MgLayerBase* layer, MgFeatureReader* features, INT32 nFeatures);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Add a single selection to the set based on a collection of identity properties
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void AddFeatureIds(MgLayer layer, string className, MgPropertyCollection identityProperties);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void AddFeatureIds(MgLayer layer, String className, MgPropertyCollection identityProperties);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void AddFeatureIds(MgLayer layer, string className, MgPropertyCollection identityProperties);
    /// \htmlinclude SyntaxBottom.html
    ///
    void AddFeatureIds(MgLayerBase* layer, CREFSTRING className, MgPropertyCollection* identityProperties);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Add a single selection to the set where the identity property is a 16-bit integer.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void AddFeatureIdInt16(MgLayer layer, string className, short identifier);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void AddFeatureIdInt16(MgLayer layer, String className, short identifier);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void AddFeatureIdInt16(MgLayer layer, string className, int identifier);
    /// \htmlinclude SyntaxBottom.html
    ///
    void AddFeatureIdInt16(MgLayerBase* layer, CREFSTRING className, INT16 identifier);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Add a single selection to the set where the identity property is a 32-bit integer.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void AddFeatureIdInt32(MgLayer layer, string className, int identifier);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void AddFeatureIdInt32(MgLayer layer, String className, int identifier);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void AddFeatureIdInt32(MgLayer layer, string className, int identifier);
    /// \htmlinclude SyntaxBottom.html
    ///
    void AddFeatureIdInt32(MgLayerBase* layer, CREFSTRING className, INT32 identifier);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Add a single selection to the set where the identity property is a 64-bit integer.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void AddFeatureIdInt64(MgLayer layer, string className, long identifier);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void AddFeatureIdInt64(MgLayer layer, String className, long identifier);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void AddFeatureIdInt64(MgLayer layer, string className, string identifier);
    /// \htmlinclude SyntaxBottom.html
    ///
    void AddFeatureIdInt64(MgLayerBase* layer, CREFSTRING className, INT64 identifier);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Add a single selection to the set where the identity property is a string.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void AddFeatureIdString(MgLayer layer, string className, string identifier);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void AddFeatureIdString(MgLayer layer, String className, String identifier);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void AddFeatureIdString(MgLayer layer, string className, string identifier);
    /// \htmlinclude SyntaxBottom.html
    ///
    void AddFeatureIdString(MgLayerBase* layer, CREFSTRING className, CREFSTRING identifier);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Add a single selection to the set where the identity property is a double.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void AddFeatureIdDouble(MgLayer layer, string className, double identifier);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void AddFeatureIdDouble(MgLayer layer, String className, double identifier);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void AddFeatureIdDouble(MgLayer layer, string className, double identifier);
    /// \htmlinclude SyntaxBottom.html
    ///
    void AddFeatureIdDouble(MgLayerBase* layer, CREFSTRING className, double identifier);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns list of selected layers
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgReadOnlyLayerCollection GetLayers();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgReadOnlyLayerCollection GetLayers();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgReadOnlyLayerCollection GetLayers();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgReadOnlyLayerCollection* GetLayers();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns list of selected classes for a layer
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgStringCollection GetClasses(string layer);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgStringCollection GetClasses(String layer);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgStringCollection GetClasses(string layer);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \todo
    /// This API should be made internal
    MgStringCollection* GetClasses(CREFSTRING layer);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the class name for the layer
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetClass(string layer);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetClass(String layer);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetClass(string layer);
    /// \htmlinclude SyntaxBottom.html
    ///
    STRING GetClass(CREFSTRING layer);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Does a particular layer and class have selections.
    ///
    /// \remarks
    /// The class name can be obtained from the layer with MgLayer::GetFeatureClassName.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Contains(MgLayer layer, string className);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(MgLayer layer, String className);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(MgLayer layer, string className);
    /// \htmlinclude SyntaxBottom.html
    ///
    bool Contains(MgLayerBase* layer, CREFSTRING className);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates an Fdo filter text for the selections of the
    /// specified layer and class
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GenerateFilter(MgLayer layer, string className);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GenerateFilter(MgLayer layer, String className);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GenerateFilter(MgLayer layer, string className);
    /// \htmlinclude SyntaxBottom.html
    ///
    STRING GenerateFilter(MgLayerBase* layer, CREFSTRING className);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the extents of the selection set.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgEnvelope GetExtents(MgFeatureService featureService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgEnvelope GetExtents(MgFeatureService featureService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgEnvelope GetExtents(MgFeatureService featureService);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param featureService (MgFeatureService)
    /// MgFeatureService that can be used to retrieve the extents
    /// for each feature class definition
    ///
    /// \return
    /// Returns the MgEnvelope object with extent of selection
    ///
    MgEnvelope* GetExtents(MgFeatureService* featureService);

    ////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the given feature (represented by its base64 encoded id)
    /// to the selection set
    ///
    /// \param layer (CREFSTRING)
    /// Name of layer
    /// \param className (CREFSTRING)
    /// Name of feature class
    /// \param id (CREFSTRING)
    /// Base 64 string encoded ID of the feature
    ///
    void Add(CREFSTRING layer, CREFSTRING className, CREFSTRING id);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Loads the map object from a session repository.
    ///
    /// \param resourceService (MgResourceService)
    /// An MgResourceService that can be used to retrieve
    /// the map.
    /// \param mapName (String/string)
    /// A string that specifies the name of the map. This
    /// is the name that was specified when \link MgMapBase::Create Create \endlink
    /// was called to create the map object.
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
    /// \return
    /// Returns nothing.
    ///
    virtual void Open(MgResourceService* resourceService, CREFSTRING mapName);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Save the map object to a session repository.
    ///
    /// \param resourceService (MgResourceService)
    /// An MgResourceService that can be used to retrieve
    /// the map.
    /// \param mapName (String/string)
    /// A string that specifies the name of the map. This
    /// is the name that was specified when \link MgMapBase::Create Create \endlink
    /// was called to create the map object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Save(MgResourceService resourceService, string mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Save(MgResourceService resourceService, String mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Save(MgResourceService resourceService, string mapName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void Save(MgResourceService* resourceService, CREFSTRING mapName);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Builds a resource name based on the map name amd the session id
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// String GetResourceName(string sessionId, string mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetResourceName(String sessionId, String mapName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// String GetResourceName(string sessionId, string mapName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param sessionId (String/string)
    /// A string that specifies the session id
    /// \param mapName (String/string)
    /// A string that specifies the name of the map. This
    /// is the name that was specified when \link MgMapBase::Create Create \endlink
    /// was called to create the map object.
    ///
    /// \return
    /// Resource name.
    ///
    STRING GetResourceName(CREFSTRING sessionId, CREFSTRING mapName);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Generate a collection of FDO filter strings for the selections
    /// of the specified layer and class.
    ///
    /// \return
    /// Collection of FDO filter strings.
    ///
    MgStringCollection* GenerateFilters(MgLayerBase* layer, CREFSTRING className, INT32 selectionSize);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the name of object
    ///
    /// \return
    /// Returns the name of object
    ///
    virtual STRING GetName();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Does it allow to set the name
    ///
    /// \return
    /// true - allows to set the name
    /// false - can not set the name
    ///
    virtual bool CanSetName();

    /////////////////////////////////////////
    /// Set the map object. To be used ONLY for deserialization
    //
    void SetMap(MgMapBase* map);

    /////////////////////////////////////////
    /// Write selection as XML document
    //
    STRING ToXml(bool withXmlDecl);

    /////////////////////////////////////////
    /// Dummy constructor for Ptr<> template
    //
    MgSelectionBase();

    /////////////////////////////////////////
    /// Destructor
    //
    virtual ~MgSelectionBase();

protected:

    void Dispose();

    /////////////////////////////////////////////////////////////////
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

    virtual const char* GetResourceTypeName();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_MapLayer_SelectionBase;

protected:

    typedef std::list<wstring> SelectionList;
    typedef std::map<wstring, SelectionList*> SelectedClassMap;
    typedef std::map<wstring,SelectedClassMap*> SelectionMap;

    MgEnvelope* GetFeatureExtents(  MgFeatureService* featureService,
                                    CREFSTRING resId,
                                    CREFSTRING clsName,
                                    CREFSTRING filterText,
                                    CREFSTRING geomName);

    void Add(wstring& layer, wstring& className, SelectionList* idList);
    SelectionList* GetSelections(MgLayerBase* layer, CREFSTRING className);

    SelectionMap m_selections;
    Ptr<MgMapBase> m_map;
    MgMemoryStreamHelper* m_stream;
};
/// \}

#endif
