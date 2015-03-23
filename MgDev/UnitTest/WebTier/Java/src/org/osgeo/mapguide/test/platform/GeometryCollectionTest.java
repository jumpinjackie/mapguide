package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class GeometryCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        MgWktReaderWriter wkt = new MgWktReaderWriter();
        MgPoint geom1 = (MgPoint)wkt.read("POINT XY (1.0 1.0)");
        MgPoint geom2 = (MgPoint)wkt.read("POINT XY (2.0 2.0)");
        MgPoint geom3 = (MgPoint)wkt.read("POINT XY (1.0 1.0)");

        MgGeometryCollection coll = new MgGeometryCollection();
        coll.add(geom1);
        coll.add(geom2);
        coll.add(geom3);

        Assert.AreEqual(3, coll.getCount());
        Assert.IsTrue(geom1.equals(coll.getItem(0)));
        Assert.IsTrue(coll.getItem(0).equals(coll.getItem(2)));
        Assert.IsFalse(coll.getItem(0).equals(coll.getItem(1)));
        coll.setItem(0, coll.getItem(1));
        Assert.IsTrue(coll.getItem(0).equals(coll.getItem(1)));

        double x = 0.0;
        //for (MgGeometry geom : coll)
        for (int i = 0; i < coll.getCount(); i++)
        {
            MgGeometry geom = coll.getItem(i);
            x += ((MgPoint)geom).getCoordinate().getX();
        }
        Assert.AreEqual(5.0, x);
    }
}