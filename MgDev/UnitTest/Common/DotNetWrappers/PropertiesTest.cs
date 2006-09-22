using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using OSGeo.MapGuide;

namespace OSGeo.MapGuide
{
    [TestFixture]
    public class PropertiesTest
    {
        [SetUp]
        public void InitMapGuide()
        {
            MapGuideApi.MgInitializeWebTier("d:/mgdev/web/src/webconfig.ini");
        }

        [Test]
        public void Color()
        {
        }
    }
}