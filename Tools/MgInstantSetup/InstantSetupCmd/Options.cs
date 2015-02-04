using CommandLine;
using CommandLine.Text;
using InstantSetup.Core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace InstantSetupCmd
{
    public enum MapGuideWebConfiguration
    {
        Apache,
        IIS
    }

    public class Options
    {
        [Option('v', "viewer", DefaultValue = ApiType.Php, HelpText = "The default viewer type that the mapviewerajax virtual directory will bind to (Php, Java or DotNet)")]
        public ApiType DefaultViewer { get; set; }

        [Option("admin-port", DefaultValue = 2820, HelpText = "The port to listen on for admin connections")]
        public int ServerAdminPort { get; set; }

        [Option("client-port", DefaultValue = 2821, HelpText = "The port to listen on for client connections")]
        public int ServerClientPort { get; set; }

        [Option("site-port", DefaultValue = 2822, HelpText = "The port to listen on for site connections")]
        public int ServerSitePort { get; set; }

        [Option("write-mentor-dictionary-path", DefaultValue = false, HelpText = "Write code to set up MENTOR_DICTIONARY_PATH on batch scripts. Only applicable for MGOS 2.2 or older")]
        public bool WriteMentorDictionaryPath { get; set; }

        [Option("enable-php", DefaultValue = true, HelpText = "Enable PHP for this MapGuide installation")]
        public bool EnablePhp { get; set; }

        [Option("64bit", DefaultValue = false, HelpText = "This is a 64-bit MapGuide installation. Must be explicitly set for IIS-based install")]
        public bool Is64BitMapGuideForIIS { get; set; }

        [Option('o', "output", Required = true, HelpText = "The path where generated batch scripts will be saved to")]
        public string BatchFileOutputDirectory { get; set; }

        [Option("service-name", HelpText = "The name of the service to use if installing MapGuide as a windows service. Only applicable if --install-services is specified")]
        public string MapGuideServiceName { get; set; }

        [Option("install-services", DefaultValue = false, HelpText = "If true, will generate batch scripts to register MapGuide as a windows service")]
        public bool InstallServices { get; set; }

        [Option("backup-path", HelpText = "The directory where backups of configuration files will be copied to")]
        public string BackupConfigPath { get; set; }

        [Option("backup-files", DefaultValue = false, HelpText = "If true, will create backups of existing configuration files before overwriting")]
        public bool BackupConfigFiles { get; set; }

        [Option("source-dir", Required = true, HelpText = "The root path of the MapGuide installation (where InstantSetupCmd will do its configuration)")]
        public string SourceRootDir { get; set; }

        [Option("virtual-dir", DefaultValue = "mapguide", HelpText = "The name of the mapguide virtual directory to create on IIS/Apache")]
        public string VirtualDirectoryName { get; set; }

        [Option("webtier-config", Required = false, DefaultValue = MapGuideWebConfiguration.Apache, HelpText = "The web tier configuration to use (IIS / Apache)")]
        public MapGuideWebConfiguration WebTierConfig { get; set; }

        [OptionArray("fdo-providers", Required = true, HelpText = "The list of FDO providers to register (eg: SDF SHP SQLite)")]
        public string[] FdoProviders { get; set; }

#region Apache-specific
        [Option("enable-java", DefaultValue = false, HelpText = "Enables Java for this MapGuide installation")]
        public bool EnableJava { get; set; }

        [Option("tomcat-port", DefaultValue = 8019, HelpText = "The port number to use for the bundled copy of Tomcat. Only applicable if using Apache configuration and Java is enabled")]
        public int TomcatPortNumber { get; set; }

        [Option("httpd-port", DefaultValue = 8018, HelpText = "The port number to use for the bundled copy of Apache. Only applicable if using Apache configuration")]
        public int ApachePortNumber { get; set; }

        [Option("httpd-service-name", HelpText = "The service name to use for Apache if registering it as a service. Only applicable if using Apache confguration and --install-services is specified")]
        public string HttpdServiceName { get; set; }

        [Option("apache-dir-name", DefaultValue = "Apache24", HelpText = "The name of the directory in the install root that contains the bundled copy of Apache")]
        public string ApacheDirName { get; set; }
#endregion

#region IIS-specific
        [Option("enable-dotnet", DefaultValue = true, HelpText = "Enables .net for this MapGuide installation")]
        public bool EnableDotNet { get; set; }

        [Option("application-pool", DefaultValue = "MapGuideTrunkAppPool", HelpText = "The name of the IIS application pool to create")]
        public string ApplicationPool { get; set; }

        [Option("default-web-site", DefaultValue = "Default Web Site", HelpText = "The name of the default IIS web site to install the web extensions into")]
        public string WebSiteName { get; set; }

        [Option("appcmd-path", DefaultValue = "C:\\Windows\\System32\\inetsrv\\appcmd.exe", HelpText = "The path to appcmd.exe for automating IIS configuration")]
        public string AppCmdPath { get; set; }
#endregion

        [ParserState]
        public IParserState LastParserState { get; set; }

        [HelpOption]
        public string GetUsage()
        {
            return HelpText.AutoBuild(this,
              (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }
    }
}
