#region Disclaimer / License
// Copyright (C) 2011, Jackie Ng, jumpinjackie@gmail.com
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
// 
#endregion
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using InstantSetup.Core;

namespace InstantSetup
{
    public partial class ApacheConfigCtrl : UserControl, IConfigurationView
    {
        public ApacheConfigCtrl()
        {
            InitializeComponent();
            var apacheConf = new ApacheSetupConfigurationProcess();
            this.Config = apacheConf;

            numAdminPort.Value = apacheConf.ServerAdminPort;
            numClientPort.Value = apacheConf.ServerClientPort;
            numSitePort.Value = apacheConf.ServerSitePort;

            txtVirtualDirectory.Text = apacheConf.VirtualDirectoryName;
            numHttpdPort.Value = apacheConf.ApachePortNumber;
            txtServiceName.Text = apacheConf.HttpdServiceName;

            chkEnableJava.Checked = apacheConf.EnableJava;
            numTomcatPort.Value = apacheConf.TomcatPortNumber;
        }

        private ApacheSetupConfigurationProcess ApacheConfig { get { return (ApacheSetupConfigurationProcess)this.Config; } }

        public AbstractSetupConfigurationProcess Config
        {
            get;
            private set;
        }

        public Control Wrapper
        {
            get { return this; }
        }

        private void numAdminPort_ValueChanged(object sender, EventArgs e)
        {
            this.Config.ServerAdminPort = Convert.ToInt32(numAdminPort.Value);   
        }

        private void numClientPort_ValueChanged(object sender, EventArgs e)
        {
            this.Config.ServerClientPort = Convert.ToInt32(numClientPort.Value);
        }

        private void numSitePort_ValueChanged(object sender, EventArgs e)
        {
            this.Config.ServerSitePort = Convert.ToInt32(numSitePort.Value);
        }

        private void numHttpdPort_ValueChanged(object sender, EventArgs e)
        {
            ApacheConfig.ApachePortNumber = Convert.ToInt32(numHttpdPort.Value);
        }

        private void numTomcatPort_ValueChanged(object sender, EventArgs e)
        {
            ApacheConfig.TomcatPortNumber = Convert.ToInt32(numTomcatPort.Value);
        }

        private void txtVirtualDirectory_TextChanged(object sender, EventArgs e)
        {
            ApacheConfig.VirtualDirectoryName = txtVirtualDirectory.Text;
        }

        private void txtServiceName_TextChanged(object sender, EventArgs e)
        {
            ApacheConfig.HttpdServiceName = txtServiceName.Text;
        }
    }
}
