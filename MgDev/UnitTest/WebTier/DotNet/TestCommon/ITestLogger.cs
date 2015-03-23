using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common
{
    /// <summary>
    /// A simple logger interface
    /// </summary>
    public interface ITestLogger : IDisposable
    {
        void Write(string format, params object[] args);
        void WriteLine(string format, params object[] args);
    }

    /// <summary>
    /// A logger for command line output
    /// </summary>
    public class TestLoggerConsole : ITestLogger
    {
        public void Write(string format, params object[] args)
        {
            Console.Write(format, args);
        }

        public void WriteLine(string format, params object[] args)
        {
            Console.WriteLine(format, args);
        }

        public void Dispose()
        {
            
        }
    }

    /// <summary>
    /// A logger for file output
    /// </summary>
    public class TestLoggerFile : ITestLogger
    {
        private StreamWriter sw;

        public TestLoggerFile(string file, bool append)
        {
            sw = new StreamWriter(file, append);
        }

        public void Write(string format, params object[] args)
        {
            sw.Write(format, args);
        }

        public void WriteLine(string format, params object[] args)
        {
            sw.WriteLine(format, args);
        }

        public void Dispose()
        {
            if (sw != null)
            {
                sw.Close();
                sw = null;
            }
        }
    }
}
