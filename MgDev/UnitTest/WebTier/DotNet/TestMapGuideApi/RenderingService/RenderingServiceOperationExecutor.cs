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
    /// The base class of all test executors using the MgRenderingService
    /// </summary>
    /// <typeparam name="T">The test executor subclass type</typeparam>
    public abstract class RenderingServiceOperationExecutor<T> : PlatformApiTestExecutor
    {
        protected MgRenderingService _renderingService;

        protected RenderingServiceOperationExecutor(MgRenderingService renderSvc, string unitTestVm)
            : base(typeof(T).Name.ToUpper(), ApiTypes.Platform, unitTestVm)
        {
            _renderingService = renderSvc;
        }
    }
}