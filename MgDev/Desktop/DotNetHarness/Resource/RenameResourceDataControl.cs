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
    public partial class RenameResourceDataControl : UserControl
    {
        public RenameResourceDataControl()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                MgResourceService resSvc = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                MgResourceIdentifier resId = new MgResourceIdentifier(textBox1.Text);
                resSvc.RenameResourceData(resId, textBox2.Text, textBox3.Text, checkBox1.Checked);

                MessageBox.Show("Success");
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
