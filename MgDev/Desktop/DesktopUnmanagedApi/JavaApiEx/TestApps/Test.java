import org.osgeo.mapguide.*;

/**
 *
 * Compile with:
 *
 * $JAVAC -classpath .;$JARPATH Test.java
 *
 * Run with:
 *
 * $JAVA -classpath .;$JARPATH -Djava.library.path=$MGDESKTOPDIR Test
 *
 * Where:
 *
 *  $JAVAC - Path to javac.exe (JDK must be same bitness as mg-desktop dlls)
 *  $JAVA - Path to java.exe (JDK must be same bitness as mg-desktop dlls)
 *  $MGDESKTOPDIR - Path to directory containing mg-desktop dlls
 *  $JARPATH - Path to MapGuideDesktopApi.jar, presumably $MGDESKTOPDIR
 *
 */

public class Test
{
    public static void main(String[] args) {
        MgdPlatform.initialize("Platform.ini");
        MgdServiceFactory factory = new MgdServiceFactory();
        
        MgFeatureService featSvc = (MgFeatureService)factory.createService(MgServiceType.FeatureService);
        MgResourceService resSvc = (MgResourceService)factory.createService(MgServiceType.ResourceService);
        
        System.out.print("Can we make a MgCoordinateSystem? ");
        
        MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
        MgCoordinateSystem cs = csFactory.createFromCode("XY-M");
        String srsWkt = csFactory.convertCoordinateSystemCodeToWkt("XY-M");
        System.out.println("YES");
        
        String scName = "Default";
        MgResourceIdentifier fsId = new MgResourceIdentifier("Library://UnitTest/JavaApiTest.FeatureSource");
        System.out.print("Can we make a Feature Source (" + fsId.toString() + ")? ");
        MgFeatureSchema schema = new MgFeatureSchema("Default", "Test Schema");
        MgClassDefinition klass = new MgClassDefinition();
        klass.setName("TestClass");
        
        MgPropertyDefinitionCollection clsProps = klass.getProperties();
        MgPropertyDefinitionCollection idProps = klass.getIdentityProperties();
        
        MgDataPropertyDefinition idProp = new MgDataPropertyDefinition("ID");
        idProp.setDataType(MgPropertyType.Int32);
        idProp.setAutoGeneration(true);
        idProp.setNullable(false);
        
        clsProps.addItem(idProp);
        idProps.addItem(idProp);
        
        MgDataPropertyDefinition nameProp = new MgDataPropertyDefinition("Name");
        nameProp.setDataType(MgPropertyType.String);
        nameProp.setLength(255);
        nameProp.setNullable(true);
        
        clsProps.addItem(nameProp);
        
        MgGeometricPropertyDefinition geomProp = new MgGeometricPropertyDefinition("Geom");
        geomProp.setGeometryTypes(MgFeatureGeometricType.Point | MgFeatureGeometricType.Curve | MgFeatureGeometricType.Surface);
        geomProp.setSpatialContextAssociation(scName);
        
        MgClassDefinitionCollection classes = schema.getClasses();
        classes.addItem(klass);
        
        MgFileFeatureSourceParams fsParams = new MgFileFeatureSourceParams("OSGeo.SQLite", scName, srsWkt, schema);
        featSvc.createFeatureSource(fsId, fsParams);
        System.out.println("YES");
        
        System.out.print("Can we interrogate our created feature source? ");
        MgFeatureSchemaCollection schemas = featSvc.describeSchema(fsId, "Default");
        MgStringCollection classNames = new MgStringCollection();
        classNames.addItem("TestClass");
        schemas = featSvc.describeSchema(fsId, "Default", classNames);
        try {
            classNames.clear();
            classNames.addItem("IDontExist");
            schemas = featSvc.describeSchema(fsId, "Default", classNames);
        } catch (MgException ex) {
        }
        System.out.println("YES");
        
        System.out.println("Can we get the feature source XML?");
        MgByteReader content = resSvc.getResourceContent(fsId);
        System.out.println(content.toString());
        System.out.println("YES");
        
        System.out.print("Can we nuke this feature source from orbit? ");
        resSvc.deleteResource(fsId);
        System.out.println("YES");
        
        MgdPlatform.terminate();
    }
}