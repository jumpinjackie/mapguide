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
    public partial class GetSchemasControl : UserControl
    {
        public GetSchemasControl()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                MgFeatureService featSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);
                MgResourceIdentifier fsId = new MgResourceIdentifier(txtFeatureSource.Text);

                MgStringCollection schemas = featSvc.GetSchemas(fsId);
                string[] values = new string[schemas.GetCount()];

                for (int i = 0; i < schemas.GetCount(); i++)
                {
                    values[i] = schemas.GetItem(i);
                }

                MessageBox.Show(string.Join(Environment.NewLine, values), "Schemas found");
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
