//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef ACE_COMMON_H_
#define ACE_COMMON_H_

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

// Do not deprecate standard C runtimes
#define _CRT_SECURE_NO_DEPRECATE 1

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

#include "Net/PacketStructure.h"
#include "Net/StreamData.h"
#include "Net/StreamParser.h"
#include "Net/StreamHelper.h"
#include "Net/AceStreamHelper.h"
#include "Net/MemoryStreamHelper.h"
#include "Net/PacketParser.h"
#include "Net/IOperationHandler.h"

#include "System/CommonDefs.h"
#include "System/StreamHeader.h"
#include "System/StreamReader.h"
#include "System/StreamWriter.h"
#include "System/Stream.h"
#include "System/ConfigurationSection.h"
#include "System/ConfigurationHeap.h"
#include "System/Configuration.h"
#include "System/Resources.h"

#include "Common.h"

#include "Data/ByteSourceFileImpl.h"
#include "Data/ByteSourceMemoryImpl.h"
#include "Data/ByteSourceSocketStreamImpl.h"
#include "Data/ByteSourceStreamImpl.h"

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
# else
#   define MG_HTONS(x) ACE_SWAP_WORD(x)
#   define MG_NTOHS(x) ACE_SWAP_WORD(x)
#   define MG_HTONL(X) ACE_SWAP_LONG (X)
#   define MG_NTOHL(X) ACE_SWAP_LONG (X)
# endif /* ACE_LITTLE_ENDIAN */

// Flag used for SIGPIPE handling
#ifdef _WIN32
    static const int MG_MSG_NOSIGNAL = 0;
#else
    static const int MG_MSG_NOSIGNAL = MSG_NOSIGNAL;
#endif

#endif
