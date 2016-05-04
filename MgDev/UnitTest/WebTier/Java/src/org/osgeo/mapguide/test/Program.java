package org.osgeo.mapguide.test;

import java.io.*;
import java.util.*;
import java.util.logging.Logger;
import java.util.logging.LogManager;
import java.util.logging.Level;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class Program
{
    static MgUserInformation userInfo;
    static MgSiteConnection siteConn;

    public static void main(String[] args) {
        try {
            System.out.println("Working Directory is: " + System.getProperty("user.dir"));
            LogManager.getLogManager().reset(); //Needed for Linux
            Logger.getLogger("com.almworks.sqlite4java").setLevel(Level.OFF);
            if (args.length >= 1 && args.length <= 2) {
                String webConfigPath = args[0];
                String logFile = "UnitTests.log";
                if (args.length == 2)
                    logFile = args[1];
                
                int failures = 0;
                try (ITestLogger logger = new TestLoggerFile(logFile)) {
                    logger.WriteLine(String.format("Run started: %s", new Date().toString()));
                    MapGuideJavaApiEx.MgInitializeWebTier(webConfigPath);

                    userInfo = new MgUserInformation("Administrator", "admin");
                    siteConn = new MgSiteConnection();
                    siteConn.open(userInfo);

                    //Load the Sheboygan sample data set required by some tests
                    Console.WriteLine("Loading Sheboygan sample data package");
                    MgByteSource bs = new MgByteSource("../../TestData/Samples/Sheboygan/Sheboygan.mgp");
                    MgByteReader rdr = bs.getReader();
                    MgResourceService resSvc = (MgResourceService)siteConn.createService(MgServiceType.ResourceService);
                    resSvc.applyResourcePackage(rdr);

                    Console.WriteLine("MapGuide Initialized. Running tests");

                    IPlatformFactory factory = new PlatformFactory(siteConn);

                    MutableInteger testsRun = new MutableInteger(0);
                    boolean isEnterprise = false;
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/ResourceService/ResourceServiceTest.dump", testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/DrawingService/DrawingServiceTest.dump", testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/FeatureService/FeatureServiceTest.dump", testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/SiteService/SiteServiceTest.dump", testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/MappingService/MappingServiceTest.dump", testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/ServerAdmin/ServerAdminTest.dump", testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/MapLayer/MapLayerTest.dump", testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/WebLayout/WebLayoutTest.dump", testsRun, logger, isEnterprise);
                    failures += ExecuteTest(ApiTypes.Platform, "../../TestData/Unicode/UnicodeTest.dump", testsRun, logger, isEnterprise);
                    //Run auxillary tests not part of the SQLite-defined suite
                    failures += CommonTests.Execute(factory, logger, testsRun);
                    failures += MapGuideTests.Execute(factory, logger, testsRun);
                    logger.WriteLine("\n\nTests failed/run: %d/%d", failures, testsRun.getValue());
                    Console.Write("\n\nTests failed/run: %d/%d\n", failures, testsRun.getValue());
                    logger.WriteLine(String.format("Run ended: %s", new Date().toString()));
                }
                System.exit(failures);
            } else {
                System.out.println("Usage: java -jar MgUnitTest.jar <webconfig.ini path> [test log path]");
                System.exit(1);
            }
        } catch (Exception ex) {
            System.err.println(CommonUtility.ExceptionToString(ex));
            System.exit(1);
        }
    }

    private static int ExecuteTest(String apiType, String dumpFile, MutableInteger testsRun, ITestLogger logger, boolean isEnterprise) {
        ITestExecutorCollection exec = null;
        if (apiType.equals(ApiTypes.Platform))
            exec = new MapGuideTestExecutorCollection(userInfo, siteConn);

        int ret = 0;
        if (exec != null)
        {
            //"validate" is currently the only test execution mode supported
            exec.Initialize("validate", dumpFile);
            ret += exec.Execute(testsRun, logger, isEnterprise);
            exec.Cleanup();
        }
        return ret;
    }
}