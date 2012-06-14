#region Disclaimer / License
// Copyright (C) 2012, Jackie Ng, jumpinjackie@gmail.com
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
using System.IO;
using System.Xml;

namespace InstantSetup.Core
{
    public class FdoProviderRegistration
    {
        private string _fdoRegUtilPath;

        private IEnumerable<string> _providerDllPaths;

        public FdoProviderRegistration(string fdoRegUtilPath, IEnumerable<string> providerDllPaths)
        {
            _fdoRegUtilPath = fdoRegUtilPath;
            _providerDllPaths = providerDllPaths;
        }

        public FdoRegistrationResult[] Execute()
        {
            var providersXmlPath = Path.Combine(Path.GetDirectoryName(_fdoRegUtilPath), "providers.xml");
            var doc = new XmlDocument();
            doc.InnerXml = "<FeatureProviderRegistry></FeatureProviderRegistry>";
            doc.Save(providersXmlPath);

            List<FdoRegistrationResult> results = new List<FdoRegistrationResult>();
            foreach (var dllPath in _providerDllPaths)
            {
                var dll = System.IO.Path.GetFileName(dllPath);
                var proc = new System.Diagnostics.Process();
                var psi = proc.StartInfo;
                psi.FileName = _fdoRegUtilPath;
                psi.WorkingDirectory = System.IO.Path.GetDirectoryName(_fdoRegUtilPath);
                psi.Arguments = "-r " + dll;

                proc.Start();
                proc.WaitForExit();

                results.Add(new FdoRegistrationResult() { DllPath = dllPath, ReturnCode = proc.ExitCode });
            }
            return results.ToArray();
        }
    }

    public class FdoRegistrationResult
    {
        public string DllPath { get; set; }

        public int ReturnCode { get; set; }
    }
}
