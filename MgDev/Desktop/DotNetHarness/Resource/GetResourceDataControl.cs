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
    public partial class GetResourceDataControl : UserControl
    {
        public GetResourceDataControl()
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
                MgByteReader response = resSvc.GetResourceData(resId, textBox2.Text);

                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    MgByteSink sink = new MgByteSink(response);
                    sink.ToFile(saveFileDialog1.FileName);
                    MessageBox.Show("Saved to: " + saveFileDialog1.FileName);
                }
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
