package org.osgeo.mapguide.test.platform;

import java.io.*;
import java.util.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class ClassDefinitionCollectionTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        MgClassDefinition class1 = new MgClassDefinition();
        class1.setName("class1");

        MgClassDefinition class2 = new MgClassDefinition();
        class2.setName("class2");

        MgClassDefinition class3 = new MgClassDefinition();
        class3.setName("class3");

        MgClassDefinitionCollection coll = new MgClassDefinitionCollection();
        coll.insert(0, class3);
        coll.insert(0, class2);
        coll.insert(0, class1);

        Assert.AreEqual(3, coll.getCount());
        Assert.AreEqual("class2", coll.getItem(1).getName());

        MgClassDefinition tmp = coll.getItem(0);
        coll.setItem(0, coll.getItem(1));
        coll.setItem(1, coll.getItem(2));
        coll.setItem(2, tmp);

        String str = "";
        for (MgClassDefinition def : coll)
        {
            str = str + ".getItem(" + def.getName() + ")";
        }
        Assert.AreEqual(".getItem(class2).getItem(class3).getItem(class1)", str);

        MgFeatureSchema parent = new MgFeatureSchema("Parent", "");
        MgClassDefinitionCollection items = parent.getClasses();
        MgClassDefinition item1 = new MgClassDefinition(); item1.setName("Item1");
        MgClassDefinition item2 = new MgClassDefinition(); item2.setName("Item2");
        MgClassDefinition item3 = new MgClassDefinition(); item3.setName("Item3");
        MgClassDefinition item4 = new MgClassDefinition(); item4.setName("Item4");

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
        for (MgClassDefinition item : items) {
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
        Collection<MgClassDefinition> facade = items;
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

        ArrayList<MgClassDefinition> testCollection = new ArrayList<MgClassDefinition>();
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
        Assert.IsTrue(!facade.remove(new MgFeatureSchema()), "Expected remove() of different type to return false");
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
            Assert.IsTrue(o instanceof MgClassDefinition, "Expected item to be MgClassDefinition");
        }
        MgClassDefinition[] fitems = facade.toArray(new MgClassDefinition[0]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 2, "Expected 2 item array");

        fitems = facade.toArray(new MgClassDefinition[1]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 2, "Expected 2 item array");

        fitems = facade.toArray(new MgClassDefinition[3]);
        Assert.IsTrue(fitems != null, "Expected non-null item array");
        Assert.IsTrue(fitems.length == 3, "Expected 3 item array");
        Assert.IsTrue(fitems[0] != null, "Expected non-null item #1");
        Assert.IsTrue(fitems[1] != null, "Expected non-null item #2");
        Assert.IsTrue(fitems[2] == null, "Expected null item #3");
    }
}