using OSGeo.MapGuide;
using OSGeo.MapGuide.Test.Common;
using OSGeo.MapGuide.Test.Web;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MgTestRunner
{
    //TODO: This test runner does not support generation/update mode yet. Please use the existing PHP test runner for doing this.

    class Program
    {
        static MgUserInformation userInfo;
        static MgSiteConnection siteConn;

        class PlatformFactory : IPlatformFactory
        {
            private MgSiteConnection _siteConn;

            public PlatformFactory(MgSiteConnection siteConn)
            {
                _siteConn = siteConn;
            }

            public MgService CreateService(int serviceType)
            {
                return _siteConn.CreateService(serviceType);
            }

            public MgMapBase CreateMap(MgResourceIdentifier mapDefinition)
            {
                var map = new MgMap(_siteConn);
                map.Create(mapDefinition, mapDefinition.Name);
                return map;
            }

            public MgMapBase CreateMap(string coordSys, MgEnvelope env, string name)
            {
                var map = new MgMap(_siteConn);
                map.Create(coordSys, env, name);
                return map;
            }

            public MgLayerBase CreateLayer(MgResourceIdentifier resId)
            {
                MgResourceService resSvc = (MgResourceService)_siteConn.CreateService(MgServiceType.ResourceService);
                return new MgLayer(resId, resSvc);
            }
        }

        //Usage: MgTestRunner.exe <webconfig.ini path> <MENTOR_DICTIONARY_PATH> [test log path]
        static void Main(string[] args)
        {
            if (args.Length >= 2 && args.Length <= 3)
            {
                string webconfig = args[0];
                string logFile = "UnitTests.log";
                if (args.Length == 3)
                    logFile = args[2];

                int failures = 0;
                using (var logger = new TestLoggerFile(logFile, false))
                {
                    logger.Write("Run started: {0}\n\n", DateTime.Now.ToString());

                    Environment.SetEnvironmentVariable("MENTOR_DICTIONARY_PATH", args[1], EnvironmentVariableTarget.Process);

                    MapGuideApi.MgInitializeWebTier(args[0]);
                    userInfo = new MgUserInformation("Administrator", "admin");
                    siteConn = new MgSiteConnection();
                    siteConn.Open(userInfo);

                    var factory = new PlatformFactory(siteConn);

                    int testsRun = 0;
                    bool isEnterprise = false;
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/ResourceService/ResourceServiceTest.dump", ref testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/DrawingService/DrawingServiceTest.dump", ref testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/FeatureService/FeatureServiceTest.dump", ref testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/SiteService/SiteServiceTest.dump", ref testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/MappingService/MappingServiceTest.dump", ref testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/ServerAdmin/ServerAdminTest.dump", ref testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/MapLayer/MapLayerTest.dump", ref testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/WebLayout/WebLayoutTest.dump", ref testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/Unicode/UnicodeTest.dump", ref testsRun, logger, isEnterprise);
                    //Run auxillary tests not part of the SQLite-defined suite
                    failures += CommonTests.Execute(factory, logger, ref testsRun);
                    failures += MapGuideTests.Execute(factory, logger, ref testsRun);
                    logger.Write("\n\nTests failed/run: {0}/{1}\n", failures, testsRun);
                    Console.Write("\n\nTests failed/run: {0}/{1}\n", failures, testsRun);
                    logger.Write("Run ended: {0}\n\n", DateTime.Now.ToString());
                }
                Environment.ExitCode = failures;
            }
            else
            {
                Console.WriteLine("Usage: MgTestRunner.exe <webconfig.ini path> <MENTOR_DICTIONARY_PATH> [test log path]");
                Environment.ExitCode = 1;
            }
        }

        private static int ExecuteTest(string apiType, string dumpFile, ref int testsRun, TestLoggerFile logger, bool isEnterprise)
        {
            ITestExecutorCollection exec = null;
            if (apiType == ApiTypes.Platform)
                exec = new MapGuideTestExecutorCollection(userInfo, siteConn);

            int ret = 0;
            if (exec != null)
            {
                //"validate" is currently the only test execution mode supported
                exec.Initialize("validate", dumpFile);
                ret += exec.Execute(ref testsRun, logger, isEnterprise);
                exec.Cleanup();
            }
            return ret;
        }
    }
}
