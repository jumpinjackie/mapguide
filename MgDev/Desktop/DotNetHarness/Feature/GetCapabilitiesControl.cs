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
    public partial class GetCapabilitiesControl : UserControl
    {
        private MgdServiceFactory fact;

        public GetCapabilitiesControl()
        {
            InitializeComponent();
            fact = new MgdServiceFactory();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                MgFeatureService featSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);

                MgByteReader response = featSvc.GetCapabilities(txtFdoProvider.Text);
                new XmlResponseDialog(response).ShowDialog();
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                MgFeatureService featSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);

                MgByteReader response = featSvc.GetCapabilities(textBox1.Text, txtConnectionString.Text);
                new XmlResponseDialog(response).ShowDialog();
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
