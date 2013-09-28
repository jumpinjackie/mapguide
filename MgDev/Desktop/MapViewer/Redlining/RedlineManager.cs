using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Globalization;
using System.Xml;

namespace OSGeo.MapGuide.Viewer.Redlining
{
    internal class RedlineManager
    {
        private MgResourceService _resSvc;
        private MgFeatureService _featSvc;
        private MgMapBase _map;
        private IMapViewer _viewer;
        private RedlineRegistry _registry;

        public RedlineManager(IMapViewer viewer)
        {
            _map = viewer.GetMap();
            _viewer = viewer;
            var provider = _viewer.GetProvider();
            _resSvc = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);
            _featSvc = (MgFeatureService)provider.CreateService(MgServiceType.FeatureService);
            _registry = new RedlineRegistry(SessionID, _resSvc, _featSvc);
        }

        static string smSessionID;

        static string SessionID
        {
            get
            {
                if (string.IsNullOrEmpty(smSessionID))
                {
                    smSessionID = Guid.NewGuid().ToString();
                }
                return smSessionID;
            }
        }

        public RedlineLayer CreateRedlineLayer(CreateRedlineLayerParams param, out bool bAddedToMap)
        {
            bAddedToMap = false;
            MgResourceIdentifier fsId = GenerateRedlineFeatureSourceId(param);

            string className = null;
            string providerName = null;
            if (_resSvc.ResourceExists(fsId))
            {
                MgFeatureSchemaCollection schemas = _featSvc.DescribeSchema(fsId, string.Empty, null);
                MgFeatureSchema schema = schemas.GetItem(0);
                MgClassDefinitionCollection classes = schema.GetClasses();
                MgClassDefinition cls = classes.GetItem(0);
                className = schema.Name + ":" + cls.Name;
            }
            else
            {
                MgFeatureSchema schema = RedlineSchemaFactory.CreateSchema(param.GeometryTypes);

                providerName = "OSGeo.SDF";
                if (param.Format == RedlineDataStoreFormat.SHP)
                    providerName = "OSGeo.SHP";
                else if (param.Format == RedlineDataStoreFormat.SQLite)
                    providerName = "OSGeo.SQLite";

                MgFileFeatureSourceParams createParams = new MgFileFeatureSourceParams(providerName, RedlineSchemaFactory.SPATIAL_CONTEXT, _map.GetMapSRS(), schema);
                _featSvc.CreateFeatureSource(fsId, createParams);

                //HACK: SQLite leaky abstraction (hard-coded schema name), SHP probably has some leaks of its own, so we can't assume MarkupSchema:Markup
                //as the class name so re-interrogate our schema to figure it out
                MgFeatureSchemaCollection schemas = _featSvc.DescribeSchema(fsId, string.Empty, null);
                schema = schemas.GetItem(0);
                MgClassDefinitionCollection classes = schema.GetClasses();
                MgClassDefinition cls = classes.GetItem(0);
                className = schema.Name + ":" + cls.Name;
            }

            MgResourceIdentifier ldfId = GenerateRedlineLayerDefinitionId(param);
            if (!_resSvc.ResourceExists(ldfId))
            {
                string layerDefContent = CreateRedlineLayerDefinitionContent(fsId, className, param.Style, param.StyleType);
                byte[] bytes = Encoding.UTF8.GetBytes(layerDefContent);
                MgByteSource byteSource = new MgByteSource(bytes, bytes.Length);
                MgByteReader byteReader = byteSource.GetReader();

                _resSvc.SetResource(ldfId, byteReader, null);
            }

            if (param.AddToMap)
            {
                AddRedlineLayerToMap(ldfId);
                bAddedToMap = true;
            }

            var layer = new RedlineLayer(ldfId.Name, fsId.ToString(), ldfId.ToString(), param.GeometryTypes, param.StyleType);
            //If provider name was set, then we register this layer in the registry. Otherwise it means
            //the layer already exists and by extension already registered
            if (providerName != null)
                _registry.AddRedlineLayer(layer, providerName);
            return layer;
        }

        private void AddRedlineLayerToMap(MgResourceIdentifier ldfId)
        {
            MgLayerGroup group = null;
            var layers = _map.GetLayers();
            var groups = _map.GetLayerGroups();
            if (groups.Contains(GROUP_NAME))
            {
                group = groups.GetItem(GROUP_NAME);
            }
            else
            {
                group = new MgLayerGroup(GROUP_NAME);
                group.SetVisible(true);
                group.SetLegendLabel(Strings.RedlineLayerGroupLabel);
                group.SetDisplayInLegend(true);
                groups.Add(group);
            }

            var provider = _viewer.GetProvider();
            var newLayer = provider.CreateLayer(ldfId);
            newLayer.SetDisplayInLegend(true);
            newLayer.SetName("_" + ldfId.Name);
            newLayer.SetLegendLabel(ldfId.Name);
            newLayer.SetSelectable(true);
            newLayer.SetGroup(group);

            layers.Insert(0, newLayer);
        }

        private MgResourceIdentifier GenerateRedlineLayerDefinitionId(CreateRedlineLayerParams param)
        {
            return new MgResourceIdentifier(ResourceIdPrefix + param.Name + "." + MgResourceType.LayerDefinition);
        }

        static double SizeInMM(double value, SizeUnit unit)
        {
            switch (unit)
            {
                case SizeUnit.Points:
                    return value * 0.352777778;
                case SizeUnit.Inches:
                    return value * 25.4;
                case SizeUnit.Centimeters:
                    return value * 10;
                case SizeUnit.Meters:
                    return value * 1000;
                default: //assume mm
                    return value;
            }
        }

        static string GetBorderPatternGeometry(LinePattern pattern)
        {
            switch (pattern)
            {
                case LinePattern.Solid:
                    return Strings.line_solid;
                case LinePattern.Dash:
                    return Strings.line_dash;
                case LinePattern.Dot:
                    return Strings.line_dot;
                case LinePattern.DashDot:
                    return Strings.line_dashdot;
                case LinePattern.DashDotDot:
                    return Strings.line_dashdotdot;
                case LinePattern.Rail:
                    return Strings.line_rail;
                case LinePattern.BORDER:
                    return Strings.line_border;
                case LinePattern.DIVIDE:
                    return Strings.line_divide;
                case LinePattern.FENCELINE1:
                    return Strings.line_fenceline1;
                default:
                    throw new Exception("Unsupported or unrecognized border pattern: " + pattern.ToString()); 
            }
        }

        static string GetFillPatternTemplate(FillPattern pattern)
        {
            switch (pattern)
            {
                case FillPattern.Solid:
                    return Strings.area_solid;
                case FillPattern.Net:
                    return Strings.area_net;
                case FillPattern.Line:
                    return Strings.area_line;
                case FillPattern.Line_45:
                    return Strings.area_line_45;
                case FillPattern.Line_90:
                    return Strings.area_line_90;
                case FillPattern.Line_135:
                    return Strings.area_line_135;
                case FillPattern.Square:
                    return Strings.area_square;
                case FillPattern.Box:
                    return Strings.area_box;
                case FillPattern.Cross:
                    return Strings.area_cross;
                case FillPattern.Dash:
                    return Strings.area_dash;
                case FillPattern.Dolmit:
                    return Strings.area_dolmit;
                case FillPattern.Hex:
                    return Strings.area_hex;
                case FillPattern.Sacncr:
                    return Strings.area_sacncr;
                case FillPattern.Steel:
                    return Strings.area_steel;
                default:
                    throw new Exception("Unsupported or unrecognized fill pattern: " + pattern.ToString());
            }
        }

        private string CreateRedlineLayerDefinitionContent(MgResourceIdentifier fsId, string className, RedlineStyle style, RedlineStylizationType type)
        {
            double transpc = (100 - style.FillTransparency) / 100;
            string hexFgTransparency = string.Format("{0:X2}", Convert.ToInt32(255 * transpc)); //Convert % to alpha value
            string hexBgTransparency = style.FillBackTransparency ? "FF" : "00"; //All or nothing
            if (type == RedlineStylizationType.Basic)
            {
                string template = Strings.markuplayerdefinition;
                return Util.Substitute(template, 
                    new string [] {
                        fsId.ToString(),						                    //<ResourceId> - Feature Source
                        className,                                                  //<FeatureName> - Class Name
                        style.LabelSizeUnits.ToString(),						    //<Unit> - Mark Label
                        style.LabelFontSize.ToString(CultureInfo.InvariantCulture),	//<SizeX> - Mark Label Size
                        style.LabelFontSize.ToString(CultureInfo.InvariantCulture),	//<SizeY> - Mark Label Size
                        "FF" + Util.ToHtmlColor(style.LabelForeColor),				//<ForegroundColor> - Mark Label
                        "FF" + Util.ToHtmlColor(style.LabelBackColor),				//<BackgroundColor> - Mark Label
                        style.LabelBackStyle.ToString(),						    //<BackgroundStyle> - Mark Label
                        style.LabelBold.ToString().ToLower(),						//<Bold> - Mark Label
                        style.LabelItalic.ToString().ToLower(),						//<Bold> - Mark Label
                        style.LabelUnderline.ToString().ToLower(),					//<Underlined> - Mark Label
                        style.MarkerSizeUnits.ToString(),						    //<Unit> - Mark
                        style.MarkerSize.ToString(CultureInfo.InvariantCulture),	//<SizeX> - Mark
                        style.MarkerSize.ToString(CultureInfo.InvariantCulture),	//<SizeY> - Mark
                        style.MarkerType.ToString(),							    //<Shape> - Mark
                        "FF" + Util.ToHtmlColor(style.MarkerColor),					//<ForegroundColor> - Mark
                        "FF" + Util.ToHtmlColor(style.MarkerColor),					//<Color> - Mark
                        style.LabelSizeUnits.ToString(),						    //<Unit> - Line Label
                        style.LabelFontSize.ToString(CultureInfo.InvariantCulture),	//<SizeX> - Line Label Size
                        style.LabelFontSize.ToString(CultureInfo.InvariantCulture),	//<SizeY> - Line Label Size
                        "FF" + Util.ToHtmlColor(style.LabelForeColor),				//<ForegroundColor> - Line Label
                        "FF" + Util.ToHtmlColor(style.LabelBackColor),				//<BackgroundColor> - Line Label
                        style.LabelBackStyle.ToString(),						    //<BackgroundStyle> - Line Label
                        style.LabelBold.ToString().ToLower(),                       //<Bold> - Line Label
                        style.LabelItalic.ToString().ToLower(),                     //<Bold> - Line Label
                        style.LabelUnderline.ToString().ToLower(),                  //<Underlined> - Line Label
                        style.LinePattern.ToString(),							    //<LineStyle> - Line
                        style.LineThickness.ToString(CultureInfo.InvariantCulture),	//<Thickness> - Line
                        "FF" + Util.ToHtmlColor(style.LineColor),					//<Color> - Line
                        style.LineSizeUnits.ToString(),						        //<Unit> - Line
                        style.LabelSizeUnits.ToString(),						    //<Unit> - Polygon Label
                        style.LabelFontSize.ToString(CultureInfo.InvariantCulture),	//<SizeX> - Polygon Label Size
                        style.LabelFontSize.ToString(CultureInfo.InvariantCulture),	//<SizeY> - Polygon Label Size
                        "FF" + Util.ToHtmlColor(style.LabelForeColor),				//<ForegroundColor> - Polygon Label
                        "FF" + Util.ToHtmlColor(style.LabelBackColor),				//<BackgroundColor> - Polygon Label
                        style.LabelBackStyle.ToString(),						    //<BackgroundStyle> - Polygon Label
                        style.LabelBold.ToString().ToLower(),						//<Bold> - Polygon Label
                        style.LabelItalic.ToString().ToLower(),						//<Bold> - Polygon Label
                        style.LabelUnderline.ToString().ToLower(),					//<Underlined> - Polygon Label
                        style.FillPattern.ToString(), 						        //<FillPattern> - Fill
                        hexFgTransparency + Util.ToHtmlColor(style.FillForeColor), 	//<ForegroundColor> - Fill
                        hexBgTransparency + Util.ToHtmlColor(style.FillBackColor), 	//<BackgroundColor> - Fill
                        style.BorderPattern.ToString(),						        //<LineStyle> - Fill
                        style.BorderThickness.ToString(CultureInfo.InvariantCulture),	//<Thickness> - Fill
                        "FF" + Util.ToHtmlColor(style.BorderColor), 					//<Color> - Fill
                        style.BorderSizeUnits.ToString()                                //<Unit> - Fill
                    }); 					            
            }
            else
            {
                StringBuilder template = new StringBuilder(Strings.markuplayerdefinition_advanced);
                double fontHeight = SizeInMM(style.LabelFontSize, style.LabelSizeUnits);
                string labelForeColor = "FF" + Util.ToHtmlColor(style.LabelForeColor);
                string labelBackColor = "FF" + Util.ToHtmlColor(style.LabelBackColor);

                //Substitute inner templates first, so their placeholders tokens are also brought in
                template.Replace("#{FILL_PATTERN_TEMPLATE}", GetFillPatternTemplate(style.FillPattern));
                template.Replace("#{BORDER_PATTERN_TEMPLATE}", GetBorderPatternGeometry(style.BorderPattern));

                //For non-opaque labels we need to empty the frame fill color
                if (style.LabelBackStyle == LabelStyle.Opaque)
                {
                    template.Replace("#{FRAME_FILL_COLOR}", "0xffffffff");
                }
                else
                {
                    template.Replace("#{FRAME_FILL_COLOR}", string.Empty);
                    if (style.LabelBackStyle == LabelStyle.Transparent)
                    {
                        labelBackColor = string.Empty;
                    }
                }

                //I don't think this is correct wrt to the UI, but this is to replicate the behaviour that we currently have under basic stylization
                //If fill is solid and background color is fully transparent, comment that portion out of the composite type style as it will interfere
                //with the area foreground symbol
                if (style.FillPattern == FillPattern.Solid && hexBgTransparency == "FF")
                {
                    template.Replace("#{START_BACKGROUND_FILL}", "<!--");
                    template.Replace("#{END_BACKGROUND_FILL}", "-->");
                }
                else
                {
                    template.Replace("#{START_BACKGROUND_FILL}", "");
                    template.Replace("#{END_BACKGROUND_FILL}", "");
                }

                //Then do a find/replace of all known tokens
                template.Replace("#{RESOURCE_ID}", fsId.ToString());
                template.Replace("#{FEATURE_CLASS}", className);
                template.Replace("#{FONT_HEIGHT}", fontHeight.ToString(CultureInfo.InvariantCulture));
                template.Replace("#{LABEL_FORE_COLOR}", labelForeColor);
                template.Replace("#{LABEL_BACK_COLOR}", labelBackColor);
                template.Replace("#{BOLD}", style.LabelBold.ToString().ToLower());
                template.Replace("#{ITALIC}", style.LabelItalic.ToString().ToLower());
                template.Replace("#{UNDERLINE}", style.LabelUnderline.ToString().ToLower());
                template.Replace("#{MARKER_GEOMETRY}", GetMarkerGeometry(style.MarkerType));
                template.Replace("#{MARKER_SIZE_X}", GetMarkerSize(style.MarkerSize, style.MarkerSizeUnits));
                template.Replace("#{MARKER_SIZE_Y}", GetMarkerSize(style.MarkerSize, style.MarkerSizeUnits));
                template.Replace("#{MARKER_COLOR}", "FF" + Util.ToHtmlColor(style.MarkerColor));
                template.Replace("#{LINE_PATTERN_GEOMETRY}", GetLinePatternGeometry(style.LinePattern));
                template.Replace("#{LINE_THICKNESS}", GetLineThickness(style.LineThickness, style.LineSizeUnits));
                template.Replace("#{LINE_COLOR}", "FF" + Util.ToHtmlColor(style.LineColor));
                template.Replace("#{FILL_BACK_COLOR}", hexBgTransparency + Util.ToHtmlColor(style.FillBackColor));
                template.Replace("#{FILL_FORE_COLOR}", hexFgTransparency + Util.ToHtmlColor(style.FillForeColor));
                template.Replace("#{BORDER_THICKNESS}", SizeInMM(style.BorderThickness, style.BorderSizeUnits).ToString(CultureInfo.InvariantCulture));
                template.Replace("#{BORDER_COLOR}", "FF" + Util.ToHtmlColor(style.BorderColor));

                //Fill in the UI values under ExtendedData, so we can read from this element if we need to go back to Edit Style UI
                template.Replace("#{MG_RESOURCE_ID}", fsId.ToString());
                template.Replace("#{MG_FEATURE_CLASS}", className);
                template.Replace("#{MG_FILL_PATTERN}", style.FillPattern.ToString());
                template.Replace("#{MG_BORDER_PATTERN}", style.BorderPattern.ToString());
                template.Replace("#{MG_LINE_PATTERN}", style.LinePattern.ToString());
                template.Replace("#{MG_LABEL_FONT_SIZE}", style.LabelFontSize.ToString(CultureInfo.InvariantCulture));
                template.Replace("#{MG_LABEL_FONT_UNITS}", style.LabelSizeUnits.ToString());
                template.Replace("#{MG_LABEL_FORECOLOR}", Util.ToHtmlColor(style.LabelForeColor));
                template.Replace("#{MG_LABEL_BACKCOLOR}", Util.ToHtmlColor(style.LabelBackColor));
                template.Replace("#{MG_BOLD}", style.LabelBold.ToString().ToLower());
                template.Replace("#{MG_ITALIC}", style.LabelItalic.ToString().ToLower());
                template.Replace("#{MG_UNDERLINE}", style.LabelUnderline.ToString().ToLower());
                template.Replace("#{MG_MARKER_TYPE}", style.MarkerType.ToString());
                template.Replace("#{MG_MARKER_SIZE}", style.MarkerSize.ToString(CultureInfo.InvariantCulture));
                template.Replace("#{MG_MARKER_UNITS}", style.MarkerSizeUnits.ToString());
                template.Replace("#{MG_MARKER_COLOR}", Util.ToHtmlColor(style.MarkerColor));
                template.Replace("#{MG_LINE_THICKNESS}", style.LineThickness.ToString(CultureInfo.InvariantCulture));
                template.Replace("#{MG_LINE_UNITS}", style.LineSizeUnits.ToString());
                template.Replace("#{MG_LINE_COLOR}", Util.ToHtmlColor(style.LineColor));
                template.Replace("#{MG_FILL_BACK_COLOR}", Util.ToHtmlColor(style.FillBackColor));
                template.Replace("#{MG_FILL_FORE_COLOR}", Util.ToHtmlColor(style.FillForeColor));
                template.Replace("#{MG_FILL_FORE_TRANSPARENCY}", hexFgTransparency);
                template.Replace("#{MG_FILL_BACK_TRANSPARENCY}", hexBgTransparency);
                template.Replace("#{MG_BORDER_THICKNESS}", style.BorderThickness.ToString(CultureInfo.InvariantCulture));
                template.Replace("#{MG_BORDER_UNITS}", style.BorderSizeUnits.ToString());
                template.Replace("#{MG_BORDER_COLOR}", Util.ToHtmlColor(style.BorderColor));
                template.Replace("#{MG_LABEL_STYLE}", style.LabelBackStyle.ToString());

                return template.ToString();
            }
        }

        static string GetLineThickness(double value, SizeUnit unit)
        {
            return SizeInMM(value, unit).ToString(CultureInfo.InvariantCulture);
        }

        static string GetLinePatternGeometry(LinePattern linePattern)
        {
            return GetBorderPatternGeometry(linePattern);
        }

        static string GetMarkerSize(double value, SizeUnit sizeUnit)
        {
            //Halve the resulting size to get approximate same size as it would look under basic stylization
            return (SizeInMM(value, sizeUnit) / 2.0).ToString(CultureInfo.InvariantCulture);
        }

        static string GetMarkerGeometry(MarkerType markerType)
        {
            /*
            Square:     M -1.0,-1.0 L 1.0,-1.0 L 1.0,1.0 L -1.0,1.0 L -1.0,-1.0
            Circle:     M -1,0 A 1,1 0 1 1 1,0 A 1,1 0 1 1 -1,0
            Triangle:   M -1.0,-1.0 h 2.0 l -1.0,2.0 z
            Star:       M -0.618033988749895,-0.951056516295154 L 0,-0.502028539715568 L 0.618033988749895,-0.951056516295154 L 0.381966011250105,-0.273457471994639 L 1,0.175570504584946 L 0.23606797749979,0.175570504584946 L 0,0.951056516295154 L -0.23606797749979,0.175570504584946 L -1,0.175570504584946 L -0.381966011250105,-0.273457471994639 L -0.618033988749895,-0.951056516295154
            Cross:      M -0.190983005625053,-1 L 0.190983005625053,-1 L 0.190983005625053,-0.190983005625053 L 1,-0.190983005625053 L 1,0.190983005625053 L 0.190983005625053,0.190983005625053 L 0.190983005625053,1 L -0.190983005625053,1 L -0.190983005625053,0.190983005625053 L -1,0.190983005625053 L -1,-0.190983005625053 L -0.190983005625053,-0.190983005625053 L -0.190983005625053,-1
            X:          M -0.459818486524547,-1 L 0,-0.540181513475453 L 0.459818486524547,-1 L 1,-0.459818486524547 L 0.540181513475453,0 L 1,0.459818486524547 L 0.459818486524547,1 L 0,0.540181513475453 L -0.459818486524547,1 L -1,0.459818486524547 L -0.540181513475453,0 L -1,-0.459818486524547 L -0.459818486524547,-1
            Star-Old:   M -0.707106781186548,0.707106781186548 L 0.707106781186548,-0.707106781186548 M -0.707106781186548,-0.707106781186548 L 0.707106781186548,0.707106781186548 M -1,0 L 1,0 M 0,-1 L 0,1
            Cross-Old:  M -1,0 L 1,0 M 0,-1 L 0,1
            X-Old:      M -0.707106781186548,0.707106781186548 L 0.707106781186548,-0.707106781186548 M -0.707106781186548,-0.707106781186548 L 0.707106781186548,0.707106781186548        
            */
            switch (markerType)
            {
                case MarkerType.Square:
                    return "M -1.0,-1.0 L 1.0,-1.0 L 1.0,1.0 L -1.0,1.0 L -1.0,-1.0";
                case MarkerType.Circle:
                    return "M -1,0 A 1,1 0 1 1 1,0 A 1,1 0 1 1 -1,0";
                case MarkerType.Triangle:
                    return "M -1.0,-1.0 h 2.0 l -1.0,2.0 z";
                case MarkerType.Star:
                    return "M -0.618033988749895,-0.951056516295154 L 0,-0.502028539715568 L 0.618033988749895,-0.951056516295154 L 0.381966011250105,-0.273457471994639 L 1,0.175570504584946 L 0.23606797749979,0.175570504584946 L 0,0.951056516295154 L -0.23606797749979,0.175570504584946 L -1,0.175570504584946 L -0.381966011250105,-0.273457471994639 L -0.618033988749895,-0.951056516295154";
                case MarkerType.Cross:
                    return "M -0.190983005625053,-1 L 0.190983005625053,-1 L 0.190983005625053,-0.190983005625053 L 1,-0.190983005625053 L 1,0.190983005625053 L 0.190983005625053,0.190983005625053 L 0.190983005625053,1 L -0.190983005625053,1 L -0.190983005625053,0.190983005625053 L -1,0.190983005625053 L -1,-0.190983005625053 L -0.190983005625053,-0.190983005625053 L -0.190983005625053,-1";
                case MarkerType.X:
                    return "M -0.459818486524547,-1 L 0,-0.540181513475453 L 0.459818486524547,-1 L 1,-0.459818486524547 L 0.540181513475453,0 L 1,0.459818486524547 L 0.459818486524547,1 L 0,0.540181513475453 L -0.459818486524547,1 L -1,0.459818486524547 L -0.540181513475453,0 L -1,-0.459818486524547 L -0.459818486524547,-1";
                default:
                    throw new Exception("Unrecognized or unsupported marker type: " + markerType.ToString());
            }
        }

        private MgResourceIdentifier GenerateRedlineFeatureSourceId(CreateRedlineLayerParams param)
        {
            return new MgResourceIdentifier(ResourceIdPrefix + param.Name + "." + MgResourceType.FeatureSource);
        }

        public void AddLayerToMap(RedlineLayer layer)
        {
            var ldfId = new MgResourceIdentifier(layer.LayerDefinition);
            AddRedlineLayerToMap(ldfId);
        }

        public void RemoveLayerFromMap(RedlineLayer layer)
        {
            var layers = _map.GetLayers();
            var lidx = layers.IndexOf("_" + layer.Name);
            if (lidx >= 0)
            {
                layers.RemoveAt(lidx);
            }
        }

        public RedlineLayer[] GetAvailableLayers()
        {
            return _registry.GetAvailableLayers(_map);
        }

        public const string GROUP_NAME = "_Markup";

        static string ResourceIdPrefix
        {
            get
            {
                return "Session:" + SessionID + "//";
            }
        }

        static T ParseEnumValue<T>(string value)
        {
            return (T)Enum.Parse(typeof(T), value);
        }

        static string GetNodeValue(XmlElement el, string name)
        {
            var child = el.GetElementsByTagName(name)[0];
            return child.InnerText;
        }

        public RedlineStyle GetLayerStyle(RedlineLayer layer)
        {
            var style = RedlineStyle.CreateDefault();

            var ldfId = new MgResourceIdentifier(layer.LayerDefinition);
            var br = _resSvc.GetResourceContent(ldfId);
            var doc = new XmlDocument();
            doc.LoadXml(br.ToString());

            if (layer.StyleType == RedlineStylizationType.Basic)
            {
                var vsr = doc.GetElementsByTagName("VectorScaleRange")[0];
                var pts = vsr["PointTypeStyle"];
                var ats = vsr["AreaTypeStyle"];
                var lts = vsr["LineTypeStyle"];

                if (pts != null)
                {
                    var label = (XmlElement)pts.GetElementsByTagName("Label")[0];
                    var psym = pts.GetElementsByTagName("PointSymbolization2D");
                    if (psym.Count > 0)
                    {
                        var mark = psym[0]["Mark"];

                        style.MarkerSizeUnits = ParseEnumValue<SizeUnit>(GetNodeValue(mark, "Unit"));
                        style.MarkerSize = Convert.ToDouble(GetNodeValue(mark, "SizeX"));
                        style.MarkerType = ParseEnumValue<MarkerType>(GetNodeValue(mark, "Shape"));
                        style.MarkerColor = Util.FromHtmlColor(GetNodeValue(mark, "ForegroundColor"), false);
                    }
                    style.LabelSizeUnits = ParseEnumValue<SizeUnit>(GetNodeValue(label, "Unit"));
                    style.LabelFontSize = Convert.ToDouble(GetNodeValue(label, "SizeX"));
                    style.LabelForeColor = Util.FromHtmlColor(GetNodeValue(label, "ForegroundColor"), false);
                    style.LabelBackColor = Util.FromHtmlColor(GetNodeValue(label, "BackgroundColor"), false);
                    style.LabelBackStyle = ParseEnumValue<LabelStyle>(GetNodeValue(label, "BackgroundStyle"));
                    style.LabelBold = Convert.ToBoolean(GetNodeValue(label, "Bold"));
                    style.LabelItalic = Convert.ToBoolean(GetNodeValue(label, "Italic"));
                    style.LabelUnderline = Convert.ToBoolean(GetNodeValue(label, "Underlined"));
                }
                if (lts != null)
                {
                    var label = (XmlElement)lts.GetElementsByTagName("Label")[0];
                    
                    style.LabelSizeUnits = ParseEnumValue<SizeUnit>(GetNodeValue(label, "Unit"));
                    style.LabelFontSize = Convert.ToDouble(GetNodeValue(label, "SizeX"));
                    style.LabelForeColor = Util.FromHtmlColor(GetNodeValue(label, "ForegroundColor"), false);
                    style.LabelBackColor = Util.FromHtmlColor(GetNodeValue(label, "BackgroundColor"), false);
                    style.LabelBackStyle = ParseEnumValue<LabelStyle>(GetNodeValue(label, "BackgroundStyle"));

                    if (pts == null)
                    {
                        style.LabelBold = Convert.ToBoolean(GetNodeValue(label, "Bold"));
                        style.LabelItalic = Convert.ToBoolean(GetNodeValue(label, "Italic"));
                        style.LabelUnderline = Convert.ToBoolean(GetNodeValue(label, "Underlined"));
                    }

                    var lsym = lts.GetElementsByTagName("LineSymbolization2D");
                    if (lsym.Count > 0)
                    {
                        var line = (XmlElement)lsym[0];
                        style.LinePattern = ParseEnumValue<LinePattern>(GetNodeValue(line, "LineStyle"));
                        style.LineThickness = Convert.ToDouble(GetNodeValue(line, "Thickness"));
                        style.LineColor = Util.FromHtmlColor(GetNodeValue(line, "Color"), false);
                        style.LineSizeUnits = ParseEnumValue<SizeUnit>(GetNodeValue(line, "Unit"));
                    }
                }
                if (ats != null)
                {
                    var label = (XmlElement)ats.GetElementsByTagName("Label")[0];

                    style.LabelSizeUnits = ParseEnumValue<SizeUnit>(GetNodeValue(label, "Unit"));
                    style.LabelFontSize = Convert.ToDouble(GetNodeValue(label, "SizeX"));
                    style.LabelForeColor = Util.FromHtmlColor(GetNodeValue(label, "ForegroundColor"), false);
                    style.LabelBackColor = Util.FromHtmlColor(GetNodeValue(label, "BackgroundColor"), false);
                    style.LabelBackStyle = ParseEnumValue<LabelStyle>(GetNodeValue(label, "BackgroundStyle"));

                    if (pts == null && lts == null)
                    {
                        style.LabelBold = Convert.ToBoolean(GetNodeValue(label, "Bold"));
                        style.LabelItalic = Convert.ToBoolean(GetNodeValue(label, "Italic"));
                        style.LabelUnderline = Convert.ToBoolean(GetNodeValue(label, "Underlined"));
                    }

                    var asym = ats.GetElementsByTagName("AreaSymbolization2D");
                    if (asym.Count > 0)
                    {
                        var fill = asym[0]["Fill"];
                        var stroke = asym[0]["Stroke"];

                        style.FillPattern = ParseEnumValue<FillPattern>(GetNodeValue(fill, "FillPattern"));
                        style.FillForeColor = Util.FromHtmlColor(GetNodeValue(fill, "ForegroundColor"), false);
                        style.FillBackColor = Util.FromHtmlColor(GetNodeValue(fill, "BackgroundColor"), false);

                        style.BorderPattern = ParseEnumValue<LinePattern>(GetNodeValue(stroke, "LineStyle"));
                        style.BorderThickness = Convert.ToDouble(GetNodeValue(stroke, "Thickness"));
                        style.BorderColor = Util.FromHtmlColor(GetNodeValue(stroke, "Color"));
                        style.BorderSizeUnits = ParseEnumValue<SizeUnit>(GetNodeValue(stroke, "Unit"));
                    }
                }
            }
            else
            {
                var extData = doc.GetElementsByTagName("ExtendedData1");
                if (extData.Count == 1)
                {
                    var ext = (XmlElement)extData[0];
                    style.MarkerColor = Util.FromHtmlColor(GetNodeValue(ext, "MG_MARKER_COLOR"));
                    style.MarkerType = ParseEnumValue<MarkerType>(GetNodeValue(ext, "MG_MARKER_TYPE"));
                    style.MarkerSizeUnits = ParseEnumValue<SizeUnit>(GetNodeValue(ext, "MG_MARKER_UNITS"));
                    style.MarkerSize = Convert.ToDouble(GetNodeValue(ext, "MG_MARKER_SIZE"));
                    style.LineColor = Util.FromHtmlColor(GetNodeValue(ext, "MG_LINE_COLOR"));
                    style.LinePattern = ParseEnumValue<LinePattern>(GetNodeValue(ext, "MG_LINE_PATTERN"));
                    style.LineSizeUnits = ParseEnumValue<SizeUnit>(GetNodeValue(ext, "MG_LINE_UNITS"));
                    style.LineThickness = Convert.ToDouble(GetNodeValue(ext, "MG_LINE_THICKNESS"));
                    style.FillPattern = ParseEnumValue<FillPattern>(GetNodeValue(ext, "MG_FILL_PATTERN"));
                    style.FillTransparency = 100 - ((Convert.ToInt32(GetNodeValue(ext, "MG_FILL_FORE_TRANSPARENCY"), 16) / 255.0) * 100.0);
                    style.FillForeColor = Util.FromHtmlColor(GetNodeValue(ext, "MG_FILL_FORE_COLOR"));
                    style.FillBackColor = Util.FromHtmlColor(GetNodeValue(ext, "MG_FILL_BACK_COLOR"));
                    style.FillBackTransparency = (GetNodeValue(ext, "MG_FILL_BACK_TRANSPARENCY") == "FF");
                    style.BorderPattern = ParseEnumValue<LinePattern>(GetNodeValue(ext, "MG_BORDER_PATTERN"));
                    style.BorderSizeUnits = ParseEnumValue<SizeUnit>(GetNodeValue(ext, "MG_BORDER_UNITS"));
                    style.BorderColor = Util.FromHtmlColor(GetNodeValue(ext, "MG_BORDER_COLOR"));
                    style.BorderThickness = Convert.ToDouble(GetNodeValue(ext, "MG_BORDER_THICKNESS"));
                    style.LabelSizeUnits = ParseEnumValue<SizeUnit>(GetNodeValue(ext, "MG_LABEL_FONT_UNITS"));
                    style.LabelFontSize = Convert.ToDouble(GetNodeValue(ext, "MG_LABEL_FONT_SIZE"));
                    style.LabelBold = Convert.ToBoolean(GetNodeValue(ext, "MG_BOLD"));
                    style.LabelItalic = Convert.ToBoolean(GetNodeValue(ext, "MG_ITALIC"));
                    style.LabelUnderline = Convert.ToBoolean(GetNodeValue(ext, "MG_UNDERLINE"));
                    style.LabelForeColor = Util.FromHtmlColor(GetNodeValue(ext, "MG_LABEL_FORECOLOR"));
                    style.LabelBackColor = Util.FromHtmlColor(GetNodeValue(ext, "MG_LABEL_BACKCOLOR"));
                    style.LabelBackStyle = ParseEnumValue<LabelStyle>(GetNodeValue(ext, "MG_LABEL_STYLE"));
                }
            }

            return style;
        }

        public void DeleteLayer(RedlineLayer layer)
        {
            _registry.DeleteLayer(layer);
        }

        internal void UpdateLayerStyle(RedlineLayer layer, RedlineStyle updatedStyle)
        {
            //HACK: SQLite leaky abstraction (hard-coded schema name), SHP probably has some leaks of its own, so we can't assume MarkupSchema:Markup
            //as the class name interrogate our schema to figure it out
            MgResourceIdentifier fsId = new MgResourceIdentifier(layer.FeatureSource);

            MgFeatureSchemaCollection schemas = _featSvc.DescribeSchema(fsId, string.Empty, null);
            MgFeatureSchema schema = schemas.GetItem(0);
            MgClassDefinitionCollection classes = schema.GetClasses();
            MgClassDefinition cls = classes.GetItem(0);
            string className = schema.Name + ":" + cls.Name;

            MgResourceIdentifier ldfId = new MgResourceIdentifier(layer.LayerDefinition);

            string layerDefContent = CreateRedlineLayerDefinitionContent(fsId, className, updatedStyle, layer.StyleType);
            byte[] bytes = Encoding.UTF8.GetBytes(layerDefContent);
            MgByteSource byteSource = new MgByteSource(bytes, bytes.Length);
            MgByteReader byteReader = byteSource.GetReader();

            _resSvc.SetResource(ldfId, byteReader, null);
        }
    }

    public enum RedlineStylizationType : int
    {
        Basic = 1,
        Advanced = 2
    }

    internal class CreateRedlineLayerParams
    {
        public string Name { get; set; }
        public int GeometryTypes { get; set; }
        public RedlineDataStoreFormat Format { get; set; }
        public RedlineStyle Style { get; set; }
        public bool AddToMap { get; set; }
        public RedlineStylizationType StyleType { get; set; }
    }

    internal enum RedlineGeometryType
    {
        Point,
        Line,
        LineString,
        Polygon,
        Circle,
        Rectangle
    }

    public enum RedlineDataStoreFormat
    {
        SDF,
        SHP,
        SQLite
    }

    internal enum LabelStyle
    {
        Ghosted,
        Opaque,
        Transparent
    }

    internal enum LinePattern
    {
        Solid,
        Dash,
        Dot,
        DashDot,
        DashDotDot,
        Rail,
        BORDER,
        DIVIDE,
        FENCELINE1
    }

    internal enum FillPattern
    {
        Solid,
        Net,
        Line,
        Line_45,
        Line_90,
        Line_135,
        Square,
        Box,
        Cross,
        Dash,
        Dolmit,
        Hex,
        Sacncr,
        Steel
    }

    internal enum MarkerType
    {
        Square,
        Circle,
        Triangle,
        Star,
        Cross,
        X
    }

    internal enum SizeUnit
    {
        Points,
        Inches,
        Millimeters,
        Centimeters,
        Meters
    }

    internal class RedlineStyle
    {
        public Color MarkerColor { get; set; }
        public MarkerType MarkerType { get; set; }
        public double MarkerSize { get; set; }
        public SizeUnit MarkerSizeUnits { get; set; }
        public Color LineColor { get; set; }
        public LinePattern LinePattern { get; set; }
        public SizeUnit LineSizeUnits { get; set; }
        public double LineThickness { get; set; }
        public FillPattern FillPattern { get; set; }
        public double FillTransparency { get; set; }
        public Color FillForeColor { get; set; }
        public Color FillBackColor { get; set; }
        public bool FillBackTransparency { get; set; }
        public LinePattern BorderPattern { get; set; }
        public double BorderThickness { get; set; }
        public SizeUnit BorderSizeUnits { get; set; }
        public Color BorderColor { get; set; }
        public SizeUnit LabelSizeUnits { get; set; }
        public double LabelFontSize { get; set; }
        public bool LabelBold { get; set; }
        public bool LabelItalic { get; set; }
        public bool LabelUnderline { get; set; }
        public Color LabelForeColor { get; set; }
        public Color LabelBackColor { get; set; }
        public LabelStyle LabelBackStyle { get; set; }

        internal static RedlineStyle CreateDefault()
        {
            return new RedlineStyle()
            {
                BorderColor = Color.Red,
                BorderPattern = LinePattern.Solid,
                BorderSizeUnits = SizeUnit.Points,
                BorderThickness = 0,
                FillBackColor = Color.Black,
                FillBackTransparency = true,
                FillForeColor = Color.Red,
                FillPattern = FillPattern.Solid,
                FillTransparency = 35,
                LabelBackColor = Color.White,
                LabelBackStyle = LabelStyle.Opaque,
                LabelBold = false,
                LabelFontSize = 10,
                LabelForeColor = Color.Black,
                LabelItalic = false,
                LabelSizeUnits = SizeUnit.Points,
                LabelUnderline = false,
                LineColor = Color.Red,
                LinePattern = LinePattern.Solid,
                LineSizeUnits = SizeUnit.Centimeters,
                LineThickness = 0,
                MarkerColor = Color.Red,
                MarkerSize = 10,
                MarkerSizeUnits = SizeUnit.Points,
                MarkerType = MarkerType.Square
            };
        }
    }

    internal class RedlineLayer
    {
        public string Name { get; private set; }

        public string SystemName { get { return "_" + this.Name; } }

        public string FeatureSource { get; private set; }

        public string LayerDefinition { get; private set; }

        public int GeometryTypes { get; private set; }

        public RedlineStylizationType StyleType { get; private set; }

        public RedlineLayer(string name, string fsId, string layerDef, int geomTypes, RedlineStylizationType styleType)
        {
            this.Name = name;
            this.FeatureSource = fsId;
            this.LayerDefinition = layerDef;
            this.GeometryTypes = geomTypes;
            this.StyleType = styleType;
        }
    }

    internal class RedlineObject
    {
        public RedlineObject(int id, string text)
        {
            this.ID = id;
            this.Text = text;
        }

        public int ID { get; private set; }

        public string Text { get; private set; }
    }
}
