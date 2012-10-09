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
    public partial class ExecuteSqlControl : UserControl
    {
        public ExecuteSqlControl()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                MgFeatureService featSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);
                MgResourceIdentifier fsId = new MgResourceIdentifier(textBox1.Text);
                string sql = textBox2.Text;

                MgSqlDataReader reader = featSvc.ExecuteSqlQuery(fsId, sql);
                new ReaderResponseDialog(reader).ShowDialog();
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
