using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Xml.Serialization;
using System.IO;
using System.Diagnostics;
using System.Drawing;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    [ToolboxItem(true)]
    public class MgViewerOptionsComponent : MgViewerComponent
    {
        public MgViewerOptionsComponent()
        {
            this.Label = this.ToolTipText = Strings.TextViewerOptions;
            this.Icon = Properties.Resources.options;
            this.PreferencesDirectory = string.Empty;
        }

        [Description("The directory where the preferences are saved to and loaded from")] //NOXLATE
        [MgComponentProperty]
        public string PreferencesDirectory
        {
            get;
            set;
        }

        protected override void SubscribeViewerEvents(IMapViewer viewer)
        {
            base.SubscribeViewerEvents(viewer);
            //This is a new viewer instance. So load our prefs and apply settings to this viewer
            var ser = new XmlSerializer(typeof(MgViewerOptions));
            var path = Path.Combine(this.PreferencesDirectory, MgViewerOptions.FILENAME);
            if (File.Exists(path))
            {
                using (var stream = File.OpenRead(path))
                {
                    try
                    {
                        var options = (MgViewerOptions)ser.Deserialize(stream);
                        //Apply settings
                        viewer.ShowVertexCoordinatesWhenDigitizing = options.ShowVertexCoordinates;
                        viewer.SelectionColor = Util.FromHtmlColor(options.SelectionColor);
                        viewer.ZoomInFactor = options.ZoomInFactor;
                        viewer.ZoomOutFactor = options.ZoomOutFactor;
                        viewer.ConvertTiledGroupsToNonTiled = options.ConvertTiledLayersToNonTiled;
                        Trace.TraceInformation("Applied viewer settings from: " + path); //NOXLATE
                    }
                    catch { }
                }
            }
            else
            {
                Trace.TraceInformation("No viewer settings found in " + path + ". Doing nothing"); //NOXLATE
            }
        }

        protected override MgControlView CreateControlView()
        {
            return new MgViewerOptionsControlImpl(this.Viewer, this.PreferencesDirectory);
        }
    }
}
