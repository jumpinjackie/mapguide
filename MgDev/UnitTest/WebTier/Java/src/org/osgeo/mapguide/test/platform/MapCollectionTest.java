package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class MapCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        String coordsys = "GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]]";
        MgEnvelope env = new MgEnvelope(10, 10, 20, 20);
        MgMapBase map1 = factory.createMap();
        map1.create(coordsys, env, "map1");
        MgMapBase map2 = factory.createMap();
        map2.create(coordsys, env, "map2");
        MgMapBase map3 = factory.createMap();
        map3.create(coordsys, env, "map3");
        MgMapBase map4 = factory.createMap();
        map4.create(coordsys, env, "map4");

        MgMapCollection coll = new MgMapCollection();
        coll.add(map1);
        coll.insert(1, map2);
        coll.add(map3);

        Assert.AreEqual(3, coll.getCount());
        Assert.AreEqual(coll.getItem(2).getName(), "map3");

        coll.setItem(1, map4);

        String txt = "";
        //for (MgMapBase map : coll)
        for (int i = 0; i < coll.getCount(); i++)
        {
            MgMapBase map = coll.getItem(i);
            txt += "[" + map.getName() + "]";
        }
        Assert.AreEqual("[map1][map4][map3]", txt);
    }
}