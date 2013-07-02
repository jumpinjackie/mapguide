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
    /// The base class of all test executors using the MgResourceService
    /// </summary>
    /// <typeparam name="T">The test executor subclass type</typeparam>
    public abstract class ResourceServiceOperationExecutor<T> : PlatformApiTestExecutor
    {
        protected MgResourceService _resourceService;

        protected ResourceServiceOperationExecutor(MgResourceService resSvc, string unitTestVm)
            : base(typeof(T).Name.ToUpper(), ApiTypes.Platform, unitTestVm)
        {
            _resourceService = resSvc;
        }
    }
}
