package org.osgeo.mapguide.test.platform;

import java.io.*;
import java.util.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class StringCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        Vector<String> strColl = new Vector<String>();
        strColl.add("string1");
        strColl.add("string2");
        strColl.add("string3");
        strColl.add("string3");

        MgStringCollection coll1 = new MgStringCollection();
        coll1.add("Hello");

        MgStringCollection coll2 = new MgStringCollection();
        for (String str : strColl) {
            coll2.add(str);
        }
        Assert.AreEqual(4, coll2.getCount());
        MgStringCollection coll3 = coll2;
        Assert.AreEqual(4, coll3.getCount());
        for (int i = 0; i < 4; i++)
        {
            Assert.AreEqual(strColl.get(i), coll2.getItem(i));
            Assert.AreEqual(strColl.get(i), coll3.getItem(i));
        }

        MgStringCollection items = new MgStringCollection();
        String item1 = "Item1";
        String item2 = "Item2";
        String item3 = "Item3";
        String item4 = "Item4";

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
                    Assert.IsTrue(items.getItem(i).equals("Item1"), "Expected item #" + (i+1) + " to be Item1");
                    break;
                case 1:
                    Assert.IsTrue(items.getItem(i).equals("Item2"), "Expected item #" + (i+1) + " to be Item2");
                    break;
                case 2:
                    Assert.IsTrue(items.getItem(i).equals("Item3"), "Expected item #" + (i+1) + " to be Item3");
                    break;
            }
        }

        //Looping the iterative way
        i = 0;
        for (String item : items) {
            String itemName = item;
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
        Collection<String> facade = items;
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

        ArrayList<String> testCollection = new ArrayList<String>();
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
            Assert.IsTrue(o instanceof String, "Expected item to be String");
        }
        String[] fitems = facade.toArray(new String[0]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 2, "Expected 2 item array");

        fitems = facade.toArray(new String[1]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 2, "Expected 2 item array");

        fitems = facade.toArray(new String[3]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 3, "Expected 3 item array");
        Assert.IsTrue(fitems[0] != null, "Expected non-null item #1");
        Assert.IsTrue(fitems[1] != null, "Expected non-null item #2");
        Assert.IsTrue(fitems[2] == null, "Expected null item #3");
    }
}