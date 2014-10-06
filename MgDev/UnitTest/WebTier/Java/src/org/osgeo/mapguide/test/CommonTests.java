package org.osgeo.mapguide.test;

import java.util.*;

import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class CommonTests
{
    public static int Execute(IPlatformFactory factory, ITestLogger logger, MutableInteger testsRun) {
        int failures = 0;
        Vector<IExternalTest> tests = new Vector<IExternalTest>();

        //--------------- BEGIN Test Registration -----------------//
        tests.add(new ByteReaderTest());
        tests.add(new ByteReaderMemoryConstructorTest());
        tests.add(new ByteReaderStringConstructorTest());
        tests.add(new BatchPropertyCollectionTest());
        tests.add(new ClassDefinitionCollectionTest());
        tests.add(new CoordinateCollectionTest());
        tests.add(new CurvePolygonCollectionTest());
        tests.add(new CurveRingCollectionTest());
        tests.add(new CurveSegmentCollectionTest());
        tests.add(new CurveStringCollectionTest());
        tests.add(new FeatureCommandCollectionTest());
        tests.add(new FeatureSchemaCollectionTest());
        tests.add(new GeometryCollectionTest());
        tests.add(new IntCollectionTest());
        tests.add(new LayerCollectionTest());
        tests.add(new LayerGroupCollectionTest());
        tests.add(new LinearRingCollectionTest());
        tests.add(new LineStringCollectionTest());
        tests.add(new MapCollectionTest());
        tests.add(new PropertyCollectionTest());
        tests.add(new PropertyDefinitionCollectionTest());
        tests.add(new StringCollectionTest());
        tests.add(new StringPropertyCollectionTest());
        //--------------- END Test Registration -------------------//

        for (IExternalTest test : tests)
        {
            try
            {
                String clsName = CommonUtility.GetTypeName(test);
                logger.WriteLine("****** Executing platform test: " + clsName + " *********");
                Console.WriteLine("Executing external platform test: " + clsName);
                test.Execute(factory, logger);
            }
            catch (AssertException ex)
            {
                logger.WriteLine("Assertion failure: " + ex.getMessage());
                Console.WriteLine("Assertion failure: " + ex.getMessage());
                failures++;
            }
            catch (Exception ex)
            {
                logger.WriteLine("General failure: " + CommonUtility.ExceptionToString(ex));
                Console.WriteLine("General failure: " + CommonUtility.ExceptionToString(ex));
                failures++;
            }
            finally
            {
                testsRun.increment();
            }
        }
        return failures;
    }
}