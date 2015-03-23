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
    public partial class SelectFeaturesControl : UserControl
    {
        class Pair
        {
            public string Name { get; set; }

            public string Expr { get; set; }

            public override string ToString()
            {
                return Name + " => " + Expr;
            }
        }

        public SelectFeaturesControl()
        {
            InitializeComponent();
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            btnRemoveExpr.Enabled = (lstComputed.SelectedItem != null);
        }

        private void btnAddExpr_Click(object sender, EventArgs e)
        {
            lstComputed.Items.Add(new Pair() { Name = txtAlias.Text, Expr = txtExpr.Text });
        }

        private void btnRemoveExpr_Click(object sender, EventArgs e)
        {
            if (lstComputed.SelectedItem != null)
                lstComputed.Items.Remove(lstComputed.SelectedItem);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                MgFeatureService featSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);
                MgFeatureQueryOptions query = new MgFeatureQueryOptions();
                string[] propNames = txtProps.Text.Split(',');
                foreach (var p in propNames)
                {
                    if (string.IsNullOrEmpty(p))
                        continue;

                    query.AddFeatureProperty(p);
                }

                foreach (Pair p in lstComputed.Items)
                {
                    query.AddComputedProperty(p.Name, p.Expr);
                }

                if (!string.IsNullOrEmpty(txtFilter.Text.Trim()))
                    query.SetFilter(txtFilter.Text.Trim());

                MgResourceIdentifier fsId = new MgResourceIdentifier(txtFeatureSource.Text);

                MgFeatureReader reader = featSvc.SelectFeatures(fsId, txtClass.Text, query);
                new ReaderResponseDialog(reader).ShowDialog();
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
