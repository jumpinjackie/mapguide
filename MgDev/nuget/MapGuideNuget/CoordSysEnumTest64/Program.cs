using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using OSGeo.MapGuide;
using System.IO;

namespace CoordSysEnumTest
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            MgCoordinateSystemFactory fact = new MgCoordinateSystemFactory();
            MgCoordinateSystemCatalog cat = fact.GetCatalog();
            cat.SetDictionaryDir(Path.Combine(Application.StartupPath, "Dictionaries"));

            Application.Run(new MainForm(fact));
        }
    }
}
