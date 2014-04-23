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
    public partial class GetClassesControl : UserControl
    {
        public GetClassesControl()
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


                MgStringCollection classNames = featSvc.GetClasses(fsId, txtSchema.Text);
                string[] values = new string[classNames.GetCount()];

                for (int i = 0; i < classNames.GetCount(); i++)
                {
                    values[i] = classNames.GetItem(i);
                }

                MessageBox.Show(string.Join(Environment.NewLine, values), "Classes found");
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
