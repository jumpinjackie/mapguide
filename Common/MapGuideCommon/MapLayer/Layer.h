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

#ifndef _MG_LAYER_H_
#define _MG_LAYER_H_

class MgLayer;
template class MG_MAPGUIDE_API Ptr<MgLayer>;

#ifdef _WIN32
#pragma warning(disable:4251)
#endif

/// \defgroup MgLayer MgLayer
/// \ingroup Maps_and_Layers_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Gets and sets properties of a layer.
/// \todo
class MG_MAPGUIDE_API MgLayer : public MgLayerBase
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgLayer)

PUBLISHED_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a MgLayer object from a layer definition.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayer(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayer(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayer(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param layerDefinition (MgResourceIdentifier)
    /// Identifier of the layer definition in a
    /// repository.
    /// \param resourceService (MgResourceService)
    /// An MgResourceService that can be used to
    /// retrieve the layer definition.
    ///
    /// \todo
    ///   * [[Job for Philip: I need a code sample here.]]
    ///
    MgLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class definition for the feature class of the layer.  If the
    /// feature class of the layer is extended with properties from other feature
    /// classes, then all those properties are also contained in the returned
    /// class definition. See \link Feature_Schema_Module Feature Schema \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgClassDefinition GetClassDefinition(string schemaName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgClassDefinition GetClassDefinition(String schemaName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgClassDefinition GetClassDefinition(string schemaName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgClassDefinition instance for the feature class of the layer.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    virtual MgClassDefinition* GetClassDefinition();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Selects features from a feature source according to the
    /// criteria set in the MgFeatureQueryOptions argument The
    /// criteria are applied to all of the features in the feature
    /// source. If you want to apply the criteria to a subset of the
    /// features, use the \link MgFeatureService::SelectAggregate MgFeatureService::SelectAggregate Method \endlink.
    /// See \link FiltersAndExpressions Filters and expressions \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgFeatureQueryOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgFeatureQueryOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgFeatureQueryOptions options);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param options (MgFeatureQueryOptions)
    /// MgFeatureQueryOptions instance
    /// containing all required filters for this
    /// select operation.
    ///
    /// \return
    /// Returns an MgFeatureReader containing the set of selected
    /// features.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    virtual MgFeatureReader* SelectFeatures(MgFeatureQueryOptions* options);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Selects groups of features from a feature source and applies
    /// filters to each of the groups according to the criteria set
    /// in the MgFeatureAggregateOptions argument. If you want to
    /// apply the criteria to all features without grouping them, use
    /// the \link MgFeatureService::SelectFeatures MgFeatureService::SelectFeatures Method \endlink.
    /// See \link FiltersAndExpressions Filters and expressions \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgDataReader SelectAggregate(MgFeatureAggregateOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgDataReader SelectAggregate(MgFeatureAggregateOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgDataReader SelectAggregate(MgFeatureAggregateOptions options);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param options (MgFeatureAggregateOptions)
    /// An MgFeatureAggregateOptions instance
    /// containing all the criteria and filters
    /// required for this select operation.
    ///
    /// \return
    /// Returns an MgDataReader containing the group values.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    virtual MgDataReader* SelectAggregate(MgFeatureAggregateOptions* options);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Executes the MgDeleteFeatures, MgInsertFeatures,
    /// MgUpdateFeatures, MgLockFeatures or MgUnlockFeatures commands
    /// contained in the given MgFeatureCommandCollection object.
    ///
    /// \remarks
    /// The XML returned by MgFeatureService::GetCapabilities says
    /// whether a provider supports SQL commands. See \link ProviderCapabilities Provider Capabilities \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgPropertyCollection UpdateFeatures(MgFeatureCommandCollection commands);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgPropertyCollection UpdateFeatures(MgFeatureCommandCollection commands);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgPropertyCollection UpdateFeatures(MgFeatureCommandCollection commands);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param commands (MgFeatureCommandCollection)
    /// A collection of feature commands to be
    /// executed.
    ///
    /// \return
    /// Returns an MgPropertyCollection object. Each property in the
    /// collection corresponds to a command in the
    /// MgFeatureCommandCollection argument. The property name is the
    /// index of the command in the feature command collection.
    /// <ul>
    ///   <li>If the command is of type MgDeleteFeatures, the property
    ///     type is an MgPropertyType::Int32, and its value is the number
    ///     of features deleted.</li>
    ///   <li>If the command is of type MgInsertFeatures, the property
    ///     type is an MgPropertyType::Feature, and its value is a
    ///     MgFeatureReader object. The feature reader object contains
    ///     the set of properties inserted into the datastore by the
    ///     insert command.</li>
    ///   <li>If the command is of type MgUpdateFeatures, the property
    ///     type is MgPropertyType::Int32, and its value is the number of
    ///     features updated.</li>
    ///   <li>If the command is of type MgLockFeatures, the property
    ///     type is MgPropertyType::Feature, and its value is the number
    ///     of features locked.</li>
    ///   <li>If the command is of type MgUnLockFeatures, the property
    ///     type is MgPropertyType::Int32, and its value is the number of
    ///     features unlocked.</li>
    /// </ul>
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidOperationException
    /// \exception MgFdoException
    ///
    virtual MgPropertyCollection* UpdateFeatures(MgFeatureCommandCollection* commands);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an empty MgLayer object
    ///
    MgLayer();

    //////////////////////////////////////////////////////////////////
    /// Parse the layer definition XML and extracts scale ranges,
    /// feature source and feature class from it
    void GetLayerInfoFromDefinition(MgResourceService* resourceService);

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

protected:
    /// \brief
    /// Destruct a MgLayer object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgLayer();

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
    virtual void Dispose()
    {
        delete this;
    }

private:

    MgMapBase* GetMap();

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_MapLayer_Layer;

};
/// \}

#endif
