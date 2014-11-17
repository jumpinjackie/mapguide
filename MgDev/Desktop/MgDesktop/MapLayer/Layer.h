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

    virtual MgPropertyCollection* UpdateFeatures(MgFeatureCommandCollection* commands, MgTransaction* trans);

    virtual MgTransaction* BeginTransaction();

    virtual MgdScrollableFeatureReader* SelectFeaturesExtended(MgFeatureQueryOptions* options);

    virtual MgFeatureReader* InsertFeatures(MgPropertyCollection* propertyValues);

    virtual INT32 UpdateMatchingFeatures(MgPropertyCollection* propertyValues, CREFSTRING filter);

    virtual INT32 DeleteFeatures(CREFSTRING filter);

    virtual MgFeatureReader* InsertFeatures(MgPropertyCollection* propertyValues, MgTransaction* trans);

    virtual INT32 UpdateMatchingFeatures(MgPropertyCollection* propertyValues, CREFSTRING filter, MgTransaction* trans);

    virtual INT32 DeleteFeatures(CREFSTRING filter, MgTransaction* trans);

    virtual bool IsPotentiallyVisibleAtScale(double scale, bool bConsiderParentGroupVisibility);

    virtual bool HasTooltips();

    virtual MgSpatialContextReader* GetSpatialContexts(bool active);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the list of geometry type styles for this layer at the map's current scale. Returns NULL if there are no applicable geometry types
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \remarks
    /// The map's current scale is used to determine what scale range in the layer definition to search for.
    /// For a scale range with multiple composite styles, multiple instances of (4 = composite) will be in the resulting collection
    ///
    /// \return
    /// The list of geometry type styles for this layer at the map's current scale. Returns NULL if there are no applicable geometry types
    ///
    /// \since 3.0
    MgIntCollection* GetGeometryTypeStyles();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of theme categories for this layer at the map's current scale for the given geometry type style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetThemeCategoryCount(int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetThemeCategoryCount(int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetThemeCategoryCount(int geomType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param geomType (int)
    /// The geometry type
    ///
    /// \remarks
    /// The map's current scale is used to determine what scale range in the layer definition to search for.
    /// When geomType = 4, it will only count the number of the theme categories for the first composite style it finds. For a scale range
    /// with multiple composite type styles, you should use GetCompositeThemeCategoryCount() instead
    ///
    /// \return
    /// The number of theme categories for this layer at the map's current scale for the given geometry type style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// \since 3.0
    INT32 GetThemeCategoryCount(INT32 geomType);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of composite theme categories for this layer at the map's current scale for the given composite style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param compositeOffset (int)
    /// The zero-based index denoting the particular composite style to count from. 0 = 1st composite style, 1 = 2nd composite style
    ///
    /// \return
    /// The number of theme categories for this layer at the map's current scale for the given composite style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// \since 3.0
    INT32 GetCompositeThemeCategoryCount(INT32 compositeOffset);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the legend image for the specified geometry type and theme category
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(int width, int height, string format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(int width, int height, String format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(int width, int height, string format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Input
    /// MgResourceIdentifier object identifying the layer definition resource.
    /// \param scale (double)
    /// Input
    /// The scale at which the symbolization is requested.
    /// \param width (int)
    /// Input
    /// The requested image width in pixels.
    /// \param height (int)
    /// Input
    /// The requested image height in pixels.
    /// \param format (String/string)
    /// Input
    /// Image format, from MgImageFormats. Example: PNG, JPG, PNG8, etc 
    /// \param geomType (int)
    /// Input
    /// The type of symbolization required: 1=Point, 2=Line, 3=Area, 4=Composite
    /// \param themeCategory (int)
    /// Input
    /// The value indicating which theme category swatch to return.
    /// Used when there is a theme defined at this scale. An exception will be
    /// thrown if a requested them category doesn't exist.
    ///
    /// \remarks
    /// The map's current scale is used to determine what scale range in the layer definition to search for
    ///
    /// \return
    /// Returns a stream representing the legend image.
    /// The default returned image format will be PNG8 unless a different supported
    /// format is requested. An exception will be thrown if an unsupported image
    /// format is requested.
    ///
    /// \exception MgArgumentOutOfRangeException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgNullArgumentException
    /// \exception MgInvalidImageFormatException
    ///
    /// \since 3.0
    MgByteReader* GenerateLegendImage(INT32 width, INT32 height, CREFSTRING format, INT32 geomType, INT32 themeCategory);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the list of geometry type styles for this layer at the map's current scale. Returns NULL if there are no applicable geometry types
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles(double scale);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles(double scale);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles(double scale);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param scale (double)
    /// The scale at which to retrive the list of applicable geometry types
    ///
    /// \remarks
    /// For a scale range with multiple composite styles, multiple instances of (4 = composite) will be in the resulting collection
    ///
    /// \return
    /// The list of geometry type styles for this layer at the map's current scale. Returns NULL if there are no applicable geometry types
    ///
    /// \since 3.0
    MgIntCollection* GetGeometryTypeStyles(double scale);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of theme categories for this layer at the map's current scale for the given geometry type style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param scale (double)
    /// The scale at which to count the number of applicable theme categories
    /// \param geomType (int)
    /// The geometry type
    ///
    /// \remarks
    /// When geomType = 4, it will only count the number of the theme categories for the first composite style it finds. For a scale range
    /// with multiple composite type styles, you should use GetCompositeThemeCategoryCount() instead
    ///
    /// \return
    /// The number of theme categories for this layer at the map's current scale for the given geometry type style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// \since 3.0
    INT32 GetThemeCategoryCount(double scale, INT32 geomType);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of composite theme categories for this layer at the map's current scale for the given composite style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param scale (double)
    /// The scale at which to count the number of applicable theme categories
    /// \param compositeOffset (int)
    /// The zero-based index denoting the particular composite style to count from. 0 = 1st composite style, 1 = 2nd composite style
    ///
    /// \return
    /// The number of theme categories for this layer at the map's current scale for the given composite style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// \since 3.0
    INT32 GetCompositeThemeCategoryCount(double scale, INT32 compositeOffset);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the legend image for the specified geometry type and theme category
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(double scale, int width, int height, string format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(double scale, int width, int height, String format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(double scale, int width, int height, string format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Input
    /// MgResourceIdentifier object identifying the layer definition resource.
    /// \param scale (double)
    /// Input
    /// The scale at which the symbolization is requested.
    /// \param width (int)
    /// Input
    /// The requested image width in pixels.
    /// \param height (int)
    /// Input
    /// The requested image height in pixels.
    /// \param format (String/string)
    /// Input
    /// Image format, from MgImageFormats. Example: PNG, JPG, PNG8, etc 
    /// \param geomType (int)
    /// Input
    /// The type of symbolization required: 1=Point, 2=Line, 3=Area, 4=Composite
    /// \param themeCategory (int)
    /// Input
    /// The value indicating which theme category swatch to return.
    /// Used when there is a theme defined at this scale. An exception will be
    /// thrown if a requested them category doesn't exist.
    ///
    /// \return
    /// Returns a stream representing the legend image.
    /// The default returned image format will be PNG8 unless a different supported
    /// format is requested. An exception will be thrown if an unsupported image
    /// format is requested.
    ///
    /// \exception MgArgumentOutOfRangeException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgNullArgumentException
    /// \exception MgInvalidImageFormatException
    ///
    /// \since 3.0
    MgByteReader* GenerateLegendImage(double scale, INT32 width, INT32 height, CREFSTRING format, INT32 geomType, INT32 themeCategory);

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