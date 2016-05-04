using OSGeo.MapGuide.Test.Common;
using OSGeo.MapGuide.Test.Common.ExternalTests;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Web.ExternalTests
{
    public class ResourceMetadataTest : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/Metadata.FeatureSource");
            try
            {
                if (resSvc.ResourceExists(fsId))
                    resSvc.DeleteResource(fsId);

                string metadata = resSvc.EnumerateResourceDocuments(null, MgResourceType.FeatureSource, MgResourceHeaderProperties.Metadata);
                File.WriteAllText("metadata.txt", metadata);

                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SDF", featSvc);
                string headerXml = string.Format(Properties.Resources.ResourceHeaderTemplate, "Foo", "Bar");
                var bytes = Encoding.UTF8.GetBytes(headerXml);
                var source = new MgByteSource(bytes, bytes.Length);
                var rdr = source.GetReader();

                resSvc.SetResource(fsId, null, rdr);
                string metadata2 = resSvc.EnumerateResourceDocuments(null, MgResourceType.FeatureSource, MgResourceHeaderProperties.Metadata);
                File.WriteAllText("metadata2.txt", metadata2);

                Assert.IsFalse(metadata == metadata2);
            }
            finally
            {
                if (resSvc.ResourceExists(fsId))
                    resSvc.DeleteResource(fsId);
            }
        }
    }

    public class ResourceModifiedTest : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            MgResourceIdentifier fsId = new MgResourceIdentifier("Library://UnitTests/Data/Timestamp.FeatureSource");
            string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SDF", featSvc);
            MgByteReader content = resSvc.GetResourceContent(fsId);
            MgDateTime mod1 = resSvc.GetResourceModifiedDate(fsId);
            Thread.Sleep(1500);
            resSvc.SetResource(fsId, content, null);
            MgDateTime mod2 = resSvc.GetResourceModifiedDate(fsId);

            Assert.IsFalse(mod1.ToString() == mod2.ToString());
        }
    }

}
