package org.osgeo.mapguide.test.platform;

import java.io.*;
import java.util.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class PropertyCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        MgPropertyCollection coll = new MgPropertyCollection();
        MgDoubleProperty dblProp = new MgDoubleProperty("DoubleProp", 1.1111);
        MgInt32Property intProp = new MgInt32Property("IntProp", 1);
        MgDateTime dateTime = new MgDateTime((short)2006, (short)9, (short)21);
        MgDateTimeProperty dateProp = new MgDateTimeProperty("DateProp", dateTime);
        MgSingleProperty single = new MgSingleProperty("SingleProp", (float)2.2222);
        coll.add(dblProp);
        coll.add(intProp);
        coll.add(dateProp);
        coll.setItem(2, single);

        MgProperty prop1 = coll.getItem(0);
        MgProperty prop2 = coll.getItem(1);
        MgProperty prop3 = coll.getItem(2);
        Assert.AreEqual(1.1111, ((MgDoubleProperty)prop1).getValue());
        Assert.AreEqual(MgPropertyType.Double, prop1.getPropertyType());
        Assert.AreEqual(MgPropertyType.Int32, prop2.getPropertyType());
        Assert.AreEqual(MgPropertyType.Single, prop3.getPropertyType());
        Assert.AreEqual((float)2.2222, ((MgSingleProperty)prop3).getValue());
        Assert.AreEqual(3, coll.getCount());

        Assert.AreEqual(MgPropertyType.Double, prop1.getPropertyType());

        String str = "";
        for (MgProperty prop : coll)
        {
            str = str + "[" + prop.getName() + "]";
        }
        Assert.AreEqual("[DoubleProp][IntProp][SingleProp]", str);

        MgPropertyCollection items = new MgPropertyCollection();
        MgProperty item1 = new MgStringProperty("Item1", "Hello World");
        MgProperty item2 = new MgInt32Property("Item2", 42);
        MgProperty item3 = new MgBooleanProperty("Item3", false);
        MgProperty item4 = new MgByteProperty("Item4", (short)4);

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
        for (MgProperty item : items) {
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
        Collection<MgProperty> facade = items;
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

        ArrayList<MgProperty> testCollection = new ArrayList<MgProperty>();
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
        i = 0;
        for (Object o : itemArray) {
            Assert.IsTrue(o instanceof MgProperty, "Expected item to be MgProperty");
            switch(i) {
                case 0:
                    Assert.IsTrue(o instanceof MgStringProperty, "Expected item to be MgStringProperty");
                    break;
                case 1:
                    Assert.IsTrue(o instanceof MgBooleanProperty, "Expected item to be MgBooleanProperty");
                    break;
            }
            i++;
        }

        MgProperty[] fitems = facade.toArray(new MgProperty[0]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 2, "Expected 2 item array");

        fitems = facade.toArray(new MgProperty[1]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 2, "Expected 2 item array");

        fitems = facade.toArray(new MgProperty[3]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 3, "Expected 3 item array");
        Assert.IsTrue(fitems[0] != null, "Expected non-null item #1");
        Assert.IsTrue(fitems[1] != null, "Expected non-null item #2");
        Assert.IsTrue(fitems[2] == null, "Expected null item #3");
    }
}