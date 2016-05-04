using OSGeo.MapGuide;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace MapGuideNuget
{
    public partial class Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            MgUserInformation user = new MgUserInformation("Anonymous", "");
            MgSite site = new MgSite();
            site.Open(user);
            string sessionId = site.CreateSession();
            user.SetMgSessionId(sessionId);

            MgSiteConnection siteConn = new MgSiteConnection();
            siteConn.Open(user);

            MgResourceIdentifier mdfId = new MgResourceIdentifier("Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition");

            System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
            sw.Start();
            MgMap map1 = new MgMap(siteConn);
            map1.Create(mdfId, "ColdStart");
            sw.Stop();
            Trace.Write("ColdStart - " + sw.ElapsedMilliseconds + "ms");

            sw.Reset();

            sw.Start();
            MgMap map2 = new MgMap(siteConn);
            map2.Create(mdfId, "WarmStart");
            sw.Stop();

            Trace.Write("WarmStart - " + sw.ElapsedMilliseconds + "ms");

            sw.Start();
            MgMap map3 = new MgMap(siteConn);
            map3.Create(mdfId, "WarmStart2");
            sw.Stop();

            Trace.Write("WarmStart2 - " + sw.ElapsedMilliseconds + "ms");
        }
    }
}