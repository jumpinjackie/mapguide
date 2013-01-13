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
            string wl = "Library://Samples/Sheboygan/Layouts/SheboyganAsp.WebLayout";
            Response.Redirect("http://localhost/mapguide/mapviewerajax/?WEBLAYOUT=" + wl + "&SESSION=" + sessionId);
        }
    }
}