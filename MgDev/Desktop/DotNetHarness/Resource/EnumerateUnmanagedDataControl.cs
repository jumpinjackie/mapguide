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
    public partial class EnumerateUnmanagedDataControl : UserControl
    {
        public EnumerateUnmanagedDataControl()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                MgdServiceFactory fact = new MgdServiceFactory();
                MgResourceService resSvc = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

                MgByteReader response = resSvc.EnumerateUnmanagedData(txtPath.Text, chkRecursive.Checked, txtType.Text, txtFilter.Text);
                new XmlResponseDialog(response).ShowDialog();
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
