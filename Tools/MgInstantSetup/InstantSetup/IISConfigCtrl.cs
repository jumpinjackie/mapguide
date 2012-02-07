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
    public partial class IISConfigCtrl : UserControl, IConfigurationView
    {
        private IISSetupConfigurationProcess _confImpl;

        public IISConfigCtrl()
        {
            InitializeComponent();
            this.Config = _confImpl = new IISSetupConfigurationProcess();

            numAdminPort.Value = _confImpl.ServerAdminPort;
            numClientPort.Value = _confImpl.ServerClientPort;
            numSitePort.Value = _confImpl.ServerSitePort;

            txtWebSiteName.Text = _confImpl.WebSiteName;
            txtVirtualDir.Text = _confImpl.VirtualDirectoryName;
            txtAppPool.Text = _confImpl.ApplicationPool;
            txtAppCmd.Text = _confImpl.AppCmdPath;

            rdDotNet.Checked = _confImpl.EnableDotNet;
            rdPhp.Checked = _confImpl.EnablePhp;
        }

        public AbstractSetupConfigurationProcess Config
        {
            get;
            private set;
        }

        public Control Wrapper
        {
            get { return this; }
        }

        private void rdDotNet_CheckedChanged(object sender, EventArgs e)
        {
            _confImpl.EnableDotNet = rdDotNet.Checked;
            _confImpl.EnablePhp = rdPhp.Checked;
        }

        private void rdPhp_CheckedChanged(object sender, EventArgs e)
        {
            _confImpl.EnableDotNet = rdDotNet.Checked;
            _confImpl.EnablePhp = rdPhp.Checked;
        }

        private void txtAppCmd_TextChanged(object sender, EventArgs e)
        {
            _confImpl.AppCmdPath = txtAppCmd.Text;
        }

        private void txtWebSiteName_TextChanged(object sender, EventArgs e)
        {
            _confImpl.WebSiteName = txtWebSiteName.Text;
        }

        private void txtVirtualDir_TextChanged(object sender, EventArgs e)
        {
            this.Config.VirtualDirectoryName = txtVirtualDir.Text;
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

        private void txtAppPool_TextChanged(object sender, EventArgs e)
        {
            _confImpl.ApplicationPool = txtAppPool.Text;
        }
    }
}
