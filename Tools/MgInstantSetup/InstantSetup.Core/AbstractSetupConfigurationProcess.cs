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
    public enum ApiType
    {
        Php,
        Java,
        DotNet
    }

    /// <summary>
    /// Base configuration setup process
    /// </summary>
    public abstract class AbstractSetupConfigurationProcess
    {
        protected AbstractSetupConfigurationProcess()
        {
            this.EnablePhp = true;
            this.WriteMentorDictionaryPath = false;
            this.DefaultViewer = ApiType.Php;

            //Port numbers specified not to intentionally clash with an existing 
            //MGOS installation
            this.ServerAdminPort = 2820;
            this.ServerClientPort = 2821;
            this.ServerSitePort = 2822;
        }

        /// <summary>
        /// Gets or sets whether the installation of MapGuide to be configured is 64-bit
        /// </summary>
        public bool Is64BitMapGuide { get; set; }

        public bool WriteMentorDictionaryPath { get; set; }

        public string BatchFileOutputDirectory { get; set; }

        public string MapGuideServiceName { get; set; }

        public bool InstallServices { get; set; }

        public string BackupConfigPath { get; set; }

        public bool BackupConfigFiles { get; set; }

        /// <summary>
        /// Gets or sets the build output path where the result of build.bat
        /// in the MapGuide Source root copies files to 
        /// </summary>
        public string BuildOutputPath { get; set; }

        /// <summary>
        /// The server bin directory
        /// </summary>
        public virtual string ServerBinDir
        {
            get
            {
                return Path.Combine(ServerRootDir, "bin");
            }
        }

        /// <summary>
        /// The server tier root directory
        /// </summary>
        public virtual string ServerRootDir
        {
            get { return Path.Combine(BuildOutputPath, "Server"); }
        }

        /// <summary>
        /// The CS-Map Dictionary directory
        /// </summary>
        public virtual string CsMapDictionaryDir
        {
            get { return Path.Combine(BuildOutputPath, "CS-Map\\Dictionaries"); }
        }

        /// <summary>
        /// The www directory of the web tier
        /// </summary>
        public virtual string WebTierPublicDir
        {
            get { return Path.Combine(WebTierRootDir, "www"); }
        }

        /// <summary>
        /// The web tier root directory
        /// </summary>
        public virtual string WebTierRootDir
        {
            get
            {
                return Path.Combine(BuildOutputPath, "Web");
            }
        }

        /// <summary>
        /// The mapagent directory of the web tier
        /// </summary>
        public virtual string WebTierMapAgentDir
        {
            get { return Path.Combine(WebTierPublicDir, "mapagent"); }
        }

        /// <summary>
        /// The PHP web tier directory
        /// </summary>
        public virtual string WebTierPhpDir
        {
            get
            {
                return Path.Combine(WebTierRootDir, "Php");
            }
        }

        public bool EnablePhp { get; set; }

        /// <summary>
        /// The name of the mapguide virtual directory to create
        /// </summary>
        public string VirtualDirectoryName { get; set; }

        /// <summary>
        /// TCP/IP number for client operations
        /// </summary>
        public int ServerClientPort { get; set; }

        /// <summary>
        /// TCP/IP number for admin operations
        /// </summary>
        public int ServerAdminPort { get; set; }

        /// <summary>
        /// TCP/IP number for port operations
        /// </summary>
        public int ServerSitePort { get; set; }

        /// <summary>
        /// The default viewer that mapviewerajax will resolve to
        /// </summary>
        public ApiType DefaultViewer { get; set; }

        private IniFile _serverConfig;
        private IniFile _webConfig;
        private IniFile _phpConfig;

        public void Execute()
        {
            ValidateConfigSettings();

            if (this.BackupConfigFiles)
            {
                if (string.IsNullOrEmpty(this.BackupConfigPath))
                    throw new InvalidOperationException("Please specify a backup path for config files");
                else
                {
                    if (!Directory.Exists(this.BackupConfigPath))
                        Directory.CreateDirectory(this.BackupConfigPath);

                    DoBackupConfigFiles();
                }
            }

            //Process serverconfig.ini

            string serverConfigIni = Path.Combine(this.ServerBinDir, "serverconfig.ini");
            if (!File.Exists(serverConfigIni))
                throw new InvalidOperationException("Could not find expected file: " + serverConfigIni);

            string webConfigIni = Path.Combine(this.WebTierPublicDir, "webconfig.ini");
            if (!File.Exists(webConfigIni))
                throw new InvalidOperationException("Could not find expected file: " + webConfigIni);

            string phpIni = Path.Combine(this.WebTierPhpDir, "php.ini");
            if (!File.Exists(phpIni))
                throw new InvalidOperationException("Could not find expected file: " + phpIni);

            _serverConfig = new IniFile(serverConfigIni);
            _webConfig = new IniFile(webConfigIni);
            _phpConfig = new IniFile(phpIni);

            SetServerConfigProperties();
            SetWebConfigProperties();
            SetPhpConfigProperties();

            ConfigureWebServer();

            if (this.InstallServices)
            {
                throw new NotImplementedException("Service installation not implemented");
            }
            else
            {
                if (this.WriteMentorDictionaryPath)
                {
                    //Write server batch file
                    string serverText = string.Format(Properties.Resources.SERVER, this.CsMapDictionaryDir, this.ServerBinDir);
                    File.WriteAllText(Path.Combine(this.BatchFileOutputDirectory, "mgserver.bat"), serverText);

                    //Write service install batch file
                    serverText = string.Format(Properties.Resources.SERVER_INSTALL, this.CsMapDictionaryDir, this.ServerBinDir, this.MapGuideServiceName);
                    File.WriteAllText(Path.Combine(this.BatchFileOutputDirectory, "mgserverinstall.bat"), serverText);

                    //Write service uninstall batch file
                    serverText = string.Format(Properties.Resources.SERVER_UNINSTALL, this.CsMapDictionaryDir, this.ServerBinDir, this.MapGuideServiceName);
                    File.WriteAllText(Path.Combine(this.BatchFileOutputDirectory, "mgserveruninstall.bat"), serverText);
                }
                else //Post RFC 122
                {
                    //Write server batch file
                    string serverText = string.Format(Properties.Resources.SERVER_NO_MENTOR, this.ServerBinDir);
                    File.WriteAllText(Path.Combine(this.BatchFileOutputDirectory, "mgserver.bat"), serverText);

                    //Write service install batch file
                    serverText = string.Format(Properties.Resources.SERVER_INSTALL_NO_MENTOR, this.ServerBinDir, this.MapGuideServiceName);
                    File.WriteAllText(Path.Combine(this.BatchFileOutputDirectory, "mgserverinstall.bat"), serverText);

                    //Write service uninstall batch file
                    serverText = string.Format(Properties.Resources.SERVER_UNINSTALL_NO_MENTOR, this.ServerBinDir, this.MapGuideServiceName);
                    File.WriteAllText(Path.Combine(this.BatchFileOutputDirectory, "mgserveruninstall.bat"), serverText);
                }
                WriteAdditionalBatchFiles();
            }
        }

        protected abstract void WriteAdditionalBatchFiles();

        protected abstract void ValidateConfigSettings();

        protected abstract void ConfigureWebServer();

        private void SetPhpConfigProperties()
        {
            _phpConfig.IniWriteValue("PHP", "extension_dir", Path.Combine(this.WebTierPhpDir, "ext"));
            _phpConfig.IniWriteValue("Session", "session.save_path", Path.Combine(this.WebTierRootDir, "Temp"));
        }

        private void SetWebConfigProperties()
        {
            //Port numbers
            _webConfig.IniWriteValue("AdministrativeConnectionProperties","Port",this.ServerAdminPort.ToString(CultureInfo.InvariantCulture));
            _webConfig.IniWriteValue("ClientConnectionProperties","Port",this.ServerClientPort.ToString(CultureInfo.InvariantCulture));
            _webConfig.IniWriteValue("SiteConnectionProperties","Port",this.ServerSitePort.ToString(CultureInfo.InvariantCulture));

            _webConfig.IniWriteValue("GeneralProperties","ResourcesPath",Path.Combine(this.WebTierMapAgentDir, "Resources"));
            _webConfig.IniWriteValue("GeneralProperties","TempPath",Path.Combine(this.WebTierRootDir, "Temp"));

            //Post RFC 122
            if (!this.WriteMentorDictionaryPath)
                _webConfig.IniWriteValue("GeneralProperties", "MentorDictionaryPath", this.CsMapDictionaryDir);

            _webConfig.IniWriteValue("SiteConnectionProperties","IpAddress","127.0.0.1");

            _webConfig.IniWriteValue("WebApplicationProperties","TemplateRootFolder",Path.Combine(this.WebTierPublicDir, "fusion\\templates\\mapguide"));
            _webConfig.IniWriteValue("WebApplicationProperties", "WidgetInfoFolder", Path.Combine(this.WebTierPublicDir, "fusion\\widgets\\widgetinfo"));
            _webConfig.IniWriteValue("WebApplicationProperties", "ContainerInfoFolder", Path.Combine(this.WebTierPublicDir, "fusion\\containerinfo"));
        }

        private void SetServerConfigProperties()
        {
            //Port numbers
            _serverConfig.IniWriteValue("AdministrativeConnectionProperties","Port",this.ServerAdminPort.ToString(CultureInfo.InvariantCulture));
            _serverConfig.IniWriteValue("ClientConnectionProperties","Port",this.ServerClientPort.ToString(CultureInfo.InvariantCulture));
            _serverConfig.IniWriteValue("SiteConnectionProperties","Port",this.ServerSitePort.ToString(CultureInfo.InvariantCulture));

            _serverConfig.IniWriteValue("GeneralProperties","LogsPath",Path.Combine(ServerRootDir, "Logs"));

            _serverConfig.IniWriteValue("GeneralProperties","TempPath",Path.Combine(ServerRootDir, "Temp"));
            _serverConfig.IniWriteValue("GeneralProperties","ResourcesPath",Path.Combine(ServerRootDir, "Resources"));
            _serverConfig.IniWriteValue("GeneralProperties","WfsDocumentPath",Path.Combine(ServerRootDir, "Wfs"));
            _serverConfig.IniWriteValue("GeneralProperties","WmsDocumentPath",Path.Combine(ServerRootDir, "Wms"));
            _serverConfig.IniWriteValue("GeneralProperties","FdoPath",Path.Combine(ServerRootDir, "Fdo"));

            //Post RFC 122
            if (!this.WriteMentorDictionaryPath)
                _serverConfig.IniWriteValue("GeneralProperties", "MentorDictionaryPath", this.CsMapDictionaryDir);
            
            _serverConfig.IniWriteValue("ResourceServiceProperties","PackagesPath",Path.Combine(ServerRootDir, "Packages"));

            _serverConfig.IniWriteValue("ResourceServiceProperties","LibraryRepositoryPath",Path.Combine(ServerRootDir, "Repositories\\Library"));
            _serverConfig.IniWriteValue("ResourceServiceProperties","LibraryResourceDataFilePath", Path.Combine(ServerRootDir, "Repositories\\DataFiles"));
            _serverConfig.IniWriteValue("ResourceServiceProperties","SessionRepositoryPath",Path.Combine(ServerRootDir, "Repositories\\Session"));
            _serverConfig.IniWriteValue("ResourceServiceProperties","SessionResourceDataFilePath", Path.Combine(ServerRootDir, "Repositories\\Session\\DataFiles"));
            _serverConfig.IniWriteValue("ResourceServiceProperties","SiteRepositoryPath",Path.Combine(ServerRootDir, "Repositories\\Site"));
            _serverConfig.IniWriteValue("TileServiceProperties", "TileCachePath", Path.Combine(ServerRootDir, "Repositories\\TileCache"));
            _serverConfig.IniWriteValue("ResourceServiceProperties","ResourceSchemaFilePath", Path.Combine(ServerRootDir, "Schema"));
        }

        protected abstract void DoBackupConfigFiles();
    }
}
