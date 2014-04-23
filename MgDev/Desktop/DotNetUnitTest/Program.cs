using System;
using System.Collections.Generic;
using System.Text;
using OSGeo.MapGuide;
using System.Diagnostics;

namespace DotNetUnitTest
{
    public class Program
    {
        static int passed = 0;
        static int failed = 0;

        public static void Test(Action method)
        {
            try
            {
                method();
                passed++;
                Console.Write(".");
                Trace.TraceInformation("PASSED: " + method.Method.Name);
            }
            catch (AssertException ex)
            {
                //Console.WriteLine(ex.Message);
                failed++;
                Console.Write("F");
                Trace.TraceInformation("FAILED: " + method.Method.Name);
                //Trace.TraceInformation("F");
            }
        }

        public static void Main()
        {
            MgdPlatform.Initialize("Platform.ini");

            var resTest = new ResourceServiceTests();
            try
            {
                resTest.TestStart();

                Test(resTest.TestCase_ApplyResourcePackage);
                Test(resTest.TestCase_ResourceExists);
                //Test(resTest.TestCase_EnumerateResources);
                Test(resTest.TestCase_SetResource);
                Test(resTest.TestCase_MoveResource);
                Test(resTest.TestCase_CopyResource);
                Test(resTest.TestCase_GetResourceContent);
                Test(resTest.TestCase_GetResourceContents);
                Test(resTest.TestCase_GetResourceHeader);
                Test(resTest.TestCase_EnumerateReferences);
                Test(resTest.TestCase_ChangeResourceOwner);
                Test(resTest.TestCase_InheritPermissionsFrom);

                Test(resTest.TestCase_EnumerateResourceData);
                Test(resTest.TestCase_SetResourceData);
                Test(resTest.TestCase_RenameResourceData);
                Test(resTest.TestCase_GetResourceData);
                Test(resTest.TestCase_DeleteResourceData);

                Test(resTest.TestCase_DeleteResource);

                Test(resTest.TestCase_EnumerateUnmanagedData);
            }
            finally
            {
                resTest.TestEnd();
            }

            var featTest = new FeatureServiceTests();
            try
            {
                featTest.TestStart();

                Test(featTest.TestCase_GetFeatureProviders);
                Test(featTest.TestCase_TestConnectionSdfProvider);
                //Test(featTest.TestCase_TestFdoConnectionManager);
                Test(featTest.TestCase_GetConnectionPropertyValues);
                Test(featTest.TestCase_GetCapabilities);
                Test(featTest.TestCase_TestConnectionResourceIdentifier);
                Test(featTest.TestCase_GetSchemas);
                Test(featTest.TestCase_GetClasses);
                Test(featTest.TestCase_GetClassDefinition);
                Test(featTest.TestCase_DescribeSchema);
                Test(featTest.TestCase_ApplySchema);
                Test(featTest.TestCase_SelectFeatures);
                Test(featTest.TestCase_SelectAggregate);
                //    Test(featTest.TestCase_UpdateFeatures);
                //  TODO write test case when know how to make command collection
                Test(featTest.TestCase_ExecuteSqlQuery);
                Test(featTest.TestCase_ExecuteSqlNonQuery);
                Test(featTest.TestCase_GetSpatialContexts);
                Test(featTest.TestCase_GetLongTransactions);
                Test(featTest.TestCase_SetLongTransaction);
                //Test(featTest.TestCase_GetFeatures);
                //Test(featTest.TestCase_CloseFeatureReader);
                //Test(featTest.TestCase_GetSqlRows);
                //Test(featTest.TestCase_CloseSqlReader);
                //Test(featTest.TestCase_GetRaster);
                //Test(featTest.TestCase_GetDataRows);
                //Test(featTest.TestCase_CloseDataReader);
                //Test(featTest.TestCase_JoinFeatures);
                //Test(featTest.TestCase_JoinFeaturesChainedInner1ToMany);
                Test(featTest.TestCase_CreateFeatureSource);
                //Test(featTest.TestCase_BenchmarkSelectFeatures);
                //Test(featTest.TestCase_ConcurrentAccess);
            }
            finally
            {
                featTest.TestEnd();
            }

            var renTest = new RenderingServiceTests();
            try
            {
                renTest.TestStart();

                Test(renTest.TestCase_SymbologyPoints);
                Test(renTest.TestCase_SymbologyPointsParam);
                Test(renTest.TestCase_SymbologyLines);
                Test(renTest.TestCase_SymbologyLinesCrossTick);
                Test(renTest.TestCase_SymbologyPolygons);

                Test(renTest.TestCase_Annotation1);
                Test(renTest.TestCase_Annotation2);
                Test(renTest.TestCase_Annotation3);

                Test(renTest.TestCase_RenderDynamicOverlay);
                Test(renTest.TestCase_RenderMap);
                Test(renTest.TestCase_RenderMapWithWatermark);
                Test(renTest.TestCase_RenderLegend);
                //Test(renTest.TestCase_QueryFeatures);

                //Test(renTest.TestCase_RendererPerformance);
            }
            finally
            {
                renTest.TestEnd();
            }

            Console.WriteLine("\n{0} tests passed {1} tests failed", passed, failed);
            Trace.TraceInformation("{0} tests passed {1} tests failed", passed, failed);
        }
    }
}
