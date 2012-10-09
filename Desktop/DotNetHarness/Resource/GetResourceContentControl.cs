﻿using System;
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
    public partial class GetResourceContentControl : UserControl
    {
        public GetResourceContentControl()
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
                MgByteReader reader = resSvc.GetResourceContent(resId);

                new XmlResponseDialog(reader).ShowDialog();
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
