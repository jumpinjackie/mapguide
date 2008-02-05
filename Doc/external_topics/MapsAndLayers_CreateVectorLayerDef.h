/**
\addtogroup CreateAVectorLayerDefinition

<p>
	The output of the example code is a string containing the layer definition
	in xml format.
	This string is associated with an MgResourceIdentifier and
	added to the repository.
</p>
<p>
	In the example C# code the MakeDefaultStyleForMultiGeometry method is called
	because the feature geometry of the example SDF feature source can contain
	point, line or area geometries. The example xml shows the layer definition
	for a layer containing multiple geometry types.
</p>
<!-- Example (xml) -->
\htmlinclude xmlExampleTop.html
\code
<?xml version="1.0" encoding="utf-8"?>
<LayerDefinition xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xsi:noNamespaceSchemaLocation="LayerDefinition-1.0.0.xsd" version="1.0.0">
    <VectorLayerDefinition>
        <ResourceId>Library://PlatformApiDocTests/SdfFeatureClass.FeatureSource</ResourceId>
        <FeatureName>SdfFeatureClassSchema:SdfFeatureClass</FeatureName>
        <FeatureNameType>FeatureClass</FeatureNameType>
        <Geometry>theFeatureGeometry</Geometry>
        <VectorScaleRange>
            <LineTypeStyle>
                <LineRule>
                    <LegendLabel />
                    <Filter />
                    <LineSymbolization2D>
                        <LineStyle>Solid</LineStyle>
                        <Thickness>0.0</Thickness>
                        <Color>a0fbaf5a</Color>
                        <Unit>Centimeters</Unit>
                    </LineSymbolization2D>
                </LineRule>
            </LineTypeStyle>
            <AreaTypeStyle>
                <AreaRule>
                    <LegendLabel />
                    <AreaSymbolization2D>
                        <Fill>
                            <FillPattern>Solid</FillPattern>
                            <ForegroundColor>a0fbaf5a</ForegroundColor>
                            <BackgroundColor>FF000000</BackgroundColor>
                        </Fill>
                        <Stroke>
                            <LineStyle>Solid</LineStyle>
                            <Thickness>0.0</Thickness>
                            <Color>FF000000</Color>
                            <Unit>Centimeters</Unit>
                        </Stroke>
                    </AreaSymbolization2D>
                </AreaRule>
            </AreaTypeStyle>
            <PointTypeStyle>
                <DisplayAsText>false</DisplayAsText>
                <AllowOverpost>false</AllowOverpost>
                <PointRule>
                    <LegendLabel />
                    <PointSymbolization2D>
                        <Mark>
                            <Unit>Centimeters</Unit>
                            <SizeContext>DeviceUnits</SizeContext>
                            <SizeX>0.3</SizeX>
                            <SizeY>0.3</SizeY>
                            <Rotation>0</Rotation>
                            <Shape>Square</Shape>
                            <Fill>
                                <FillPattern>Solid</FillPattern>
                                <ForegroundColor>a0fbaf5a</ForegroundColor>
                                <BackgroundColor>FF000000</BackgroundColor>
                            </Fill>
                            <Edge>
                                <LineStyle>Solid</LineStyle>
                                <Thickness>0.0</Thickness>
                                <Color>FF000000</Color>
                                <Unit>Centimeters</Unit>
                            </Edge>
                        </Mark>
                    </PointSymbolization2D>
                </PointRule>
            </PointTypeStyle>
        </VectorScaleRange>
    </VectorLayerDefinition>
</LayerDefinition>
\endcode
\htmlinclude ExampleBottom.html

<!-- Example (C#) -->
\htmlinclude CSharpExampleTop.html
\code
using OSGeo.MapGuide;
using OSGeo.MapGuide.Schema.LayerDefinition;

public string CreateLayerDefinitionXml(string layerDefName,
	MgResourceIdentifier featureSourceId, string schemaName, string className,
	string geometryName, int geometricType)
{
	VectorLayerDefinitionType layer = MakeDefaultStyle(geometricType, new MgColor(GetColor(geometricType)));
	layer.ResourceId = featureSourceId.ToString();
	layer.FeatureName = schemaName + ":" + className;
	layer.Geometry = geometryName;

	LayerDefinition layerDefinition  = new LayerDefinition();
	layerDefinition.Item = layer;
	layerDefinition.version = "1.0.0";
            
	using (StringWriter writer = new StringWriter())
	{
		System.Type type = layerDefinition.GetType();
		XmlSerializer xs = new XmlSerializer(type);
		xs.Serialize(writer, layerDefinition);
		return writer.ToString();
	}
}

private static UInt32 _polygonColor = 0xFFFBAF5A;
private static UInt32 _pointColor = 0xFFFFF467;
private static UInt32 _curveColor = 0xFFF8000;
private static UInt32 _delta = 0x00208040;

// Generate a color which is supposed to be unique.
// returns a string representing color in ARGB format
private string GetColor(int geometricType)
{
	UInt32 color = 0;
	switch (geometricType)
	{
		case MgFeatureGeometricType.Point:
			color = _pointColor;
			_pointColor += _delta;
			_pointColor |= 0xFF000000;
			break;
		case MgFeatureGeometricType.Curve:
			color = _curveColor;
			_curveColor += _delta;
			_curveColor |= 0xFF000000;
			break;
		case MgFeatureGeometricType.Surface:
			color = _polygonColor;
			_polygonColor += _delta;
			_polygonColor |= 0xFF000000;
			break;
		case MgFeatureGeometricType.Solid:
			Debugger.Break(); // TODO
			break;
		default:
			color = _polygonColor;
			_polygonColor += _delta;
			_polygonColor |= 0xFF000000;
			break;
	}
	color |= 0xA0000000;
	color &= 0xA0FFFFFF;

	return color.ToString("X");
}

private VectorLayerDefinitionType MakeDefaultStyle(int geometricType, MgColor defaultColor)
{
	VectorLayerDefinitionType layerDefType = null;
	switch (geometricType)
	{
		case MgFeatureGeometricType.Point:
			layerDefType = MakeDefaultStyleForPoint(defaultColor);
			break;
		case MgFeatureGeometricType.Curve:
			layerDefType = MakeDefaultStyleForCurve(defaultColor);
			break;
		case MgFeatureGeometricType.Surface:
			layerDefType = MakeDefaultStyleForSurface(defaultColor);
			break;
		case MgFeatureGeometricType.Solid:
			Debugger.Break(); // TODO
			break;
		default:
			layerDefType = MakeDefaultStyleForMultiGeometry(defaultColor);
			break;
		}
	return layerDefType;
}

private VectorLayerDefinitionType MakeDefaultStyleForPoint(MgColor defaultColor)
{
	// <Fill>
	FillType fill = new FillType();
	fill.FillPattern = "Solid";
	fill.ForegroundColor = defaultColor.Color;
	fill.BackgroundColor = "FF000000";

	// <Edge>
	StrokeType edge = new StrokeType();
	edge.LineStyle = "Solid";
	edge.Thickness = "0.0";
	edge.Color = "FF000000";
	edge.Unit = LengthUnitType.Centimeters;

	// <MarkSymbolType>
	MarkSymbolType mark = new MarkSymbolType();
	mark.Unit = LengthUnitType.Centimeters;
	mark.SizeContext = SizeContextType.DeviceUnits;
	mark.SizeX = "0.3";
	mark.SizeY = "0.3";
	mark.Rotation = "0";
	mark.Shape = ShapeType.Square;
	mark.Fill = fill;
	mark.Edge = edge;

	// <PointSymbolization2D>
	PointSymbolization2DType pointSym = new PointSymbolization2DType();
	pointSym.Item = mark;

	// <PointRule>
	PointRuleType pointRule = new PointRuleType();
	pointRule.LegendLabel = "";
	pointRule.Item = pointSym;

	// <PointTypeStyle>
	PointTypeStyleType pointStyle = new PointTypeStyleType();
	pointStyle.DisplayAsText = false;
	pointStyle.AllowOverpost = false;
	pointStyle.PointRule = new PointRuleType[] { pointRule };

	// <VectorScaleRange>
	VectorScaleRangeType range = new VectorScaleRangeType();
	range.Items = new PointTypeStyleType[1];
	range.Items.SetValue(pointStyle, 0);

	// <VectorLayerDefinition>
	VectorLayerDefinitionType layerDef = new VectorLayerDefinitionType();
	layerDef.VectorScaleRange = new VectorScaleRangeType[1];
	layerDef.VectorScaleRange.SetValue(range, 0);

	return layerDef;
}

private VectorLayerDefinitionType MakeDefaultStyleForCurve(MgColor defaultColor)
{
	// <Stroke>
	StrokeType stroke = new StrokeType();
	stroke.LineStyle = "Solid";
	stroke.Thickness = "0.0";
	stroke.Color = defaultColor.Color;
	stroke.Unit = LengthUnitType.Centimeters;

	LineRuleType lr = new LineRuleType();
	lr.Items = new StrokeType[] { stroke };
	lr.Filter = "";

	// <AreaTypeStyle>
	// <VectorScaleRange>
	VectorScaleRangeType range = new VectorScaleRangeType();
	range.Items = new LineRuleType[1][];
	range.Items.SetValue(new LineRuleType[] { lr }, 0);
	range.MinScale = 0;
	range.MaxScale = Double.MaxValue;

	// <VectorLayerDefinition>
	VectorLayerDefinitionType layerDef = new VectorLayerDefinitionType();
	layerDef.VectorScaleRange = new VectorScaleRangeType[1];
	layerDef.VectorScaleRange.SetValue(range, 0);

	return layerDef;
}

private VectorLayerDefinitionType MakeDefaultStyleForSurface(MgColor defaultColor)
{
	// <Fill>
	FillType fill = new FillType();
	fill.FillPattern = "Solid";
	fill.ForegroundColor = defaultColor.Color;
	fill.BackgroundColor = "FF000000";

	// <Stroke>
	StrokeType stroke = new StrokeType();
	stroke.LineStyle = "Solid";
	stroke.Thickness = "0.0";
	stroke.Color = "FF000000";
	stroke.Unit = LengthUnitType.Centimeters;

	// <AreaSymbolization2D>
	AreaSymbolizationFillType areaSym = new AreaSymbolizationFillType();
	areaSym.Fill = fill;
	areaSym.Stroke = stroke;

	// <AreaRule>
	AreaRuleType areaRuleType = new AreaRuleType();
	areaRuleType.Item = areaSym;
	areaRuleType.LegendLabel = "";

	// <AreaTypeStyle>
	// <VectorScaleRange>
	VectorScaleRangeType range = new VectorScaleRangeType();
	range.Items = new AreaRuleType[1][];
	range.Items.SetValue(new AreaRuleType[] { areaRuleType }, 0);

	// <VectorLayerDefinition>
	VectorLayerDefinitionType layerDef = new VectorLayerDefinitionType();
	layerDef.VectorScaleRange = new VectorScaleRangeType[1];
	layerDef.VectorScaleRange.SetValue(range, 0);

	return layerDef;
}

private VectorLayerDefinitionType MakeDefaultStyleForMultiGeometry(MgColor defaultColor)
{
	// <LineTypeStyle> 

	// <Stroke>
	StrokeType lineStroke = new StrokeType();
	lineStroke.LineStyle = "Solid";
	lineStroke.Thickness = "0.0";
	lineStroke.Color = defaultColor.Color;
	lineStroke.Unit = LengthUnitType.Centimeters;

	// <LineRuleType>
	LineRuleType lineRuleType = new LineRuleType();
	lineRuleType.Items = new StrokeType[] { lineStroke };
	lineRuleType.Filter = "";
	lineRuleType.LegendLabel = "";

	// <LineTypeStyleType>
	LineTypeStyleType lineTypeStyle = new LineTypeStyleType();
	lineTypeStyle.LineRule = new LineRuleType[] { lineRuleType };

	// <AreaTypeStyle>

	// <Fill>
	FillType areaFill = new FillType();
	areaFill.FillPattern = "Solid";
	areaFill.ForegroundColor = defaultColor.Color;
	areaFill.BackgroundColor = "FF000000";

	// <Stroke>
	StrokeType areaStroke = new StrokeType();
	areaStroke.LineStyle = "Solid";
	areaStroke.Thickness = "0.0";
	areaStroke.Color = "FF000000";
	areaStroke.Unit = LengthUnitType.Centimeters;

	// <AreaSymbolization2D>
	AreaSymbolizationFillType areaSym = new AreaSymbolizationFillType();
	areaSym.Fill = areaFill;
	areaSym.Stroke = areaStroke;

	// <AreaRule>
	AreaRuleType areaRuleType = new AreaRuleType();
	areaRuleType.Item = areaSym;
	areaRuleType.LegendLabel = "";

	// <AreaTypeStyleType>
	AreaTypeStyleType areaTypeStyle = new AreaTypeStyleType();
	areaTypeStyle.AreaRule = new AreaRuleType[] { areaRuleType };

	// <PointTypeStyle>

	// <Fill>
	FillType pointFill = new FillType();
	pointFill.FillPattern = "Solid";
	pointFill.ForegroundColor = defaultColor.Color;
	pointFill.BackgroundColor = "FF000000";

	// <Edge>
	StrokeType edge = new StrokeType();
	edge.LineStyle = "Solid";
	edge.Thickness = "0.0";
	edge.Color = "FF000000";
	edge.Unit = LengthUnitType.Centimeters;

	// <MarkSymbolType>
	MarkSymbolType mark = new MarkSymbolType();
	mark.Unit = LengthUnitType.Centimeters;
	mark.SizeContext = SizeContextType.DeviceUnits;
	mark.SizeX = "0.3";
	mark.SizeY = "0.3";
	mark.Rotation = "0";
	mark.Shape = ShapeType.Square;
	mark.Fill = pointFill;
	mark.Edge = edge;

	// <PointSymbolization2D>
	PointSymbolization2DType pointSym = new PointSymbolization2DType();
	pointSym.Item = mark;

	// <PointRule>
	PointRuleType pointRule = new PointRuleType();
	pointRule.LegendLabel = "";
	pointRule.Item = pointSym;

	// <PointTypeStyle>
	PointTypeStyleType pointStyle = new PointTypeStyleType();
	pointStyle.DisplayAsText = false;
	pointStyle.AllowOverpost = false;
	pointStyle.PointRule = new PointRuleType[] { pointRule };

	// <VectorScaleRange>
	VectorScaleRangeType vectorRange = new VectorScaleRangeType();

	object[] items = new object[3];
	items[0] = lineTypeStyle;
	items[1] = areaTypeStyle;
	items[2] = pointStyle;

	vectorRange.Items = items;
	vectorRange.MinScale = 0;
	vectorRange.MaxScale = Double.MaxValue;

	// <VectorLayerDefinition>
	VectorLayerDefinitionType layerDef = new VectorLayerDefinitionType();
	layerDef.VectorScaleRange = new VectorScaleRangeType[1];
	layerDef.VectorScaleRange.SetValue(vectorRange, 0);

	return layerDef;
}
\endcode
\htmlinclude ExampleBottom.html


**/
