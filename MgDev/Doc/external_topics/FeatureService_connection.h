/**
\addtogroup ConnectionToFeatureSource
<h2> About feature sources </h2>
<p>
  A feature source contains feature data. The term 'feature
  source' refers to a feature data resource in the site server
  repository. The feature source is installed in the repository
  using the resource service (MgResourceService).
</p>
<p>
  What constitutes a feature source varies depending on the
  nature of the actual data source. For example, some data
  sources are text files, some are binary files, and some are
  relational databases. If the data source is a file, the file
  itself is stored in the repository. If the data source is a
  relational database, a connection specification is stored in
  the repository. Some relational databases support
  partitioning into multiple data sources. Each partition
  requires a separate connection specification.
</p>
<h2> Storing and identifying feature sources </h2>
<p>
  In addition to features sources, the site server repository
  contains many other objects, which are collectively referred
  to as resources and identified by resource identifiers
  (MgResourceIdentifier). The resource identifier specifies the
  location of a resource in the repository by using a URL.
</p>
<p>
  The feature source URL is of the form
  Library:://.../&lt;featureSourceName&gt;.FeatureSource. The
  ellipsis (...) represents an optional folder structure
  contained within the root Library folder. An example
  identifier is
  &quot;Library://FeatureService/PointsMDB.FeatureSource&quot;. The
  FeatureSource extension is specific to the feature source
  resource. All of the methods in the MgFeatureService API that
  do actual work require an MgResourceIdentifier object as an
  argument.
</p>
<h2> Before installing a feature source in the repository </h2>
<p>
  The MgFeatureService API provides a \link MgFeatureService::TestConnection TestConnection \endlink
  method for testing the connection parameter values for a
  feature source prior to installation in the repository. For
  file-based feature sources the connection value could be a
  file name, a folder name, or a symbolic name which is used
  by the provider to query the operating system for the file
  location. For relational database feature sources, possible
  connection values include host name, service name, service
  instance name, user name, password, and datastore name.
</p>
<p>
  The connection string used to connect to a feature source is
  specific to the feature source. Examples follow for various
  providers.
</p>
<p>
  The MgFeatureService API provides a \link MgFeatureService::GetConnectionPropertyValues GetConnectionPropertyValues \endlink
  method for listing the names of the feature source partitions
  within a relational database. You can then construct a
  connection string for each partition and test it using 
  \link MgFeatureService::TestConnection MgFeatureService::TestConnection Method (CREFSTRING\, CREFSTRING) \endlink.
</p>
<p>
  After the installation of the feature source in the
  repository, connection is done by a different 
  \link MgFeatureService::TestConnection TestConnection \endlink
  method that takes the feature source's resource identifier as
  an argument.
</p>
<h2> Installing the feature source in the repository </h2>
<p>
  Install a feature source in the repository by:
  \li  Specifying the location of the feature source in the
  repository in a resource identifier.
  \li  Uploading a feature source XML value containing the
  property values used to connect to and configure the feature
  source. The structure of a feature source XML value conforms
  to the \link FeatureSource_schema FeatureSource \endlink XML schema. The element values
  vary according to the feature source type. Examples for each
  provider follow.
  \li  Uploading an XML value containing values governing the
  security of the feature source. See the \link ResourceDocumentHeader_schema ResourceDocumentHeader \endlink
  and \link ResourceSecurity_schema ResourceSecurity\endlink schema topics for a general
  discussion about the XML element definitions. See the
  contents of an example header file at the end of this topic.
  \li  Uploading the file or files which constitute the feature
  source if the feature source is file-based.
  Upload the feature source property XML file and document
  header XML file using the 
  \link MgResourceService::SetResource MgResourceService::SetResource Method\endlink.
  Upload a feature source file using the
  \link MgResourceService::SetResourceData MgResourceService::SetResourceData Method\endlink.
  
</p>
<h2>Examples for various providers</h2>
<h3> Autodesk.Oracle.2.0 Provider </h3>
<p>
  The feature source is a relational database, which supports
  multiple partitions, referred to as datastores. Each
  datastore can be a feature source. You use Sysadmin.exe to
  create datastores and add usernames and passwords. The tool
  and documentation for doing these and other tasks is
  contained in the installation. For a typical installation,
  the path is <i>C:\\Program Files\\Common Files\\Autodesk
  Shared\\GIS\\FDO\\2.0\\Oracle</i>.
</p>
<h3> Example Connection String </h3>
<p>
  Username=smith;Password=test;Service=TuxService;DataStore=TUX
</p>
<p>
  All parameters are required.
</p>
<p>
  The DataStore property value identifies the feature source
  partition within the database. This value can be retrieved
  from the database using the 
  \link MgFeatureService::GetConnectionPropertyValues MgFeatureService::GetConnectionPropertyValues Method\endlink.
</p>
<p>
  The Service property value identifies an entry in the <i>tnsnames.ora</i> file located in the Oracle instance or Oracle client
  installation on the local host.
</p>
<h3> Example Feature Source XML File </h3>
<p>
  The filename is TuxService.FeatureSource.
</p>
<pre>
&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;
&lt;FeatureSource version=&quot;1.0.0&quot; xmlns:xsi=&quot;http://www.w3.org/2001/XMLSchema-instance &quot; xsi:noNamespaceSchemaLocation=&quot;FeatureSource-1.0.0.xsd&quot;&gt;
  &lt;Provider&gt;Autodesk.Oracle.2.0&lt;/Provider&gt;
  &lt;ConnectionProperties&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;DataStore&lt;/Name&gt;
      &lt;Value&gt;TUX&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;Username&lt;/Name&gt;
      &lt;Value&gt;smith&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;Password&lt;/Name&gt;
      &lt;Value&gt;test&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;Service&lt;/Name&gt;
      &lt;Value&gt;TuxService&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
  &lt;/ConnectionProperties&gt;
  &lt;Configuration/&gt;
  &lt;SpatialContext/&gt;
  &lt;LongTransaction/&gt;
&lt;/FeatureSource&gt;
</pre>
<h3> Autodesk.ArcSDE.1.0 Provider </h3>
<p>
  The feature source is a relational database. Access to it is
  indirect. The provider talks to an ArcSDE client, which talks
  to the ArcSDE server, which talks to the feature source.
</p>
<h4> Example Connection String </h4>
<p>
  Username=smith;Password=test;Server=otwhost1;Instance=sde_inst1;Datastore=Default
  Datastore
</p>
<p>
  All parameters are required.
</p>
<p>
  The Server value is a hostname whose IP address can be
  resolved using a network domain name server.
</p>
<p>
  The instance value must appear in the services file on the
  local host. The path is <i>C:\\WINDOWS\\system32\\drivers\\etc\\services</i>.
  An example entry is &quot;sde_inst1 5151/tcp \#ArcSDE Server
  listening port&quot;.
</p>
<p>
  The Datastore property value identifies the feature source
  partition within the database. This value can be retrieved
  from the database using the \link MgFeatureService::GetConnectionPropertyValues MgFeatureService::GetConnectionPropertyValues Method \endlink.
  A value of &quot;Default Datastore&quot; indicates that only one
  partition exists.
</p>
<h4> Example Feature Source XML File </h4>
The filename is <i>sde_inst1.FeatureSource</i>.
<pre>
&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;
&lt;FeatureSource version=&quot;1.0.0&quot; xmlns:xsi=&quot;http://www.w3.org/2001/XMLSchema-instance &quot; xsi:noNamespaceSchemaLocation=&quot;FeatureSource-1.0.0.xsd&quot;&gt;
  &lt;Provider&gt;Autodesk.ArcSDE.1.0&lt;/Provider&gt;
  &lt;ConnectionProperties&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;Datastore&lt;/Name&gt;
      &lt;Value&gt;Default Datastore&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;Username&lt;/Name&gt;
      &lt;Value&gt;smith&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;Password&lt;/Name&gt;
      &lt;Value&gt;test&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;Server&lt;/Name&gt;
      &lt;Value&gt;otwhost1&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;Instance&lt;/Name&gt;
      &lt;Value&gt;sde_inst1&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
  &lt;/ConnectionProperties&gt;
  &lt;Configuration/&gt;
  &lt;SpatialContext/&gt;
&lt;LongTransaction/&gt;
&lt;/FeatureSource&gt;
</pre>
<h3> Autodesk.Sdf.3.0 Provider</h3>
<p>
  The feature source is a file. As part of installing the
  feature source in the repository, the <i>.sdf</i> file is
  loaded into the repository.
</p>
<h4>Example Connection String</h4>
<p>
  File=C:/SDFFeatureResources/testSDF.sdf;ReadOnly=FALSE
</p>
<p>
  The File parameter is mandatory, and the ReadOnly parameter
  defaults to FALSE.
</p>
<h4> Example Feature Source XML File </h4>
<p>
  The ConnectionProperty element whose Name is File has a Value
  element that is a symbolic reference to the location of the <i>.sdf</i> file in the repository.
</p>
<pre>
&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;
&lt;FeatureSource version=&quot;1.0.0&quot; xmlns:xsi=&quot;http://www.w3.org/2001/XMLSchema-instance &quot; xsi:noNamespaceSchemaLocation=&quot;FeatureSource-1.0.0.xsd&quot;&gt;
  &lt;Provider&gt;Autodesk.Sdf.3.0&lt;/Provider&gt;
  &lt;ConnectionProperties&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;File&lt;/Name&gt;
      &lt;Value&gt;%MG_DATA_FILE_PATH%&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;ReadOnly&lt;/Name&gt;
      &lt;Value&gt;FALSE&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
  &lt;/ConnectionProperties&gt;
  &lt;SpatialContext/&gt;
  &lt;LongTransaction/&gt;
&lt;/FeatureSource&gt;
</pre>
<h3> Autodesk.Shp.1.0 Provider </h3>
<p>
  The feature source is a set of up to 4 related files,
  collectively referred to as shapefiles. They consist of a <i>.shp</i> file containing the shape geometry, the <i>.shx</i> file
  containing the shape row index, the <i>.dbf</i> file
  containing shape attributes in dBASE format, and the <i>.prj</i> file containing the coordinate system.
</p>
<p>
  It is possible to connect to a <i>.shp</i> file by itself.
  The SHP provider treats each <i>.shp</i> and associated <i>.dbf</i> file as a feature class with a single geometry property and
  optionally, with data attribute properties.
</p>
<p>
  The four files can be accessed by applications other than the
  provider. In addition the provider may create two files that
  are used by the provider and not by other applications: an <i>.idx</i> file containing a spatial index and an .xml configuration
  file containing a mapping of the SHP data in the <i>.shp</i> file and the DBF data in the <i>.dbf</i> file to feature
  classes and properties in the FDO data model.
</p>
<p>
  The connection string used as an argument to the
  TestConnection() method references a folder in the file
  system. As part of installing the feature source in the
  repository, the shapefiles (<i>.shp</i>, <i>.shx</i>, <i>.dbf</i>,
  and <i>.prj</i> files) are loaded into the repository. The
  value of the DefaultFileLocation element as specified in the
  feature source XML file is a symbolic reference to the
  location of the shapefiles in the repository.
</p>
<h4> Example Connection String </h4>
<p>
  DefaultFileLocation=C:/SHPFeatureResources;TemporaryFileLocation=C:/SHPFeatureResources
</p>
<h4> Example Feature Source XML File </h4>
<pre>
&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;
&lt;FeatureSource version=&quot;1.0.0&quot; xmlns:xsi=&quot;http://www.w3.org/2001/XMLSchema-instance &quot; xsi:noNamespaceSchemaLocation=&quot;FeatureSource-1.0.0.xsd&quot;&gt;
  &lt;Provider&gt;Autodesk.Shp.1.0&lt;/Provider&gt;
  &lt;ConnectionProperties&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;DefaultFileLocation&lt;/Name&gt;
      &lt;Value&gt;%MG_DATA_FILE_PATH%&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;TemporaryFileLocation&lt;/Name&gt;
    &lt;/ConnectionProperty&gt;
  &lt;/ConnectionProperties&gt;
  &lt;SpatialContext/&gt;
  &lt;LongTransaction/&gt;
&lt;/FeatureSource&gt;
</pre>
<h3> Autodesk.RFP.1.0 Provider </h3>
<h4> Example Connection String </h4>
<p>
  DefaultRasterFileLocation=C:/RasterFeatureResources
</p>
<h4> Example Feature Source XML File </h4>
<pre>
&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;
&lt;FeatureSource version=&quot;1.0.0&quot; xmlns:xsi=&quot;http://www.w3.org/2001/XMLSchema-instance &quot; xsi:noNamespaceSchemaLocation=&quot;FeatureSource-1.0.0.xsd&quot;&gt;
  &lt;Provider&gt;Autodesk.RFP.1.0&lt;/Provider&gt;
  &lt;ConnectionProperties&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;DefaultRasterFileLocation&lt;/Name&gt;
      &lt;Value&gt;%MG_DATA_FILE_PATH%&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
  &lt;/ConnectionProperties&gt;
  &lt;SpatialContext/&gt;
  &lt;LongTransaction/&gt;
&lt;/FeatureSource&gt;
</pre>
<h3>Autodesk.ODBC.1.0 Provider</h3>
<p>
  Typically you use this provider is to access text-based
  feature source files.
</p>
<p>
  The essential first step is to use a Windows OS
  administrative tool to define a data source name. This action
  associates a symbolic name with a file path. You then use the
  symbolic name in the connection string and XML feature source
  file.
</p>
<p>
  On an XP computer, the path to the ODBC Data Source
  Administrator is Start &gt; Settings &gt; Control Panel &gt;
  Administrative Tools &gt; Data Sources (ODBC). You can create a
  User DSN (Data Source Name), a System DSN or File DSN in the
  ODBC Data Source Administrator dialog box.
</p>
<p>
  The sequence of clicks or data entry to create a User or
  System DSN for a Microsoft Access <i>.mdb</i> file is System
  DSN or User DSN/Add &gt; Create New Data Source/Microsoft
  Access Driver (<i>*.mdb</i>) &gt; Finish &gt; ODBC Microsoft
  Access Setup/&lt;type name&gt; &gt; Select &gt; Select
  Database/&lt;select folder&gt; &gt; &lt;select file&gt; &gt; OK &gt; ODBC
  Microsoft Access Setup/OK. The name that you enter is the DSN
  name and is used in the connection string and the XML feature
  source file.
</p>
<p>
  The sequence of clicks or data entry to create a User or
  System DSN for a dBase <i>.dbf</i> file is System DNS or User
  DSN/Add &gt; Create New Data Source/Microsoft dBase Driver (<i>*.dbf</i>)
  &gt; Finish &gt; ODBC dBase Setup/&lt;type name&gt; &gt; Save &gt; Create
  New Data Source/Next &gt; Finish &gt; Select Directory&gt; Select
  Database/&lt;select folder&gt; &gt; &lt;select file&gt; &gt; OK &gt; ODBC
  dBase Setup/OK. If &quot;Select Directory&quot; is unavailable, clear
  &quot;Use Current Directory&quot;. The name that you enter is the DSN
  name and is used in the connection string and XML feature
  source file.
</p>
<p>
  The sequence of clicks or data entry to create a File DSN for
  a Microsoft Access <i>.mdb</i> file is File DSN/Add &gt; Create
  New Data Source/Microsoft Access Driver (<i>*.mdb</i>) &gt;
  Next&gt; Browse &gt; Save As/&lt;pick folder&gt; &gt; &lt;type filename&gt;
  Finish &gt; ODBC Microsoft Access Setup/Select &gt; Select
  Database/&lt;select folder&gt; &gt; &lt;select file&gt; &gt; OK &gt; ODBC
  Microsoft Access Setup/OK. The filename that you enter will
  be given an extension of <i>.dsn</i>.
</p>
<h4> Example Connection String </h4>
<p>
  DataSourceName=Country
</p>
<h4> Example Feature Source XML File </h4>
<pre>
&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;
&lt;FeatureSource version=&quot;1.0.0&quot; xmlns:xsi=&quot;http://www.w3.org/2001/XMLSchema-instance &quot; xsi:noNamespaceSchemaLocation=&quot;FeatureSource-1.0.0.xsd&quot;&gt;
  &lt;Provider&gt;Autodesk.ODBC.1.0&lt;/Provider&gt;
  &lt;ConnectionProperties&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;DataSourceName&lt;/Name&gt;
      &lt;Value&gt;Country&lt;/Value&gt;
    &lt;/ConnectionProperty&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;UserId&lt;/Name&gt;
    &lt;/ConnectionProperty&gt;
    &lt;ConnectionProperty&gt;
      &lt;Name&gt;Password&lt;/Name&gt;
    &lt;/ConnectionProperty&gt;
  &lt;/ConnectionProperties&gt;
  &lt;Configuration/&gt;
  &lt;SpatialContext/&gt;
  &lt;LongTransaction/&gt;
&lt;/FeatureSource&gt;
</pre>
<h4> Example ResourceDocumentHeader XML File </h4>
<pre>
&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt;
&lt;!-- Valid according to XML Spy --&gt;
&lt;ResourceDocumentHeader xmlns:xsi=&quot;http://www.w3.org/2001/XMLSchema-instance &quot; xsi:noNamespaceSchemaLocation=&quot;ResourceDocumentHeader-1.0.0.xsd&quot;&gt;
  &lt;Security&gt;
    &lt;Inherited&gt;true&lt;/Inherited&gt;
  &lt;/Security&gt;
&lt;/ResourceDocumentHeader&gt;
</pre>
<h2>Example (PHP)</h2>
<p>
  The code samples here how to add feature sources to the
  repository. You connect to a feature source in the
  repository. For example code showing how to connect to a
  feature source, see 
  \link MgFeatureService::TestConnection MgFeatureService::TestConnection Method (MgResourceIdentifier*)\endlink.
</p>
<p>
  The first example adds a resource to the repository for a
  connection to an FDO Provider for Oracle. Since the
  connection is to a relational database and not a file, you
  need only call MgResourceService::SetResource passing in the
  FeatureSource XML containing the property values for the
  connection to the provider.
</p>
<pre>
&lt;?php
// creating the resource in the repository for a
// connection to an FDO Provider for Oracle
// first arg to $resourceService-&gt;SetResource()
$featSrcResIdArg = &quot;Library://FeatureService/testOracle.FeatureSource&quot;;
$featSrcResId = new MgResourceIdentifier($featSrcResIdArg);

// second arg to $resourceService-&gt;SetResource()
$featSrcFileByteContent = new MgByteSource($pathToFeatSrcFile);
$featSrcFileReaderContent = $featSrcFileByteContent-&gt;GetReader();

// third arg to $resourceService-&gt;SetResource()
$headerPathByteContent = new MgByteSource($headerPath);
$headerPathReaderContent = $headerPathByteContent-&gt;GetReader();

// add the resource to the repository
$resourceService-&gt;SetResource($featSrcResId, $featSrcFileReaderContent, $headerPathReaderContent);
?&gt;
</pre>
The second example adds a resource to the repository for a
connection to an FDO Provider for SDF. Since the connection
is to a file, you must call MgResourceService::SetResource
passing in the FeatureSource XML containing the property
values for the connection to the SDF file, and you must call
MgResourceService::SetResourceData passing in the contents of
the SDF file.
<pre>
&lt;?php
// creating the resource in the repository for a
// connection to an FDO Provider for SDF
// first arg to $resourceService-&gt;SetResource()
$featSrcResIdArg = &quot;Library://FeatureService/testSDF.FeatureSource&quot;;
$featSrcResId = new MgResourceIdentifier($featSrcResIdArg);

// second arg to $resourceService-&gt;SetResource()
$featSrcFileByteContent = new MgByteSource($pathToFeatSrcFile);
$featSrcFileReaderContent = $featSrcFileByteContent-&gt;GetReader();

// third arg to $resourceService-&gt;SetResource()
$headerPathByteContent = new MgByteSource($headerPath);
$headerPathReaderContent = $headerPathByteContent-&gt;GetReader();

// add the resource to the repository
$resourceService-&gt;SetResource($featSrcResId, $featSrcFileReaderContent, $headerPathReaderContent);

// add the contents of the SDF file to the repository
$fileByteContent = new MgByteSource($pathToDataFile);
$fileReader = $fileByteContent-&gt;GetReader();
$resourceService-&gt;SetResourceData($featSrcResId, $fileName, &quot;File&quot;, $fileReader);
?&gt;
</pre>
**/