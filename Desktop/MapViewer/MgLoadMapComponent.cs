using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace OSGeo.MapGuide.Viewer
{
    [ToolboxItem(true)]
    public class MgLoadMapComponent : MgComponent
    {
        [Description("The map definition to load")]
        public string MapDefinition
        {
            get;
            set;
        }

        protected override bool RequiresLoadedMap
        {
            get
            {
                return false;
            }
        }

        public override void Invoke()
        {
            var provider = this.Viewer.GetProvider();
            var mdfId = new MgResourceIdentifier(this.MapDefinition);
            var map = provider.CreateMap(mdfId, mdfId.GetName());
            provider.LoadMap(map);
        }
    }
}
