using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common
{
    //Defines tests outside of the SQLite-based test suite

    public interface IExternalTest
    {
        void Execute(IPlatformFactory factory, ITestLogger logger);
    }

    public interface IPlatformFactory
    {
        MgService CreateService(int serviceType);
        MgMapBase CreateMap(MgResourceIdentifier mapDefinition);
        MgMapBase CreateMap(string coordSys, MgEnvelope env, string name);
        MgLayerBase CreateLayer(MgResourceIdentifier resId);
    }

    public class CommonTests
    {
        public static int Execute(IPlatformFactory factory, ITestLogger logger, ref int testsRun)
        {
            int failures = 0;
            var types = typeof(IPlatformFactory).Assembly.GetTypes();
            foreach (var type in types)
            {
                if (typeof(IExternalTest).IsAssignableFrom(type) && type.IsClass)
                {
                    var test = (IExternalTest)Activator.CreateInstance(type);
                    try
                    {
                        logger.WriteLine("****** Executing platform test: " + type.Name + " *********");
                        Console.WriteLine("Executing external platform test: " + type.Name);
                        test.Execute(factory, logger);
                    }
                    catch (AssertException ex)
                    {
                        logger.WriteLine("Assertion failure: " + ex.Message);
                        Console.WriteLine("Assertion failure: " + ex.Message);
                        failures++;
                    }
                    catch (Exception ex)
                    {
                        logger.WriteLine("General failure: " + ex.ToString());
                        Console.WriteLine("General failure: " + ex.ToString());
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
