package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class CurveStringCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        GeomBuild build = new GeomBuild();
        MgCurveString geom1 = build.createCurveString(2.0);
        MgCurveString geom2 = build.createCurveString(12.0);
        MgCurveString geom3 = build.createCurveString(2.0);

        MgCurveStringCollection coll = new MgCurveStringCollection();
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
        //for (MgCurveString geom : coll)
        for (int i = 0; i < coll.getCount(); i++)
        {
            MgCurveString geom = coll.getItem(i);
            width += geom.envelope().getWidth();
        }
        Assert.AreEqual(geom1.envelope().getWidth() * 3.0, width);
    }
}