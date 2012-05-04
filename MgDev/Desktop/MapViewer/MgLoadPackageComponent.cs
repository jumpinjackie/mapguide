using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;

namespace OSGeo.MapGuide.Viewer
{
    [ToolboxItem(true)]
    public class MgLoadPackageComponent : MgComponent
    {
        public MgLoadPackageComponent() 
        {
            this.Label = this.ToolTipText = Properties.Resources.TextLoadPackage;
            this.Icon = Properties.Resources.load_package;
        }

        public override void Invoke()
        {
            using (var open = new OpenFileDialog())
            {
                open.Title = Properties.Resources.TextLoadPackage;
                open.Filter = "*.mgp|*.mgp";
                if (open.ShowDialog() == DialogResult.OK)
                {
                    var viewer = this.Viewer;
                    var provider = viewer.GetProvider();
                    var resSvc = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);
                    var source = new MgByteSource(open.FileName);
                    var br = source.GetReader();

                    resSvc.ApplyResourcePackage(br);
                    MessageBox.Show(Properties.Resources.TextLoadPackage);
                }
            }
        }
    }
}
