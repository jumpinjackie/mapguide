AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerResourceService.vcproj ServerResourceService.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I$(map_fdo_include) \
  -I../../../../Oem/dbxml-2.3.10/xerces-c-src/src \
  -I../../../../Oem/dbxml-2.3.10/install/include \
  -I../../../../Oem/dbxml-2.3.10/dbxml/include \
  -I../../../../Oem/dbxml-2.3.10/dbxml/src \
  -I../../../../Oem/DWFTK7.1/develop/global/src \
  -I../../../../Common/Security \
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MapGuideCommon \
  -I../../../../Common/MdfModel \
  -I../../../../Common/MdfParser \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Cache \
  -I../../Common/Manager \
  -I../../Common/ServerUtil \
  -I../Feature \
  -I../Site

AM_CXXFLAGS = @CXXFLAGS@ -DDWFTK_USE_DWFCORE_ZLIB

lib_LTLIBRARIES = libMgServerResourceService.la

MgServerResourceServiceBuild.lo : Makefile

libMgServerResourceService_la_SOURCES = \
  ServerResourceServiceBuild.cpp

include_SOURCES = \
  ApplicationRepository.cpp \
  ApplicationRepositoryManager.cpp \
  ApplicationResourceContentManager.cpp \
  ByteSourceDwfInputStreamImpl.cpp \
  Database.cpp \
  DataBindingInfo.cpp \
  DbEnvironment.cpp \
  LibraryRepository.cpp \
  LibraryRepositoryManager.cpp \
  LibraryResourceContentManager.cpp \
  OpApplyResourcePackage.cpp \
  OpChangeResourceOwner.cpp \
  OpCopyResource.cpp \
  OpCreateRepository.cpp \
  OpDeleteRepository.cpp \
  OpDeleteResource.cpp \
  OpDeleteResourceData.cpp \
  OpEnumerateParentMapDefinitions.cpp \
  OpEnumerateRepositories.cpp \
  OpEnumerateResourceData.cpp \
  OpEnumerateResourceDocuments.cpp \
  OpEnumerateResourceReferences.cpp \
  OpEnumerateResources.cpp \
  OpEnumerateUnmanagedData.cpp \
  OperationInfo.cpp \
  OperationParameter.cpp \
  OpGetRepositoryContent.cpp \
  OpGetRepositoryHeader.cpp \
  OpGetResourceContent.cpp \
  OpGetResourceData.cpp \
  OpGetResourceHeader.cpp \
  OpGetResourceModifiedDate.cpp \
  OpInheritPermissionsFrom.cpp \
  OpMoveResource.cpp \
  OpRenameResourceData.cpp \
  OpResourceExists.cpp \
  OpSetResource.cpp \
  OpSetResourceData.cpp \
  OpUpdateRepository.cpp \
  Repository.cpp \
  RepositoryManager.cpp \
  ResourceContainer.cpp \
  ResourceContentManager.cpp \
  ResourceDatabase.cpp \
  ResourceDataFileManager.cpp \
  ResourceDataManager.cpp \
  ResourceDataStreamManager.cpp \
  ResourceDefinitionManager.cpp \
  ResourceHeader.cpp \
  ResourceHeaderManager.cpp \
  ResourceInfo.cpp \
  ResourceManager.cpp \
  ResourceOperation.cpp \
  ResourceOperationFactory.cpp \
  ResourcePackageHandler.cpp \
  ResourcePackageLoader.cpp \
  ResourcePackageMaker.cpp \
  ResourcePackageManifestHandler.cpp \
  ResourcePackageManifestParser.cpp \
  ResourcePackageManifestSerializer.cpp \
  ResourceServiceHandler.cpp \
  ServerResourceService.cpp \
  SessionRepository.cpp \
  SessionRepositoryManager.cpp \
  SessionResourceContentManager.cpp \
  SiteRepository.cpp \
  SiteRepositoryManager.cpp \
  SiteResourceContentManager.cpp \
  SystemRepository.cpp \
  SystemRepositoryManager.cpp \
  SystemResourceContentManager.cpp \
  TagInfo.cpp \
  TagManager.cpp \
  XmlSchemaResolver.cpp \
  ZipFileHandler.cpp \
  ZipFileReader.cpp \
  ZipFileWriter.cpp

noinst_HEADERS = $(include_SOURCES) \
  ApplicationRepository.h \
  ApplicationRepositoryManager.h \
  ApplicationResourceContentManager.h \
  ByteSourceDwfInputStreamImpl.h \
  Database.h \
  DataBindingInfo.h \
  DbEnvironment.h \
  LibraryRepository.h \
  LibraryRepositoryManager.h \
  LibraryResourceContentManager.h \
  OpApplyResourcePackage.h \
  OpChangeResourceOwner.h \
  OpCopyResource.h \
  OpCreateRepository.h \
  OpDeleteRepository.h \
  OpDeleteResource.h \
  OpDeleteResourceData.h \
  OpEnumerateParentMapDefinitions.h \
  OpEnumerateRepositories.h \
  OpEnumerateResourceData.h \
  OpEnumerateResourceDocuments.h \
  OpEnumerateResourceReferences.h \
  OpEnumerateResources.h \
  OpEnumerateUnmanagedData.h \
  OperationInfo.h \
  OperationParameter.h \
  OpGetRepositoryContent.h \
  OpGetRepositoryHeader.h \
  OpGetResourceContent.h \
  OpGetResourceData.h \
  OpGetResourceHeader.h \
  OpGetResourceModifiedDate.h \
  OpInheritPermissionsFrom.h \
  OpMoveResource.h \
  OpRenameResourceData.h \
  OpResourceExists.h \
  OpSetResource.h \
  OpSetResourceData.h \
  OpUpdateRepository.h \
  Repository.h \
  RepositoryManager.h \
  ResourceContainer.h \
  ResourceContentManager.h \
  ResourceDatabase.h \
  ResourceDataFileManager.h \
  ResourceDataManager.h \
  ResourceDataStreamManager.h \
  ResourceDefinitionManager.h \
  ResourceHeader.h \
  ResourceHeaderManager.h \
  ResourceInfo.h \
  ResourceManager.h \
  ResourceOperationFactory.h \
  ResourcePackageHandler.h \
  ResourcePackageLoader.h \
  ResourcePackageMaker.h \
  ResourcePackageManifestHandler.h \
  ResourcePackageManifestParser.h \
  ResourcePackageManifestSerializer.h \
  ResourceOperation.h \
  ResourceServiceDefs.h \
  ResourceServiceHandler.h \
  ServerResourceDllExport.h \
  ServerResourceService.h \
  SessionRepository.h \
  SessionRepositoryManager.h \
  SessionResourceContentManager.h \
  SiteRepository.h \
  SiteRepositoryManager.h \
  SiteResourceContentManager.h \
  SystemRepository.h \
  SystemRepositoryManager.h \
  SystemResourceContentManager.h \
  TagInfo.h \
  TagManager.h \
  XmlSchemaResolver.h \
  ZipFileHandler.h \
  ZipFileReader.h \
  ZipFileWriter.h \
  ../../Common/stdafx.h

libMgServerResourceService_la_LIBADD = -lMgFoundation -lMgGeometry -lMgPlatformBase -lMgMapGuideCommon -lMgServerBase -lMgServerCache -lMgServerManager -lxerces-c -lxqilla -ldb_cxx -ldbxml

libMgServerResourceService_la_LDFLAGS = -version-info 7:0:0 \
  -L../../../../Common/Foundation \
  -L../../../../Common/Geometry \
  -L../../../../Common/PlatformBase \
  -L../../../../Common/MapGuideCommon \
  -L../../Common/Base \
  -L../../Common/Cache \
  -L../../Common/Manager \
  -L../../../../Oem/dbxml-2.3.10/install/lib
