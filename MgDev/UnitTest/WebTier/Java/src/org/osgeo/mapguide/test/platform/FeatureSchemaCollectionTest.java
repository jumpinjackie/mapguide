package org.osgeo.mapguide.test.platform;

import java.io.*;
import java.util.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class FeatureSchemaCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        MgFeatureSchema schema1 = new MgFeatureSchema("schema1", "desc1");
        MgFeatureSchema schema2 = new MgFeatureSchema("schema2", "desc2");
        MgFeatureSchema schema3 = new MgFeatureSchema("schema3", "desc3");

        MgFeatureSchemaCollection coll = new MgFeatureSchemaCollection();
        coll.add(schema1);
        coll.add(schema2);
        coll.add(schema3);

        Assert.AreEqual(3, coll.getCount());
        Assert.AreEqual("schema3", coll.getItem(2).getName());
        coll.setItem(0, coll.getItem(2));

        String txt = "";
        for (MgFeatureSchema schema : coll)
        {
            txt += "[" + schema.getName() + "]";
        }
        Assert.AreEqual("[schema3][schema2][schema3]", txt);

        MgFeatureSchemaCollection items = new MgFeatureSchemaCollection();
        MgFeatureSchema item1 = new MgFeatureSchema("Item1", "");
        MgFeatureSchema item2 = new MgFeatureSchema("Item2", "");
        MgFeatureSchema item3 = new MgFeatureSchema("Item3", "");
        MgFeatureSchema item4 = new MgFeatureSchema("Item4", "");

        //Items inserted the MapGuide API way
        items.addItem(item1);
        items.addItem(item2);
        items.addItem(item3);

        Assert.IsTrue(items.getCount() == 3, "Expected 3 items");
        Assert.IsTrue(!items.isEmpty(), "Expected non-empty item collection");

        int i = 0;
        //Looping the classic way
        for (i = 0; i < items.getCount(); i++) {
            switch (i){
                case 0:
                    Assert.IsTrue(items.getItem(i).getName().equals("Item1"), "Expected item #" + (i+1) + " to be Item1");
                    break;
                case 1:
                    Assert.IsTrue(items.getItem(i).getName().equals("Item2"), "Expected item #" + (i+1) + " to be Item2");
                    break;
                case 2:
                    Assert.IsTrue(items.getItem(i).getName().equals("Item3"), "Expected item #" + (i+1) + " to be Item3");
                    break;
            }
        }

        //Looping the iterative way
        i = 0;
        for (MgFeatureSchema item : items) {
            String itemName = item.getName();
            switch (i){
                case 0:
                    Assert.IsTrue(itemName.equals("Item1"), "Expected item #" + (i+1) + " to be Item1");
                    break;
                case 1:
                    Assert.IsTrue(itemName.equals("Item2"), "Expected item #" + (i+1) + " to be Item2");
                    break;
                case 2:
                    Assert.IsTrue(itemName.equals("Item3"), "Expected item #" + (i+1) + " to be Item3");
                    break;
            }
            i++;
        }

        items.clear();
        Assert.IsTrue(items.getCount() == 0, "Expected empty item collection");
        Assert.IsTrue(items.isEmpty(), "Expected empty item collection");

        //Now test through java.util.Collection facade
        Collection<MgFeatureSchema> facade = items;
        //add()
        Assert.IsTrue(facade.add(item1), "Could not add item1 the java.util.Collection way");
        Assert.IsTrue(facade.add(item2), "Could not add item2 the java.util.Collection way");
        Assert.IsTrue(facade.add(item3), "Could not add item3 the java.util.Collection way");

        Assert.IsTrue(facade.size() == 3, "Expected 3 items");
        Assert.IsTrue(!facade.isEmpty(), "Expected non-empty item collection");

        //contains()
        Assert.IsTrue(facade.contains(item1), "item1 should exist");
        Assert.IsTrue(facade.contains(item2), "item2 should exist");
        Assert.IsTrue(facade.contains(item3), "item3 should exist");

        ArrayList<MgFeatureSchema> testCollection = new ArrayList<MgFeatureSchema>();
        testCollection.add(item1);
        testCollection.add(item2);
        testCollection.add(item3);

        //containsAll()
        Assert.IsTrue(facade.containsAll(testCollection), "All items in test collection should match");
        testCollection.clear();
        testCollection.add(item1);
        testCollection.add(item4);
        Assert.IsTrue(!facade.containsAll(testCollection), "Should not have matched test collection");

        facade.clear();
        Assert.IsTrue(facade.size() == 0, "Expected 0 items");
        Assert.IsTrue(facade.isEmpty(), "Expected empty item collection");

        //addAll()
        testCollection.clear();
        testCollection.add(item1);
        testCollection.add(item2);
        testCollection.add(item3);
        Assert.IsTrue(facade.addAll(testCollection), "Expected addAll() success");
        Assert.IsTrue(facade.size() == 3, "Expected 3 items");
        Assert.IsTrue(!facade.isEmpty(), "Expected non-empty item collection");

        //remove()
        Assert.IsTrue(!facade.remove(item4), "Expected remove() of non-existent item to return false");
        Assert.IsTrue(!facade.remove(new MgClassDefinition()), "Expected remove() of different type to return false");
        Assert.IsTrue(facade.remove(item2), "Expected remove() of item2 to return true");

        //removeAll()
        testCollection.remove(item2);
        Assert.IsTrue(facade.removeAll(testCollection), "Expected removeAll() to succeed");

        //retainAll()
        Assert.IsTrue(facade.add(item1), "Could not add item1 the java.util.Collection way");
        Assert.IsTrue(facade.add(item2), "Could not add item2 the java.util.Collection way");
        Assert.IsTrue(facade.add(item3), "Could not add item3 the java.util.Collection way");
        Assert.IsTrue(facade.size() == 3, "Expected 3 schemas");
        Assert.IsTrue(!facade.isEmpty(), "Expected non-empty schema collection");

        testCollection.clear();
        testCollection.add(item1);
        testCollection.add(item3);
        Assert.IsTrue(facade.retainAll(testCollection), "Expected retainAll() to succeed");
        Assert.IsTrue(facade.contains(item1), "Expected item1 to exist");
        Assert.IsTrue(!facade.contains(item2), "Expected item2 to not exist");
        Assert.IsTrue(facade.contains(item3), "Expected item3 to exist");

        //toArray()
        Object[] itemArray = facade.toArray();
        Assert.IsTrue(itemArray != null, "Expected non-null item array");
        Assert.IsTrue(itemArray.length == 2, "Expected 2 item array");
        for (Object o : itemArray) {
            Assert.IsTrue(o instanceof MgFeatureSchema, "Expected item to be MgFeatureSchema");
        }
        MgFeatureSchema[] fitems = facade.toArray(new MgFeatureSchema[0]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 2, "Expected 2 item array");

        fitems = facade.toArray(new MgFeatureSchema[1]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 2, "Expected 2 item array");

        fitems = facade.toArray(new MgFeatureSchema[3]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 3, "Expected 3 item array");
        Assert.IsTrue(fitems[0] != null, "Expected non-null item #1");
        Assert.IsTrue(fitems[1] != null, "Expected non-null item #2");
        Assert.IsTrue(fitems[2] == null, "Expected null item #3");
    }
}