using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide;
using OSGeo.MapGuide.Viewer.AppLayoutEngine;

namespace SampleExtension
{
    public partial class AnalyzingFeatures : MgControlView
    {
        private IMapViewer _viewer;

        public AnalyzingFeatures(IMapViewer viewer)
        {
            InitializeComponent();
            this.Title = "Analyzing Features";
            _viewer = viewer;
        }

        private void btnCreateBuffer_Click(object sender, EventArgs e)
        {
            MgSelectionBase selection = _viewer.GetSelection();
            MgReadOnlyLayerCollection layers = selection.GetLayers();
            if (layers == null)
            {
                MessageBox.Show("Select a parcel");
                return;
            }
            MgLayerBase parcels = null;
            for (int i = 0; i < layers.GetCount(); i++)
            {
                MgLayerBase layer = layers.GetItem(i);
                if (layer.Name == "Parcels")
                {
                    parcels = layer;
                    break;
                }
            }
            if (parcels == null)
            {
                MessageBox.Show("Select a parcel");
                return;
            }

            int bufferRingSize = 100; // measured in metres
            int bufferRingCount = 5;

            // Set up some objects for coordinate conversion
            MgMapBase map = _viewer.GetMap();
            MgLayerCollection mapLayers = map.GetLayers();
            MgMapViewerProvider provider = _viewer.GetProvider();
            MgResourceService resourceService = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);
            //Casting to MgdFeatureService because we want to use convenience APIs
            MgFeatureService featureService = (MgdFeatureService)provider.CreateService(MgServiceType.FeatureService);

            String mapWktSrs = map.GetMapSRS();
            MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
            MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
            MgCoordinateSystemFactory coordinateSystemFactory = new MgCoordinateSystemFactory();
            MgCoordinateSystem srs = coordinateSystemFactory.Create(mapWktSrs);
            MgMeasure srsMeasure = srs.GetMeasure();
            string sessionId = Guid.NewGuid().ToString();

            BufferHelper helper = new BufferHelper();

            // Check for a buffer layer. If it exists, delete
            // the current features.
            // If it does not exist, create a feature source and
            // a layer to hold the buffer.

            MgdLayer bufferLayer = null;
            int layerIndex = mapLayers.IndexOf("Buffer");
            if (layerIndex < 0)
            {
                // The layer does not exist and must be created.

                MgResourceIdentifier bufferFeatureResId = new MgResourceIdentifier("Session:" + sessionId + "//Buffer.FeatureSource");
                helper.CreateBufferFeatureSource(featureService, mapWktSrs, bufferFeatureResId);
                bufferLayer = helper.CreateBufferLayer(resourceService, bufferFeatureResId, sessionId);
                mapLayers.Insert(0, bufferLayer);
            }
            else
            {
                bufferLayer = (MgdLayer)map.GetLayers().GetItem(layerIndex);
                bufferLayer.DeleteFeatures("ID like '%'");
            }

            // Get the selected features from the MgSelection object
            MgFeatureReader featureReader = selection.GetSelectedFeatures(parcels, parcels.GetFeatureClassName(), false);

            // Process each item in the MgFeatureReader. Get the
            // geometries from all the selected features and
            // merge them into a single geometry.

            MgGeometryCollection inputGeometries = new MgGeometryCollection();
            while (featureReader.ReadNext())
            {
                MgByteReader featureGeometryData = featureReader.GetGeometry(parcels.GetFeatureGeometryName());
                MgGeometry featureGeometry = agfReaderWriter.Read(featureGeometryData);

                inputGeometries.Add(featureGeometry);
            }

            MgGeometryFactory geometryFactory = new MgGeometryFactory();
            MgGeometry mergedGeometries = geometryFactory.CreateMultiGeometry(inputGeometries);

            // Add buffer features to the temporary feature source.
            // Create multiple concentric buffers to show area.
            // If the stylization for the layer draws the features
            // partially transparent, the concentric rings will be
            // progressively darker towards the center.
            // The stylization is set in the layer template file, which
            // is used in function CreateBufferLayer().

            for (int bufferRing = 0; bufferRing < bufferRingCount; bufferRing++)
            {
                double bufferDist = srs.ConvertMetersToCoordinateSystemUnits(bufferRingSize * (bufferRing + 1));
                MgGeometry bufferGeometry = mergedGeometries.Buffer(bufferDist, srsMeasure);

                MgPropertyCollection properties = new MgPropertyCollection();
                properties.Add(new MgGeometryProperty("BufferGeometry", agfReaderWriter.Write(bufferGeometry)));

                MgFeatureReader fr = bufferLayer.InsertFeatures(properties);
                fr.Close();
            }

            bufferLayer.SetVisible(true);
            bufferLayer.ForceRefresh();
            bufferLayer.SetDisplayInLegend(true);

            MessageBox.Show("Buffer created");
            _viewer.RefreshMap();
            IMapLegend legend = Shell.Instance.Legend;
            if (legend != null)
                legend.RefreshLegend();
        }

        private void btnFindFeaturesInBuffer_Click(object sender, EventArgs e)
        {
            MgSelectionBase selection = _viewer.GetSelection();
            MgReadOnlyLayerCollection selectedLayers = selection.GetLayers();
            if (selectedLayers == null)
            {
                MessageBox.Show("Select a parcel");
                return;
            }
            MgLayerBase parcels = null;
            for (int i = 0; i < selectedLayers.GetCount(); i++)
            {
                MgLayerBase layer = selectedLayers.GetItem(i);
                if (layer.Name == "Parcels")
                {
                    parcels = layer;
                    break;
                }
            }
            if (parcels == null)
            {
                MessageBox.Show("Select a parcel");
                return;
            }

            int bufferRingSize = 500; // measured in metres

            // Set up some objects for coordinate conversion

            MgMapBase map = _viewer.GetMap();
            MgLayerCollection mapLayers = map.GetLayers();
            MgMapViewerProvider provider = _viewer.GetProvider();
            MgResourceService resourceService = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);
            //Casting to MgdFeatureService because we want to use convenience APIs
            MgFeatureService featureService = (MgdFeatureService)provider.CreateService(MgServiceType.FeatureService);
            string sessionId = Guid.NewGuid().ToString();

            String mapWktSrs = map.GetMapSRS();
            MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
            MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
            MgCoordinateSystemFactory coordinateSystemFactory = new MgCoordinateSystemFactory();
            MgCoordinateSystem srs = coordinateSystemFactory.Create(mapWktSrs);
            MgMeasure srsMeasure = srs.GetMeasure();

            // Check for a buffer layer. If it exists, delete
            // the current features.
            // If it does not exist, create a feature source and
            // a layer to hold the buffer.

            BufferHelper helper = new BufferHelper();
            MgdLayer bufferLayer = null;
            int layerIndex = map.GetLayers().IndexOf("Buffer");
            if (layerIndex < 0)
            {
                // The layer does not exist and must be created.

                MgResourceIdentifier bufferFeatureResId = new MgResourceIdentifier("Session:" + sessionId + "//Buffer.FeatureSource");
                helper.CreateBufferFeatureSource(featureService, mapWktSrs, bufferFeatureResId);
                bufferLayer = helper.CreateBufferLayer(resourceService, bufferFeatureResId, sessionId);
                map.GetLayers().Insert(0, bufferLayer);
            }
            else
            {
                bufferLayer = (MgdLayer)map.GetLayers().GetItem(layerIndex);
                bufferLayer.DeleteFeatures("ID like '%'");
            }

            // Check for a parcel marker layer. If it exists, delete
            // the current features.
            // If it does not exist, create a feature source and
            // a layer to hold the parcel markers.

            MgdLayer parcelMarkerLayer = null;
            layerIndex = map.GetLayers().IndexOf("ParcelMarker");
            if (layerIndex < 0)
            {
                MgResourceIdentifier parcelFeatureResId = new MgResourceIdentifier("Session:" + sessionId + "//ParcelMarker.FeatureSource");
                helper.CreateParcelMarkerFeatureSource(featureService, mapWktSrs, parcelFeatureResId);
                parcelMarkerLayer = helper.CreateParcelMarkerLayer(resourceService, parcelFeatureResId, sessionId);
                map.GetLayers().Insert(0, parcelMarkerLayer);
            }
            else
            {
                parcelMarkerLayer = (MgdLayer)map.GetLayers().GetItem(layerIndex);
                parcelMarkerLayer.DeleteFeatures("ID like '%'");
            }

            // Check each layer in the selection.

            for (int i = 0; i < selectedLayers.GetCount(); i++)
            {
                // Only check selected features in the Parcels layer.

                MgdLayer layer = (MgdLayer)selectedLayers.GetItem(i);

                if (layer.GetName() == "Parcels")
                {
                    string geomName = layer.GetFeatureGeometryName();
                    System.Diagnostics.Trace.TraceInformation("Marking all parcels inside the buffer that are of type 'MFG'");

                    MgFeatureReader featureReader = selection.GetSelectedFeatures(layer, layer.GetFeatureClassName(), false);


                    // Process each item in the MgFeatureReader. Get the
                    // geometries from all the selected features and
                    // merge them into a single geometry.

                    MgGeometryCollection inputGeometries = new MgGeometryCollection();
                    while (featureReader.ReadNext())
                    {
                        MgByteReader featureGeometryData = featureReader.GetGeometry(geomName);
                        MgGeometry featureGeometry = agfReaderWriter.Read(featureGeometryData);

                        inputGeometries.Add(featureGeometry);
                    }

                    MgGeometryFactory geometryFactory = new MgGeometryFactory();
                    MgGeometry mergedGeometries = geometryFactory.CreateMultiGeometry(inputGeometries);

                    // Create a buffer from the merged geometries

                    double bufferDist = srs.ConvertMetersToCoordinateSystemUnits(bufferRingSize);
                    MgGeometry bufferGeometry = mergedGeometries.Buffer(bufferDist, srsMeasure);

                    // Create a filter to select parcels within the buffer. Combine
                    // a basic filter and a spatial filter to select all parcels
                    // within the buffer that are of type "MFG".

                    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
                    queryOptions.SetFilter("RTYPE = 'MFG'");
                    queryOptions.SetSpatialFilter(geomName, bufferGeometry, MgFeatureSpatialOperations.Inside);

                    featureReader = layer.SelectFeatures(queryOptions);

                    // Get the features from the feature source,
                    // determine the centroid of each selected feature, and
                    // add a point to the ParcelMarker layer to mark the
                    // centroid.
                    // Collect all the points into an MgFeatureCommandCollection,
                    // so they can all be added in one operation.

                    MgFeatureCommandCollection parcelMarkerCommands = new MgFeatureCommandCollection();
                    int inserted = 0;
                    while (featureReader.ReadNext())
                    {
                        MgByteReader byteReader = featureReader.GetGeometry(geomName);
                        MgGeometry geometry = agfReaderWriter.Read(byteReader);
                        MgPoint point = geometry.GetCentroid();

                        // Create an insert command for this parcel.
                        MgPropertyCollection properties = new MgPropertyCollection();
                        properties.Add(new MgGeometryProperty("ParcelLocation", agfReaderWriter.Write(point)));
                        //parcelMarkerCommands.Add(new MgInsertFeatures("ParcelMarkerClass", properties));
                        MgFeatureReader fr = parcelMarkerLayer.InsertFeatures(properties);
                        fr.Close();
                        inserted++;
                    }
                    featureReader.Close();

                    if (inserted == 0)
                    {
                        MessageBox.Show("No parcels within the buffer area match.");
                        return;
                    }

                    // Create a feature in the buffer feature source to show the area covered by the buffer.

                    MgPropertyCollection props = new MgPropertyCollection();
                    props.Add(new MgGeometryProperty("BufferGeometry", agfReaderWriter.Write(bufferGeometry)));
                    bufferLayer.InsertFeatures(props);

                    // Ensure that the buffer layer is visible and in the legend.

                    bufferLayer.SetVisible(true);
                    bufferLayer.ForceRefresh();
                    bufferLayer.SetDisplayInLegend(true);
                    parcelMarkerLayer.SetVisible(true);
                    parcelMarkerLayer.ForceRefresh();

                    MessageBox.Show("Done");
                    _viewer.RefreshMap();
                    IMapLegend legend = Shell.Instance.Legend;
                    if (legend != null)
                        legend.RefreshLegend();
                }
            }
        }
    }
}
