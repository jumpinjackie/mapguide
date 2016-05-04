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

#include "Foundation.h"

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a ByteSourceFileImpl object
///</summary>
///<param name="filename">Name of the file</param>
///
ByteSourceFileImpl::ByteSourceFileImpl(CREFSTRING filename, bool temporary) :
    m_file(ACE_INVALID_HANDLE),
    m_size(0),
    m_temporary(temporary)
{
    LoadFile(filename);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a ByteSourceFileImpl object
///</summary>
///
ByteSourceFileImpl::~ByteSourceFileImpl()
{
    ACE_OS::close(m_file);
    m_file = ACE_INVALID_HANDLE;
    m_size = 0;
    if (m_temporary)
    {
        ACE_OS::unlink(MG_WCHAR_TO_TCHAR(m_name));
    }
    m_name = L"";
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Reads a buffer of data
///</summary>
///<param name="buffer">
/// A buffer receiving the data.
///</param>
///<param name="length">
///Maximum number of bytes to read
///</param>
///<returns>Actual number of bytes put in the buffer. 0 means end of bytes</returns>
///
INT32 ByteSourceFileImpl::Read(BYTE_ARRAY_OUT buffer, INT32 length)
{
    INT32 bytesRead = (INT32)ACE_OS::read(m_file, buffer, length);

    if (bytesRead < 0)
    {
        throw new MgFileIoException(L"ByteSourceFileImpl.Read",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_size -= bytesRead;

    return bytesRead;
}

INT64 ByteSourceFileImpl::GetLength()
{
    return m_size;
}

bool ByteSourceFileImpl::IsRewindable()
{
    return true;
}

void ByteSourceFileImpl::Rewind()
{
    m_size = ACE_OS::llseek(m_file, 0, SEEK_END);
    ACE_OS::llseek(m_file, 0, SEEK_SET);
}

void ByteSourceFileImpl::Rename(CREFSTRING newName)
{
    if (m_temporary == false)
    {
        throw new MgInvalidOperationException(L"ByteSourceFileImpl.Rename",
            __LINE__,__WFILE__, NULL, L"", NULL);
    }

    ACE_OS::close(m_file);
    m_file = ACE_INVALID_HANDLE;

    ACE_OS::rename(MG_WCHAR_TO_TCHAR(m_name), MG_WCHAR_TO_TCHAR(newName));

    m_name = L"";
    m_size = 0;
    m_temporary = false;

    LoadFile(newName);
}

void ByteSourceFileImpl::LoadFile(CREFSTRING filename)
{
    //Use ACE open insted of _wfopen to allow Linux compatibility
    m_file = ACE_OS::open(MG_WCHAR_TO_TCHAR(filename), 0, ACE_DEFAULT_OPEN_PERMS, 0);
    if(m_file == ACE_INVALID_HANDLE)
    {
        MgStringCollection arguments;
        arguments.Add(filename);

        if(errno == ENOENT)
        {
            MG_FILE_IO_EXCEPTION_ADD_ERRNO();
            throw new MgFileNotFoundException(L"ByteSourceFileImpl.LoadFile",
                __LINE__, __WFILE__, &arguments, L"MgFileIoErrNo", &errNoArgs);
        }
        else
        {
            MG_FILE_IO_EXCEPTION_ADD_ERRNO();
            throw new MgFileIoException(L"ByteSourceFileImpl.LoadFile",
                __LINE__, __WFILE__, &arguments, L"MgFileIoErrNo", &errNoArgs);
        }
    }

    m_size = ACE_OS::llseek(m_file, 0, SEEK_END);
    ACE_OS::llseek(m_file, 0, SEEK_SET);

    m_name = filename;
}
