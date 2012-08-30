using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using OSGeo.MapGuide;
using System.Diagnostics;

namespace DotNetHarness
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            var sw = new Stopwatch();
            sw.Start();
            MgdPlatform.Initialize("Platform.ini");
            sw.Stop();

            Trace.TraceInformation("Platform initialization took {0}ms", sw.ElapsedMilliseconds);

            Application.ApplicationExit += (s, e) =>
            {
                MgdPlatform.Terminate();
            };
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }
    }
}
