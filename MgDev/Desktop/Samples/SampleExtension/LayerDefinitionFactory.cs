using System;
using System.Collections.Generic;
using System.Text;

namespace SampleExtension
{
    public class LayerDefinitionFactory
    {
        //Creates Area Rule
        //Parameters:
        // legendLabel - string for the legend label
        // filterText - filter string
        // fillColor - fill color
        public String CreateAreaRule(String legendLabel, String filterText, String fillColor)
        {
            String areaRule = Layers.arearule;
            areaRule = TemplateUtil.Substitute(areaRule, legendLabel, filterText, fillColor);
            return areaRule;
        }

        //Creates AreaTypeStyle.
        //Parameters:
        //areaRules - call CreateAreaRule to create area rules
        public String CreateAreaTypeStyle(String areaRules)
        {
            String style = Layers.areatypestyle;
            style = TemplateUtil.Substitute(style, areaRules);
            return style;
        }

        //Creates line rule
        //Parameters:
        //color - color code for the line
        //legendLabel - string for the legend label
        //filter - filter string
        public String CreateLineRule(String legendLabel, String filter, String color)
        {
            String lineRule = Layers.linerule;
            lineRule = TemplateUtil.Substitute(lineRule, legendLabel, filter, color);
            return lineRule;
        }

        //Creates LineTypeStyle
        //Parameters:
        //lineRules - call CreateLineRule to create line rules
        public String CreateLineTypeStyle(String lineRules)
        {
            String lineStyle = Layers.linetypestyle;
            lineStyle = TemplateUtil.Substitute(lineStyle, lineRules);
            return lineStyle;
        }

        //Creates mark symbol
        //Parameters:
        //resourceId - resource identifier for the resource to be used
        //symbolName - the name of the symbol
        //width - the width of the symbol
        //height - the height of the symbol
        //color - color code for the symbol color
        public String CreateMarkSymbol(String resourceId, String symbolName, String width, String height, String color)
        {
            String markSymbol = Layers.marksymbol;
            markSymbol = TemplateUtil.Substitute(markSymbol, width, height, resourceId, symbolName, color);
            return markSymbol;
        }

        //Creates text symbol
        //Parameters:
        //text - string for the text
        //fontHeight - the height for the font
        //TODO:Can we pass it as a integer (ex. 10) or string (ex"10")
        //foregroundColor - color code for the foreground color
        public String CreateTextSymbol(String text, String fontHeight, String foregroundColor)
        {
            String textSymbol = Layers.textsymbol;
            textSymbol = TemplateUtil.Substitute(textSymbol, fontHeight, fontHeight, text, foregroundColor);
            return textSymbol;
        }

        //Creates a point rule
        //Parameters:
        //pointSym - point symbolization. Use CreateMarkSymbol to create it
        //legendlabel - string for the legend label
        //filter - string for the filter
        //label - use CreateTextSymbol to create it
        public String CreatePointRule(String legendLabel, String filter, String label, String pointSym)
        {
            String pointRule = Layers.pointrule;
            pointRule = TemplateUtil.Substitute(pointRule, legendLabel, filter, label, pointSym);
            return pointRule;
        }

        //Creates PointTypeStyle
        //Parameters:
        //pointRule - use CreatePointRule to define rules
        public String CreatePointTypeStyle(String pointRule)
        {
            String pointTypeStyle = Layers.pointtypestyle;
            pointTypeStyle = TemplateUtil.Substitute(pointTypeStyle, pointRule);
            return pointTypeStyle;
        }

        //Creates ScaleRange
        //Parameterss
        //minScale - minimum scale
        //maxScale - maximum scale
        //typeStyle - use one CreateAreaTypeStyle, CreateLineTypeStyle, or CreatePointTypeStyle
        public String CreateScaleRange(String minScale, String maxScale, String typeStyle)
        {
            String scaleRange = Layers.scalerange;
            scaleRange = TemplateUtil.Substitute(scaleRange, minScale, maxScale, typeStyle);
            return scaleRange;
        }

        //Creates a layer definition
        //resourceId - resource identifier for the new layer
        //featureClass - the name of the feature class
        //geometry - the name of the geometry
        //featureClassRange - use CreateScaleRange to define it.
        public String CreateLayerDefinition(String resourceId, String featureClass, String geometry, String featureClassRange)
        {
            String layerDef = Layers.layerdefinition;
            layerDef = TemplateUtil.Substitute(layerDef, resourceId, featureClass, geometry, featureClassRange);
            return layerDef;
        }
    }


    public class TemplateUtil
    {
        public static String Substitute(String templ, params String[] vals)
        {
            StringBuilder res = new StringBuilder();
            int index = 0, val = 0;
            bool found;
            do
            {
                found = false;
                int i = templ.IndexOf('%', index);
                if (i != -1)
                {
                    found = true;
                    res.Append(templ.Substring(index, i - index));
                    if (i < templ.Length - 1)
                    {
                        if (templ[i + 1] == '%')
                            res.Append('%');
                        else if (templ[i + 1] == 's')
                            res.Append(vals[val++]);
                        else
                            res.Append('@');    //add a character illegal in jscript so we know the template was incorrect
                        index = i + 2;
                    }
                }
            } while (found);
            res.Append(templ.Substring(index));
            return res.ToString();
        }
    }
}
