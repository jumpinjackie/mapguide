using System;
using System.Collections.Generic;
using System.Text;
using System.Globalization;
using System.Drawing;

namespace OSGeo.MapGuide.Viewer
{
    internal static class Util
    {
        public static string ToHtmlColor(Color color)
        {
            return String.Format("{0:X2}{1:X2}{2:X2}", color.R, color.G, color.B);
        }

        public static Color FromHtmlColor(string html)
        {
            int rgb = int.Parse(html, System.Globalization.NumberStyles.HexNumber);
            return Color.FromArgb(rgb);
        }

        public static MgLayerBase FindLayer(MgLayerCollection layers, String layerName)
        {
            MgLayerBase layer = null;
            int i = 0;
            for (i = 0; i < layers.GetCount(); i++)
            {
                MgLayerBase layer1 = layers.GetItem(i);

                if (layer1.GetName() == layerName)
                {
                    layer = layer1;
                    break;
                }
            }
            return layer;
        }

        public static string GetMapSrs(MgMapBase map)
        {
            try
            {
                String srs = map.GetMapSRS();
                if (srs != "")
                    return srs;
            }
            catch (MgException e)
            {
            }

            //No SRS, set to ArbitrayXY meters
            //
            return "LOCALCS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
        }

        public static void ClearDataSource(MgFeatureService featSvc, MgResourceIdentifier fsId, String featureName)
        {
            MgDeleteFeatures deleteCmd = new MgDeleteFeatures(featureName, "ID >= 0");
            MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
            commands.Add(deleteCmd);
            featSvc.UpdateFeatures(fsId, commands, false);
        }

        public static void ReleaseReader(MgPropertyCollection res, MgFeatureCommandCollection commands)
        {
            if (res == null)
                return;

            for (int i = 0; i < res.GetCount(); i++)
            {
                MgFeatureCommand cmd = commands.GetItem(i);
                if (cmd is MgInsertFeatures)
                {
                    MgFeatureProperty resProp = res.GetItem(i) as MgFeatureProperty;
                    if (resProp != null)
                    {
                        MgFeatureReader reader = resProp.GetValue() as MgFeatureReader;
                        if (reader == null)
                            return;
                        reader.Close();
                    }
                }
            }
        }

        public static void AddFeatureToCollection(MgBatchPropertyCollection propCollection, MgAgfReaderWriter agfRW, int featureId, MgGeometry featureGeom)
        {
            MgPropertyCollection bufferProps = new MgPropertyCollection();
            MgInt32Property idProp = new MgInt32Property("ID", featureId);
            bufferProps.Add(idProp);
            MgByteReader geomReader = agfRW.Write(featureGeom);
            MgGeometryProperty geomProp = new MgGeometryProperty("GEOM", geomReader);
            bufferProps.Add(geomProp);
            propCollection.Add(bufferProps);
        }

        public static String GetFeaturePropertyValue(MgReader featureReader, String propName)
        {
            String value = "";
            int propertyType = featureReader.GetPropertyType(propName);
            switch (propertyType)
            {
                case MgPropertyType.Boolean:
                    value = featureReader.GetBoolean(propName).ToString();
                    break;
                case MgPropertyType.Byte:
                    value = featureReader.GetByte(propName).ToString();
                    break;
                case MgPropertyType.Single:
                    value = featureReader.GetSingle(propName).ToString();
                    break;
                case MgPropertyType.Double:
                    value = featureReader.GetDouble(propName).ToString();
                    break;
                case MgPropertyType.Int16:
                    value = featureReader.GetInt16(propName).ToString();
                    break;
                case MgPropertyType.Int32:
                    value = featureReader.GetInt32(propName).ToString();
                    break;
                case MgPropertyType.Int64:
                    value = featureReader.GetInt64(propName).ToString();
                    break;
                case MgPropertyType.String:
                    value = featureReader.GetString(propName);
                    break;
                case MgPropertyType.DateTime:
                case MgPropertyType.Null:
                case MgPropertyType.Blob:
                case MgPropertyType.Clob:
                case MgPropertyType.Feature:
                case MgPropertyType.Geometry:
                case MgPropertyType.Raster:
                    value = "[unsupported data type]";
                    break;
            }
            return value;
        }

        public static String MakeUniqueLayerName(MgMapBase map, String layerName, String themeName)
        {
            String desiredName = "_" + layerName + themeName;
            String uniqueName = desiredName;
            int index = 1;

            var layers = map.GetLayers();
            while (layers.Contains(uniqueName))
            {
                uniqueName = desiredName + index.ToString();
                index++;
            }
            return uniqueName;
        }

        public static String InterpolateColor(double portion, String startColor, String endColor, int percentTransparent)
        {
            int alpha = (int)(255 * (100.0 - percentTransparent) / 100.0);
            String result = "";
            if (startColor.Equals(endColor))
            {
                result = String.Format("{0:X2}{1}", alpha, startColor);
            }
            else
            {
                int red = CalculateRGB(portion, startColor.Substring(0, 2), endColor.Substring(0, 2));
                int green = CalculateRGB(portion, startColor.Substring(2, 2), endColor.Substring(2, 2));
                int blue = CalculateRGB(portion, startColor.Substring(4, 2), endColor.Substring(4, 2));
                result = String.Format("{0:X2}{1:X2}{2:X2}{3:X2}", alpha, red, green, blue);
            }
            return result;
        }

        public static int CalculateRGB(double portion, String startRGB, String endRGB)
        {
            double result = Int32.Parse(startRGB, NumberStyles.HexNumber) + portion * (Int32.Parse(endRGB, NumberStyles.HexNumber) - Int32.Parse(startRGB, NumberStyles.HexNumber));
            return (int)result;
        }
    }
}
