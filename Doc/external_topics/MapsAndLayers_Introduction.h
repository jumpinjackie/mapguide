/**
\addtogroup Maps_and_Layers_Module

<p>Maps and layers are fundamental components of the Geospatial Platform API. A map is
composed of layers, where each layer represents data from a single feature source.</p>
<p>
  For additional information about manipulating and rendering maps,
  see the \DevGuide.
</p>
<h2>Creating a Map</h2>
<p>Do the following</p>
	<ol>
		<li>
		Add the feature source(s) to the repository.
		Read about \link ConnectionToFeatureSource Connection To a Feature Source \endlink.
		</li>
		<li>
		Create layer definitions for the distinguishable sets of features in the sources
		and add these layer definitions to the repository.
		The rule followed for the example code is one layer per feature class.
		</li>
		<li>
		Create a map and add the layer definitions to it.
		</li>
	</ol>
</p>
<p>
	The example used to illustrate this procedure creates a map from the
	features in an SDF file. The SDF file has one feature class, and its
	feature geometry can store geometries of type point, line, or area.
</p>
<h3>Create A Layer Definition</h3>
<p>
	This step defines the display characteristics of the features in the layer.
	Display attributes differ according to whether the data is raster, vector, or DWF data.
	Vector data display attributes are further differentiated according to geometric type: point, line, or area.
</p>
<p>
	The structure of the layer definition and its attributes are specified as an xml schema.
	See \ref LayerDefinition_schema "LayerDefinition schema" for details.
</p>
<p>
	To use the schema in your C# code, you must first convert it to a C# class using the
	Microsoft .NET Framework SDK v2.0 command-line tool, xsd.exe.
	The invocation string used for this project is
	\code
	xsd.exe LayerDefinition-1.0.0.xsd /l:CS /n:OSGeo.MapGuide.Schema.LayerDefinition /c
	\endcode
	Run the command from the SDK command prompt.
	Get an SDK command prompt by clicking Start > Programs > Microsoft .NET Framework SDK v2.0 > SDK Command Prompt.
	Type 
	\code
	xsd.exe -h
	\endcode
	to get option descriptions. The /l option is programming language. CS stands
	for C#. The /n option is namespace. The /c option is the direction to generate classes.
</p>
<p>
	After adding the LayerDefinition-1_0_0.cs file to your project, reference it with the statement
	"using OSGeo.MapGuide.Schema.LayerDefinition;".
</p>
<p>
	Click this link \link CreateAVectorLayerDefinition Create A Vector Layer Definition \endlink to view the example code.
</p>
<h3>Create A Map</h3>
<p>
	Click this link \link CreateAMap Create A Map \endlink to view the example code.
</p>


**/
