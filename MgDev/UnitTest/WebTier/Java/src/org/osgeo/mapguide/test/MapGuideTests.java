package org.osgeo.mapguide.test;

import java.util.*;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.mapguide.*;

public class MapGuideTests
{
    public static int Execute(IPlatformFactory factory, ITestLogger logger, MutableInteger testsRun) {
        int failures = 0;
        Vector<IExternalTest> tests = new Vector<IExternalTest>();
        
        //--------------- BEGIN Test Registration -----------------//
        tests.add(new ReadOnlyLayerCollectionTest());
        tests.add(new RenderingServiceTest());
        //--------------- END Test Registration -------------------//

        for (IExternalTest test : tests)
        {
            try
            {
                String clsName = CommonUtility.GetTypeName(test);
                logger.WriteLine("****** Executing MapGuide test: " + clsName + " *********");
                Console.WriteLine("Executing external MapGuide test: " + clsName);
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