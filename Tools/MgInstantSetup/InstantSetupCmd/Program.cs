using InstantSetup.Core;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InstantSetupCmd
{
    class Program
    {
        static void Main(string[] args)
        {
            Options options = new Options();
            if (CommandLine.Parser.Default.ParseArguments(args, options))
            {
                AbstractSetupConfigurationProcess proc = null;
                switch (options.WebTierConfig)
                {
                    case MapGuideWebConfiguration.Apache:
                        proc = CreateApacheConfig(options);
                        break;
                    case MapGuideWebConfiguration.IIS:
                        proc = CreateIISConfig(options);
                        break;
                    default:
                        Console.WriteLine("Unknown web tier configuration: {0}", options.WebTierConfig);
                        Environment.Exit(1);
                        break;
                }
                if (proc != null)
                {
                    proc.WriteSummary(Console.WriteLine);
                    //Do FDO registration first
                    var fdoPath = Path.Combine(options.SourceRootDir, "Server", "FDO");
                    var regUtil = Path.Combine(fdoPath, "FdoRegUtil.exe");
                    var reg = new FdoProviderRegistration(regUtil, options.FdoProviders.Select(n => Path.Combine(fdoPath, n + "Provider.dll")));
                    reg.WriteSummary(Console.WriteLine);
                    
                    var results = reg.Execute();
                    foreach (var res in results)
                    {
                        Console.WriteLine("FdoRegUtil - {0} returned {1}", res.DllPath, res.ReturnCode);
                    }

                    proc.Execute();
                    Console.WriteLine("Batch scripts saved to: {0}", options.BatchFileOutputDirectory);
                    Environment.Exit(0);
                }
            }
        }

        private static AbstractSetupConfigurationProcess CreateIISConfig(Options options)
        {
            IISSetupConfigurationProcess proc = new IISSetupConfigurationProcess();

            proc.AppCmdPath = options.AppCmdPath;
            proc.ApplicationPool = options.ApplicationPool;
            proc.BackupConfigFiles = options.BackupConfigFiles;
            proc.BackupConfigPath = options.BackupConfigPath;
            proc.BatchFileOutputDirectory = options.BatchFileOutputDirectory;
            proc.BuildOutputPath = options.SourceRootDir;
            proc.DefaultViewer = options.DefaultViewer;
            proc.EnableDotNet = options.EnableDotNet;
            proc.EnablePhp = options.EnablePhp;
            proc.InstallServices = options.InstallServices;
            proc.Is64BitMapGuide = options.Is64BitMapGuideForIIS;
            proc.MapGuideServiceName = options.MapGuideServiceName;
            proc.ServerAdminPort = options.ServerAdminPort;
            proc.ServerClientPort = options.ServerClientPort;
            proc.ServerSitePort = options.ServerSitePort;
            proc.UsingIIS7 = true;
            proc.VirtualDirectoryName = options.VirtualDirectoryName;
            proc.WebSiteName = options.WebSiteName;
            proc.WriteMentorDictionaryPath = options.WriteMentorDictionaryPath;

            return proc;
        }

        private static AbstractSetupConfigurationProcess CreateApacheConfig(Options options)
        {
            ApacheSetupConfigurationProcess proc = new ApacheSetupConfigurationProcess();

            proc.ApacheDirName = options.ApacheDirName;
            proc.ApachePortNumber = options.ApachePortNumber;
            proc.BackupConfigFiles = options.BackupConfigFiles;
            proc.BackupConfigPath = options.BackupConfigPath;
            proc.BatchFileOutputDirectory = options.BatchFileOutputDirectory;
            proc.BuildOutputPath = options.SourceRootDir;
            proc.DefaultViewer = options.DefaultViewer;
            proc.EnableJava = options.EnableJava;
            proc.EnablePhp = options.EnablePhp;
            proc.HttpdServiceName = options.HttpdServiceName;
            proc.InstallServices = options.InstallServices;
            proc.MapGuideServiceName = options.MapGuideServiceName;
            proc.ServerAdminPort = options.ServerAdminPort;
            proc.ServerClientPort = options.ServerClientPort;
            proc.ServerSitePort = options.ServerSitePort;
            proc.TomcatPortNumber = options.TomcatPortNumber;
            proc.VirtualDirectoryName = options.VirtualDirectoryName;
            proc.WriteMentorDictionaryPath = options.WriteMentorDictionaryPath;

            return proc;
        }
    }
}
