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
                return mIsWrapper ? ((MgException)this).GetStackTrace() : mStackTrace;
            }
        }

        public virtual void Dispose()
        {
            //implemented by derived classes
        }
    }
}
