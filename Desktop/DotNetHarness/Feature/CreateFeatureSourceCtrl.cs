using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;
using System.IO;

namespace DotNetHarness.Feature
{
    public partial class CreateFeatureSourceCtrl : UserControl
    {
        public CreateFeatureSourceCtrl()
        {
            InitializeComponent();
        }

        private void btnBrowse_Click(object sender, EventArgs e)
        {
            using (var open = new OpenFileDialog())
            {
                open.Filter = "*.xml|*.xml";
                if (open.ShowDialog() == DialogResult.OK)
                {
                    txtSchemaXml.Text = open.FileName;
                }
            }
        }

        private void btnCreateFile_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                var featureSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);
                var param = new MgFileFeatureSourceParams();
                param.CoordinateSystemWkt = txtCoordinateSystemWkt.Text;
                param.FileName = txtFileName.Text;

                var schemas = featureSvc.XmlToSchema(File.ReadAllText(txtSchemaXml.Text));
                param.FeatureSchema = schemas.GetItem(0);

                param.ProviderName = txtProvider.Text;
                param.SpatialContextDescription = "Created from MgFileFeatureSourceParams";
                param.SpatialContextName = txtCoordinateSystemName.Text;
                param.XYTolerance = double.Parse(txtXYTolerance.Text);
                param.ZTolerance = double.Parse(txtZTolerance.Text);

                var fsId = new MgResourceIdentifier(txtResourceId.Text);

                featureSvc.CreateFeatureSource(fsId, param);
                MessageBox.Show("Success");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void btnCreateRdbms_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                var featureSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);
                var param = new MgdRdbmsFeatureSourceParams();
                param.CoordinateSystemWkt = txtCoordinateSystemWkt.Text;

                param.Service = txtService.Text;
                param.DataStore = txtDataStore.Text;
                param.Username = txtUsername.Text;
                param.Password = txtPassword.Text;

                var schemas = featureSvc.XmlToSchema(File.ReadAllText(txtSchemaXml.Text));
                param.FeatureSchema = schemas.GetItem(0);

                param.ProviderName = txtProvider.Text;
                param.SpatialContextDescription = "Created from MgFileFeatureSourceParams";
                param.SpatialContextName = txtCoordinateSystemName.Text;
                param.XYTolerance = double.Parse(txtXYTolerance.Text);
                param.ZTolerance = double.Parse(txtZTolerance.Text);

                string[] tokens = txtExtents.Text.Split(',').Select(x => x.Trim()).ToArray();
                MgEnvelope env = new MgEnvelope(
                    double.Parse(tokens[0]),
                    double.Parse(tokens[1]),
                    double.Parse(tokens[2]),
                    double.Parse(tokens[3]));

                param.SpatialContextExtents = env;
                

                var fsId = new MgResourceIdentifier(txtResourceId.Text);

                featureSvc.CreateFeatureSource(fsId, param);
                MessageBox.Show("Success");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
    }
}
