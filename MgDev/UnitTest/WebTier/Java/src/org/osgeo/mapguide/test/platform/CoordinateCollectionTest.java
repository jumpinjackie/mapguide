package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class CoordinateCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        MgGeometryFactory gf = new MgGeometryFactory();
        MgCoordinate c1 = gf.createCoordinateXY(1.0, 1.0);
        MgCoordinate c2 = gf.createCoordinateXY(2.0, 2.0);
        MgCoordinate c3 = gf.createCoordinateXY(3.0, 3.0);
        MgCoordinate c4 = gf.createCoordinateXY(4.0, 4.0);

        MgCoordinateCollection coll = new MgCoordinateCollection();
        coll.add(c1);
        coll.insert(1, c2);
        coll.add(c3);
        coll.add(c4);

        Assert.AreEqual(4, coll.getCount());
        Assert.AreEqual(1.0, coll.getItem(0).getX());
        coll.setItem(3, coll.getItem(2));
        Assert.AreEqual(3.0, coll.getItem(3).getX());

        double sum = 0.0;
        //for (MgCoordinate coord : coll)
        for (int i = 0; i < coll.getCount(); i++)
        {
            MgCoordinate coord = coll.getItem(i);
            sum += coord.getX();
        }
        Assert.AreEqual(9.0, sum);
    }
}