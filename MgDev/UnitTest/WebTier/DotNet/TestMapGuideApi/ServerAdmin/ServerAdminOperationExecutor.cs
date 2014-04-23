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
    /// The base class of all test executors using the MgServerAdmin
    /// </summary>
    /// <typeparam name="T">The test executor subclass type</typeparam>
    public abstract class ServerAdminOperationExecutor<T> : PlatformApiTestExecutor
    {
        protected MgServerAdmin _serverAdmin;

        protected ServerAdminOperationExecutor(MgServerAdmin serverAdmin, string unitTestVm)
            : base(typeof(T).Name.ToUpper(), ApiTypes.Platform, unitTestVm)
        {
            _serverAdmin = serverAdmin;
        }
    }
}