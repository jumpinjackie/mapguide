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
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace InstantSetup
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            _apacheConf = new ApacheConfigCtrl();
            _iis7Conf = new IISConfigCtrl();

            txtServiceName.Text = "MapGuide Server Trunk";
        }

        protected override void OnLoad(EventArgs e)
        {
            _apacheConf.Wrapper.Dock = DockStyle.Fill;
            _iis7Conf.Wrapper.Dock = DockStyle.Fill;
            TAB_APACHE.Controls.Add(_apacheConf.Wrapper);
            TAB_IIS7.Controls.Add(_iis7Conf.Wrapper);
        }

        private IConfigurationView _apacheConf;
        private IConfigurationView _iis7Conf;

        private IConfigurationView GetActiveView()
        {
            if (tabControl1.SelectedTab == TAB_APACHE)
                return _apacheConf;
            else
                return _iis7Conf;
        }

        private void btnSourceDir_Click(object sender, EventArgs e)
        {
            if (folderBrowser.ShowDialog() == DialogResult.OK)
            {
                txtSourceDirectory.Text = folderBrowser.SelectedPath;
            }
        }

        private void txtSourceDirectory_TextChanged(object sender, EventArgs e)
        {
            _iis7Conf.Config.BuildOutputPath = _apacheConf.Config.BuildOutputPath = txtSourceDirectory.Text;
        }

        private void txtBatchOutput_TextChanged(object sender, EventArgs e)
        {
            _iis7Conf.Config.BatchFileOutputDirectory = _apacheConf.Config.BatchFileOutputDirectory = txtBatchOutput.Text;
        }

        private void txtServiceName_TextChanged(object sender, EventArgs e)
        {
            _iis7Conf.Config.MapGuideServiceName = _apacheConf.Config.MapGuideServiceName = txtServiceName.Text;
        }

        private void chkInstallServices_CheckedChanged(object sender, EventArgs e)
        {
            _iis7Conf.Config.InstallServices = _apacheConf.Config.InstallServices = chkInstallServices.Checked;
        }

        private void chkWriteMentorDictPath_CheckedChanged(object sender, EventArgs e)
        {
            _iis7Conf.Config.WriteMentorDictionaryPath = _apacheConf.Config.WriteMentorDictionaryPath = chkWriteMentorDictPath.Checked;
        }

        private void btnConfigure_Click(object sender, EventArgs e)
        {
            try
            {
                GetActiveView().Config.Execute();
                if (!chkInstallServices.Checked)
                    MessageBox.Show("Batch scripts saved to " + GetActiveView().Config.BatchFileOutputDirectory);
                //Service install not implemented
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (folderBrowser.ShowDialog() == DialogResult.OK)
            {
                txtBatchOutput.Text = folderBrowser.SelectedPath;
            }
        }

        private void chk64Bit_CheckedChanged(object sender, EventArgs e)
        {
            _iis7Conf.Config.Is64BitMapGuide = _apacheConf.Config.Is64BitMapGuide = chk64Bit.Checked;
        }
    }
}
