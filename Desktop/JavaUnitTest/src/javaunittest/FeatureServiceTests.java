/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package javaunittest;

import org.osgeo.mapguide.*;

/**
 *
 * @author Jackie Ng
 */
public class FeatureServiceTests extends AbstractTestSuite {

    public FeatureServiceTests() {
        super();
    }

    @Override
    protected void setup() {
        System.out.println("=== Feature Service Tests - SETUP ===");
        try {
            MgServiceFactory fact = new MgServiceFactory();
            MgdResourceService pService = (MgdResourceService)fact.CreateService(MgServiceType.ResourceService);

            // publish the map definition
            MgResourceIdentifier mapres1 = new MgResourceIdentifier("Library://UnitTests/Maps/SheboyganWriteable.MapDefinition");
            MgByteSource mdfsrc1 = new MgByteSource("../UnitTestFiles/UT_Sheboygan_Writeable.mdf");
            MgByteReader mdfrdr1 = mdfsrc1.GetReader();
            pService.SetResource(mapres1, mdfrdr1, null);

            MgResourceIdentifier ldfres3 = new MgResourceIdentifier("Library://UnitTests/Layers/ParcelsWriteable.LayerDefinition");
            MgByteSource ldfsrc3 = new MgByteSource("../UnitTestFiles/UT_Parcels_Writeable.ldf");
            MgByteReader ldfrdr3 = ldfsrc3.GetReader();
            pService.SetResource(ldfres3, ldfrdr3, null);

            MgResourceIdentifier fsres3 = new MgResourceIdentifier("Library://UnitTests/Data/ParcelsWriteable.FeatureSource");
            MgByteSource fssrc3 = new MgByteSource("../UnitTestFiles/UT_Parcels_writeable.fs");
            MgByteReader fsrdr3 = fssrc3.GetReader();
            pService.SetResource(fsres3, fsrdr3, null);

            MgByteSource fsSource3 = new MgByteSource("../UnitTestFiles/UT_Parcels.sdf");
            MgByteReader fsReader3 = fsSource3.GetReader();
            pService.SetResourceData(fsres3, "UT_Parcels.sdf", "File", fsReader3);

            MgResourceIdentifier resourceIdentifier1 = new MgResourceIdentifier("Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
            MgResourceIdentifier resourceIdentifier2 = new MgResourceIdentifier("Library://UnitTests/Data/Redding_Parcels.FeatureSource");
            MgResourceIdentifier resourceIdentifier3 = new MgResourceIdentifier("Library://UnitTests/Data/Sheboygan_BuildingOutlines.FeatureSource");
            MgResourceIdentifier resourceIdentifier4 = new MgResourceIdentifier("Library://UnitTests/Data/Sheboygan_VotingDistricts.FeatureSource");
            MgResourceIdentifier resourceIdentifier5 = new MgResourceIdentifier("Library://UnitTests/Data/TestChainedInner1ToManyJoin.FeatureSource");
            MgResourceIdentifier resourceIdentifier6 = new MgResourceIdentifier("Library://UnitTests/Data/Empty.FeatureSource");
            MgResourceIdentifier resourceIdentifier7 = new MgResourceIdentifier("Library://UnitTests/Data/Sheboygan_Parcels_Writable.FeatureSource");

            String resourceContentFileName1 = "..\\UnitTestFiles\\Sheboygan_Parcels.FeatureSource";
            String resourceContentFileName2 = "..\\UnitTestFiles\\Redding_Parcels.FeatureSource";
            String resourceContentFileName3 = "..\\UnitTestFiles\\Sheboygan_BuildingOutlines.FeatureSource";
            String resourceContentFileName4 = "..\\UnitTestFiles\\Sheboygan_VotingDistricts.FeatureSource";
            String resourceContentFileName5 = "..\\UnitTestFiles\\TESTChainedInner1ToManyJoin.FeatureSource";
            String resourceContentFileName6 = "..\\UnitTestFiles\\Empty.FeatureSource";
            String resourceContentFileName7 = "..\\UnitTestFiles\\Sheboygan_Parcels_Writable.FeatureSource";
            String dataFileName1 = "..\\UnitTestFiles\\Sheboygan_Parcels.sdf";
            String dataFileName2 = "..\\UnitTestFiles\\Redding_Parcels.shp";
            String dataFileName3 = "..\\UnitTestFiles\\Redding_Parcels.dbf";
            String dataFileName4 = "..\\UnitTestFiles\\Redding_Parcels.shx";
            String dataFileName5 = "..\\UnitTestFiles\\Sheboygan_BuildingOutlines.sdf";
            String dataFileName6 = "..\\UnitTestFiles\\Sheboygan_VotingDistricts.sdf";
            String dataFileName7 = "..\\UnitTestFiles\\Empty.sdf";

            MgByteSource contentSource1 = new MgByteSource(resourceContentFileName1);
            MgByteReader contentReader1 = contentSource1.GetReader();
            pService.SetResource(resourceIdentifier1, contentReader1, null);

            MgByteSource contentSource2 = new MgByteSource(resourceContentFileName2);
            MgByteReader contentReader2 = contentSource2.GetReader();
            pService.SetResource(resourceIdentifier2, contentReader2, null);

            MgByteSource contentSource3 = new MgByteSource(resourceContentFileName3);
            MgByteReader contentReader3 = contentSource3.GetReader();
            pService.SetResource(resourceIdentifier3, contentReader3, null);

            MgByteSource contentSource4 = new MgByteSource(resourceContentFileName4);
            MgByteReader contentReader4 = contentSource4.GetReader();
            pService.SetResource(resourceIdentifier4, contentReader4, null);

            MgByteSource contentSource5 = new MgByteSource(resourceContentFileName5);
            MgByteReader contentReader5 = contentSource5.GetReader();
            pService.SetResource(resourceIdentifier5, contentReader5, null);

            MgByteSource contentSource6 = new MgByteSource(resourceContentFileName6);
            MgByteReader contentReader6 = contentSource6.GetReader();
            pService.SetResource(resourceIdentifier6, contentReader6, null);

            MgByteSource contentSource7 = new MgByteSource(resourceContentFileName7);
            MgByteReader contentReader7 = contentSource7.GetReader();
            pService.SetResource(resourceIdentifier7, contentReader7, null);

            //Set the resource data
            MgByteSource dataSource1 = new MgByteSource(dataFileName1);
            MgByteReader dataReader1 = dataSource1.GetReader();
            pService.SetResourceData(resourceIdentifier1, "Sheboygan_Parcels.sdf", "File", dataReader1);

            MgByteSource dataSource2 = new MgByteSource(dataFileName2);
            MgByteReader dataReader2 = dataSource2.GetReader();
            pService.SetResourceData(resourceIdentifier2, "Redding_Parcels.shp", "File", dataReader2);

            MgByteSource dataSource3 = new MgByteSource(dataFileName3);
            MgByteReader dataReader3 = dataSource3.GetReader();
            pService.SetResourceData(resourceIdentifier2, "Redding_Parcels.dbf", "File", dataReader3);

            MgByteSource dataSource4 = new MgByteSource(dataFileName4);
            MgByteReader dataReader4 = dataSource4.GetReader();
            pService.SetResourceData(resourceIdentifier2, "Redding_Parcels.shx", "File", dataReader4);

            MgByteSource dataSource5 = new MgByteSource(dataFileName5);
            MgByteReader dataReader5 = dataSource5.GetReader();
            pService.SetResourceData(resourceIdentifier3, "Sheboygan_BuildingOutlines.sdf", "File", dataReader5);

            MgByteSource dataSource6 = new MgByteSource(dataFileName6);
            MgByteReader dataReader6 = dataSource6.GetReader();
            pService.SetResourceData(resourceIdentifier4, "Sheboygan_VotingDistricts.sdf", "File", dataReader6);

            MgByteSource dataSource7 = new MgByteSource(dataFileName1);
            MgByteReader dataReader7 = dataSource7.GetReader();
            pService.SetResourceData(resourceIdentifier5, "Sheboygan_Parcels.sdf", "File", dataReader7);

            MgByteSource dataSource8 = new MgByteSource(dataFileName7);
            MgByteReader dataReader8 = dataSource8.GetReader();
            pService.SetResourceData(resourceIdentifier6, "Empty.sdf", "File", dataReader8);

            MgByteSource dataSource9 = new MgByteSource(dataFileName1);
            MgByteReader dataReader9 = dataSource9.GetReader();
            pService.SetResourceData(resourceIdentifier7, "Sheboygan_Parcels.sdf", "File", dataReader9);
        } catch (Exception ex) {
            System.err.println("ERROR: " + ex.toString());
        }
    }

    @Override
    protected void teardown() {
        System.out.println("=== Feature Service Tests - TEARDOWN ===");
        try {

        } catch (Exception ex) {

        }
    }

    @Override
    protected void runTests() {
        testCase_GetFeatureProviders();
        testCase_TestConnectionSdfProvider();
        testCase_GetConnectionPropertyValues();
        testCase_GetCapabilities();
        testCase_TestConnectionResourceIdentifier();
        testCase_GetSchemas();
        testCase_GetClasses();
        testCase_GetClassDefinition();
        testCase_GetClassDefinitionImmutability();
        testCase_DescribeSchema();
        testCase_DescribeSchemaImmutability();
        testCase_ApplySchema();
        testCase_SelectFeatures();
        testCase_SelectScrollable();
        testCase_SelectAggregate();
        testCase_InsertFeatures();
        testCase_UpdateFeatures();
        testCase_DeleteFeatures();
        testCase_ExecuteSqlQuery();
        testCase_ExecuteSqlNonQuery();
        testCase_GetSpatialContexts();
        testCase_GetLongTransactions();
        testCase_SetLongTransaction();
        testCase_JoinFeatures();
        testCase_JoinFeaturesChainedInner1ToMany();
        testCase_CreateFeatureSource();

        testCase_LayerInsertFeatures();
        testCase_LayerUpdateFeatures();
        testCase_LayerDeleteFeatures();
        testCase_LayerSelectScrollable();
    }

    private void testCase_GetFeatureProviders() {
        System.out.print("Test - GetFeatureProviders");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_TestConnectionSdfProvider() {
        System.out.print("Test - TestConnectionSdfProvider");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_GetConnectionPropertyValues() {
        System.out.print("Test - GetConnectionPropertyValues");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_GetCapabilities() {
        System.out.print("Test - GetCapabilities");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_TestConnectionResourceIdentifier() {
        System.out.print("Test - TestConnectionResourceIdentifier");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_GetSchemas() {
        System.out.print("Test - GetSchemas");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_GetClasses() {
        System.out.print("Test - GetClasses");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_GetClassDefinition() {
        System.out.print("Test - GetClassDefinition");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_GetClassDefinitionImmutability() {
        System.out.print("Test - GetClassDefinitionImmutability");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_DescribeSchema() {
        System.out.print("Test - DescribeSchema");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_DescribeSchemaImmutability() {
        System.out.print("Test - DescribeSchemaImmutability");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_ApplySchema() {
        System.out.print("Test - ApplySchema");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_SelectFeatures() {
        System.out.print("Test - SelectFeatures");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_SelectScrollable() {
        System.out.print("Test - SelectScrollable");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_SelectAggregate() {
        System.out.print("Test - SelectAggregate");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_InsertFeatures() {
        System.out.print("Test - InsertFeatures");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_UpdateFeatures() {
        System.out.print("Test - UpdateFeatures");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_DeleteFeatures() {
        System.out.print("Test - DeleteFeatures");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_ExecuteSqlQuery() {
        System.out.print("Test - ExecuteSqlQuery");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_ExecuteSqlNonQuery() {
        System.out.print("Test - ExecuteSqlNonQuery");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_GetSpatialContexts() {
        System.out.print("Test - GetSpatialContexts");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_GetLongTransactions() {
        System.out.print("Test - GetLongTransactions");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_SetLongTransaction() {
        System.out.print("Test - SetLongTransaction");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_JoinFeatures() {
        System.out.print("Test - JoinFeatures");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_JoinFeaturesChainedInner1ToMany() {
        System.out.print("Test - JoinFeaturesChainedInner1ToMany");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_CreateFeatureSource() {
        System.out.print("Test - CreateFeatureSource");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_LayerInsertFeatures() {
        System.out.print("Test - LayerInsertFeatures");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_LayerUpdateFeatures() {
        System.out.print("Test - LayerUpdateFeatures");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_LayerDeleteFeatures() {
        System.out.print("Test - LayerDeleteFeatures");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }

    private void testCase_LayerSelectScrollable() {
        System.out.print("Test - LayerSelectScrollable");
        try {
            _passes++;
            System.out.println(": OK");
        } catch (Exception ex) {
            _failures.add(ex);
            System.out.println(": F");
        } finally {

        }
    }
}
