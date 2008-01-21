/**
\addtogroup FiltersAndExpressions
<p>
  Use a filter to select a subset of the features in a
  datastore. Filters are used in calls to
  MgFeatureService::SelectFeatures() and
  MgFeatureService::SelectAggregate(). There are two types of
  filters: basic and spatial.
</p>
<h2> Spatial Filter </h2>
<p>
  A spatial filter relates two geometries by way of a spatial
  operator. A spatial filter is set by a call to
  MgFeatureQueryOptions::SetSpatialFilter() or
  MgFeatureAggregateOptions::SetSpatialFilter(). These methods
  take 3 arguments: a name which identifies a geometry property
  of a feature in a datastore, a geometry object, and a
  spatial operation identifier. The effect of the filter is to
  select features from the datastore whose geometry property is
  related according to the spatial operator to the geometry
  object argument. For example, if the spatial operator is
  MgFeatureSpatialOperations::Within, and the geometry object's
  WKT representation is POLYGON((0 0, 2 0, 2 2, 0 2, 0 0)),
  then the effect of the filter is to select those features
  which have geometries within this polygon.
</p>
<h2> Basic Filter </h2>
<p>
  Use non-geometry feature property names, data values, and
  operators to construct a filter string which is passed as an
  argument to the MgFeatureQueryOption::SetFilter() or
  MgFeatureAggregateOptions::SetFilter() method. For example,
  if FEATID is a feature property name, then the filter &quot;FEATID
  &gt; 20&quot; selects the features whose FEATID has a value greater
  than 20.
</p>
<h2> Expressions </h2>
<p>
  Expressions are subcomponents of a basic filter. One
  expression might constitute the entire filter, or several
  expressions can be strung together using operators.
</p>
<h2> Basic Filter Grammar </h2>
<pre>
&lt;Filter&gt; ::= '(' Filter ')'
| &lt;LogicalOperator&gt;
| &lt;SearchCondition&gt;
&lt;LogicalOperator&gt; ::= &lt;BinaryLogicalOperator&gt;
| &lt;UnaryLogicalOperator&gt;

&lt;BinaryLogicalOperator&gt; ::=

&lt;Filter&gt; &lt;BinaryLogicalOperations&gt; &lt;Filter&gt;

&lt;SearchCondition&gt; ::= &lt;InCondition&gt;
| &lt;ComparisonCondition&gt;
| &lt;GeometricCondition&gt;
| &lt;NullCondition&gt;

&lt;InCondition&gt; ::= &lt;Identifier&gt; IN '('
ValueExpressionCollection ')'

&lt;ValueExpressionCollection&gt; ::= &lt;ValueExpression&gt;
| &lt;ValueExpressionCollection&gt; ',' &lt;ValueExpression&gt;

&lt;ComparisonCondition&gt; ::=

&lt;Expression&gt; &lt;ComparisonOperations&gt; &lt;Expression&gt;

&lt;GeometricCondition&gt; ::= &lt;SpatialCondition&gt; |
&lt;DistanceCondition&gt;

&lt;NullCondition&gt; ::= &lt;Identifier&gt; NULL

&lt;SpatialCondition&gt; ::= &lt;Identifier&gt; &lt;SpatialOperations&gt;
&lt;Expression&gt;

&lt;DistanceCondition&gt; ::= &lt;Identifier&gt; &lt;DistanceOperations&gt;
&lt;Expression&gt; &lt;distance&gt;

&lt;UnaryLogicalOperator&gt; ::= NOT &lt;Filter&gt;

&lt;BinaryLogicalOperations&gt; ::= AND | OR

&lt;ComparisionOperations&gt; ::= = 
| &lt;&gt;
| &gt;
| &gt;=
| &lt;
| &lt;=
| LIKE

&lt;SpatialOperations&gt; ::= CONTAINS | CROSSES | DISJOINT |
EQUALS | INTERSECTS | OVERLAPS | TOUCHES | WITHIN | COVEREDBY
| INSIDE

&lt;DistanceOperations&gt; ::= BEYOND | WITHINDISTANCE

&lt;distance&gt; ::= BEYOND | WITHINDISTANCE
</pre>
<h2> Expression Grammar </h2>
<pre>
&lt;Expression&gt; ::= '(' Expression ')'
| &lt;UnaryExpression&gt;
| &lt;BinaryExpression&gt;
| &lt;Function&gt;
| &lt;Identifier&gt;
| &lt;ValueExpression&gt;

&lt;BinaryExpression&gt; ::= &lt;Expression&gt; '+' &lt;Expression&gt;
| &lt;Expression&gt; '-' &lt;Expression&gt;
| &lt;Expression&gt; '*' &lt;Expression&gt;
| &lt;Expression&gt; '/' &lt;Expression&gt;

&lt;ValueExpression&gt; ::= &lt;LiteralValue&gt;

&lt;LiteralValue&gt; ::= &lt;GeometryValue&gt; | &lt;DataValue&gt;

&lt;GeometryValue&gt; ::= GEOMFROMTEXT '(' STRING ')'

&lt;DataValue&gt; ::= TRUE
| FALSE
| DATETIME
| DOUBLE
| INTEGER
| STRING
| BLOB
| CLOB
| NULL

&lt;Function&gt; ::= &lt;Identifier&gt; '(' &lt;ExpressionCollection&gt;
')'

&lt;ExpressionCollection&gt; ::=
| &lt;Expression&gt;
| &lt;ExpressionCollection&gt; ',' &lt;Expression&gt;

&lt;Identifier&gt; ::= IDENTIFIER

&lt;UnaryExpression&gt; ::= '-' &lt;Expression&gt;
</pre>
<h2> Operator Precedence </h2>
<p>
  The operator precedence from highest to lowest is:
  Negate NOT<br>
  Multiply Divide<br>
  Add Subtract<br>
  = &lt;&gt; &gt; &gt;= &lt; &lt;=<br>
  AND<br>
  OR
</p>
<h2>Filter and Expression Keywords</h2>
<p>
  The following case-insensitive keywords are reserved in the
  language. That is, they cannot be used as identifier or
  function names:
</p>
<p>
  AND BEYOND COMPARE CONTAINS COVEREDBY CROSSES DATA DISJOINT
  DISTANCE EQUALS FALSE GEOMFROMTEXT IN INSIDE INTERSECTS LIKE
  NOT NULL OR OVERLAPS RELATE SPATIAL TIME TIMESTAMP TOUCHES
  TRUE WITHIN WITHINDISTANCE
</p>
<H2> STRING </H2>
Strings are literal constants enclosed in single quotes. If
you need to include a single quote character inside a string,
you can double the character, for example, 'aaa''bbb'.
<H2> IDENTIFIER </H2>
An identifier can be any alphanumeric sequence of characters
other than a keyword. Identifiers can be enclosed in double
quotes to show special characters and white space. If you
need to include a double quote character inside an
identifier, you can double the character, for example
&quot;abc&quot;&quot;def&quot;.
<H2> INTEGER </H2>
Integers allow only decimal characters with an optional unary
minus sign. Unary plus is not supported. If an integer is out
of the 32-bit precision range, it is converted to floating
point.
<H2> DOUBLE </H2>
Floating point numbers have a decimal point, can be signed
(-), and include an optional exponent (e{[0-9]}).
<H2> DATETIME </H2>
<p>
  Date and time are parsed using the standard literal strings:
</p>
<p>
  DATE 'YYYY-MM-DD'
</p>
<p>
  TIME 'HH:MM:SS[.sss]'
</p>
<p>
  TIMESTAMP 'YYYY-MM-DD HH:MM:SS[.sss]'
</p>
<H2> CLOB/BLOB </H2>
<p>
  These data types are not currently supported. If you need to
  support binary input, use parameters.
</p>
<H2> &lt;Function&gt; </H2>
<p>
  Some functions such as ceil, floor, concat, lower, and upper
  take as an argument the value of a property from a single
  feature and return a value related to the property value.
</p>
<p>
  Some functions such as avg, count, max, min, stddev, and sum
  take as an argument the values of a property from multiple
  features and return a single value related to the values of
  the group of property arguments.
</p>
<TABLE class="RuledTable">
  <tr>
    <th>Function</th>
    <th>Description</th>
  </tr>
  <tr>
    <td>Double Avg(n)</td>
    <td>Average value of n, ignoring nulls</td>
  </tr>
  <tr>
    <td>Int64 Ceil(Double)</td>
    <td>Smallest integer &gt;= Double</td>
  </tr>
  <tr>
    <td>String Concat(Str1, Str2)</td>
    <td>Concatenates Str1 and Str2</td>
  </tr>
  <tr>
    <td>Int64 Count(expression)</td>
    <td>Number of features where expression is not null</td>
  </tr>
  <tr>
    <td>Int64 Floor(Double)</td>
    <td>Largest integer &lt;= Double</td>
  </tr>
  <tr>
    <td>String Lower(Str)</td>
    <td>Str with all lowercase letters</td>
  </tr>
  <tr>
    <td>Double Min(expression)</td>
    <td>Minimum value of expression</td>
  </tr>
  <tr>
    <td>Double Max(expression)</td>
    <td>Maximum value of expression</td>
  </tr>
  <tr>
    <td>Double Stddev(n)</td>
    <td>Standard deviation of n, ignoring nulls</td>
  </tr>
  <tr>
    <td>Double Sum(n)</td>
    <td>Sum of values of n</td>
  </tr>
  <tr>
    <td>String Upper(Str)</td>
    <td>Str with all uppercase letters</td>
  </tr>
</TABLE>
<h2>Examples</h2>
<p>
	PHP and C# sample code for the setting of filters for select operations is
	presented. The SQL expression equivalent to the PHP and C# code is also
	presented.
</p>
<pre>
&lt;?php
$queryOptions = new MgFeatureQueryOptions();
$stringCollection = new MgStringCollection();
$wktReaderWriter = new MgWktReaderWriter();
?&gt;
</pre>
<pre>
using OSGeo.MapGuide;
private MgFeatureQueryOptions queryOptions;
private MgStringCollection stringCollection;
private MgWktReaderWriter wktReaderWriter;
private String featClassName = &quot;SdfFeatureClass&quot;;
private MgResourceIdentifier featureSrcResourceId;
private MgFeatureService featureService;

stringCollection = new MgStringCollection();
queryOptions = new MgFeatureQueryOptions();
// the feature source has already been installed in the repository
featureSrcResourceId = new MgResourceIdentifier(&quot;Library://PlatformApiDocTests/SdfFeatureClass.FeatureSource&quot;);
wktReaderWriter = new MgWktReaderWriter();
</pre>
<H3> &lt;Identifier&gt; NULL </H3>
<p>
  SometimesNULL is a string property. If you have not given a
  value to it when inserting certain features and you apply
  the filter, sometimesNULL NULL, you select those
  features. If you have given a value to it when inserting other
  features and you apply the filter, NOT sometimesNULL
  NULL, you select those other features.
</p>
<pre>
sqlplus&gt; select sometimesnull from featclass where sometimesnull is null;
</pre>
<pre>
&lt;?php
$queryOptions-&gt;AddFeatureProperty(&quot;sometimesNULL&quot;);
$queryOptions-&gt;SetFilter(&quot;sometimesNULL NULL&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader->Close();
?&gt;
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;SometimesNull&quot;);
queryOptions.SetFilter(&quot;SometimesNull NULL&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process featureReader
featureReader.Close();
</pre>
<pre>
sqlplus&gt; select sometimesnull from featclass where sometimesnull is not null;
</pre>
<pre>
&lt;?php
$queryOptions-&gt;AddFeatureProperty(&quot;sometimesNULL&quot;);
$queryOptions-&gt;SetFilter(&quot;NOT sometimesNULL NULL&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader->Close();
?&gt;
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;SometimesNull&quot;);
queryOptions.SetFilter(&quot;NOT SometimesNull NULL&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process featureReader
featureReader.Close();
</pre>
<H3> &lt;Identifier&gt; LIKE &lt;String&gt; </H3>
<p>
  Identifier is the name of a property whose type is
  MgPropertyType::String. String contains a pattern. A percent
  character (%) in a pattern matches zero or more characters.
  An underscore character (_) matches one character.
</p>
<p>
  Description is a string property. There are 2 features with
  this property in the datastore, and the contents of the two
  properties are: &quot;POINT XY (1 1)&quot; and &quot;POLYGON XY ((0 0, 2 0, 2 2, 0 2, 0 0))&quot;.
</p>
<p>
  The filter, Description LIKE '%POLYGON%', returns
  &quot;POLYGON XY ((0 0, 2 0, 2 2, 0 2, 0 0))&quot;, the filter,
  NOT Description LIKE '%POLYGON%', returns
  &quot;POINT XY (1 1)&quot;, and the filter, Description LIKE
  '%POL_GON%', returns &quot;POLYGON XY ((0 0, 2 0, 2 2, 0
  2, 0 0))&quot;.
</p>
<pre>
sqlplus&gt; select Description from featclass where Description LIKE '%POLYGON%';
</pre>
<pre>
$queryOptions-&gt;AddFeatureProperty(&quot;Description&quot;);
$queryOptions-&gt;SetFilter(&quot;Description LIKE '%POLYGON%'&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader->Close();
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;Description&quot;);
queryOptions.SetFilter(&quot;Description LIKE '%POLYGON%'&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process featureReader
featureReader.Close();
</pre>
<pre>
sqlplus&gt; select Description from featclass where Description NOT LIKE '%POLYGON%';
</pre>
<pre>
$queryOptions-&gt;AddFeatureProperty(&quot;Description&quot;);
$queryOptions-&gt;SetFilter(&quot;NOT Description LIKE '%POLYGON%'&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader->Close();
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;Description&quot;);
queryOptions.SetFilter(&quot;NOT Description LIKE '%POLYGON%'&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process featureReader
featureReader.Close();
</pre>
<pre>
sqlplus&gt; select Description from featclass where Description LIKE '%POL_GON%';
</pre>
<pre>
$queryOptions-&gt;AddFeatureProperty(&quot;Description&quot;);
$queryOptions-&gt;SetFilter(&quot;Description LIKE '%POL_GON%'&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader->Close();
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;Description&quot;);
queryOptions.SetFilter(&quot;Description LIKE '%POL_GON%'&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process featureReader
featureReader.Close();
</pre>
<H3>&lt;Identifier&gt; IN ( &lt;ValueExpressionCollection&gt; ) </H3>
<p>
  anInt16 is an Int16 property. In one feature instance the
  value of anInt16 is -7033. If you apply &lt;filter&gt;anInt16 IN
  ( -5995, -7033 ), you select this feature.
</p>
<pre>
sqlplus&gt; select anInt16 from featclass where anInt16 in ( -5995, -7033 );
</pre>
<pre>
&lt;?php
$queryOptions-&gt;AddFeatureProperty(&quot;anInt16&quot;);
$queryOptions-&gt;SetFilter(&quot;anInt16 IN ( -5995, -7033 )&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader-&gt;Close();
?&gt;
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;anInt16&quot;);
queryOptions.SetFilter(&quot;anInt16 IN ( -5995, -7033 )&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process featureReader
featureReader.Close();
</pre>
<H3>&lt;Identifier&gt;  &gt;  &lt;DataValue&gt;</H3>
<p>
  featid is an identity property. If you apply the filter, featid
  &gt; 20, you select the features whose featid has a
  value &gt; 20. If you apply the filter, featid &gt; 0 AND featid &lt;
  5, you select the features whose featid belongs to
  { 1, 2, 3, 4}. If you apply the filter, featid &lt; 3 OR featid
  &gt; 3, you select features whose featid is not 3.
</p>
<p>
  aDateTime is a date property. There is a feature whose
  aDateTime property has the value 9/20/2005::10:9:34:0. If you
  apply the filter, aDateTime &lt; '2005-09-21', you
  select this feature.
</p>
<pre>
sqlplus&gt; select anInt16 from featclass where adatetime &lt; '21-SEP-05';
</pre>
<pre>
&lt;?php
$queryOptions-&gt;AddFeatureProperty(&quot;anInt16&quot;);
$queryOptions-&gt;SetFilter(&quot;aDateTime &lt; '2005-09-21'&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader-&gt;Close();
?&gt;
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;anInt16&quot;);
queryOptions.SetFilter(&quot;aDateTime &lt; '2005-09-21'&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process featureReader
featureReader.Close();
</pre>
<H3>&lt;Expression&gt;  &lt;  &lt;DataValue&gt;</H3>
<p>
  anInt16 is an Int16 property. Two features have non-NULL
  values for this property. One has a value -7033, and the
  other -5995. If you apply the filter, ( anInt16 + 1000 ) &lt;
  -5995, you select the feature whose anInt16
  property has the value -7033. The parentheses in this filter
  are optional because operator precedence would dictate that
  the filter, anInt16 + 1000 &lt; -5995, is equivalent.
</p>
<pre>
sqlplus&gt; select anInt16 from featclass where anInt16 + 1000 &lt; -5995;
</pre>
<pre>
&lt;?php
$queryOptions-&gt;AddFeatureProperty(&quot;anInt16&quot;);
$queryOptions-&gt;SetFilter(&quot;anInt16 + 1000 &lt; -5995&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader-&gt;Close();
?&gt;
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;anInt16&quot;);
queryOptions.SetFilter(&quot;anInt16 + 1000 &lt; -5995&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process featureReader
featureReader.Close();
</pre>
<H3>&lt;FunctionValue&gt; = &lt;DataValue&gt;</H3>
<p>
  aDouble is a double property. One feature has aDouble
  property with a value of 8103.08393. If you apply
  the filter, ceil(aDouble) = 8104, you select this
  feature.
</p>
<pre>
sqlplus&gt; select aDouble from featclass where ceil(aDouble) = 8104;
</pre>
<pre>
&lt;?php
$queryOptions-&gt;AddFeatureProperty(&quot;aDouble&quot;);
$queryOptions-&gt;SetFilter(&quot;ceil(aDouble) = 8104&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader-&gt;Close();
?&gt;
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;aDouble&quot;);
queryOptions.SetFilter(&quot;ceil(aDouble) = 8104&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process featureReader
featureReader.Close();
</pre>
<H3>Group Function</H3>
<p>
  aDouble is a double property. sum is a group function.
  sum(aDouble) sums the values of the aDouble property taken
  from a group of features.
</p>
<pre>
sqlplus&gt; select sum(aDouble) from featclass;
</pre>
<pre>
&lt;?php
$queryOptions-&gt;AddComputedProperty(&quot;sumDbl&quot;, &quot;sum(aDouble)&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader-&gt;Close();
?&gt;
</pre>
<pre>
queryOptions.AddComputedProperty(&quot;sumDbl&quot;, &quot;sum(aDouble)&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process featureReader
featureReader.Close();
</pre>
<h3>Ordering</h3>
<p>
  aDouble is a double property. anInt32Key is the identity
  property. The first example returns aDouble values in
  ascending order, and the second example returns them in
  descending order.
</p>
<h4>Ascending</h4>
<pre>
sqlplus&gt; select anint32key,adouble from tuxuniversalclassxy order by adouble ASC;
</pre>
<pre>
&lt;?php
$queryOptions-&gt;AddFeatureProperty(&quot;aDouble&quot;);
$queryOptions-&gt;AddFeatureProperty(&quot;anInt32Key&quot;);
$stringCollection-&gt;Add(&quot;aDouble&quot;);
$queryOptions-&gt;SetOrderingFilter($stringCollection, MgOrderingOption::Ascending);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader-&gt;Close();
?&gt;
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;aDouble&quot;);
queryOptions.AddFeatureProperty(&quot;anInt32Key&quot;);
stringCollection.Add(&quot;aDouble&quot;);
queryOptions.SetOrderingFilter(stringCollection, MgOrderingOption::Ascending);
featureReader = $featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process $featureReader
featureReader.Close();
</pre>
<h4>Descending</h4>
<pre>
sqlplus&gt; select anint32key,adouble from tuxuniversalclassxy order by adouble DESC;
</pre>
<pre>
&lt;?php
$queryOptions-&gt;AddFeatureProperty(&quot;aDouble&quot;);
$queryOptions-&gt;AddFeatureProperty(&quot;anInt32Key&quot;);
$stringCollection-&gt;Add(&quot;aDouble&quot;);
$queryOptions-&gt;SetOrderingFilter($stringCollection, MgOrderingOption::Descending);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader-&gt;Close();
?&gt;
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;aDouble&quot;);
queryOptions.AddFeatureProperty(&quot;anInt32Key&quot;);
stringCollection.Add(&quot;aDouble&quot;);
queryOptions.SetOrderingFilter(stringCollection, MgOrderingOption::Descending);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process $featureReader
featureReader.Close();
</pre>
<h3> Basic Filter OR Spatial Filter </h3>
<p>
  featId is an identity property, and geometry is a geometry
  property. The feature whose featId value is 0 has a geometry
  value of POINT(1 1). The feature whose featid value is 1 has
  a null geometry value. The spatial filter requests features
  whose geometry intersects with POINT(1 1). The following
  select operation returns both of these features. The
  operation can be coded in two ways. The first way uses the
  SetFilter(), SetSpatialFilter() and SetBinaryOperator()
  methods, and the second way uses only the SetFilter() method.
</p>
<pre>
sqlplus&gt; select a.featId from featclass a where a.featId = 1 or sdo_relate(a.geometry, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(1,1,NULL), NULL, NULL), 'mask=anyinteract') = 'TRUE';
</pre>
<pre>
&lt;?php
$queryOptions-&gt;AddFeatureProperty(&quot;featId&quot;);
$queryOptions-&gt;SetFilter(&quot;featId = 1&quot;);
$queryOptions-&gt;SetBinaryOperator(false);
$geometry = $wktReaderWriter-&gt;Read(&quot;POINT(1 1)&quot;);
$queryOptions-&gt;SetSpatialFilter(&quot;geometry&quot;, $geometry, MgFeatureSpatialOperations::Intersects);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader-&gt;Close();
?&gt;
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;featId&quot;);
queryOptions.SetFilter(&quot;featId = 1&quot;);
queryOptions.SetBinaryOperator(false);
geometry = wktReaderWriter.Read(&quot;POINT(1 1)&quot;);
queryOptions.SetSpatialFilter(&quot;geometry&quot;, geometry, MgFeatureSpatialOperations::Intersects);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process $featureReader
featureReader.Close();
</pre>
<pre>
&lt;?php
$queryOptions-&gt;AddFeatureProperty(&quot;featId&quot;);
$queryOptions-&gt;SetFilter(&quot;(featId = 1) OR (geometry INTERSECTS GEOMFROMTEXT ( 'POINT(1 1)' )&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader-&gt;Close();
?&gt;
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;featId&quot;);
geometry = wktReaderWriter.Read(&quot;POINT(1 1)&quot;);
queryOptions.SetFilter(&quot;(featId = 1) OR (geometry INTERSECTS GEOMFROMTEXT ( 'POINT(1 1)' )&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process $featureReader
featureReader.Close();
</pre>
<h3>Distance Filter</h3>
<p>
  featId is an identity property, and geometry is a geometry
  property. The feature whose featId value is 0 has a geometry
  value of POINT(1 1). The distance filter requests features
  whose geometry is within a distance of 1 from POINT(2 1). The
  following select operation returns the feature whose featId
  is 0.
</p>
<pre>
</pre>
sqlplus&gt; select a.featId from featclass a where sdo_within_distance(a.geometry, MDSYS SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(2,1 NULL), NULL, NULL), 'distance=1') = 'TRUE';
<pre>
&lt;?php
$queryOptions-&gt;AddFeatureProperty(&quot;featId&quot;);
$queryOptions-&gt;SetFilter(&quot;geometry WITHINDISTANCE GEOMFROMTEXT ('POINT(2 1)') 1&quot;);
$featureReader = $featureService-&gt;SelectFeatures($featureSrcResourceId, $featClassName, $queryOptions);
# process $featureReader
$featureReader-&gt;Close();
?&gt;
</pre>
<pre>
queryOptions.AddFeatureProperty(&quot;featId&quot;);
queryOptions.SetFilter(&quot;geometry WITHINDISTANCE GEOMFROMTEXT ('POINT(2 1)') 1&quot;);
featureReader = featureService.SelectFeatures(featureSrcResourceId, featClassName, queryOptions);
// process $featureReader
featureReader.Close();
</pre>
**/
