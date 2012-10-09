using System;
using System.Collections.Generic;
using System.Text;
using OSGeo.MapGuide.Viewer;

namespace SampleExtension
{
    // Components are the entry points to your custom functionality
    //
    // Components are akin to Commands in the AJAX viewer and Widgets in Fusion.
    // As you can see below, all these components are simply creating MgControlView
    // instances which will either be housed in the application's Task Pane, or in
    // a new window. For these examples, all UI views are housed in the Task Pane

    public class MgHelloMapComponent : MgViewerComponent
    {
        public MgHelloMapComponent()
        {
            this.Target = MgViewerTarget.TaskPane;
        }

        protected override MgControlView CreateControlView()
        {
            return new HelloMap(this.Viewer);
        }
    }

    public class MgHelloViewerComponent : MgViewerComponent
    {
        public MgHelloViewerComponent()
        {
            this.Target = MgViewerTarget.TaskPane;
        }

        protected override MgControlView CreateControlView()
        {
            return new HelloViewer(this.Viewer);
        }
    }

    public class MgInteractingWithLayersComponent : MgViewerComponent
    {
        public MgInteractingWithLayersComponent()
        {
            this.Target = MgViewerTarget.TaskPane;
        }

        protected override MgControlView CreateControlView()
        {
            return new InteractingWithLayers(this.Viewer);
        }
    }

    public class MgWorkingWithFeatureDataComponent : MgViewerComponent
    {
        public MgWorkingWithFeatureDataComponent()
        {
            this.Target = MgViewerTarget.TaskPane;
        }

        protected override MgControlView CreateControlView()
        {
            return new WorkingWithFeatureData(this.Viewer);
        }
    }

    public class MgModifyingMapsAndLayersComponent : MgViewerComponent
    {
        public MgModifyingMapsAndLayersComponent()
        {
            this.Target = MgViewerTarget.TaskPane;
        }

        protected override MgControlView CreateControlView()
        {
            return new ModifyingMapsAndLayers(this.Viewer);
        }
    }

    public class MgAnalyzingFeaturesComponent : MgViewerComponent
    {
        public MgAnalyzingFeaturesComponent()
        {
            this.Target = MgViewerTarget.TaskPane;
        }

        protected override MgControlView CreateControlView()
        {
            return new AnalyzingFeatures(this.Viewer);
        }
    }

    public class MgDigitizingAndRedliningComponent : MgViewerComponent
    {
        public MgDigitizingAndRedliningComponent()
        {
            this.Target = MgViewerTarget.TaskPane;
        }

        protected override MgControlView CreateControlView()
        {
            return new DigitizingAndRedlining(this.Viewer);
        }
    }

    public class MgCustomOutputComponent : MgViewerComponent
    {
        public MgCustomOutputComponent()
        {
            this.Target = MgViewerTarget.TaskPane;
        }

        protected override MgControlView CreateControlView()
        {
            return new CustomOutput(this.Viewer);
        }
    }

    public class MgPrePostRenderingComponent : MgViewerComponent
    {
        public MgPrePostRenderingComponent()
        {
            this.Target = MgViewerTarget.TaskPane;
        }

        protected override MgControlView CreateControlView()
        {
            return new PrePostRendering(this.Viewer);
        }
    }
}
