using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common
{
    public abstract class TestExecutorBase : ITestExecutor
    {
        public abstract string Api
        {
            get;
        }

        public abstract string OperationName
        {
            get;
        }

        public abstract TestResult Execute(int paramSetId);

        public abstract void Dispose();
    }
}
