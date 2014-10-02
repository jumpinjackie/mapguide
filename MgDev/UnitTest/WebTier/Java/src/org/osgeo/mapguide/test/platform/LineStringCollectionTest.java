package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class LineStringCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        GeomBuild build = new GeomBuild();
        MgLineString geom1 = build.createLineString(5.0);
        MgLineString geom2 = build.createLineString(11.0);
        MgLineString geom3 = build.createLineString(5.0);

        MgLineStringCollection coll = new MgLineStringCollection();
        coll.add(geom1);
        coll.add(geom2);
        coll.add(geom3);

        Assert.AreEqual(3, coll.getCount());
        Assert.IsTrue(geom1.equals(coll.getItem(0)));
        Assert.IsTrue(coll.getItem(0).equals(coll.getItem(2)));
        Assert.IsFalse(coll.getItem(0).equals(coll.getItem(1)));
        coll.setItem(0, coll.getItem(1));
        Assert.IsTrue(coll.getItem(0).equals(coll.getItem(1)));

        double width = 0.0;
        //for (MgLineString geom : coll)
        for (int i = 0; i < coll.getCount(); i++)
        {
            MgLineString geom = coll.getItem(i);
            width += geom.envelope().getWidth();
        }
        Assert.AreEqual(geom1.envelope().getWidth() * 3.0, width);
    }
}