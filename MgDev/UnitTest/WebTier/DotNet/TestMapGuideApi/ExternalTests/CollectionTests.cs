using OSGeo.MapGuide.Test.Common;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Web.ExternalTests
{
    public class CollectionTestsMapPlotCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            string coordsys = "GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]]";
            MgEnvelope env = new MgEnvelope(10, 10, 20, 20);
            MgMap map1 = (MgMap)factory.CreateMap(coordsys, env, "map1");
            MgMap map2 = (MgMap)factory.CreateMap(coordsys, env, "map2");
            MgMap map3 = (MgMap)factory.CreateMap(coordsys, env, "map3");

            MgPlotSpecification spec = new MgPlotSpecification((float)8.0, (float)11.0, "in", (float)0.0, (float)0.0, (float)5.0, (float)5.0);
            MgResourceIdentifier resId = new MgResourceIdentifier("Library://test.WebLayout");
            MgLayout layout1 = new MgLayout(resId, "Title1", "in");
            MgMapPlot plot1 = new MgMapPlot(map1, spec, layout1);

            MgLayout layout2 = new MgLayout(resId, "Title2", "in");
            MgMapPlot plot2 = new MgMapPlot(map2, spec, layout2);

            MgLayout layout3 = new MgLayout(resId, "Title3", "in");
            MgMapPlot plot3 = new MgMapPlot(map3, spec, layout3);

            MgMapPlotCollection coll = new MgMapPlotCollection();
            coll.Add(plot1);
            coll.Add(plot2);
            coll.Remove(plot1);

            Assert.AreEqual(1, coll.Count);

            coll.Insert(0, plot1);
            coll.Insert(2, plot3);
            MgMapPlot tmp = coll[2];
            coll[2] = tmp;

            Assert.AreEqual(3, coll.Count);


            string txt = "";
            foreach (MgMapPlot plot in coll)
            {
                txt += "[" + plot.GetLayout().GetTitle() + "]";
            }
            Assert.AreEqual("[Title1][Title2][Title3]", txt);
        }
    }
}
