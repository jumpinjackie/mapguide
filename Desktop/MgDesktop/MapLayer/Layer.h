#ifndef DESKTOP_MAP_LAYER_H
#define DESKTOP_MAP_LAYER_H

#include "MgDesktop.h"

class MgdScrollableFeatureReader;

/// \ingroup Desktop_Map_and_Layers_Module
/// \{
class MG_DESKTOP_API MgdLayer : public MgLayerBase
{
	MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgdLayer)

PUBLISHED_API:
    MgdLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService);

    virtual MgClassDefinition* GetClassDefinition();

    virtual MgFeatureReader* SelectFeatures(MgFeatureQueryOptions* options);

    virtual MgDataReader* SelectAggregate(MgFeatureAggregateOptions* options);

    virtual MgPropertyCollection* UpdateFeatures(MgFeatureCommandCollection* commands);

    virtual MgTransaction* BeginTransaction();

    virtual MgdScrollableFeatureReader* SelectFeaturesExtended(MgFeatureQueryOptions* options);

    virtual MgFeatureReader* InsertFeatures(MgPropertyCollection* propertyValues);

	virtual int UpdateFeatures(MgPropertyCollection* propertyValues, CREFSTRING filter);

	virtual int DeleteFeatures(CREFSTRING filter);

    virtual MgFeatureReader* InsertFeatures(MgPropertyCollection* propertyValues, MgTransaction* trans);

	virtual int UpdateFeatures(MgPropertyCollection* propertyValues, CREFSTRING filter, MgTransaction* trans);

	virtual int DeleteFeatures(CREFSTRING filter, MgTransaction* trans);

    virtual bool IsPotentiallyVisibleAtScale(double scale, bool bConsiderParentGroupVisibility);

    virtual bool HasTooltips();

	virtual MgSpatialContextReader* GetSpatialContexts(bool active);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an empty MgLayer object
    ///
    MgdLayer();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgLayer object and pulls identity properties if required
    ///
    MgdLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService, bool initIdProps);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgLayerBase object from a layer definition, pulls identity properties if required, and init layer definition if required.
    ///
    MgdLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService, bool initIdProps, bool initLayerDefinition);

    //////////////////////////////////////////////////////////////////
    /// Parse the layer definition XML and extracts scale ranges,
    /// feature source and feature class from it
    void GetLayerInfoFromDefinition(MgResourceService* resourceService);

    //////////////////////////////////////////////////////////////////
    /// Populate identity properties using a class definition collection
    void PopulateIdentityProperties(MgClassDefinition* classDef);

    ////////////////////////////////////////////////////////////////////
    /// Parse feature name to populate schema name and return class name
    void ParseFeatureName(MgFeatureService* featureService, REFSTRING className, REFSTRING schemaName);

    virtual void Serialize(MgStream* stream);

    virtual void Deserialize(MgStream* stream);

protected:
    virtual ~MgdLayer();

    virtual void Dispose()
    {
        delete this;
    }

private:

    MgMapBase* GetMap();

    bool m_initIdProps;

protected:
    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_MapLayer_Layer;
};
/// \}
#endif