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
    public partial class SetResourceControl : UserControl
    {
        public SetResourceControl()
        {
            InitializeComponent();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox2.Text = openFileDialog1.FileName;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                MgResourceService resSvc = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                MgResourceIdentifier resId = new MgResourceIdentifier(textBox1.Text);
                MgByteSource source = new MgByteSource(textBox2.Text);
                MgByteReader reader = source.GetReader();

                resSvc.SetResource(resId, reader, null);
                MessageBox.Show("Success!");
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
