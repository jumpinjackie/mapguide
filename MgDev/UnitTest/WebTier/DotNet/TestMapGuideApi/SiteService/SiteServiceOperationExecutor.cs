using OSGeo.MapGuide.Test.Common;
using SqliteDotNet;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Operations
{
    /// <summary>
    /// The base class of all test executors using the MgSite
    /// </summary>
    /// <typeparam name="T">The test executor subclass type</typeparam>
    public abstract class SiteServiceOperationExecutor<T> : PlatformApiTestExecutor
    {
        protected MgSite _site;

        protected SiteServiceOperationExecutor(MgSite site, string unitTestVm)
            : base(typeof(T).Name.ToUpper(), ApiTypes.Platform, unitTestVm)
        {
            _site = site;
        }
    }
}