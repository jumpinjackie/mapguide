using System;
using System.Collections.Generic;
using System.Text;
using OSGeo.MapGuide;

namespace DotNetUnitTest
{
    public class ResourceServiceTests
    {
        MgResourceIdentifier sessionRepositoryIdentifier = new MgResourceIdentifier("Session:TestRepository//");
        MgResourceIdentifier sessionRepositoryNotExist = new MgResourceIdentifier("Session:DoesNotExist//");

        MgResourceIdentifier libraryRepositoryIdentifier = new MgResourceIdentifier("Library://");
        MgResourceIdentifier resourceIdentifier = new MgResourceIdentifier("Library://UnitTests/Data/test-1.FeatureSource"); // Original resource added
        MgResourceIdentifier resourceIdentifier2 = new MgResourceIdentifier("Library://UnitTests/Data/test-2.FeatureSource"); // Used in copy and move
        MgResourceIdentifier resourceIdentifier3 = new MgResourceIdentifier("Library://UnitTests/Data/World.DrawingSource"); // Added by package
        MgResourceIdentifier resourceNotExist = new MgResourceIdentifier("Library://UnitTests/Data/resourcedoesnotexist.FeatureSource");
        MgResourceIdentifier resourceIdentifier4 = new MgResourceIdentifier("Library://UnitTests/Layer/test-1.LayerDefinition"); // For cascade move resource

        string resourceDataName = "World_Countries.sdf";
        string resourceDataName2 = "New_World_Countries.sdf";

        string repositoryHeaderFileName = "..\\UnitTestFiles\\SampleRepositoryHeader.xm";
        string repositoryContentFileName = "..\\UnitTestFiles\\SampleRepositoryContent.xm";
        string resourceContentFileName = "..\\UnitTestFiles\\TEST.FeatureSource";
        string resourceContentFileName2 = "..\\UnitTestFiles\\TEST.LayerDefinition";
        string dataFileName = "..\\UnitTestFiles\\World_Countries.sdf";
        string packageName = "..\\UnitTestFiles\\Shuttle.zip";

        
        public void TestStart()
        {
        }

        
        public void TestEnd()
        {
            try
            {
                var fact = new MgdServiceFactory();
                MgdResourceService service = (MgdResourceService)fact.CreateService(MgServiceType.ResourceService);
                MgResourceIdentifier mapres1 = new MgResourceIdentifier("Library://UnitTests/Data/Shuttle.DrawingSource");
                service.DeleteResource(mapres1);

                if (service.ResourceExists(resourceIdentifier))
                    service.DeleteResource(resourceIdentifier);

                if (service.ResourceExists(resourceIdentifier2))
                    service.DeleteResource(resourceIdentifier2);

                if (service.ResourceExists(resourceIdentifier3))
                    service.DeleteResource(resourceIdentifier3);

                if (service.ResourceExists(resourceIdentifier4))
                    service.DeleteResource(resourceIdentifier4);
            }
            catch (MgException ex)
            {
                ex.Dispose();
            }
        }

        
        public void TestCase_ApplyResourcePackage()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                Assert.Throws<MgNullArgumentException>(() => service.ApplyResourcePackage(null));

                MgByteSource source = new MgByteSource(packageName);
                source.SetMimeType(MgMimeType.Binary);
                MgByteReader reader = source.GetReader();

                service.ApplyResourcePackage(reader);
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }

        // Test Resource Management APIs
        
        public void TestCase_ResourceExists()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                Assert.Throws<MgNullArgumentException>(() => service.ResourceExists(null));

                Assert.True(service.ResourceExists(libraryRepositoryIdentifier));
                Assert.False(service.ResourceExists(resourceNotExist));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }

        // public void TestCase_EnumerateResources() {} 
        
        public void TestCase_SetResource()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

                //Try to use NULL arguments
                Assert.Throws<MgNullArgumentException>(() => service.SetResource(null, null, null));
                Assert.Throws<MgNullArgumentException>(() => service.SetResource(resourceIdentifier, null, null));

                //Add a new resource
                MgByteSource source = new MgByteSource(resourceContentFileName);
                MgByteReader reader = source.GetReader();
                service.SetResource(resourceIdentifier, reader, null);

                //Try to add the same resource again (should be fine and just update it)
                source = new MgByteSource(resourceContentFileName);
                reader = source.GetReader();
                service.SetResource(resourceIdentifier, reader, null);

                //Add another resource layer definition, which references to the feature source. This is for cascade MoveResource test.
                source = new MgByteSource(resourceContentFileName2);
                reader = source.GetReader();
                service.SetResource(resourceIdentifier4, reader, null);
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_MoveResource()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

                //Try to use NULL arguments
                Assert.Throws<MgNullArgumentException>(() => service.MoveResource(null, null, true, false));
                Assert.Throws<MgNullArgumentException>(() => service.MoveResource(resourceIdentifier, null, true, true));
                Assert.Throws<MgNullArgumentException>(() => service.MoveResource(null, resourceIdentifier, true, true));

                //Try to move a resource that doesn't exist
                Assert.Throws<MgResourceNotFoundException>(() => service.MoveResource(resourceNotExist, resourceIdentifier2, false, true));

                //Move the resource that was added earlier with cascade = true, and check referencing resource.
                service.MoveResource(resourceIdentifier, resourceIdentifier2, false, true);
                Assert.True(!service.ResourceExists(resourceIdentifier));
                //Ptr<MgByteReader> byteReader = pService.GetResourceContent(&resourceIdentifier4, L"");
                //STRING referenceContent = byteReader.ToString();
                //STRING featureIdTag = L"<ResourceId>Library://UnitTests/Data/test-2.FeatureSource</ResourceId>";
                //CPPUNIT_ASSERT(referenceContent.find(featureIdTag) != STRING::npos);

                //Move the resource again with cascade = false, and check referencing resource.
                service.MoveResource(resourceIdentifier2, resourceIdentifier, false, false);
                //byteReader = pService.GetResourceContent(&resourceIdentifier4, L"");
                //referenceContent = byteReader.ToString();
                //featureIdTag = L"<ResourceId>Library://UnitTests/Data/test-2.FeatureSource</ResourceId>";
                //CPPUNIT_ASSERT(referenceContent.find(featureIdTag) != STRING::npos);
                Assert.True(!service.ResourceExists(resourceIdentifier2));

                //Try to move the resource into itself (ie. itself)
                Assert.Throws<MgInvalidArgumentException>(() => service.MoveResource(resourceIdentifier, resourceIdentifier, false));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_CopyResource()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

                //Try to use NULL arguments
                Assert.Throws<MgNullArgumentException>(() => service.CopyResource(null, null, true));
                Assert.Throws<MgNullArgumentException>(() => service.CopyResource(resourceIdentifier, null, true));
                Assert.Throws<MgNullArgumentException>(() => service.CopyResource(null, resourceIdentifier2, true));

                //Try to use source & destination as the same thing
                Assert.Throws<MgInvalidArgumentException>(() => service.CopyResource(resourceIdentifier2, resourceIdentifier2, false));

                //Copy the moved resource to another location
                service.CopyResource(resourceIdentifier, resourceIdentifier2, false);

                //Try to copy a resource to one that should now exist
                Assert.Throws<MgDuplicateResourceException>(() => service.CopyResource(resourceIdentifier2, resourceIdentifier, false));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_GetResourceContent()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

                //Try to get the content using NULL arguments
                Assert.Throws<MgNullArgumentException>(() => service.GetResourceContent(null));

                //Try to get the content of a resource that doesn't exist
                Assert.Throws<MgResourceNotFoundException>(() => service.GetResourceContent(resourceNotExist));

                //Get the content of the resource that was added in TestCase_SetResource
                var byteReader = service.GetResourceContent(resourceIdentifier);
                var mimeType = byteReader.GetMimeType();
                Assert.True(mimeType.Equals(MgMimeType.Xml));

                string xml = byteReader.ToString();
                Assert.True(xml.Length > 0);
                Assert.True(xml.Contains("<FeatureSource"));
                Assert.True(xml.EndsWith("</FeatureSource>"));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_GetResourceContents()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

                //Try to get the content using NULL arguments
                Assert.Throws<MgNullArgumentException>(() => service.GetResourceContents(null, null));

                //Try to get the content of a resource that doesn't exist
                MgStringCollection resourceNotExistCol = new MgStringCollection();
                resourceNotExistCol.Add(resourceNotExist.ToString());
                resourceNotExistCol.Add(resourceIdentifier.ToString());
                Assert.Throws<MgResourceNotFoundException>(() => service.GetResourceContents(resourceNotExistCol, null));

                //Get the content of the resource that was added in TestCase_SetResource
                MgStringCollection resIds = new MgStringCollection();
                resIds.Add(resourceIdentifier.ToString());
                resIds.Add(resourceIdentifier4.ToString());
                var ret = service.GetResourceContents(resIds, null);
                Assert.AreEqual(2, ret.GetCount());

                for (int i = 0; i < ret.GetCount(); i++)
                {
                    Assert.False(string.IsNullOrEmpty(ret.GetItem(i)));
                }
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_GetResourceHeader()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                Assert.Throws<MgNotImplementedException>(() => service.GetResourceHeader(null));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_EnumerateReferences()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                Assert.Throws<MgNotImplementedException>(() => service.EnumerateReferences(null));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_ChangeResourceOwner()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                Assert.Throws<MgNotImplementedException>(() => service.ChangeResourceOwner(null, "", false));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_InheritPermissionsFrom()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                Assert.Throws<MgNotImplementedException>(() => service.InheritPermissionsFrom(null));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }

        // Test Resource Data Management APIs
        
        public void TestCase_EnumerateResourceData()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

                Assert.Throws<MgNullArgumentException>(() => service.EnumerateResourceData(null));
                Assert.Throws<MgResourceNotFoundException>(() => service.EnumerateResourceData(resourceNotExist));

                var byteReader = service.EnumerateResourceData(resourceIdentifier);
                Assert.AreEqual(MgMimeType.Xml, byteReader.GetMimeType());
                string xml = byteReader.ToString();

                Assert.True(xml.Length > 0);
                Assert.True(xml.StartsWith("<ResourceDataList"));
                Assert.True(xml.EndsWith("</ResourceDataList>"));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_SetResourceData()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

                Assert.Throws<MgNullArgumentException>(() => service.SetResourceData(null, "", "", null));

                var dataSource = new MgByteSource(dataFileName);
                var dataReader = dataSource.GetReader();

                Assert.Throws<MgNullArgumentException>(() => service.SetResourceData(resourceIdentifier, "", "File", dataReader));

                dataSource = new MgByteSource(dataFileName);
                dataReader = dataSource.GetReader();
                service.SetResourceData(resourceIdentifier, resourceDataName, "File", dataReader);

                var reader = service.EnumerateResourceData(resourceIdentifier);
                string xml = reader.ToString();

                Assert.True(xml.Contains("<Name>" + resourceDataName + "</Name>"));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_RenameResourceData()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

                Assert.Throws<MgNullArgumentException>(() => service.RenameResourceData(null, resourceDataName, resourceDataName2, true));
                Assert.Throws<MgResourceNotFoundException>(() => service.RenameResourceData(resourceNotExist, resourceDataName, resourceDataName2, true));
                Assert.Throws<MgResourceDataNotFoundException>(() => service.RenameResourceData(resourceIdentifier, "DoesNotExist", resourceDataName2, true));
                Assert.Throws<MgNullArgumentException>(() => service.RenameResourceData(resourceIdentifier, resourceDataName, "", true));

                service.RenameResourceData(resourceIdentifier, resourceDataName, resourceDataName2, false);
                var reader = service.EnumerateResourceData(resourceIdentifier);
                string xml = reader.ToString();
                Assert.True(xml.Contains("<Name>" + resourceDataName2 + "</Name>"));
                Assert.False(xml.Contains("<Name>" + resourceDataName + "</Name>"));

                service.RenameResourceData(resourceIdentifier, resourceDataName2, resourceDataName, false);
                reader = service.EnumerateResourceData(resourceIdentifier);
                xml = reader.ToString();
                Assert.True(xml.Contains("<Name>" + resourceDataName + "</Name>"));
                Assert.False(xml.Contains("<Name>" + resourceDataName2 + "</Name>"));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_GetResourceData()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

                Assert.Throws<MgNullArgumentException>(() => service.GetResourceData(null, resourceDataName));
                Assert.Throws<MgNullArgumentException>(() => service.GetResourceData(resourceIdentifier, ""));
                Assert.Throws<MgResourceNotFoundException>(() => service.GetResourceData(resourceNotExist, resourceDataName));
                Assert.Throws<MgResourceDataNotFoundException>(() => service.GetResourceData(resourceIdentifier, "DoesNotExist"));

                var reader = service.GetResourceData(resourceIdentifier, resourceDataName);
                Assert.True(reader.GetLength() > 0);
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
        
        public void TestCase_DeleteResourceData()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

                Assert.Throws<MgNullArgumentException>(() => service.DeleteResourceData(null, resourceDataName));
                Assert.Throws<MgNullArgumentException>(() => service.DeleteResourceData(resourceIdentifier, ""));
                Assert.Throws<MgResourceDataNotFoundException>(() => service.DeleteResourceData(resourceIdentifier, "DoesNotExist"));

                service.DeleteResourceData(resourceIdentifier, resourceDataName);
                //var reader = service.EnumerateResourceData(resourceIdentifier);
                //string xml = reader.ToString();
                //Assert.False(xml.Contains("<Name>" + resourceDataName + "</Name>"));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }

        public void TestCase_DeleteResource()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                var featSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);

                Assert.Throws<MgNullArgumentException>(() => service.DeleteResource(null));

                service.DeleteResource(resourceNotExist);
                
                service.DeleteResource(resourceIdentifier);
                service.DeleteResource(resourceIdentifier2);
                service.DeleteResource(resourceIdentifier3);

                var resource = new MgResourceIdentifier("Library://UnitTests/Data/TEST.FeatureSource");

                var source = new MgByteSource("..\\UnitTestFiles\\TEST.FeatureSource");
                var reader = source.GetReader();
                service.SetResource(resource, reader, null);

                var dataSource = new MgByteSource("..\\UnitTestFiles\\TEST.sdf");
                var dataReader = dataSource.GetReader();
                service.SetResourceData(resource, "TEST.sdf", "File", dataReader);

                MgFeatureAggregateOptions agg = new MgFeatureAggregateOptions();
                agg.AddFeatureProperty("Data");

                var mgdr = featSvc.SelectAggregate(resource, "TEST", agg);
                Assert.True(mgdr.ReadNext());
                mgdr.Close();

                service.DeleteResource(resource);
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }

        public void TestCase_EnumerateUnmanagedData()
        {
            try
            {
                var fact = new MgdServiceFactory();
                var service = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
                Assert.Throws<MgNotImplementedException>(() => service.EnumerateUnmanagedData("", true, "", ""));
            }
            catch (MgException ex)
            {
                Assert.Fail(ex.Message);
                ex.Dispose();
            }
        }
    }
}
