package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class StringPropertyCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        MgStringProperty prop1 = new MgStringProperty("prop1", "val1");
        MgStringProperty prop2 = new MgStringProperty("prop2", "val2");
        MgStringProperty prop3 = new MgStringProperty("prop3", "val3");
        MgStringProperty prop4 = new MgStringProperty("prop4", "val4");

        MgStringPropertyCollection coll = new MgStringPropertyCollection();
        coll.add(prop1);
        coll.add(prop2);
        coll.remove(prop3);
        coll.remove(coll.getItem(1));
        Assert.AreEqual(1, coll.getCount());
        coll.add(prop3);
        coll.setItem(1, prop2);
        coll.insert(2, prop3);
        Assert.AreEqual(2, coll.indexOf(prop3));
        coll.add(prop4);

        String txt = "";
        //for (MgStringProperty prop : coll)
        for (int i = 0; i < coll.getCount(); i++)
        {
            MgStringProperty prop = coll.getItem(i);
            txt += ".getItem(" + prop.getName() + ")";
        }
        Assert.AreEqual(".getItem(prop1).getItem(prop2).getItem(prop3).getItem(prop4)", txt);
    }
}