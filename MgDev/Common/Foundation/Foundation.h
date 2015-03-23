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

#ifndef MG_FOUNDATION_H
#define MG_FOUNDATION_H

#include <cassert>

// Enable logging, our log files use this
#ifdef ACE_NLOGGING
#undef ACE_NLOGGING
#define ACE_NLOGGING 0
#endif

// Disable tracing by default
#ifdef ACE_NTRACE
#undef ACE_NTRACE
#define ACE_NTRACE  1
#endif

#ifdef _DEBUG
#ifdef ACE_NDEBUG
#undef ACE_NDEBUG
#endif
#else
#ifndef ACE_NDEBUG
#define ACE_NDEBUG
#endif
#endif

// ignore warnings about using deprecated methods
#pragma warning(disable: 4996)

#include "ace/Init_ACE.h"

#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/Log_Msg.h"
#include "ace/SString.h"
#include "ace/OS.h"
#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_sys_stat.h"
#include "ace/UUID.h"

#include "ace/Acceptor.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/LOCK_SOCK_Acceptor.h"
#include "ace/Reactor.h"
#include "ace/Thread_Manager.h"
#include "ace/OS_NS_Thread.h"
#include "ace/Task_T.h"
#include "ace/Service_Object.h"
#include "ace/svc_export.h"
#include "ace/Configuration.h"
#include "ace/Configuration_Import_Export.h"
#include "ace/Process_Manager.h"
#include "ace/Date_Time.h"

#undef GetObject
#include "FoundationDefs.h"
#include "System/FoundationClassId.h"
#include "System/Object.h"
#include "System/Ptr.h"
#include "System/Disposable.h"
#include "System/ClassFactory.h"
#include "System/GuardDisposable.h"
#include "System/Serializable.h"
#include "System/NamedSerializable.h"
#include "Data/Byte.h"
#include "Data/MimeType.h"
#include "System/ServerConnectionBase.h"
#include "Data/ByteReader.h"
#include "System/ByteSourceImpl.h"
#include "Data/ByteSource.h"
#include "Data/ByteSink.h"

#include "System/PacketStructure.h"
#include "System/StreamData.h"
#include "System/PacketParser.h"
#include "System/StreamHeader.h"
#include "System/StreamHelper.h"
#include "System/AceStreamHelper.h"
#include "System/MemoryStreamHelper.h"
#include "System/StreamReader.h"
#include "System/StreamWriter.h"
#include "System/Stream.h"
#include "System/StreamParser.h"

#include "../MdfModel/UnicodeString.h"
#include "Data/Collection.h"
#include "System/DisposableCollection.h"
#include "Data/StringCollection.h"
#include "Data/NamedCollection.h"
#include "Data/DateTime.h"
#include "Data/Property.h"
#include "Data/FeaturePropertyType.h"
#include "Data/PropertyDefinition.h"
#include "Data/PropertyCollection.h"
#include "Data/NullableProperty.h"
#include "Data/StringProperty.h"
#include "Data/BatchPropertyCollection.h"
#include "System/FoundationConfigProperties.h"
#include "System/FileUtil.h"
#include "System/Util.h"

#include "System/ConfigurationSection.h"
#include "System/ConfigurationSectionCollection.h"
#include "System/ConfigurationHeap.h"
#include "System/Configuration.h"
#include "System/Resources.h"

// Base exception classes and defines
#include "Exception/ExceptionDefs.h"
#include "Exception/Exception.h"
#include "Exception/SystemException.h"
#include "Exception/OutOfRangeException.h"
#include "Exception/IoException.h"
#include "Exception/FileIoException.h"
#include "Exception/StreamIoException.h"

#include "Exception/ApplicationException.h"
#include "Exception/ArgumentOutOfRangeException.h"
#include "Exception/ClassNotFoundException.h"
#include "Exception/ConfigurationException.h"
#include "Exception/ConfigurationLoadFailedException.h"
#include "Exception/ConfigurationSaveFailedException.h"
#include "Exception/DateTimeException.h"
#include "Exception/DecryptionException.h"
#include "Exception/DirectoryNotFoundException.h"
#include "Exception/DivideByZeroException.h"
#include "Exception/DomainException.h"
#include "Exception/DuplicateDirectoryException.h"
#include "Exception/DuplicateFileException.h"
#include "Exception/DuplicateObjectException.h"
#include "Exception/EncryptionException.h"
#include "Exception/FileNotFoundException.h"
#include "Exception/GridDensityException.h"
#include "Exception/IndexOutOfRangeException.h"
#include "Exception/InvalidArgumentException.h"
#include "Exception/InvalidCastException.h"
#include "Exception/InvalidMimeTypeException.h"
#include "Exception/InvalidOperationException.h"
#include "Exception/InvalidPropertyTypeException.h"
#include "Exception/InvalidStreamHeaderException.h"
#include "Exception/LengthException.h"
#include "Exception/LogicException.h"
#include "Exception/NotFiniteNumberException.h"
#include "Exception/NotImplementedException.h"
#include "Exception/NullArgumentException.h"
#include "Exception/NullPropertyValueException.h"
#include "Exception/NullReferenceException.h"
#include "Exception/ObjectNotFoundException.h"
#include "Exception/OutOfMemoryException.h"
#include "Exception/OverflowException.h"
#include "Exception/PlatformNotSupportedException.h"
#include "Exception/ResourcesException.h"
#include "Exception/ResourcesLoadFailedException.h"
#include "Exception/ResourceTagNotFoundException.h"
#include "Exception/RuntimeException.h"
#include "Exception/TemporaryFileNotAvailableException.h"
#include "Exception/ThirdPartyException.h"
#include "Exception/UnclassifiedException.h"
#include "Exception/UnderflowException.h"
#include "Exception/XmlException.h"
#include "Exception/XmlParserException.h"


#include "System/ByteSourceFileImpl.h"
#include "System/ByteSourceMemoryImpl.h"

#if !defined (ACE_HAS_WCHAR)
#error Please define ACE_HAS_WCHAR
#endif

#if !defined (ACE_MT_SAFE)
#error Please define ACE_MT_SAFE
#endif

#if !defined (ACE_HAS_THREADS)
#error Please define ACE_HAS_THREADS
#endif

#if defined (ACE_HAS_THREAD_SAFE_ACCEPT)
typedef ACE_LOCK_SOCK_Acceptor<ACE_SYNCH_MUTEX> SOCK_Acceptor;
#else
#error Please define ACE_HAS_THREAD_SAFE_ACCEPT
#endif /* ACE_HAS_THREAD_SAFE_ACCEPT */

//Macros to order bytes in little endian
# if defined (ACE_LITTLE_ENDIAN)
#   define MG_HTONS(x) x
#   define MG_NTOHS(x) x
#   define MG_HTONL(X) X
#   define MG_NTOHL(X) X
#   define MG_NTOH64(X) X
#   define MG_HTON64(X) X
# else
#   define MG_HTONS(x) ACE_SWAP_WORD(*reinterpret_cast<WORD *>(&x))
#   define MG_NTOHS(x) ACE_SWAP_WORD(*reinterpret_cast<WORD *>(&x))
#   define MG_HTONL(X) ACE_SWAP_LONG (*reinterpret_cast<UINT *>(&X))
#   define MG_NTOHL(X) ACE_SWAP_LONG (*reinterpret_cast<UINT *>(&X))
#   define MG_NTOH64(X) ACE_SWAP_64 (*reinterpret_cast<i64u *>(&X))
#   define MG_HTON64(X) ACE_SWAP_64 (*reinterpret_cast<i64u *>(&X))

# endif /* ACE_LITTLE_ENDIAN */

// Flag used for SIGPIPE handling
#ifdef _WIN32
    static const int MG_MSG_NOSIGNAL = 0;
#else
    static const int MG_MSG_NOSIGNAL = MSG_NOSIGNAL;
#endif

#endif
