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

#ifndef _MG_SELECTION_H_
#define _MG_SELECTION_H_

/// \defgroup MgSelection MgSelection
/// \ingroup Maps_and_Layers_Module
/// \{

class MG_MAPGUIDE_API MgMemoryStreamHelper;

/// Exposed API class for handling selection
/// Represents a set of selections for multiple layers and classes
class MG_MAPGUIDE_API MgSelection : public MgSelectionBase
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgSelection)

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
    MgSelection(MgMap* map);

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
    MgSelection(MgMap* map, CREFSTRING xmlSelectionString);

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
    /// \note2
    MgFeatureReader* GetSelectedFeatures(MgLayerBase* layer, CREFSTRING className, bool mappedOnly);

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
    /// \note2
    MgFeatureReader* GetSelectedFeatures(MgLayerBase* layer, CREFSTRING className, MgStringCollection* propertyNames);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Loads the selection object from a session repository.
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
    /// Save the selection object to a session repository.
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
    void Save(MgResourceService* resourceService, CREFSTRING mapName);


    /////////////////////////////////////////
    /// Dummy constructor for Ptr<> template
    //
    MgSelection();


    /////////////////////////////////////////
    /// Destructor
    //
    virtual ~MgSelection();

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

    STRING GetResourceName(CREFSTRING sessionId, CREFSTRING mapName);

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_MapLayer_Selection;

};
/// \}

#endif
