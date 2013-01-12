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
    public partial class GetSchemaMappingCtrl : UserControl
    {
        public GetSchemaMappingCtrl()
        {
            InitializeComponent();
        }

        private void btnExecute_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                var featureSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);
                var response = featureSvc.GetSchemaMapping(txtProvider.Text, txtConnString.Text);
                new XmlResponseDialog(response).ShowDialog();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
    }
}
