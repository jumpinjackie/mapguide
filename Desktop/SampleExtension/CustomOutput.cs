using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide.Viewer.Desktop;
using OSGeo.MapGuide;
using System.IO;

namespace SampleExtension
{
    public partial class CustomOutput : MgControlView
    {
        private IMapViewer _viewer;

        public CustomOutput(IMapViewer viewer)
        {
            InitializeComponent();
            this.Title = "Custom Output";
            _viewer = viewer;
        }

        private void btnRenderMap1_Click(object sender, EventArgs e)
        {
            //Note we're casting to mg-desktop specific subclasses as this is what mg-desktop rendering APIs expect
            MgdMap map = (MgdMap)_viewer.GetMap();
            MgdSelection selection = (MgdSelection)_viewer.GetSelection();
            MgMapViewerProvider provider = _viewer.GetProvider();
            MgdRenderingService renderSvc = (MgdRenderingService)provider.CreateService(MgServiceType.RenderingService);
            MgPoint centerPt = map.ViewCenter;
            MgCoordinate centerCoord = centerPt.Coordinate;

            //MgdTransientMapState is a helper class which lets us apply transient state to a map, which is automatically
            //undone on disposal. This is how we can render custom views of a map with specific display parameters without
            //permanently changing the display parameters used by the map viewer
            using (MgTransientMapState tempState = provider.CreateTransientState(map))
            {
                MgMapDisplayParameters state = new MgMapDisplayParameters(centerCoord.X, centerCoord.Y, 4000, 640, 480, 96);
                tempState.PushState(state);

                MgByteReader br = renderSvc.RenderMap(map, selection, "PNG");
                using (SaveFileDialog save = new SaveFileDialog())
                {
                    save.Filter = "Portable Network Graphics (*.png)|*.png";
                    if (save.ShowDialog() == DialogResult.OK)
                    {
                        //MgReadOnlyStream is a stream adapter class that provides a .net stream
                        //interface to the MgByteReader, allowing MgByteReader objects to be used
                        //anywhere a System.IO.Stream is expected.
                        using (MgReadOnlyStream stream = new MgReadOnlyStream(br))
                        {
                            Image img = Image.FromStream(stream);
                            img.Save(save.FileName);
                            MessageBox.Show("Image saved to: " + save.FileName);
                        }
                    }
                    br.Dispose();
                }
            }
        }

        private void btnRenderMap2_Click(object sender, EventArgs e)
        {
            //Note we're casting to mg-desktop specific subclasses as this is what mg-desktop rendering APIs expect
            MgdMap map = (MgdMap)_viewer.GetMap();
            MgdSelection selection = (MgdSelection)_viewer.GetSelection();
            MgMapViewerProvider provider = _viewer.GetProvider();
            MgdRenderingService renderSvc = (MgdRenderingService)provider.CreateService(MgServiceType.RenderingService);
            MgPoint centerPt = map.ViewCenter;
            MgCoordinate centerCoord = centerPt.Coordinate;

            //MgdTransientMapState is a helper class which lets us apply transient state to a map, which is automatically
            //undone on disposal. This is how we can render custom views of a map with specific display parameters without
            //permanently changing the display parameters used by the map viewer
            using (MgTransientMapState tempState = provider.CreateTransientState(map))
            {
                MgMapDisplayParameters state = new MgMapDisplayParameters(centerCoord.X, centerCoord.Y, 5000, 1024, 768, 96);
                tempState.PushState(state);

                MgByteReader br = renderSvc.RenderMap(map, selection, "PNG");
                using (SaveFileDialog save = new SaveFileDialog())
                {
                    save.Filter = "Portable Network Graphics (*.png)|*.png";
                    if (save.ShowDialog() == DialogResult.OK)
                    {
                        //MgReadOnlyStream is a stream adapter class that provides a .net stream
                        //interface to the MgByteReader, allowing MgByteReader objects to be used
                        //anywhere a System.IO.Stream is expected.
                        using (MgReadOnlyStream stream = new MgReadOnlyStream(br))
                        {
                            Image img = Image.FromStream(stream);
                            img.Save(save.FileName);
                            MessageBox.Show("Image saved to: " + save.FileName);
                        }
                    }
                    br.Dispose();
                }
            }
        }

        private void btnRenderMap3_Click(object sender, EventArgs e)
        {
            //Note we're casting to mg-desktop specific subclasses as this is what mg-desktop rendering APIs expect
            MgdMap map = (MgdMap)_viewer.GetMap();
            MgdSelection selection = (MgdSelection)_viewer.GetSelection();
            MgMapViewerProvider provider = _viewer.GetProvider();
            MgdRenderingService renderSvc = (MgdRenderingService)provider.CreateService(MgServiceType.RenderingService);
            MgPoint centerPt = map.ViewCenter;
            MgCoordinate centerCoord = centerPt.Coordinate;

            //MgdTransientMapState is a helper class which lets us apply transient state to a map, which is automatically
            //undone on disposal. This is how we can render custom views of a map with specific display parameters without
            //permanently changing the display parameters used by the map viewer
            using (MgTransientMapState tempState = provider.CreateTransientState(map))
            {
                MgMapDisplayParameters state = new MgMapDisplayParameters(centerCoord.X, centerCoord.Y, 6000, 1920, 1680, 192);
                tempState.PushState(state);

                MgByteReader br = renderSvc.RenderMap(map, selection, "PNG");
                using (SaveFileDialog save = new SaveFileDialog())
                {
                    save.Filter = "Portable Network Graphics (*.png)|*.png";
                    if (save.ShowDialog() == DialogResult.OK)
                    {
                        //MgReadOnlyStream is a stream adapter class that provides a .net stream
                        //interface to the MgByteReader, allowing MgByteReader objects to be used
                        //anywhere a System.IO.Stream is expected.
                        using (MgReadOnlyStream stream = new MgReadOnlyStream(br))
                        {
                            Image img = Image.FromStream(stream);
                            img.Save(save.FileName);
                            MessageBox.Show("Image saved to: " + save.FileName);
                        }
                    }
                    br.Dispose();
                }
            }
        }

        private void btnRenderMap4_Click(object sender, EventArgs e)
        {
            //Note we're casting to mg-desktop specific subclasses as this is what mg-desktop rendering APIs expect
            MgdMap map = (MgdMap)_viewer.GetMap();
            MgdSelection selection = (MgdSelection)_viewer.GetSelection();
            MgMapViewerProvider provider = _viewer.GetProvider();
            MgdRenderingService renderSvc = (MgdRenderingService)provider.CreateService(MgServiceType.RenderingService);
            MgPoint centerPt = map.ViewCenter;
            MgCoordinate centerCoord = centerPt.Coordinate;

            //MgdTransientMapState is a helper class which lets us apply transient state to a map, which is automatically
            //undone on disposal. This is how we can render custom views of a map with specific display parameters without
            //permanently changing the display parameters used by the map viewer
            using (MgTransientMapState tempState = provider.CreateTransientState(map))
            {
                MgMapDisplayParameters state = new MgMapDisplayParameters(centerCoord.X, centerCoord.Y, 7000, 2048, 1920, 192);
                tempState.PushState(state);

                MgByteReader br = renderSvc.RenderMap(map, selection, "PNG");
                using (SaveFileDialog save = new SaveFileDialog())
                {
                    save.Filter = "Portable Network Graphics (*.png)|*.png";
                    if (save.ShowDialog() == DialogResult.OK)
                    {
                        //MgReadOnlyStream is a stream adapter class that provides a .net stream
                        //interface to the MgByteReader, allowing MgByteReader objects to be used
                        //anywhere a System.IO.Stream is expected.
                        using (MgReadOnlyStream stream = new MgReadOnlyStream(br))
                        {
                            Image img = Image.FromStream(stream);
                            img.Save(save.FileName);
                            MessageBox.Show("Image saved to: " + save.FileName);
                        }
                    }
                    br.Dispose();
                }
            }
        }

        private void btnSinglePlot_Click(object sender, EventArgs e)
        {
            MgdMap map = (MgdMap)_viewer.GetMap();
            MgMapViewerProvider provider = _viewer.GetProvider();
            MgdMappingService mappingSvc = (MgdMappingService)provider.CreateService(MgServiceType.MappingService);
            MgResourceIdentifier layoutId = new MgResourceIdentifier("Library://Samples/Sheboygan/Layouts/SheboyganMap.PrintLayout");
            using (SaveFileDialog save = new SaveFileDialog())
            {
                save.Filter = "DWF Files (*.dwf)|*.dwf";
                if (save.ShowDialog() == DialogResult.OK)
                {
                    MgdDwfVersion dwfVer = new MgdDwfVersion("6.01", "1.2");

                    MgdLayout layout = new MgdLayout(layoutId, "City of Sheboygan", MgdPageUnitsType.Inches);
                    MgdPlotSpecification plotSpec = new MgdPlotSpecification(8.5f, 11.0f, MgdPageUnitsType.Inches, 0.5f, 0.5f, 0.5f, 0.5f);

                    MgByteReader result = mappingSvc.GeneratePlot(map, plotSpec, layout, dwfVer);
                    MgByteSink sink = new MgByteSink(result);
                    sink.ToFile(save.FileName);

                    MessageBox.Show("Saved to " + save.FileName);
                }
            }
        }

        private void btnMultiPlot_Click(object sender, EventArgs e)
        {
            MgdMap map = (MgdMap)_viewer.GetMap();
            MgMapViewerProvider provider = _viewer.GetProvider();
            MgdMappingService mappingSvc = (MgdMappingService)provider.CreateService(MgServiceType.MappingService);
            MgResourceIdentifier layoutId = new MgResourceIdentifier("Library://Samples/Sheboygan/Layouts/SheboyganMap.PrintLayout");
            using (SaveFileDialog save = new SaveFileDialog())
            {
                save.Filter = "DWF Files (*.dwf)|*.dwf";
                if (save.ShowDialog() == DialogResult.OK)
                {
                    MgdDwfVersion dwfVer = new MgdDwfVersion("6.01", "1.2");

                    MgdLayout layout = new MgdLayout(layoutId, "City of Sheboygan", MgdPageUnitsType.Inches);
                    MgdPlotSpecification plotSpec = new MgdPlotSpecification(8.5f, 11.0f, MgdPageUnitsType.Inches, 0.5f, 0.5f, 0.5f, 0.5f);

                    MgdMapPlotCollection plotCollection = new MgdMapPlotCollection();

                    MgdMapPlot plot1 = new MgdMapPlot(map, plotSpec, layout);
                    plot1.SetCenterAndScale(map.GetViewCenter().GetCoordinate(), map.GetViewScale() * 2);
                    plotCollection.Add(plot1);

                    // Create a second map for the second sheet in the DWF. This second sheet uses the print layout
                    // to display a page title and legend.

                    MgdMap map2 = new MgdMap(map.GetMapDefinition(), "Sheet 2");
                    MgdMapPlot plot2 = new MgdMapPlot(map2, plotSpec, layout);
                    plot2.SetCenterAndScale(map.GetViewCenter().GetCoordinate(), map.GetViewScale());
                    plotCollection.Add(plot2);

                    MgByteReader result = mappingSvc.GenerateMultiPlot(plotCollection, dwfVer);
                    MgByteSink sink = new MgByteSink(result);
                    sink.ToFile(save.FileName);

                    MessageBox.Show("Saved to " + save.FileName);
                }
            }
        }
    }
}
