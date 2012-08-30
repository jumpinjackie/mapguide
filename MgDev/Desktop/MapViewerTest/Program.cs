using System;
using System.Collections.Generic;
using System.Windows.Forms;
using OSGeo.MapGuide;
using System.Diagnostics;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide.Viewer.Desktop;

namespace MapViewerTest
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
                MgdPlatform.Initialize("Platform.ini");
                sw.Stop();
                Trace.TraceInformation("Platform initialization took {0}ms", sw.ElapsedMilliseconds);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(), "Error");
                return;
            }
            
            //MgAppWindow is our Main Window class full of drag-and-drop component goodness. Go ahead and
            //take a look at MgAppWindow.cs source, not much code there except for the 3 generic invoke components and the
            //single custom selection handler. Everything else is encapsulated by designer-friendly drag and drop components
            //Most of the work is dragging and dropping components into the designer surface, and setting lots of properties
            var frm = new MgAppWindow();
            
            //We can initialize without specifying a map (allowing for components that do not 
            //require a loaded map to still be used). You just have to call LoadMap() on the MgMapViewer
            //instance when ready (not demonstrated here). So if you do load a package here, you will
            //have to restart the application
            MgResourceIdentifier resId = null;
            if (args.Length == 1)
            {
                resId = new MgResourceIdentifier(args[0]);
                resId.Validate();
            }

            var fact = new MgdServiceFactory();
            var resSvc = (MgdResourceService)fact.CreateService(MgServiceType.ResourceService);
            MgdMap map = null;

            if (resId != null && resSvc.ResourceExists(resId))
                map = new MgdMap(resId);

            //This is just a pass-through call to MgMapViewer.Init()
            frm.LoadMap(new MgDesktopMapViewerProvider(map));

            Application.ApplicationExit += new EventHandler(OnAppExit);
            Application.Run(frm);
        }

        static void OnAppExit(object sender, EventArgs e)
        {
            MgdPlatform.Terminate();
        }

        static void Application_ThreadException(object sender, System.Threading.ThreadExceptionEventArgs e)
        {
            MessageBox.Show(e.Exception.ToString());
        }
    }
}
