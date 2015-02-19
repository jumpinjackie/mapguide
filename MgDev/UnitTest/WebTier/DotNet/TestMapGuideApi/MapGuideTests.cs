using OSGeo.MapGuide.Test.Common;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Web
{
    //Defines tests outside of the SQLite-based test suite

    public class MapGuideTests
    {
        public static int Execute(IPlatformFactory factory, ITestLogger logger, ref int testsRun)
        {
            int failures = 0;
            var types = typeof(MapGuideTests).Assembly.GetTypes();
            foreach (var type in types)
            {
                if (typeof(IExternalTest).IsAssignableFrom(type) && type.IsClass && !type.IsAbstract)
                {
                    var test = (IExternalTest)Activator.CreateInstance(type);
                    try
                    {
                        logger.WriteLine("****** Executing MapGuide test: " + type.Name + " *********");
                        Console.WriteLine("Executing external MapGuide test: " + type.Name);
                        test.Execute(factory, logger);
                    }
                    catch (AssertException ex)
                    {
                        logger.WriteLine("Assertion failure: " + ex.Message);
                        failures++;
                    }
                    catch (Exception ex)
                    {
                        logger.WriteLine("General failure: " + ex.ToString());
                        failures++;
                    }
                    finally
                    {
                        testsRun++;
                    }
                }
            }
            return failures;
        }
    }
}
