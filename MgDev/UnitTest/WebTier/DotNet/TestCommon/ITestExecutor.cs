using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common
{
    /// <summary>
    /// Executes a particular MapGuide API
    /// </summary>
    public interface ITestExecutor : IDisposable
    {
        /// <summary>
        /// The API flavour we're executing. See <see cref="OSGeo.MapGuide.Test.Common.ApiTypes"/>
        /// </summary>
        string Api { get; }

        /// <summary>
        /// The name of the operation
        /// </summary>
        string OperationName { get; }

        /// <summary>
        /// Executes the API with the particular parameter set
        /// </summary>
        /// <param name="paramSetId"></param>
        /// <returns></returns>
        TestResult Execute(int paramSetId);
    }
}
