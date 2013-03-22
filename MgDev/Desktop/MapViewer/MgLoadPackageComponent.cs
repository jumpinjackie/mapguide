using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    [ToolboxItem(true)]
    public class MgLoadPackageComponent : MgComponent
    {
        public MgLoadPackageComponent() 
        {
            this.Label = this.ToolTipText = Strings.TextLoadPackage;
            this.Icon = Properties.Resources.load_package;
        }

        protected override bool RequiresLoadedMap
        {
            get
            {
                return false;
            }
        }

        [Description("The LoadMap component to invoke once the package has been loaded")]
        [MgComponentProperty]
        public MgLoadMapComponent InvokeOnPackageLoad
        {
            get;
            set;
        }

        public override void Invoke()
        {
            using (var open = new OpenFileDialog())
            {
                open.Title = Strings.TextLoadPackage;
                open.Filter = "*.mgp|*.mgp";
                if (open.ShowDialog() == DialogResult.OK)
                {
                    var viewer = this.Viewer;
                    var provider = viewer.GetProvider();
                    var resSvc = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);
                    var source = new MgByteSource(open.FileName);
                    var br = source.GetReader();

                    resSvc.ApplyResourcePackage(br);
                    MessageBox.Show(Strings.TextPackageLoaded);
                    if (this.InvokeOnPackageLoad != null)
                        this.InvokeOnPackageLoad.Invoke();
                }
            }
        }
    }
}
