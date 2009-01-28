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

#ifndef MGFEATUREQUERY_OPTIONS_H_
#define MGFEATUREQUERY_OPTIONS_H_

/// \defgroup MgFeatureQueryOptions MgFeatureQueryOptions
/// \ingroup Feature_Service_classes
/// \{

class MgStream;
class MgGeometry;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Use this class to set filter criteria for selecting features
/// from a datastore. Read the discussion of \link FiltersAndExpressions filters and expressions \endlink.
/// An instance of this class is an argument to
/// MgFeatureService::SelectFeatures(). This is the base class
/// for MgFeatureAggregateOptions.
///
class MG_PLATFORMBASE_API MgFeatureQueryOptions : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgFeatureQueryOptions)

PUBLISHED_API:
    ///////////////////////////////////////////////
    /// \brief
    /// Constructs an MgFeatureQueryOptions object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgFeatureQueryOptions();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgFeatureQueryOptions();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgFeatureQueryOptions();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgFeatureQueryOptions();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Call this method for each class property whose value you want
    /// retrieved by the MgFeatureService::SelectFeatures() call. Not
    /// calling this method results in the retrieval of all property
    /// values for each feature selected.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int AddFeatureProperty(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int AddFeatureProperty(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int AddFeatureProperty(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The name of the class property to
    /// be added.
    ///
    /// \return
    /// Returns the total number of properties added.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// <?php
    /// $queryOptions->AddFeatureProperty("aDouble");
    /// $queryOptions->AddFeatureProperty("aSingle");
    /// $featureService->SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
    /// ?>
    /// sqlplus> select aDouble,aSingle from featclass;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidArgumentException if property name is empty
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgFeatureQueryOptions queryOptions;
    /// private MgFeatureService featureService;
    /// private String className = "SdfFeatureClass";
    /// // the SDF file identified by this MgResourceIdentifier exists in the repository
    /// private MgResourceIdentifier resourceId;
    /// private MgFeatureReader featureReader;
    ///
    /// resourceId = new MgResourceIdentifier("Library://PlatformApiDocTests/SdfFeatureClass.FeatureSource");
    /// queryOptions = new MgFeatureQueryOptions();
    /// queryOptions.AddFeatureProperty("FeatId");
    /// queryOptions.AddFeatureProperty("anInt16");
    /// featureReader = featureService.SelectFeatures(resourceId, className, queryOptions);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    INT32 AddFeatureProperty(CREFSTRING propertyName);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Define an alias for a computed property and an expression for
    /// what is to be computed. In this SQL example, "select
    /// min(aDouble + aSingle) as minF, min(anInt16 + anInt16) as
    /// minI from featclass;", minF and minI are computed properties.
    /// Call this method for each computed property that you add.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int AddComputedProperty(string aliasName, string expression);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int AddComputedProperty(String aliasName, String expression);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int AddComputedProperty(string aliasName, string expression);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param aliasName (String/string)
    /// The name of the computed property.
    /// \param expression (String/string)
    /// The expression to be associated with
    /// the name of the computed property.
    ///
    /// \return
    /// Returns the total number of properties added.
    ///
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// <?php
    /// $queryOptions->AddComputedProperty("minF", "aDouble + aSingle");
    /// $queryOptions->AddComputedProperty("minI", "anInt64 + anInt16");
    /// $featureService->SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
    /// $featureService->SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
    /// \endcode
    /// \code
    /// ?>
    /// sqlplus> select min(aDouble + aSingle) as minF, min(anInt16 + anInt16) as minI from featclass;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgFeatureQueryOptions queryOptions;
    /// private MgFeatureService featureService;
    /// private String className = "SdfFeatureClass";
    /// // the SDF file identified by this MgResourceIdentifier exists in the repository
    /// private MgResourceIdentifier resourceId;
    /// private MgFeatureReader featureReader;
    ///
    /// resourceId = new MgResourceIdentifier("Library://PlatformApiDocTests/SdfFeatureClass.FeatureSource");
    /// queryOptions = new MgFeatureQueryOptions();
    /// queryOptions.AddFeatureProperty("FeatId");
    /// queryOptions.AddFeatureProperty("aDouble");
    /// queryOptions.AddComputedProperty("ceilADouble", "Ceil(aDouble)");
    /// featureReader = featureService.SelectFeatures(resourceId, className, queryOptions);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidArgumentException if aliasName or expression is empty
    ///
    INT32 AddComputedProperty(CREFSTRING aliasName, CREFSTRING expression);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the filter for this select operation. The filter acts
    /// like the where clause in an SQL Select statement. Not calling
    /// this method causes all instances of the feature class to be
    /// selected. For more details on defining filters, see the \link FiltersAndExpressions Filter and Expression \endlink
    /// documentation.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetFilter(string filterText);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetFilter(String filterText);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetFilter(string filterText);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param filterText (String/string)
    /// The filter text to be applied.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// <?php
    /// $queryOption->SetFilter("featId > 20");
    /// $featureService->SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
    /// ?>
    /// sqlplus> select * from featclass where featId > 20;
    ///
    /// <?php
    /// $featureService->SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
    /// ?>
    /// sqlplus> select * from featclass;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidArgumentException
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgFeatureQueryOptions queryOptions;
    /// private MgFeatureService featureService;
    /// private String className = "SdfFeatureClass";
    /// // the SDF file identified by this MgResourceIdentifier exists in the repository
    /// private MgResourceIdentifier resourceId;
    /// private MgFeatureReader featureReader;
    ///
    /// resourceId = new MgResourceIdentifier("Library://PlatformApiDocTests/SdfFeatureClass.FeatureSource");
    /// queryOptions = new MgFeatureQueryOptions();
    /// queryOptions.AddFeatureProperty("FeatId");
    /// queryOptions.AddFeatureProperty("anInt32");
    /// queryOptions.SetFilter("anInt32 + 1 < 2");
    /// featureReader = featureService.SelectFeatures(resourceId, className, queryOptions);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    void SetFilter(CREFSTRING filterText);

    //////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Use this method to join a regular filter with a spatial
    /// filter An argument of TRUE results in an AND join, and an
    /// argument of FALSE results in an OR join. The binary operator
    /// applies only when both the SetFilter() and SetSpatialFilter()
    /// methods have been called. If both have been called and
    /// SetBinaryOperator() has not been called, an AND join is
    /// applied.
    ///
    /// \remarks
    /// The default is an AND join.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetBinaryOperator(bool andOr);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetBinaryOperator(boolean andOr);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetBinaryOperator(bool andOr);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param andOr (boolean/bool)
    /// If true, apply an AND join; otherwise
    /// apply an OR join.
    ///
    /// \return
    /// Nothing
    ///
    void SetBinaryOperator(bool andOr);

    ///////////////////////////////////////////////////////////
    /// \brief
    /// Use this method to select features whose geometry property
    /// satisfies the relationship argument to the geometry argument.
    /// The effect is geometryProperty.spatialOperation(geometryArg).
    /// By default this filter is joined by the AND operator with the
    /// value of the regular filter (see \link MgFeatureQueryOptions::SetFilter MgFeatureQueryOptions::SetFilter Method \endlink).
    /// This filter can also be joined by the OR operator (see \link MgFeatureQueryOptions::SetBinaryOperator MgFeatureQueryOptions::SetBinaryOperator Method \endlink).
    /// The syntax for including a spatial expression in a regular
    /// filter is described in the topic on \link FiltersAndExpressions filters and expressions \endlink.
    ///
    /// \remarks
    /// The XML returned by MgFeatureService::GetCapabilities says
    /// whether a provider supports spatial conditions. Even if it
    /// supports spatial conditions, the provider may not support all
    /// spatial operations. See \link ProviderCapabilities Provider Capabilities \endlink.
    /// \n
    /// For example, let's call this method with the arguments
    /// "geometry" (the name of the geometry property),
    /// "MgFeatureSpatialOperations::Touches (the spatial operation)
    /// and the geometry POLYGON((0 2, 5 2, 5 3, 0 3, 0 2)) after
    /// conversion into an MgGeometry object. Let's assume that the
    /// MgFeatureQueryOptions::SetBinaryOperator method is not called
    /// and the value of the MgFeatureQueryOptions::SetFilter()
    /// method argument is "featid > 0". Let's also assume that the
    /// MgFeatureQueryOptions::AddFeatureProperty() method has been
    /// called twice, one with an argument of "featid" and once with
    /// an argument "geometry". We further assume that the
    /// MgFeatureService::SelectFeatures() method is called with the
    /// classname argument set to "featclass". This is equivalent to
    /// doing this select statement: "SELECT a.featid,a.geometry FROM
    /// featclass a WHERE (a.featid > 0) AND sdo_relate(a.geometry,
    /// MDSYS.SDO_GEOMETRY(2003, NULL, NULL,
    /// MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1),
    /// MDSYS.SDO_ORDINATE_ARRAY(0,2, 5,2, 5,3, 0,3, 0,2)),
    /// 'mask=touch) = 'TRUE';"
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetSpatialFilter(string geometryProperty, MgGeometry geometry, int spatialOperation);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetSpatialFilter(String geometryProperty, MgGeometry geometry, int spatialOperation);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetSpatialFilter(string geometryProperty, MgGeometry geometry, int spatialOperation);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param geometryProperty (String/string)
    /// The name of the geometry property belonging the
    /// features selected from the datastore
    /// \param geometry (MgGeometry)
    /// The geometry object set in relation to the
    /// geometry property.
    /// \param spatialOperation (int)
    /// The spatial operation to perform between the
    /// lefthand operand (the geometry property) and
    /// the righthand operand (the geometry object).
    /// The spatial operation value of is one of the
    /// MgFeatureSpatialOperations types.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgInvalidArgumentException
    ///
    void SetSpatialFilter(CREFSTRING geometryProperty, MgGeometry* geometry,
                          INT32 spatialOperation);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Use this method to specify the order (ascending or
    /// descending) in which the selected features are returned.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetOrderingFilter(MgStringCollection orderByProperties, int orderOption);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetOrderingFilter(MgStringCollection orderByProperties, int orderOption);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetOrderingFilter(MgStringCollection orderByProperties, int orderOption);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param orderByProperties (MgStringCollection)
    /// The collection of properties to which the
    /// ordering option specified in the second
    /// parameter is applied.
    /// \param orderOption (int)
    /// The ordering option,
    /// MgOrderingOption::Ascending or
    /// MgOrderingOption::Descending.
    ///
    /// \return
    /// Nothing
    ///
    /// \htmlinclude PHPExampleTop.html
    /// aDouble is a double property. anInt32Key is the identity
    /// property. The first example returns aDouble values in
    /// ascending order, and the second example returns them in
    /// descending order.
    /// \code
    /// <?php
    /// $queryOptions->AddFeatureProperty("aDouble");
    /// $queryOptions->AddFeatureProperty("anInt32Key");
    /// $stringCollection->Add("aDouble");
    /// $queryOptions->SetOrderingFilter($stringCollection, MgOrderingOption::Ascending);
    /// $featureService->SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
    /// $queryOptions->SetOrderingFilter($stringCollection, MgOrderingOption::Descending);
    /// $featureService->SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
    /// ?>
    /// sqlplus> select anint32key,adouble from universalclassxy order by adouble ASC;
    /// sqlplus> select anint32key,adouble from universalclassxy order by adouble DESC;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidArgumentException
    ///
    void SetOrderingFilter(MgStringCollection* orderByProperties, INT32 orderOption);

    //////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes a class property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void RemoveFeatureProperty(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void RemoveFeatureProperty(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void RemoveFeatureProperty(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The name of the class property to
    /// be removed.
    ///
    /// \return
    /// Returns the total number of properties remaining.
    ///
    /// \exception MgInvalidArgumentException
    ///  if the property name is empty.
    ///
    void RemoveFeatureProperty(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes a computed property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void RemoveComputedProperty(string aliasName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void RemoveComputedProperty(String aliasName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void RemoveComputedProperty(string aliasName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param aliasName (String/string)
    /// The name of the computed property to
    /// be removed.
    ///
    /// \return
    /// Returns the total number of properties remaining.
    ///
    /// \exception MgInvalidArgumentException
    ///  if the property name is empty.
    ///
    void RemoveComputedProperty(CREFSTRING aliasName);

INTERNAL_API:

    virtual ~MgFeatureQueryOptions();

    MgStringCollection* GetClassProperties();
    MgStringPropertyCollection* GetComputedProperties();
    MgStringCollection* GetOrderingProperties();

    STRING GetFilter();
    bool GetBinaryOperator();

    STRING GetGeometryProperty();
    MgGeometry* GetGeometry();

    INT32 GetOrderOption();
    INT32 GetSpatialOperation();

    virtual void Serialize(MgStream* stream);
    virtual void Deserialize(MgStream* stream);
    STRING GetLogString();

    virtual INT32 GetClassId() { return m_cls_id; }
    virtual void Dispose() { delete this; }

    void ValidateEmptyArgument(CREFSTRING value);

private:

    STRING m_filterText;                                  // Where Clause
    Ptr<MgStringCollection> m_classProperties;            // Actual class Properties
    Ptr<MgStringPropertyCollection> m_computedProperties; // MIN(x) AS MINX property

    Ptr<MgStringCollection> m_orderByProperties;        // Order By Clause
    INT32 m_orderOption;                                // Ordering options (Ascending/Descending)

    STRING m_geometryProperty;                          // Property on which Geometric operation to be applied
    INT32 m_operation;                                  // Geometric operation to be applied
    Ptr<MgGeometry> m_geometry;                         // Actual geometry like Circle; Rectangle etc
    bool m_binaryOp;                                    // Binary operator between spatial and regular filter

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_FeatureQueryOptions;
};
/// \}

#endif
