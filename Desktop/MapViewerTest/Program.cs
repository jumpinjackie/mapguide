using System;
using System.Collections.Generic;
using System.Windows.Forms;
using OSGeo.MapGuide;
using System.Diagnostics;

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
            var frm = new Form1();
            if (args.Length == 1)
            {
                try
                {
                    var resId = new MgResourceIdentifier(args[0]);
                    resId.Validate();
                    var fact = new MgServiceFactory();
                    var resSvc = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                    if (resSvc.ResourceExists(resId))
                    {
                        frm.Load += (s, e) =>
                        {
                            frm.LoadMap(resId);
                        };
                    }
                }
                catch (MgException ex)
                {
                    ex.Dispose();
                }
            }
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
