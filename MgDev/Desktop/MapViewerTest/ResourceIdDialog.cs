using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;

namespace MapViewerTest
{
    public partial class ResourceIdDialog : Form
    {
        public ResourceIdDialog()
        {
            InitializeComponent();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }

        public MgResourceIdentifier ResourceID { get; private set; }

        private void btnOk_Click(object sender, EventArgs e)
        {
            try
            {
                var resId = new MgResourceIdentifier(txtResourceId.Text);
                resId.Validate();

                var fact = new MgdServiceFactory();
                var resSvc = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                if (!resSvc.ResourceExists(resId))
                {
                    MessageBox.Show("The specified resource does not exist");
                    return;
                }
                this.ResourceID = resId;
                this.DialogResult = DialogResult.OK;
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.Message);
                ex.Dispose();
            }
        }
    }
}
