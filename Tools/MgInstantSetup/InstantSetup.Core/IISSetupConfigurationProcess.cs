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
using System.Text;

namespace InstantSetup.Core
{
    public class IISSetupConfigurationProcess : AbstractSetupConfigurationProcess
    {
        public IISSetupConfigurationProcess()
            : base()
        {
            this.VirtualDirectoryName = "mapguide";

            this.UsingIIS7 = true;
            this.AppCmdPath = "C:\\Windows\\System32\\inetsrv\\appcmd.exe";
            this.WebSiteName = "Default Web Site";
            this.ApplicationPool = "MapGuideTrunkAppPool";

            this.EnableDotNet = true;
            this.EnablePhp = false;
        }

        public bool EnableDotNet { get; set; }

        /// <summary>
        /// Gets whether we are using IIS7 or above. Meaning we can use appcmd.exe
        /// Otherwise we will use the IIS Admin COM interfaces
        /// </summary>
        public bool UsingIIS7 { get; set; }

        public string AppCmdPath { get; set; }

        public string WebSiteName { get; set; }

        public string ApplicationPool { get; set; }

        protected override void DoBackupConfigFiles()
        {
            
        }

        protected override void ValidateConfigSettings()
        {
            
        }

        protected override void WriteAdditionalBatchFiles()
        {
            
        }

        protected override void ConfigureWebServer()
        {
            var cmd = new AppCmd(this.AppCmdPath, this.WebSiteName, this.VirtualDirectoryName, this.ApplicationPool, this.WebTierRootDir, this.WebTierPhpDir);
            cmd.SetupCore();
            if (this.EnableDotNet)
                cmd.SetDefaultDotNetViewer();
            else if (this.EnablePhp)
                cmd.SetDefaultPhpViewer();
        }
    }
}
