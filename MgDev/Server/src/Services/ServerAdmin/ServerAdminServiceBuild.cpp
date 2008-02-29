//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifdef _WIN32
#include "Dll.cpp"
#endif
#include "OpBringOnline.cpp"
#include "OpClearLog.cpp"
#include "OpDeleteLog.cpp"
#include "OpDeletePackage.cpp"
#include "OpEnableMaximumLogSize.cpp"
#include "OpEnumerateLogs.cpp"
#include "OpEnumeratePackages.cpp"
#include "OpGetConfigurationProperties.cpp"
#include "OpGetDocument.cpp"
#include "OpGetInformationProperties.cpp"
#include "OpGetLog.cpp"
#include "OpGetLogFile.cpp"
#include "OpGetPackageLog.cpp"
#include "OpGetPackageStatus.cpp"
#include "OpIsOnline.cpp"
#include "OpIsMaximumLogSizeEnabled.cpp"
#include "OpLoadPackage.cpp"
#include "OpMakePackage.cpp"
#include "OpNotifyResourcesChanged.cpp"
#include "OpRegisterServicesOnServers.cpp"
#include "OpRenameLog.cpp"
#include "OpSetConfigurationProperties.cpp"
#include "OpRemoveConfigurationProperties.cpp"
#include "OpSetDocument.cpp"
#include "OpSetLogDelimiter.cpp"
#include "OpSetMaximumLogSize.cpp"
#include "OpTakeOffline.cpp"
#include "OpUnregisterServicesOnServers.cpp"
#include "ServerAdminOperation.cpp"
#include "ServerAdminOperationFactory.cpp"
#include "ServerAdminService.cpp"
#include "ServerAdminServiceHandler.cpp"
