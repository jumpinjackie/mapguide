<%--
  Copyright (C) 2004-2011 by Autodesk, Inc.
  This library is free software; you can redistribute it and/or
  modify it under the terms of version 2.1 of the GNU Lesser
  General Public License as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--%>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Xml" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<!-- #Include File="../common/common.aspx" -->
<script language="C#" runat="server">
    
    public class BufferHelper
    {
        private HttpServerUtility _server;

        public BufferHelper(HttpServerUtility server) 
        { 
            _server = server;
        }
        
        HttpServerUtility Server { get { return _server; } }
    
        public void CreateBufferFeatureSource(MgFeatureService featureService, String wkt, MgResourceIdentifier bufferFeatureResId)
        {
            MgClassDefinition bufferClass = new MgClassDefinition();
            bufferClass.SetName("BufferClass");
            MgPropertyDefinitionCollection properties = bufferClass.GetProperties();

            MgDataPropertyDefinition idProperty = new MgDataPropertyDefinition("ID");
            idProperty.SetDataType(MgPropertyType.Int32);
            idProperty.SetReadOnly(true);
            idProperty.SetNullable(false);
            idProperty.SetAutoGeneration(true);
            properties.Add(idProperty);

            MgGeometricPropertyDefinition polygonProperty = new MgGeometricPropertyDefinition("BufferGeometry");
            polygonProperty.SetGeometryTypes(MgFeatureGeometricType.Surface);
            polygonProperty.SetHasElevation(false);
            polygonProperty.SetHasMeasure(false);
            polygonProperty.SetReadOnly(false);
            polygonProperty.SetSpatialContextAssociation("defaultSrs");
            properties.Add(polygonProperty);

            MgPropertyDefinitionCollection idProperties = bufferClass.GetIdentityProperties();
            idProperties.Add(idProperty);

            bufferClass.SetDefaultGeometryPropertyName("BufferGeometry");

            MgFeatureSchema bufferSchema = new MgFeatureSchema("BufferLayerSchema", "temporary schema to hold a buffer");
            bufferSchema.GetClasses().Add(bufferClass);

            MgCreateSdfParams sdfParams = new MgCreateSdfParams("defaultSrs", wkt, bufferSchema);

            featureService.CreateFeatureSource(bufferFeatureResId, sdfParams);
        }

        public MgLayer CreateBufferLayer(MgResourceService resourceService, MgResourceIdentifier bufferFeatureResId, String sessionId)
        {
            // Load the layer definition template into
            // a XmlDocument object, find the "ResourceId" element, and
            // modify its content to reference the temporary
            // feature source.

            XmlDocument doc = new XmlDocument();
            doc.Load(Server.MapPath("bufferlayerdefinition.xml"));
            XmlNode featureSourceNode = doc.GetElementsByTagName("ResourceId")[0];
            featureSourceNode.InnerText = bufferFeatureResId.ToString();

            // Get the updated layer definition from the XmlDocument
            // and save it to the session repository using the
            // ResourceService object.

            MgByteSource byteSource = null;
            using (MemoryStream ms = new MemoryStream())
            {
                doc.Save(ms);
                ms.Position = 0L;
                
                //Note we do this to ensure our XML content is free of any BOM characters
                byte [] layerDefinition = ms.ToArray();
                Encoding utf8 = Encoding.UTF8;
                String layerDefStr = new String(utf8.GetChars(layerDefinition));
                layerDefinition = new byte[layerDefStr.Length-1];
                int byteCount = utf8.GetBytes(layerDefStr, 1, layerDefStr.Length-1, layerDefinition, 0);
                
                byteSource = new MgByteSource(layerDefinition, layerDefinition.Length);
                byteSource.SetMimeType(MgMimeType.Xml);
            }

            MgResourceIdentifier tempLayerResId = new MgResourceIdentifier("Session:" + sessionId + "//Buffer.LayerDefinition");

            resourceService.SetResource(tempLayerResId, byteSource.GetReader(), null);

            // Create an MgLayer object based on the new layer definition
            // and return it to the caller.

            MgLayer bufferLayer = new MgLayer(tempLayerResId, resourceService);
            bufferLayer.SetName("Buffer");
            bufferLayer.SetLegendLabel("Buffer");
            bufferLayer.SetDisplayInLegend(true);
            bufferLayer.SetSelectable(false);
            
            return bufferLayer;
        }

        public void CreateParcelMarkerFeatureSource(MgFeatureService featureService, String wkt, MgResourceIdentifier parcelMarkerDataResId)
        {
            MgClassDefinition parcelClass = new MgClassDefinition();
            parcelClass.SetName("ParcelMarkerClass");
            MgPropertyDefinitionCollection properties = parcelClass.GetProperties();

            MgDataPropertyDefinition idProperty = new MgDataPropertyDefinition("ID");
            idProperty.SetDataType(MgPropertyType.Int32);
            idProperty.SetReadOnly(true);
            idProperty.SetNullable(false);
            idProperty.SetAutoGeneration(true);
            properties.Add(idProperty);

            MgGeometricPropertyDefinition pointProperty = new MgGeometricPropertyDefinition("ParcelLocation");
            pointProperty.SetGeometryTypes(MgGeometryType.Point);
            pointProperty.SetHasElevation(false);
            pointProperty.SetHasMeasure(false);
            pointProperty.SetReadOnly(false);
            pointProperty.SetSpatialContextAssociation("defaultSrs");
            properties.Add(pointProperty);

            MgPropertyDefinitionCollection idProperties = parcelClass.GetIdentityProperties();
            idProperties.Add(idProperty);

            parcelClass.SetDefaultGeometryPropertyName("ParcelLocation");

            MgFeatureSchema parcelSchema = new MgFeatureSchema("ParcelLayerSchema", "temporary schema to hold parcel markers");
            parcelSchema.GetClasses().Add(parcelClass);

            MgCreateSdfParams sdfParams = new MgCreateSdfParams("defaultSrs", wkt, parcelSchema);

            featureService.CreateFeatureSource(parcelMarkerDataResId, sdfParams);
        }

        public MgLayer CreateParcelMarkerLayer(MgResourceService resourceService, MgResourceIdentifier parcelMarkerDataResId, String sessionId)
        {
            // Load the ParcelMarker layer definition template into
            // a XmlDocument object, find the "ResourceId" element, and
            // modify its content to reference the temporary
            // feature source.

            XmlDocument doc = new XmlDocument();
            doc.Load(Server.MapPath("parcelmarker.xml"));
            XmlNode featureSourceNode = doc.GetElementsByTagName("ResourceId")[0];
            featureSourceNode.InnerText = parcelMarkerDataResId.ToString();

            // Get the updated layer definition from the DOM object
            // and save it to the session repository using the
            // ResourceService object.

            MgByteSource byteSource = null;
            using (MemoryStream ms = new MemoryStream())
            {
                doc.Save(ms);
                ms.Position = 0L;
                
                //Note we do this to ensure our XML content is free of any BOM characters
                byte [] layerDefinition = ms.ToArray();
                Encoding utf8 = Encoding.UTF8;
                String layerDefStr = new String(utf8.GetChars(layerDefinition));
                layerDefinition = new byte[layerDefStr.Length-1];
                int byteCount = utf8.GetBytes(layerDefStr, 1, layerDefStr.Length-1, layerDefinition, 0);
                
                byteSource = new MgByteSource(layerDefinition, layerDefinition.Length);
                byteSource.SetMimeType(MgMimeType.Xml);
            }

            MgResourceIdentifier tempLayerResId = new MgResourceIdentifier("Session:" + sessionId + "//ParcelMarker.LayerDefinition");

            resourceService.SetResource(tempLayerResId, byteSource.GetReader(), null);

            // Create an MgLayer object based on the new layer definition
            // and return it to the caller.

            MgLayer parcelMarkerLayer = new MgLayer(tempLayerResId, resourceService);
            parcelMarkerLayer.SetName("ParcelMarker");
            parcelMarkerLayer.SetLegendLabel("ParcelMarker");
            parcelMarkerLayer.SetDisplayInLegend(true);
            parcelMarkerLayer.SetSelectable(false);

            return parcelMarkerLayer;
        }
    }
    
</script>