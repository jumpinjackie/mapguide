using System;
using System.Collections.Generic;
using System.Text;

namespace DotNetUnitTest
{
    public delegate void Action();

    [global::System.Serializable]
    public class AssertException : Exception
    {
        //
        // For guidelines regarding the creation of new exception types, see
        //    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpgenref/html/cpconerrorraisinghandlingguidelines.asp
        // and
        //    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dncscol/html/csharp07192001.asp
        //

        public AssertException() { }
        public AssertException(string message) : base(message) { }
        public AssertException(string message, Exception inner) : base(message, inner) { }
        protected AssertException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context)
            : base(info, context) { }
    }

    public static class Assert
    {
        public static void True(bool condition)
        {
            if (!condition)
                throw new AssertException("Expected: true");
        }

        public static void False(bool condition)
        {
            if (condition)
                throw new AssertException("Expected: false");
        }

        public static void AreEqual(IComparable expected, IComparable actual)
        {
            if (expected.CompareTo(actual) != 0)
                throw new AssertException("Expected: " + expected + " got: " + actual);
        }

        public static void Fail(string message)
        {
            throw new AssertException("Assertion failed: " + message);
        }

        public static void Throws<T>(Action method) where T : Exception
        {
            try
            {
                method();
                Fail("Expected thrown exception");
            }
            catch (T ex)
            {

            }
        }
    }
}
