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
    /// The base class of all test executors using the MgFeatureService
    /// </summary>
    /// <typeparam name="T">The test executor subclass type</typeparam>
    public abstract class FeatureServiceOperationExecutor<T> : PlatformApiTestExecutor
    {
        protected MgFeatureService _featureService;

        protected FeatureServiceOperationExecutor(MgFeatureService featSvc, string unitTestVm)
            : base(typeof(T).Name.ToUpper(), ApiTypes.Platform, unitTestVm)
        {
            _featureService = featSvc;
        }
    }
}
