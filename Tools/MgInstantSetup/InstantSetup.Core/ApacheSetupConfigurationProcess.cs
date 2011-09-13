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
using System.IO;
using System.Globalization;

namespace InstantSetup.Core
{
    public class ApacheSetupConfigurationProcess : AbstractSetupConfigurationProcess
    {
        public ApacheSetupConfigurationProcess()
            : base()
        {
            this.HttpdServiceName = "ApacheMapGuideTrunk";
            this.ApachePortNumber = 8018;
            this.TomcatPortNumber = 8019;

            this.VirtualDirectoryName = "mapguide";
            this.HttpdServiceName = "ApacheMapGuide";
        }

        public bool EnableJava { get; set; }

        /// <summary>
        /// The port number that Apache Tomcat will run under. Only applies if 
        /// java is enabled and is required if EnableJava = true
        /// </summary>
        public int TomcatPortNumber { get; set; }

        public int ApachePortNumber { get; set; }

        public string HttpdServiceName { get; set; }

        public virtual string WebTierApacheDir
        {
            get
            {
                return Path.Combine(this.WebTierRootDir, "Apache2");
            }
        }

        public virtual string WebTierTomcatDir
        {
            get { return Path.Combine(this.WebTierRootDir, "Tomcat"); }
        }

        protected override void ValidateConfigSettings()
        {

        }

        private static string Apachify(string text)
        {
            string str = text.Replace("\\", "/");
            return str.EndsWith("/") ? str : (str + "/");
        }

        protected override void ConfigureWebServer()
        {
            var httpdConf = Path.Combine(this.WebTierApacheDir, "conf\\httpd.conf");
            var backup = httpdConf + ".instantsetupbak";
            if (!File.Exists(backup))
                File.Copy(httpdConf, backup); //This has to be original, so back it up
            else
                File.Copy(backup, httpdConf, true); //Restore from original backup

            var httpdBuffer = new StringBuilder(File.ReadAllText(httpdConf));

            httpdBuffer.Replace("%MG_WEB_ROOT%", Apachify(this.WebTierPublicDir));
            httpdBuffer.Replace("%MG_WEB_PORT%", this.ApachePortNumber.ToString(CultureInfo.InvariantCulture));
            httpdBuffer.Replace("%MG_WEB_PHP%", Apachify(this.WebTierPhpDir));
            httpdBuffer.Replace("%MG_WEB_APACHE%", Apachify(this.WebTierApacheDir));
            httpdBuffer.Replace("%MG_VIRTUAL_DIR%", this.VirtualDirectoryName);

            if (this.EnableJava)
            {
                httpdBuffer.Replace("%MG_INCLUDE_TOMCAT%", "Include conf/tomcat.conf");
            }
            else
            {
                httpdBuffer.Replace("%MG_INCLUDE_TOMCAT%", "#Uncomment to enable the Java API\n#Include conf/tomcat.conf");
            }

            if (this.DefaultViewer == ApiType.Java)
            {
                httpdBuffer.Replace("%MG_PHP_API%", "#");
                httpdBuffer.Replace("%MG_JAVA_API%", "");
            }
            else if (this.DefaultViewer == ApiType.Php)
            {
                httpdBuffer.Replace("%MG_PHP_API%", "");
                httpdBuffer.Replace("%MG_JAVA_API%", "#");
            }
            else
            {
                throw new InvalidOperationException(".net viewer is not supported in the Apache configuration");
            }


            //Do the tomcat substitutions anyway
            var tomcatConf = Path.Combine(this.WebTierApacheDir, "conf\\tomcat.conf");
            var tomcatBuffer = new StringBuilder(File.ReadAllText(tomcatConf));

            tomcatBuffer.Replace("%MG_WEB_APACHE%", Apachify(this.WebTierApacheDir));
            tomcatBuffer.Replace("%MG_VIRTUAL_DIR%", this.VirtualDirectoryName);

            var catalinaConf = Path.Combine(this.WebTierTomcatDir, "conf\\Catalina\\localhost\\mapguide.xml");
            var catalinaBuffer = new StringBuilder(File.ReadAllText(catalinaConf));

            catalinaBuffer.Replace("%MG_WEB_ROOT%", this.WebTierPublicDir);

            File.WriteAllText(catalinaConf, catalinaBuffer.ToString());
            File.WriteAllText(tomcatConf, tomcatBuffer.ToString());
            File.WriteAllText(httpdConf, httpdBuffer.ToString());
        }

        protected override void DoBackupConfigFiles()
        {
            
        }

        protected override void WriteAdditionalBatchFiles()
        {
            //Write httpd batch file
            string httpdText = string.Format(Properties.Resources.APACHE_WEB, this.CsMapDictionaryDir, Path.Combine(this.WebTierApacheDir, "bin"));
            File.WriteAllText(Path.Combine(this.BatchFileOutputDirectory, "mgwebtier.bat"), httpdText);

            //Write service installer batch file
            httpdText = string.Format(Properties.Resources.APACHE_WEB_INSTALL, Path.Combine(this.WebTierApacheDir, "bin"), this.HttpdServiceName);
            File.WriteAllText(Path.Combine(this.BatchFileOutputDirectory, "mgapacheinstall.bat"), httpdText);

            //Write service uninstaller batch file
            httpdText = string.Format(Properties.Resources.APACHE_WEB_UNINSTALL, Path.Combine(this.WebTierApacheDir, "bin"), this.HttpdServiceName);
            File.WriteAllText(Path.Combine(this.BatchFileOutputDirectory, "mgapacheuninstall.bat"), httpdText);
        }
    }
}
