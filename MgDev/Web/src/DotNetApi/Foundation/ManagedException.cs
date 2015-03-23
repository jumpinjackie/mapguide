//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

using System;

/// <summary>
/// ManagedException is the exception class from which the root of unmanaged exception derive
/// This class, deriving from .NET Exception, allows to use the most common properties of
/// the Exception class on exceptions wrapping unmanaged MgException classes. The implementation
/// of these properties relies on the equivalent MgException methods.
///
/// Although this class does not wrap any unmanaged class, it still holds a C++ pointer to
/// the MgException that derives from it.
/// </summary>
namespace OSGeo.MapGuide
{
    [Serializable]
    public class ManagedException : Exception
    {
        private bool mIsWrapper;
        private string mMessage;
        private string mStackTrace;

        public ManagedException()
        {
            mIsWrapper = true;
            mMessage = string.Empty;
            mStackTrace = string.Empty;
        }

        protected ManagedException(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) : base(info, context)
        {
            mIsWrapper = false;
            mMessage = info.GetString("ManagedException.Message");
            mStackTrace = info.GetString("ManagedException.StackTrace");
        }

        public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
        {
            base.GetObjectData(info, context);
            info.AddValue("ManagedException.Message", Message);
            info.AddValue("ManagedException.StackTrace", StackTrace);
        }

        public override string Message
        {
            get
            {
                return mIsWrapper ? ((MgException)this).GetExceptionMessage() : mMessage;
            }
        }

        public override string StackTrace
        {
            get
            {
                if (mIsWrapper)
                {
                    //Some cosmetic cleaning of the C++ stack trace to better line up with the .net one
                    string[] mgStackTrace = ((MgException)this).GetStackTrace().Split(new string[] { "\r\n", "\n" }, StringSplitOptions.RemoveEmptyEntries);
                    //This currently looks like the following if we re-join:
                    //
                    // at- <stack frame>
                    // at- <stack frame>
                    // at- <stack frame>
                    //
                    //The "-" being a leftover, so replace "at-" with "at" as well after re-joining.
                    //The reason we don't blindly replace "-" in the C++ stack is because we don't want to scramble any physical path
                    //that would also contain a "-"
                    string sanitizedStack = ("   at" + string.Join(Environment.NewLine + "   at", mgStackTrace)).Replace("at-", "at");
                    return string.Format("{0}{1}   ==== [C++ <-> .net] ===={1}{2}", sanitizedStack, Environment.NewLine, base.StackTrace);
                }
                else
                {
                    return mStackTrace;
                }
            }
        }
        
        public override string ToString()
        {
            string className = this.GetType().ToString();
            return string.Format("{0}: {1}{2}{3}", className, this.Message, Environment.NewLine, this.StackTrace);
        }

        public virtual void Dispose()
        {
            //implemented by derived classes
        }
    }
}
