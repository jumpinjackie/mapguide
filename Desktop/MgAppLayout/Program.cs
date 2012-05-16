using System;
using System.Collections.Generic;
using System.Windows.Forms;
using OSGeo.MapGuide;
using OSGeo.MapGuide.Viewer.AppLayoutEngine;
using System.Xml.Serialization;
using System.IO;
using OSGeo.MapGuide.Viewer.Desktop;
using System.Diagnostics;

namespace MgAppLayout
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string [] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.ThreadException += new System.Threading.ThreadExceptionEventHandler(Application_ThreadException);
            
            //Must call MgPlatform.Initialize() before we can work with anything from the MapGuide API
            try
            {
                var sw = new Stopwatch();
                sw.Start();
                MgPlatform.Initialize("Platform.ini");
                sw.Stop();
                Trace.TraceInformation("Platform initialization took {0}ms", sw.ElapsedMilliseconds);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(), "Error");
                return;
            }

            AppLayout layout = null;

            var ser = new XmlSerializer(typeof(AppLayout));
            if (args.Length == 1)
            {
                using (var file = File.OpenRead(args[0]))
                {
                    layout = (AppLayout)ser.Deserialize(file);
                }
            }/*
            else
            {
                layout = AppLayout.CreateDefault("MapGuide Desktop App Layout Example", "Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition");
                using (var file = File.OpenWrite("Sheboygan.AppLayout"))
                {
                    ser.Serialize(file, layout);
                }
            }*/

            var mdfId = new MgResourceIdentifier(layout.Map.MapDefinition);
            var provider = new MgDesktopMapViewerProvider(null);
            var resSvc = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);
            if (resSvc.ResourceExists(mdfId))
                provider.LoadMap(new MgdMap(mdfId));
            var frm = new Shell(layout, provider);
            Application.ApplicationExit += new EventHandler(OnAppExit);
            Application.Run(frm);
        }

        static void OnAppExit(object sender, EventArgs e)
        {
            MgPlatform.Terminate();
        }

        static void Application_ThreadException(object sender, System.Threading.ThreadExceptionEventArgs e)
        {
            MessageBox.Show(e.Exception.ToString());
        }
    }
}
