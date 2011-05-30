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
        public bool EnableDotNet { get; set; }

        /// <summary>
        /// Gets whether we are using IIS7 or above. Meaning we can use appcmd.exe
        /// Otherwise we will use the IIS Admin COM interfaces
        /// </summary>
        public bool UsingIIS7 { get; set; }

        protected override void DoBackupConfigFiles()
        {
            throw new NotImplementedException();
        }

        protected override void ValidateConfigSettings()
        {
            throw new NotImplementedException();
        }

        protected override void ConfigureWebServer()
        {
            throw new NotImplementedException();
        }

        protected override void WriteAdditionalBatchFiles()
        {
            throw new NotImplementedException();
        }
    }
}
