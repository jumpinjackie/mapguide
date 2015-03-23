using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using OSGeo.MapGuide;
using System.IO;

namespace DesktopConsoleTest
{
    class Program
    {
        static void Main(string[] args)
        {
            MgdPlatform.Initialize("Platform.ini");

            string currentDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);

            //Test mapguide-api-base
            MgCoordinateSystemFactory csFact = new MgCoordinateSystemFactory();

            Console.WriteLine("Testing coordinate system categories");
            MgStringCollection csCategories = csFact.EnumerateCategories();
            for (int i = 0; i < csCategories.GetCount(); i++)
            {
                Console.WriteLine(csCategories.GetItem(i));
            }

            //Test mg-desktop
            MgdServiceFactory serviceFact = new MgdServiceFactory();
            Console.Write("Can we create a MgdFeatureService? ");
            MgdFeatureService featSvc = (MgdFeatureService)serviceFact.CreateService(MgServiceType.FeatureService);
            Console.WriteLine("yes");
            Console.Write("Can we create a MgdResourceService? ");
            MgdResourceService resSvc = (MgdResourceService)serviceFact.CreateService(MgServiceType.ResourceService);
            Console.WriteLine("yes");

            MgdPlatform.Terminate();
            Console.WriteLine("Things look all good :) Press any key to continue");
            Console.Read();
        }
    }
}
