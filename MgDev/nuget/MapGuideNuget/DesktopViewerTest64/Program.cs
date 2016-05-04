using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using OSGeo.MapGuide;

namespace DesktopViewerTest
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            MgdPlatform.Initialize("Platform.ini");
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
            MgdPlatform.Terminate();
        }
    }
}
