package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class IntCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        MgIntCollection coll = new MgIntCollection();
        coll.addItem(10);
        coll.addItem(20);
        coll.addItem(30);
        coll.addItem(40);
        coll.addItem(50);
        Assert.AreEqual(20, coll.getItem(1));
        Assert.AreEqual(30, coll.getItem(2));
        Assert.AreEqual(5, coll.getCount());

        int j = 0;
        for (int i = 0; i < coll.getCount(); i++)
        {
            j += coll.getItem(i);
        }
        Assert.AreEqual(150, j);
    }
}