package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class FeatureCommandCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        MgPropertyCollection propVals = new MgPropertyCollection();
        MgInt32Property prop = new MgInt32Property("prop", 1);
        propVals.addItem(prop);
        MgUpdateFeatures update = new MgUpdateFeatures("class2", propVals, "where cat < dog");
        MgInsertFeatures insert = new MgInsertFeatures("class3", propVals);
        MgDeleteFeatures del = new MgDeleteFeatures("class1", "where cat > dog");

        MgFeatureCommandCollection coll = new MgFeatureCommandCollection();
        coll.add(update);
        coll.add(insert);
        coll.add(del);

        Assert.AreEqual(3, coll.getCount());
        Assert.AreEqual(MgFeatureCommandType.DeleteFeatures, coll.getItem(2).getCommandType());
        coll.setItem(0, coll.getItem(1));

        String txt = "";
        //for (MgFeatureCommand cmd : coll)
        for (int i = 0; i < coll.getCount(); i++)
        {
            MgFeatureCommand cmd = coll.getItem(i);
            txt += "[" + cmd.getCommandType() + "]";
        }
        Assert.AreEqual("[0][0][2]", txt);
    }
}