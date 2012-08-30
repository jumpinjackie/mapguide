using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;

namespace DotNetHarness.Feature
{
    public partial class RegisterProviderCtrl : UserControl
    {
        public RegisterProviderCtrl()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            using (var open = new OpenFileDialog())
            {
                open.Filter = "*.dll|*.dll";

                if (open.ShowDialog() == DialogResult.OK)
                {
                    txtProviderPath.Text = open.FileName;
                }
            }
        }

        private void btnCreate_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                var featureSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);
                featureSvc.RegisterProvider(txtProviderPath.Text);
                MessageBox.Show("Success");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
    }
}
