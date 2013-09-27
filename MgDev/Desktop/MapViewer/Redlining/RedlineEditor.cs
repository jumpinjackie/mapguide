using System;
using System.Collections.Generic;
using System.Text;

namespace OSGeo.MapGuide.Viewer.Redlining
{
    /// <summary>
    /// A callback that's invoked when a new redline object is added
    /// </summary>
    /// <param name="key">The key of the inserted feature. Some providers may not be able to return the key of the inserted feature and will pass null into this callback</param>
    /// <param name="text">The text of the redline</param>
    public delegate void RedlineAction(int? key, string text);

    /// <summary>
    /// A callback that retrieves the redline text
    /// </summary>
    /// <returns></returns>
    public delegate string RedlineTextFunction();

    internal class RedlineEditor
    {
        private IMapViewer _viewer;
        private RedlineLayer _layer;
        private MgFeatureService _featSvc;
        private MgAgfReaderWriter _agfRW;
        private MgWktReaderWriter _wktRW;
        private MgGeometryFactory _geomFact;

        public RedlineEditor(IMapViewer viewer, RedlineLayer layer)
        {
            _viewer = viewer;
            _layer = layer;
            var provider = viewer.GetProvider();
            _featSvc = (MgFeatureService)provider.CreateService(MgServiceType.FeatureService);
            _wktRW = new MgWktReaderWriter();
            _agfRW = new MgAgfReaderWriter();
            _geomFact = new MgGeometryFactory();
        }

        public IEnumerable<RedlineObject> GetAllFeatures()
        {
            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            MgLayerBase redlineLayer = layers.GetItem(_layer.SystemName);

            MgResourceIdentifier resId = new MgResourceIdentifier(redlineLayer.GetFeatureSourceId());
            MgFeatureReader reader = null;
            try
            {
                reader = _featSvc.SelectFeatures(resId, RedlineSchemaFactory.CLASS_NAME, null);

                //HACK: Another leaky abstraction. SHP will always choose FeatId, so once again
                //use the class definition to determine the identity property name
                MgClassDefinition cls = reader.GetClassDefinition();
                MgPropertyDefinitionCollection idProps = cls.GetIdentityProperties();
                MgPropertyDefinition keyProp = idProps.GetItem(0);
                string idName = keyProp.GetName();

                while (reader.ReadNext())
                {
                    int id = reader.GetInt32(idName);
                    string text = reader.IsNull(RedlineSchemaFactory.TEXT_NAME) ? string.Empty : reader.GetString(RedlineSchemaFactory.TEXT_NAME);

                    yield return new RedlineObject(id, text);
                }
            }
            finally
            {
                if (reader != null)
                    reader.Close();
            }
        }

        public void AddObject(RedlineGeometryType type, RedlineTextFunction retrieveTextMethod, RedlineAction onRedlineAdded)
        {
            switch (type)
            {
                case RedlineGeometryType.Point:
                    AddPoint(retrieveTextMethod, onRedlineAdded);
                    break;
                case RedlineGeometryType.Circle:
                    AddCircle(retrieveTextMethod, onRedlineAdded);
                    break;
                case RedlineGeometryType.Line:
                    AddLine(retrieveTextMethod, onRedlineAdded);
                    break;
                case RedlineGeometryType.LineString:
                    AddLineString(retrieveTextMethod, onRedlineAdded);
                    break;
                case RedlineGeometryType.Polygon:
                    AddPolygon(retrieveTextMethod, onRedlineAdded);
                    break;
                case RedlineGeometryType.Rectangle:
                    AddRectangle(retrieveTextMethod, onRedlineAdded);
                    break;
            }
        }

        internal void DeleteRedlines(int[] ids)
        {
            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            MgLayerBase redlineLayer = layers.GetItem(_layer.SystemName);

            //HACK: Workaround FeatId leaky abstraction in SHP provider
            MgClassDefinition cls = redlineLayer.GetClassDefinition();
            MgPropertyDefinitionCollection idProps = cls.GetIdentityProperties();
            MgPropertyDefinition idProp = idProps.GetItem(0);

            redlineLayer.ForceRefresh();

            //This lib doesn't reference mg-desktop so the convenience APIs aren't available to us
            //Gotta go the old verbose route
            List<string> filters = new List<string>();
            foreach (int id in ids)
            {
                filters.Add(idProp.Name + " = " + id);
            }
            string deleteFilter = string.Join(" OR ", filters.ToArray());
            MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
            
            MgDeleteFeatures delete = new MgDeleteFeatures(redlineLayer.FeatureClassName, deleteFilter);
            commands.Add(delete);

            MgPropertyCollection result = redlineLayer.UpdateFeatures(commands);
            MgInt32Property deleteResult = result.GetItem(0) as MgInt32Property;
            if (deleteResult != null && deleteResult.GetValue() > 0)
            {
                _viewer.RefreshMap();
            }
        }

        internal void UpdateRedlineText(string text, int[] ids)
        {
            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            MgLayerBase redlineLayer = layers.GetItem(_layer.SystemName);

            //HACK: Workaround FeatId leaky abstraction in SHP provider
            MgClassDefinition cls = redlineLayer.GetClassDefinition();
            MgPropertyDefinitionCollection idProps = cls.GetIdentityProperties();
            MgPropertyDefinition idProp = idProps.GetItem(0);

            redlineLayer.ForceRefresh();

            //This lib doesn't reference mg-desktop so the convenience APIs aren't available to us
            //Gotta go the old verbose route
            List<string> filters = new List<string>();
            foreach (int id in ids)
            {
                filters.Add(idProp.Name + " = " + id);
            }
            string updateFilter = string.Join(" OR ", filters.ToArray());
            MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
            MgPropertyCollection updateValues = new MgPropertyCollection();
            MgStringProperty updateProp = new MgStringProperty(RedlineSchemaFactory.TEXT_NAME, text);
            updateValues.Add(updateProp);
            
            MgUpdateFeatures update = new MgUpdateFeatures(redlineLayer.FeatureClassName, updateValues, updateFilter);
            commands.Add(update);

            MgPropertyCollection result = redlineLayer.UpdateFeatures(commands);
            MgInt32Property updateResult = result.GetItem(0) as MgInt32Property;
            MgStringProperty errorResult = result.GetItem(0) as MgStringProperty;
            if (errorResult != null)
            {
                throw new Exception(errorResult.GetValue());
            }
            _viewer.RefreshMap();
        }

        private void InsertRedlineGeometry(string text, MgGeometry geom, RedlineAction onRedlineAdded)
        {
            MgPropertyCollection feature = new MgPropertyCollection();
            MgByteReader agf = _agfRW.Write(geom);
            MgGeometryProperty geomProp = new MgGeometryProperty(RedlineSchemaFactory.GEOM_NAME, agf);
            feature.Add(geomProp);
            MgStringProperty strProp = new MgStringProperty(RedlineSchemaFactory.TEXT_NAME, text);
            feature.Add(strProp);

            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            MgLayerBase redlineLayer = layers.GetItem(_layer.SystemName);
            MgClassDefinition cls = redlineLayer.GetClassDefinition();
            MgPropertyDefinitionCollection idProps = cls.GetIdentityProperties();
            MgPropertyDefinition idProp = idProps.GetItem(0);

            redlineLayer.ForceRefresh();

            //This lib doesn't reference mg-desktop so the convenience APIs aren't available to us
            //Gotta go the old verbose route
            MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
            
            MgInsertFeatures insert = new MgInsertFeatures(redlineLayer.FeatureClassName, feature);
            commands.Add(insert);

            MgPropertyCollection result = redlineLayer.UpdateFeatures(commands);
            //Insert result is a MgFeatureProperty containing an MgFeatureReader
            MgFeatureProperty insertResult = result.GetItem(0) as MgFeatureProperty;
            MgStringProperty errorResult = result.GetItem(0) as MgStringProperty;
            if (insertResult != null)
            {
                var reader = insertResult.GetValue();
                int inserted = 0;
                int? id = null;
                try
                {
                    if (reader.ReadNext())
                    {
                        inserted++;
                        id = reader.GetInt32(idProp.Name);
                    }
                }
                catch (MgException ex)
                {
                    ex.Dispose();
                }
                finally
                {
                    reader.Close();
                }
                if (inserted > 0)
                {
                    _viewer.RefreshMap();
                    onRedlineAdded(id, text);
                }
            }
            else if (errorResult != null)
            {
                throw new Exception(errorResult.GetValue());
            }
        }

        private void AddRectangle(RedlineTextFunction retrieveTextMethod, RedlineAction onRedlineAdded)
        {
            _viewer.DigitizeRectangle((x1, y1, x2, y2) =>
            {
                string text = string.Empty;
                if (retrieveTextMethod != null)
                    text = retrieveTextMethod();
                MgGeometry geom = _wktRW.Read(Util.MakeWktPolygon(x1, y1, x2, y2));
                InsertRedlineGeometry(text, geom, onRedlineAdded);
            });
        }

        private void AddPolygon(RedlineTextFunction retrieveTextMethod, RedlineAction onRedlineAdded)
        {
            _viewer.DigitizePolygon((coordinates) =>
            {
                string text = string.Empty;
                if (retrieveTextMethod != null)
                    text = retrieveTextMethod();
                MgCoordinateCollection coords = new MgCoordinateCollection();
                for (int i = 0; i < coordinates.GetLength(0); i++)
                {
                    coords.Add(_geomFact.CreateCoordinateXY(coordinates[i, 0], coordinates[i, 1]));
                }
                coords.Add(_geomFact.CreateCoordinateXY(coordinates[0, 0], coordinates[0, 1]));
                MgLinearRing ring = _geomFact.CreateLinearRing(coords);
                MgGeometry poly = _geomFact.CreatePolygon(ring, null);
                InsertRedlineGeometry(text, poly, onRedlineAdded);
            });
        }

        private void AddLineString(RedlineTextFunction retrieveTextMethod, RedlineAction onRedlineAdded)
        {
            _viewer.DigitizeLineString((coordinates) =>
            {
                string text = string.Empty;
                if (retrieveTextMethod != null)
                    text = retrieveTextMethod();
                MgCoordinateCollection coords = new MgCoordinateCollection();
                for (int i = 0; i < coordinates.GetLength(0); i++)
                {
                    coords.Add(_geomFact.CreateCoordinateXY(coordinates[i, 0], coordinates[i, 1]));
                }
                MgGeometry line = _geomFact.CreateLineString(coords);
                InsertRedlineGeometry(text, line, onRedlineAdded);
            });
        }

        private void AddLine(RedlineTextFunction retrieveTextMethod, RedlineAction onRedlineAdded)
        {
            _viewer.DigitizeLine((x1, y1, x2, y2) =>
            {
                string text = string.Empty;
                if (retrieveTextMethod != null)
                    text = retrieveTextMethod();
                MgCoordinateCollection coords = new MgCoordinateCollection();
                coords.Add(_geomFact.CreateCoordinateXY(x1, y1));
                coords.Add(_geomFact.CreateCoordinateXY(x2, y2));
                MgGeometry line = _geomFact.CreateLineString(coords);
                InsertRedlineGeometry(text, line, onRedlineAdded);
            });
        }

        private void AddPoint(RedlineTextFunction retrieveTextMethod, RedlineAction onRedlineAdded)
        {
            _viewer.DigitizePoint((x, y) =>
            {
                string text = string.Empty;
                if (retrieveTextMethod != null)
                    text = retrieveTextMethod();
                MgCoordinate coord = _geomFact.CreateCoordinateXY(x, y);
                MgGeometry point = _geomFact.CreatePoint(coord);
                InsertRedlineGeometry(text, point, onRedlineAdded);
            });
        }

        private void AddCircle(RedlineTextFunction retrieveTextMethod, RedlineAction onRedlineAdded)
        {
            _viewer.DigitizeCircle((x, y, r) =>
            {
                string text = string.Empty;
                if (retrieveTextMethod != null)
                    text = retrieveTextMethod();
                MgGeometry geom = _wktRW.Read(Util.MakeWktCircle(x, y, r, true));
                InsertRedlineGeometry(text, geom, onRedlineAdded);
            });
        }
    }
}
