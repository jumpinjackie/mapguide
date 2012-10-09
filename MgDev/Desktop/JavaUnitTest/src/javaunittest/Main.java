/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package javaunittest;

import java.util.ArrayList;
import org.osgeo.mapguide.*;

/**
 *
 * @author Jackie Ng
 */
public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        try {
            long start = System.currentTimeMillis();
            MgPlatform.Initialize("Platform.ini");
            long end = System.currentTimeMillis();
            System.out.printf("MapGuide Platform initialized in %dms\n", (end - start));
        }
        catch(Exception ex) {
            System.err.println(ex.toString());
            return;
        }

        testFeatureService();
        testResourceService();
        testRenderingService();
    }

    static int _fsPasses = 0;
    static ArrayList<Exception> _fsFailures = new ArrayList<Exception>();

    static int _rsPasses = 0;
    static ArrayList<Exception> _rsFailures = new ArrayList<Exception>();

    static int _renPasses = 0;
    static ArrayList<Exception> _renFailures = new ArrayList<Exception>();

    private static void testFeatureService()
    {
        System.out.println("==== Testing Feature Service ===");

        AbstractTestSuite test = new FeatureServiceTests();
        test.execute();

        _fsPasses = test.getPasses();
        _fsFailures = test.getFailures();

        System.out.printf("==== %d pass, %d failed ====\n", _fsPasses, _fsFailures.size());
    }

    private static void testResourceService()
    {
        System.out.println("==== Testing Resource Service ===");

        AbstractTestSuite test = new ResourceServiceTests();
        test.execute();

        _rsPasses = test.getPasses();
        _rsFailures = test.getFailures();

        System.out.printf("==== %d pass, %d failed ====\n", _rsPasses, _rsFailures.size());
    }

    private static void testRenderingService()
    {
        System.out.println("==== Testing Rendering Service ===");

        AbstractTestSuite test = new RenderingServiceTests();
        test.execute();

        _renPasses = test.getPasses();
        _renFailures = test.getFailures();

        System.out.printf("==== %d pass, %d failed ====\n", _renPasses, _renFailures.size());
    } 
}
