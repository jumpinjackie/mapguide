using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;

namespace DotNetHarness.Resource
{
    public partial class CopyResourceControl : UserControl
    {
        public CopyResourceControl()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                MgResourceService resSvc = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                MgResourceIdentifier source = new MgResourceIdentifier(textBox1.Text);
                MgResourceIdentifier target = new MgResourceIdentifier(textBox2.Text);

                resSvc.CopyResource(source, target, checkBox1.Checked);

                MessageBox.Show("Success");
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
