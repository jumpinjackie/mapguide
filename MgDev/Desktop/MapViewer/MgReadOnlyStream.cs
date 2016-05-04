#region Disclaimer / License
// Copyright (C) 2010, Jackie Ng
// http://trac.osgeo.org/mapguide/wiki/maestro, jumpinjackie@gmail.com
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
// 
#endregion
using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using OSGeo.MapGuide;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A read-only, rewindable stream
    /// </summary>
    public abstract class ReadOnlyRewindableStream : Stream
    {
        /// <summary>
        /// Resets the internal position of the stream
        /// </summary>
        public abstract void Rewind();

        /// <summary>
        /// Gets whether this stream supports reading
        /// </summary>
        public override bool CanRead
        {
            get { return true; }
        }

        /// <summary>
        /// Gets whether this stream supports seeking
        /// </summary>
        public override bool CanSeek
        {
            get { return false; }
        }

        /// <summary>
        /// Gets whether this stream supports writing
        /// </summary>
        public override bool CanWrite
        {
            get { return false; }
        }

        /// <summary>
        /// Gets whether this stream is rewindable. If not, calls to <see cref="Rewind"/> throw a 
        /// <see cref="T:System.InvalidOperationException"/>
        /// </summary>
        public abstract bool CanRewind { get; }

        /// <summary>
        /// Gets or sets the position (not implemented)
        /// </summary>
        public override long Position
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        /// <summary>
        /// Seeks to the specified offset in the stream (not implemented)
        /// </summary>
        /// <param name="offset"></param>
        /// <param name="origin"></param>
        /// <returns></returns>
        public override long Seek(long offset, SeekOrigin origin)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the length of the stream (not implemented)
        /// </summary>
        /// <param name="value"></param>
        public override void SetLength(long value)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Writes the specified buffer into the stream (not implemented)
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="offset"></param>
        /// <param name="count"></param>
        public override void Write(byte[] buffer, int offset, int count)
        {
            throw new NotImplementedException();
        }
    }

    /// <summary>
    /// Represents a method that returns a <see cref="OSGeo.MapGuide.MgByteReader"/> instance
    /// </summary>
    /// <returns></returns>
    public delegate MgByteReader GetByteReaderMethod();

    /// <summary>
    /// A read-only <see cref="System.IO.Stream"/> adapter for the <see cref="OSGeo.MapGuide.MgByteReader"/>
    /// class.
    /// </summary>
    public class MgReadOnlyStream : ReadOnlyRewindableStream
    {
        private MgByteReader _reader;

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="method"></param>
        public MgReadOnlyStream(GetByteReaderMethod method)
        {
            _reader = method();
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="MgReadOnlyStream"/> class.
        /// </summary>
        /// <param name="reader">The reader.</param>
        public MgReadOnlyStream(MgByteReader reader)
        {
            _reader = reader;
        }

        /// <summary>
        /// Releases unmanaged resources and performs other cleanup operations before the
        /// <see cref="MgReadOnlyStream"/> is reclaimed by garbage collection.
        /// </summary>
        ~MgReadOnlyStream()
        {
            Dispose(false);
        }

        /// <summary>
        /// Releases the unmanaged resources used by the <see cref="T:System.IO.Stream"/> and optionally releases the managed resources.
        /// </summary>
        /// <param name="disposing">true to release both managed and unmanaged resources; false to release only unmanaged resources.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                _reader.Dispose();
            }
            base.Dispose(disposing);
        }

        /// <summary>
        /// When overridden in a derived class, clears all buffers for this stream and causes any buffered data to be written to the underlying device.
        /// </summary>
        /// <exception cref="T:System.IO.IOException">
        /// An I/O error occurs.
        ///   </exception>
        public override void Flush()
        {

        }

        /// <summary>
        /// When overridden in a derived class, gets the length in bytes of the stream.
        /// </summary>
        /// <returns>
        /// A long value representing the length of the stream in bytes.
        ///   </returns>
        ///   
        /// <exception cref="T:System.NotSupportedException">
        /// A class derived from Stream does not support seeking.
        ///   </exception>
        ///   
        /// <exception cref="T:System.ObjectDisposedException">
        /// Methods were called after the stream was closed.
        ///   </exception>
        public override long Length
        {
            //NOTE: MgByteReader only returns remaining length! Should we
            //be keeping track of position and adding on this value?
            get { return _reader.GetLength(); }
        }

        /// <summary>
        /// Gets whether this stream is rewindable. If not, calls to <see cref="Rewind"/> throw a
        /// <see cref="T:System.InvalidOperationException"/>
        /// </summary>
        public override bool CanRewind
        {
            get { return _reader.IsRewindable(); }
        }

        /// <summary>
        /// Resets the internal position of the stream
        /// </summary>
        public override void Rewind()
        {
            if (!CanRewind)
                throw new InvalidOperationException(Strings.ErrorStreamNotRewindable);

            _reader.Rewind();
        }

        /// <summary>
        /// When overridden in a derived class, reads a sequence of bytes from the current stream and advances the position within the stream by the number of bytes read.
        /// </summary>
        /// <param name="buffer">An array of bytes. When this method returns, the buffer contains the specified byte array with the values between <paramref name="offset"/> and (<paramref name="offset"/> + <paramref name="count"/> - 1) replaced by the bytes read from the current source.</param>
        /// <param name="offset">The zero-based byte offset in <paramref name="buffer"/> at which to begin storing the data read from the current stream.</param>
        /// <param name="count">The maximum number of bytes to be read from the current stream.</param>
        /// <returns>
        /// The total number of bytes read into the buffer. This can be less than the number of bytes requested if that many bytes are not currently available, or zero (0) if the end of the stream has been reached.
        /// </returns>
        /// <exception cref="T:System.ArgumentException">
        /// The sum of <paramref name="offset"/> and <paramref name="count"/> is larger than the buffer length.
        ///   </exception>
        ///   
        /// <exception cref="T:System.ArgumentNullException">
        ///   <paramref name="buffer"/> is null.
        ///   </exception>
        ///   
        /// <exception cref="T:System.ArgumentOutOfRangeException">
        ///   <paramref name="offset"/> or <paramref name="count"/> is negative.
        ///   </exception>
        ///   
        /// <exception cref="T:System.IO.IOException">
        /// An I/O error occurs.
        ///   </exception>
        ///   
        /// <exception cref="T:System.NotSupportedException">
        /// The stream does not support reading.
        ///   </exception>
        ///   
        /// <exception cref="T:System.ObjectDisposedException">
        /// Methods were called after the stream was closed.
        ///   </exception>
        public override int Read(byte[] buffer, int offset, int count)
        {
            int read = 0;
            //For good times, please always have the offset as 0
            if (offset == 0)
            {
                read = _reader.Read(buffer, count);
            }
            else //So you want to play the hard way eh? Bad performance for you!
            {
                byte[] b = new byte[count];
                read = _reader.Read(b, count);
                Array.Copy(b, 0, buffer, offset, read);
            }
            return read;
        }
    }
}
