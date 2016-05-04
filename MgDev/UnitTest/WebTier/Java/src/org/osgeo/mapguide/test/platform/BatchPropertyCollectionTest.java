package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class BatchPropertyCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        MgPropertyCollection coll1 = new MgPropertyCollection();
        MgDoubleProperty dblProp = new MgDoubleProperty("DoubleProp", 1.1111);
        coll1.addItem(dblProp);

        MgPropertyCollection coll2 = new MgPropertyCollection();
        MgInt32Property intProp = new MgInt32Property("IntProp", 1);
        coll2.addItem(intProp);

        MgPropertyCollection coll3 = new MgPropertyCollection();
        MgSingleProperty single = new MgSingleProperty("SingleProp", (float)2.2222);
        coll3.addItem(single);

        MgBatchPropertyCollection coll = new MgBatchPropertyCollection();
        coll.addItem(coll1);
        coll.addItem(coll2);

        Assert.AreEqual(2, coll.getCount());
        Assert.AreEqual(MgPropertyType.Double, coll.getItem(0).getItem(0).getPropertyType());
        Assert.AreEqual(MgPropertyType.Int32, coll.getItem(1).getItem(0).getPropertyType());

        coll.setItem(1, coll3);

        String str = "";
        //for (MgPropertyCollection c : coll)
        for (int i = 0; i < coll.getCount(); i++)
        {
            MgPropertyCollection c = coll.getItem(i);
            str = str + ".getItem(" + c.getItem(0).getName() + ")";
        }
        Assert.AreEqual(".getItem(DoubleProp).getItem(SingleProp)", str);
    }
}