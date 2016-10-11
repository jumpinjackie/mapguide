using OSGeo.MapGuide;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WebApiTest
{
    class Program
    {
        static void Main(string[] args)
        {
            var webConfigPath = @"C:\Program Files\OSGeo\MapGuide\Web\www\webconfig.ini";
            MapGuideApi.MgInitializeWebTier(webConfigPath);

            MgUserInformation user = new MgUserInformation("Anonymous", "");
            MgSiteConnection conn = new MgSiteConnection();
            conn.Open(user);
            MgSite site = conn.GetSite();
            string sessionId = site.CreateSession();

            Console.WriteLine("Session ID: " + sessionId);
            MgResourceIdentifier wlId = new MgResourceIdentifier("Library://Samples/Sheboygan/Layouts/SheboyganAsp.WebLayout");
            MgResourceService resSvc = (MgResourceService)conn.CreateService(MgServiceType.ResourceService);

            MgWebLayout layout = new MgWebLayout(resSvc, wlId);
            Console.WriteLine("Web Layout Title: " + layout.GetTitle());
            Console.WriteLine("Looks good! Press any key to continue");
            Console.Read();
        }
    }
}
