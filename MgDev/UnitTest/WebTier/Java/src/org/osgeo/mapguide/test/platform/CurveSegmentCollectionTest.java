package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class CurveSegmentCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        GeomBuild build = new GeomBuild();
        MgArcSegment geom1 = build.createArcSegment(2.0);
        MgArcSegment geom2 = build.createArcSegment(12.0);
        MgArcSegment geom3 = build.createArcSegment(2.0);

        MgCurveSegmentCollection coll = new MgCurveSegmentCollection();
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
        //for (MgCurveSegment geom : coll)
        for (int i = 0; i < coll.getCount(); i++)
        {
            MgCurveSegment geom = coll.getItem(i);
            width += geom.envelope().getWidth();
        }
        Assert.AreEqual(geom1.envelope().getWidth() * 3.0, width);
    }
}