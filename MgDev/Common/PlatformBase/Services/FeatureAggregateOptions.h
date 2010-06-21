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

#ifndef _MG_FEATURE_AGGREGATE_OPTIONS_H_
#define _MG_FEATURE_AGGREGATE_OPTIONS_H_

/// \defgroup MgFeatureAggregateOptions MgFeatureAggregateOptions
/// \ingroup Feature_Service_classes
/// \{

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Use this class to set the filter criteria used to select
/// groups of features from a datastore or for restricting the
/// values returned to be unique.
///
/// \remarks
/// This is a subclass of
/// MgFeatureQueryOptions. Read the topic \link FiltersAndExpressions filters and expressions \endlink
/// for a discussion of all of the basic filter capabilities. An
/// instance of this class is an argument to
/// MgFeatureService::SelectAggregate().
///
class MG_PLATFORMBASE_API MgFeatureAggregateOptions : public MgFeatureQueryOptions
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgFeatureAggregateOptions)

PUBLISHED_API:
    ///////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgFeatureAggregateOptions object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgFeatureAggregateOptions();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgFeatureAggregateOptions();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgFeatureAggregateOptions();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgFeatureAggregateOptions();

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Use this method to restrict the values returned by a select
    /// operation to be unique.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SelectDistinct(bool yes);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SelectDistinct(boolean yes);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SelectDistinct(bool yes);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param yes (boolean/bool)
    /// If true, apply the distinct operation; otherwise
    /// do not apply the distinct operation; the latter
    /// is the default.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// GroupName is a string property. Feature instances in the
    /// datastore have a GroupName property value of either 'Group1',
    /// 'Group2', or 'Group3'. To verify this fact, use the following
    /// select operation.
    /// \code
    /// <?php
    /// $aggregateOptions->SelectDistinct(true);
    /// $aggregateOptions->AddFeatureProperty("GroupName");
    /// $dataReader = $featureService->SelectAggregate($featureSrcResId, $className, $aggregateOptions);
    /// $dataReader->Close();
    /// ?>
    /// sqlplus> select distinct groupname from featclass;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// The feature schema contains a string property whose name is "GroupName".
    /// The value of this property for some features is "Group Name One" and for the rest "Group Name Two".
    /// The select aggregate operation with SelectDistinct set to true will select one feature from each group.
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgFeatureAggregateOptions queryOptions;
    /// private MgFeatureService featureService;
    /// private String className = "SdfFeatureClass";
    /// // the SDF file identified by this MgResourceIdentifier exists in the repository
    /// private MgResourceIdentifier resourceId;
    /// private MgDataReader dataReader;
    ///
    /// resourceId = new MgResourceIdentifier("Library://PlatformApiDocTests/SdfFeatureClass.FeatureSource");
    /// queryOptions = new MgFeatureAggregateOptions();
    /// queryOptions.AddFeatureProperty("GroupName");
    /// queryOptions.SelectDistinct(true);
    /// dataReader = featureService.SelectAggregate(resourceId, className, queryOptions);
    /// dataReader.Close();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    void SelectDistinct(bool yes);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Modifies a select operation with a grouping filter so that
    /// the results are grouped.
    ///
    /// \remarks
    /// For example, select the smallest value from a numerical
    /// property for each group of feature objects specified in the
    /// grouping filter expression. An instance of this example in
    /// SQL syntax is "select min(aDouble) as minD from featclass
    /// group by groupname having groupname = 'Group1' or groupname =
    /// 'Group2' or groupname = 'Group3';".
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetGroupingFilter(MgStringCollection groupByProperties, string groupFilter);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetGroupingFilter(MgStringCollection groupByProperties, String groupFilter);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetGroupingFilter(MgStringCollection groupByProperties, string groupFilter);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param groupByProperties (MgStringCollection)
    /// The collection of properties to be
    /// used in the grouping filter specified
    /// in the second parameter.
    /// \param groupFilter (String/string)
    /// The grouping filter expression.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// GroupName is a string property, and aDouble is a double
    /// property. Three values are returned by this query. Each of
    /// them is the smallest aDouble property value in one of the
    /// groups of features selected by the grouping filter.
    /// \code
    /// <?php
    /// $stringCollection->Add("GroupName");
    /// $groupingFilter = "GroupName = 'Group1' or GroupName = 'Group2' or GroupName = 'Group3'";
    /// $aggregateOptions->SetGroupingFilter($stringCollection, $groupingFilter);
    /// $aggregateOptions->SetComputedProperty("minD", "min(aDouble)");
    /// $dataReader = $featureService->SelectAggregate($featureSrcResId, $featClassName, $aggregateOptions);
    /// ?>
    /// sqlplus> select min(aDouble) as minD from featclass group by groupname having groupname = 'Group1' or groupname = 'Group2' or groupname = 'Group3';
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidArgumentException
    ///
    void SetGroupingFilter(MgStringCollection* groupByProperties, CREFSTRING groupFilter);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor
    ///
    ~MgFeatureAggregateOptions();
    MgStringCollection* GetGroupingProperties();
    STRING GetGroupFilter();
    bool GetDistinct();

    virtual void Serialize(MgStream* stream);
    virtual void Deserialize(MgStream* stream);

    virtual INT32 GetClassId() { return m_cls_id; }
    virtual void Dispose() { delete this; }

private:

    bool m_selectDistinct;                            // Apply Distinct operator
    Ptr<MgStringCollection> m_groupByProperties;      // Group By Clause
    STRING m_groupFilter;                             // Having Clause

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_FeatureAggregateOptions;
};
/// \}

#endif
