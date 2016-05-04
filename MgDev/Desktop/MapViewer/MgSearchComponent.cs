using System;
using System.Collections.Generic;
using System.Text;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    public class MgSearchComponent : MgViewerComponent
    {
        /// <summary>
        /// Gets or sets the layer name this component will search on
        /// </summary>
        [MgComponentProperty]
        public string LayerName { get; set; }

        /// <summary>
        /// Gets or sets the filter this component will apply as part of the search
        /// </summary>
        [MgComponentProperty]
        public string Filter { get; set; }

        /// <summary>
        /// Gets or sets the prompt for the user interface
        /// </summary>
        [MgComponentProperty]
        public string Prompt { get; set; }

        /// <summary>
        /// Gets or sets the Task Pane / Window title
        /// </summary>
        [MgComponentProperty]
        public string Title { get; set; }

        public MgSearchComponent()
        {
            this.Icon = Properties.Resources.search;
            this.Label = this.ToolTipText = this.Title = Strings.TitleSearch;
            this.Prompt = Strings.TextEnterSearchCriteria;
        }

        protected override MgControlView CreateControlView()
        {
            return new MgSearchControlImpl(this.Viewer, this.Title, this.LayerName, this.Prompt, this.Filter);
        }
    }
}
