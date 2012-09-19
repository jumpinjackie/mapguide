//#define PROFILE
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
#if PROFILE
                using (new CallMeasure("MgdPlatform.Initialize"))
                {
                    MgdPlatform.Initialize("Platform.ini");
                }
#else
                MgdPlatform.Initialize("Platform.ini");
#endif
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
            {
#if PROFILE
                using (new CallMeasure("MgdMap constructor (cold start)"))
                {
                    var mapCold = new MgdMap(mdfId);
                }
                MgdMap map = null;
                using (new CallMeasure("MgdMap constructor (warm start)"))
                {
                    map = new MgdMap(mdfId);
                }
                using (new CallMeasure("MgMapViewerProvider.LoadMap"))
                {
                    provider.LoadMap(map);
                }
#else
                provider.LoadMap(new MgdMap(mdfId));
#endif
            }
            var frm = Shell.Instance;
            ((Shell)frm).Initialize(layout, provider);
            Application.Run((Shell)frm);
            MgdPlatform.Terminate();
        }

        static void Application_ThreadException(object sender, System.Threading.ThreadExceptionEventArgs e)
        {
            MessageBox.Show(e.Exception.ToString());
        }
    }

    class CallMeasure : IDisposable
    {
        private Stopwatch _sw;
        private string _msg;

        public CallMeasure(string msg)
        {
            _msg = msg;
            _sw = new Stopwatch();
            _sw.Start();
        }

        public void Dispose()
        {
            _sw.Stop();
            Console.WriteLine("{0} - {1}ms", _msg, _sw.ElapsedMilliseconds);
        }
    }
}
