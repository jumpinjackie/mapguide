package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class LinearRingCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        GeomBuild build = new GeomBuild();
        MgLinearRing geom1 = build.createLinearRing(1.0);
        MgLinearRing geom2 = build.createLinearRing(5.0);
        MgLinearRing geom3 = build.createLinearRing(1.0);

        MgLinearRingCollection coll = new MgLinearRingCollection();
        coll.add(geom1);
        coll.add(geom2);
        coll.add(geom3);

        Assert.AreEqual(3, coll.getCount());
        Assert.IsTrue(geom1.envelope().contains(coll.getItem(0).envelope()));
        Assert.IsTrue(coll.getItem(0).envelope().contains(coll.getItem(2).envelope()));
        Assert.IsFalse(coll.getItem(0).envelope().contains(coll.getItem(1).envelope()));
        coll.setItem(0, coll.getItem(1));
        Assert.IsTrue(coll.getItem(0).envelope().contains(coll.getItem(1).envelope()));

        double width = 0.0;
        //for (MgLinearRing geom : coll)
        for (int i = 0; i < coll.getCount(); i++)
        {
            MgLinearRing geom = coll.getItem(i);
            width += geom.envelope().getWidth();
        }
        Assert.AreEqual(geom1.envelope().getWidth() * 3.0, width);
    }
}