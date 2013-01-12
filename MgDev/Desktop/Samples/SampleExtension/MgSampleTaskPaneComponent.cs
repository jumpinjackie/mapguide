using System;
using System.Collections.Generic;
using System.Text;
using OSGeo.MapGuide.Viewer;

namespace SampleExtension
{
    public class MgSampleTaskPaneComponent : MgViewerComponent
    {
        public MgSampleTaskPaneComponent() 
        {
            this.Target = MgViewerTarget.TaskPane;
        }

        protected override MgControlView CreateControlView()
        {
            return new SamplesTaskPane(this.Viewer);
        }
    }
}
