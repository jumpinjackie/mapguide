using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Diagnostics;

namespace InstantSetup.Core
{
    public class AppCmd
    {
        //Core

        /// <summary>
        /// 0 - app pool name
        /// </summary>
        const string CREATE_APP_POOL = "add apppool /name:\"{0}\" /managedRuntimeVersion:\"v4.0\"";

        /// <summary>
        /// 0 - php path
        /// </summary>
        const string CREATE_FASTCGI = "set config /section:system.webServer/fastCGI /+\"[fullPath='{0}\\php-cgi.exe']\"";

        /// <summary>
        /// 0 - app pool name
        /// </summary>
        const string SET_APP_POOL_PROPERTIES = "set config /section:applicationPools /[name='{0}'].processModel.idleTimeout:00:00:00  /[name='{0}'].recycling.disallowOverlappingRotation:True";

        /// <summary>
        /// no parameters
        /// </summary>
        const string UNLOCK_SERVER_HANDLERS = "unlock config /section:handlers";

        /// <summary>
        /// 0 - web site name
        /// </summary>
        const string UNLOCK_WEBSITE_HANDLERS = "unlock config \"{0}\" /section:handlers";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - web tier root dir
        /// </summary>
        const string CREATE_MAPGUIDE_VDIR = "add app /site.name:\"{0}\" /path:\"/{1}\" /physicalPath:\"{2}\\www\"";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - php dir
        /// </summary>
        const string CREATE_PHP_SCRIPT_MAPPING = "set config \"{0}/{1}\" /section:handlers /+\"[name='PHP_via_FastCGI',path='*.php',verb='*',scriptProcessor='{2}\\php-cgi.exe',modules='FastCgiModule',resourceType='File',requireAccess='Script']\"";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - web tier root dir
        /// </summary>
        const string CREATE_MAPAGENT_VDIR = "add app /site.name:\"{0}\" /path:\"/{1}/mapagent\" /physicalPath:\"{2}\\www\\mapagent\"";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir
        /// </summary>
        const string UNLOCK_MAPAGENT_HANDLERS = "set config \"{0}/{1}/mapagent\" /section:handlers /accessPolicy:Read,Script,Execute";
        
        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - web tier root dir
        /// </summary>
        const string SETUP_MAPAGENT_ISAPI = "set config \"{0}/{1}/mapagent\" /section:handlers /+\"[name='MapGuide MapAgent ISAPI handler',path='mapagent.fcgi',verb='*',scriptProcessor='{2}\\www\\mapagent\\isapi_MapAgent.dll',modules='IsapiModule']\"";

        /// <summary>
        /// 0 - web tier root dir
        /// </summary>
        const string SET_MAPAGENT_ISAPI_ACCESS = "set config /section:isapiCgiRestriction /+\"[path='{0}\\www\\mapagent\\isapi_MapAgent.dll',allowed='True',description='MapGuide MapAgent ISAPI handler']\"";

        /// <summary>
        /// 0 - app pool name
        /// </summary>
        const string SET_32BIT_APP_POOL = "set apppool /apppool.name:\"{0}\" /enable32BitAppOnWin64:true";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - app pool name
        /// </summary>
        const string SET_MAPGUIDE_VDIR_APP_POOL = "set app \"{0}/{1}\" /applicationPool:\"{2}\"";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - app pool name
        /// </summary>
        const string SET_MAPAGENT_VDIR_APP_POOL = "set app \"{0}/{1}/mapagent\" /applicationPool:\"{2}\"";

        /// <summary>
        /// no parameters
        /// </summary>
        const string UNLOCK_HTTP_ERRORS_SERVER = "unlock config /section:httpErrors";
        
        /// <summary>
        /// 0 - web site name
        /// </summary>
        const string UNLOCK_HTTP_ERRORS_WEBSITE = "unlock config \"{0}\" /section:httpErrors";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir
        /// </summary>
        const string UNLOCK_HTTP_ERRORS_MAPGUIDE = "unlock config \"{0}/{1}\" /section:httpErrors";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir
        /// </summary>
        const string RESET_401_HANDLER = "set config \"{0}/{1}\" /section:httpErrors /-[statusCode='401']";

        /// <summary>
        /// no parameters
        /// </summary>
        const string ADD_JSON_MIME_TYPE = "set config /section:staticContent /+\"[fileExtension='.json',mimeType='application/json']\"";
        
        //PHP viewer
        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - web tier root dir
        /// </summary>
        const string CREATE_MAPVIEWERAJAX_PHP = "add app /site.name:\"{0}\" /path:\"/{1}/mapviewerajax\" /physicalPath:\"{2}\\www\\mapviewerphp\"";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir
        /// </summary>
        const string SET_MAPVIEWERAJAX_DEFAULT_DOC_PHP = "set config \"{0}/{1}/mapviewerajax\" /section:defaultDocument /+files.[value='ajaxviewer.php']";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - app pool name
        /// </summary>
        const string SET_MAPVIEWERAJAX_PHP_APP_POOL = "set app \"{0}/{1}/mapviewerajax\" /applicationPool:\"{2}\"";

        //ASP.net viewer

        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - web tier root dir
        /// </summary>
        const string CREATE_MAPVIEWERAJAX_NET = "add app /site.name:\"{0}\" /path:\"/{1}/mapviewerajax\" /physicalPath:\"{2}\\www\\mapviewernet\"";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir
        /// </summary>
        const string SET_MAPVIEWERAJAX_DEFAULT_DOC_NET = "set config \"{0}/{1}/mapviewerajax\" /section:defaultDocument /+files.[value='ajaxviewer.aspx']";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - app pool name
        /// </summary>
        const string SET_MAPVIEWERAJAX_NET_APP_POOL = "set app \"{0}/{1}/mapviewerajax\" /applicationPool:\"{2}\"";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - web tier root dir
        /// </summary>
        const string CREATE_MAPVIEWERNET_VDIR = "add app /site.name:\"{0}\" /path:\"/{1}/mapviewernet\" /physicalPath:\"{2}\\www\\mapviewernet\"";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir
        /// </summary>
        const string SET_MAPVIEWERNET_DEFAULT_DOC = "set config \"{0}/{1}/mapviewernet\" /section:defaultDocument /+files.[value='ajaxviewer.aspx']";

        /// <summary>
        /// 0 - web site name, 1 - virtual dir, 2 - app pool name
        /// </summary>
        const string SET_MAPVIEWERNET_APP_POOL = "set app \"{0}/{1}/mapviewernet\" /applicationPool:\"{2}\"";

        private string _webSite;
        private string _virtualDir;
        private string _appPoolName;
        private string _webExtensionsRootDir;
        private string _phpDir;
        private string _appCmdPath;

        public AppCmd(string appCmdPath, string webSite, string virtualDir, string appPoolName, string webExtensionsRootDir, string phpDir)
        {
            if (!File.Exists(appCmdPath))
                throw new FileNotFoundException(appCmdPath);
            if (string.IsNullOrEmpty(webSite))
                throw new ArgumentException("Empty value: webSite");
            if (string.IsNullOrEmpty(virtualDir))
                throw new ArgumentNullException("Empty value: virtualDir");
            if (string.IsNullOrEmpty(appPoolName))
                throw new ArgumentNullException("Empty value: appPoolName");
            if (!Directory.Exists(webExtensionsRootDir))
                throw new DirectoryNotFoundException(webExtensionsRootDir);
            if (!Directory.Exists(phpDir))
                throw new DirectoryNotFoundException(phpDir);

            _appCmdPath = appCmdPath;
            _webSite = webSite;
            _virtualDir = virtualDir;
            _appPoolName = appPoolName;
            _webExtensionsRootDir = webExtensionsRootDir;
            _phpDir = phpDir;
        }

        private Process StartAppCmdProcess(string args)
        {
            var psi = new ProcessStartInfo(_appCmdPath);
            psi.Arguments = args;
            psi.RedirectStandardOutput = true;
            psi.UseShellExecute = false;
            Trace.TraceInformation("Running appcmd.exe " + args);
            return Process.Start(psi);
        }

        internal void SetupCore()
        {
            Process p = null;
            p = StartAppCmdProcess(string.Format(CREATE_FASTCGI, _phpDir));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(CREATE_APP_POOL, _appPoolName));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(SET_APP_POOL_PROPERTIES, _appPoolName));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(UNLOCK_SERVER_HANDLERS);
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(UNLOCK_WEBSITE_HANDLERS, _webSite));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(CREATE_MAPGUIDE_VDIR, _webSite, _virtualDir, _webExtensionsRootDir));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(CREATE_PHP_SCRIPT_MAPPING, _webSite, _virtualDir, _phpDir));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(CREATE_MAPAGENT_VDIR, _webSite, _virtualDir, _webExtensionsRootDir));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(UNLOCK_MAPAGENT_HANDLERS, _webSite, _virtualDir));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(SETUP_MAPAGENT_ISAPI, _webSite, _virtualDir, _webExtensionsRootDir));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(SET_MAPAGENT_ISAPI_ACCESS, _webExtensionsRootDir));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(SET_32BIT_APP_POOL, _appPoolName));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(SET_MAPGUIDE_VDIR_APP_POOL, _webSite, _virtualDir, _appPoolName));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(SET_MAPAGENT_VDIR_APP_POOL, _webSite, _virtualDir, _appPoolName));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(UNLOCK_HTTP_ERRORS_SERVER);
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(UNLOCK_HTTP_ERRORS_WEBSITE, _webSite));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(RESET_401_HANDLER, _webSite, _virtualDir));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(ADD_JSON_MIME_TYPE);
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();

            //Setup mapviewernet
            p = StartAppCmdProcess(string.Format(CREATE_MAPVIEWERNET_VDIR, _webSite, _virtualDir, _webExtensionsRootDir));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(SET_MAPVIEWERNET_DEFAULT_DOC, _webSite, _virtualDir));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
            p = StartAppCmdProcess(string.Format(SET_MAPVIEWERNET_APP_POOL, _webSite, _virtualDir, _appPoolName));
            Trace.TraceInformation(p.StandardOutput.ReadToEnd()); p.WaitForExit();
            //if (p.ExitCode != 0) Debugger.Break();
        }

        internal void SetDefaultDotNetViewer()
        {
            StartAppCmdProcess(string.Format(CREATE_MAPVIEWERAJAX_NET, _webSite, _virtualDir, _webExtensionsRootDir)).WaitForExit();
            StartAppCmdProcess(string.Format(SET_MAPVIEWERAJAX_DEFAULT_DOC_NET, _webSite, _virtualDir)).WaitForExit();
            StartAppCmdProcess(string.Format(SET_MAPVIEWERAJAX_NET_APP_POOL, _webSite, _virtualDir, _appPoolName)).WaitForExit();
        }

        internal void SetDefaultPhpViewer()
        {
            StartAppCmdProcess(string.Format(CREATE_MAPVIEWERAJAX_PHP, _webSite, _virtualDir, _webExtensionsRootDir)).WaitForExit();
            StartAppCmdProcess(string.Format(SET_MAPVIEWERAJAX_DEFAULT_DOC_PHP, _webSite, _virtualDir)).WaitForExit();
            StartAppCmdProcess(string.Format(SET_MAPVIEWERAJAX_PHP_APP_POOL, _webSite, _virtualDir, _appPoolName)).WaitForExit();
        }
    }
}
