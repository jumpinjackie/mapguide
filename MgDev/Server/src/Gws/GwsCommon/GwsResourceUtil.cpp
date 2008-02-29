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

#include "stdafx.h"
#include "GwsCommonImp.h"

#ifdef _WIN32
#include "GwsResource.h"
#else
#include "GwsResourceLinux.h"
#endif

#ifdef _WIN32
HMODULE CGwsResourceModule::shMod;
#endif

#define FIXED_BUF_SIZE 256
// Something is seriously wrong if the string is over this size.
static int MAX_BUF_SIZE = 8192;

#ifdef _DEBUG
CGwsResourceModule module(L"MgGwsResourced");
#else
CGwsResourceModule module(L"MgGwsResource");
#endif

CGwsResourceModule::CGwsResourceModule(const wchar_t* fileName)
{
#ifdef _WIN32
    // Try to load it
    shMod = ::LoadLibraryExW(fileName, NULL, LOAD_LIBRARY_AS_DATAFILE);
#endif
}


CGwsResourceModule::~CGwsResourceModule()
{
#ifdef _WIN32
    if (shMod)
    {
        ::FreeLibrary(shMod);
    }
#endif
}


// this function gets around the problem with loading strings
// since the ::LoadString function won't tell you what the string size is
// One might need to increase the size of the buffer.
bool
CGwsResourceModule::GetString(unsigned int id, std::wstring &str)
{
#ifdef _WIN32
    // as long as FIXED_BUF_SIZE is sufficiently large this
    // will be the fastest way to load a string.  However for
    // truly pathological cases, we will allocate increasingly
    // larger buffers if there is a problem.
    wchar_t buf[FIXED_BUF_SIZE];
    int nBufSize = FIXED_BUF_SIZE;
    wchar_t *pBuf = buf;

#pragma message("TODO: Determine if LoadStringW is thread safe")
    int nLen = ::LoadStringW(shMod, id, pBuf, FIXED_BUF_SIZE);
    if (0 == nLen)
    {
        // couldn't find the string.
        wchar_t wbuff [256];
        swprintf (wbuff, L"no string resource for id <%d>", id);
        str.append (wbuff);
        return false;
    }

    if (nLen < FIXED_BUF_SIZE - 1)
    {
        str = pBuf;
        return true;
    }
    // If the alloted space is not large enough for the string, then the
    // string is truncated.  Then the returned len will be one less than
    // nBufSize.  Its possible we have found the full string, but we
    // try again to be safe.
    while (nLen >= (nBufSize - 1))
    {
        nBufSize *= 2;
        // did we overflow?
        if (nBufSize > MAX_BUF_SIZE)
        {
            return false;
        }
        pBuf = new wchar_t [nBufSize];
        nLen = ::LoadStringW(shMod, id, pBuf, nBufSize);
        str = pBuf;
        delete [] pBuf;
    }
#else
    str = L"Message is defined for Windows only";
#endif
    return true;
}

#ifdef _WIN32
#define GET_STR(x,str) case (##x) : \
    GetString (GWS_MESSAGE_ID(x),str); \
    break
#else
#define GET_STR(x,str) case (x) : \
    str = GWS_MESSAGE_ID(x); \
    break
#endif


void CGwsResourceModule::GwsStatusMessage(EGwsStatus fes, std::wstring &str)
{
    switch(fes)
    {
    GET_STR(eGwsOk, str);
    GET_STR(eGwsWarningFailedToLockSomeFeatures, str);
    GET_STR(eGwsWarningFailedToUnlockSomeFeatures, str);
    GET_STR(eGwsWarningFailedToConnectSomeFeatureSources, str);
    GET_STR(eGwsWarningFeatureSourceSessionNotStarted, str);
    GET_STR(eGwsWarningNotCaching, str);
    GET_STR(eGwsWarningStillLoadingCache, str);
    GET_STR(eGwsWarningCacheLoadingStopped, str);
    GET_STR(eGwsWarningSomePropertyValuesAreUninitialized,str);
    GET_STR(eGwsOperationInProgress, str);
    GET_STR(eGwsOperationIsNotRunning, str);
    GET_STR(eGwsOperationCanceled, str);
    GET_STR(eGwsWarningFeatureSourceClassIsNotVersionEnabled, str);

    GET_STR(eGwsFailed, str);
    GET_STR(eGwsNullPointer, str);
    GET_STR(eGwsOutOfMemory, str);
    GET_STR(eGwsNotSupported, str);
    GET_STR(eGwsDeleteCommandNotSupported, str);
    GET_STR(eGwsUpdateCommandNotSupported, str);
    GET_STR(eGwsInsertCommandNotSupported, str);
    GET_STR(eGwsFdoProviderError, str);
    GET_STR(eGwsAlreadyOpened , str);
    GET_STR(eGwsClosed , str);
    GET_STR(eGwsDisconnected , str);
    GET_STR(eGwsConnected, str);
    GET_STR(eGwsConnectionNotInitialized, str);
    GET_STR(eGwsOutstandingSessions, str);
    GET_STR(eFailedToStartFeatureSourceSession, str);
    GET_STR(eFailedToStartLayerSourceSession, str);
    GET_STR(eFailedToStartTransaction, str);
    GET_STR(eGwsNoTransactionStarted, str);
    GET_STR(eGwsFailedToCommitCache, str);
    GET_STR(eGwsFailedToRollbackCache, str);
    GET_STR(eGwsFailedToCommitTransaction, str);
    GET_STR(eGwsFailedToRollbackTransaction, str);
    GET_STR(eGwsFailedToCommitSomeTransactions, str);
    GET_STR(eGwsFailedToRollbackSomeTransactions, str);
    GET_STR(eGwsFailedToTerminateSession, str);
    GET_STR(eGwsSessionNotFound, str);
    GET_STR(eGwsFailedToFlushChanges, str);
    GET_STR(eGwsFeatureSourceDoesnotSupportLongIdentity , str);
    GET_STR(eGwsGwsAlreadyExists, str);
    GET_STR(eGwsFailedToDesribeClass, str);
    GET_STR(eGwsFeatureStorageDoesNotExist, str);
    GET_STR(eGwsFeatureStorageAlreadyCreated, str);
    GET_STR(eGwsFailedToPrepareQuery, str);
    GET_STR(eGwsFailedToCreateQuery, str);
    GET_STR(eGwsFailedToExecuteQuery, str);
    GET_STR(eGwsDoesNotSupportRightJoin, str);
    GET_STR(eGwsDoesNotSupportLeftJoin, str);
    GET_STR(eGwsWarningStorageAlreadyLoaded, str);
    GET_STR(eGwsNoExplicitFeatureSourceInLayerQuery, str);
    GET_STR(eGwsDataSourceNotFound, str);
    GET_STR(eGwsDataSourceAlreadyExists, str);
    GET_STR(eGwsDataSourceAlreadyAdded, str);
    GET_STR(eGwsConnectionParameterNotFound, str);
    GET_STR(eGwsFailedToInitializeFdoConnection, str);
    GET_STR(eGwsFailedToOpenFdoConnection, str);
    GET_STR(eGwsFailedToCloseFdoConnection, str);
    GET_STR(eGwsFailedToReadGws, str);
    GET_STR(eGwsFailedToWriteGws, str);
    GET_STR(eGwsLayerSourceNotPartOfGws, str);
    GET_STR(eGwsFailedToDropLayerStorage, str);
    GET_STR(eGwsFailedToCreateLayerStorage, str);
    GET_STR(eGwsFailedToAddPropertyDefinition, str);
    GET_STR(eGwsFailedToAddIdentityPropertyDefinition, str);
    GET_STR(eGwsFailedToCreateGwsFeatureDefintion, str);
    GET_STR(eGwsFailedToCacheFeatures, str);
    GET_STR(eGwsFailedToDeleteFeature, str);
    GET_STR(eGwsFailedToDeleteLayerFeature, str);
    GET_STR(eGwsGetCachedFeaturesByIdsFailed, str);
    GET_STR(eGwsStorageNotLoaded, str);
    GET_STR(eGwsInvalidParameter, str);
    GET_STR(eGwsObjectsRemainInEditSet, str);
    GET_STR(eGwsFdoReadOnlyProperty, str);
    GET_STR(eGwsFdoQueryIsNotPrepared, str);
    GET_STR(eGwsFdoCommandIsNotPrepared, str);
    GET_STR(eGwsFdoLockConflict, str);
    GET_STR(eGwsFdoVersionConflict, str);
    GET_STR(eGwsFailedToExecuteCommand, str);
    GET_STR(eGwsFailedToPrepareCommand, str);
    GET_STR(eGwsFailedToFetchResults, str);
    GET_STR(eGwsFailedToGetProperty, str);
    GET_STR(eGwsFdoInvalidPropertyType, str);
    GET_STR(eGwsFdoUnsupportedPropertyType, str);
    GET_STR(eGwsFeatureDoesNotExist, str);
    GET_STR(eGwsFeatureHasBeenDeleted, str);
    GET_STR(eGwsRevisionNumbersConflict, str);
    GET_STR(eGwsInvalidPropertyName, str);
    GET_STR(eGwsFailedToLockFeatures, str);
    GET_STR(eGwsFailedToUnlockFeatures, str);
    GET_STR(eGwsCannotRemoveNewFeatureFromEditSet, str);
    GET_STR(eGwsJoinPropertiesCardinalityViolation, str);
    GET_STR(eGwsFailedToCreateThread, str);
    GET_STR(eGwsClosedFeatureIterator, str);
    GET_STR(eGwsInvalidGwsName, str);
    GET_STR(eGwsIsReadOnly, str);
    GET_STR(eGwsFileNotFound, str);
    GET_STR(eGwsCannotCopyGwsCacheFiles, str);
    GET_STR(eGwsGwsCacheCorrupted, str);
    GET_STR(eGwsInvalidCacheId, str);
    GET_STR(eGwsFailedToInsertFeature, str);
    GET_STR(eGwsFailedToInsertLayerFeature, str);
    GET_STR(eGwsFailedToUpdateFeature, str);
    GET_STR(eGwsFailedToUpdateLayerFeature, str);
    GET_STR(eGwsFailedToSaveGws, str);
    GET_STR(eGwsFailedToDescribeLayer, str);
    GET_STR(eGwsFailedToCommitLayerSourceTransaction, str);
    GET_STR(eGwsCannotGetPropertyValueOffline, str);
    GET_STR(eGwsFeatureNotFoundOnline, str);
    GET_STR(eGwsFailedToCreateMutableFeature, str);
    GET_STR(eGwsNotAFeatureClass, str);
    GET_STR(eGwsNoGeometryProperty, str);
    GET_STR(eGwsItemAlreadyAdded, str);
    GET_STR(eGwsItemNotFound, str);
    GET_STR(eGwsVersionNotFound, str);
    GET_STR(eGwsVersionResolutionFailed, str);
    GET_STR(eGwsFailedToPrepareLayerSourceQuery, str);
    GET_STR(eGwsWrongLayerFeatureId, str);
    GET_STR(eGwsFailedToSaveEditSet, str);
    GET_STR(eGwsMissingLeftJoinAttributes, str);
    GET_STR(eGwsInvalidJoinAttributeType, str);
    GET_STR(eGwsJoinAttributesAreNotComparable, str);
    GET_STR(eGwsClassNotReferencedByLayer, str);
    GET_STR(eGwsInvalidFeatureSourceName, str);
    GET_STR(eGwsEditSetUnavailable, str);
    GET_STR(eGwsFailedToTransformFeature, str);
    GET_STR(eGwsFailedToParseFeature, str);
    GET_STR(eGwsSCNotFound, str);
    GET_STR(eGwsFailedToHandleSourceCS, str);
    GET_STR(eGwsFailedToHandleDestinationCS, str);
    GET_STR(eGwsFailedToInitializeCSC, str);
    GET_STR(eGwsFeatureSourceSessionTerminated, str);
    GET_STR(eGwsLayerSourceSessionTerminated, str);
    GET_STR(eGwsCannotSetActiveVersion, str);
    GET_STR(eGwsNotAllFeatureSourcesConnected, str);
    GET_STR(eGwsCannotMakeUniqueGwsName, str);
    GET_STR(eGwsFailedToUpdateLayerSourceVersion, str);
    GET_STR(eGwsWarningExtentsNotYetAvailable, str);
    GET_STR(eGwsFailedToRefreshFeatures, str);
    GET_STR(eGwsNoLayers, str);
    GET_STR(eGwsFeatureSourceIsReadOnly, str);
    GET_STR(eGwsFeatureClassHasNoIdentity, str);
    GET_STR(eGwsFdoInvalidDataType, str);
    default:
        str = L"GWS message resource id is not defined.";
        break;
    }
}


void CGwsResourceModule::FeatureStatusMessage(EGwsFeatureStatus fstat, std::wstring& str)
{
    switch(fstat)
    {
    GET_STR(eGwsSucceeded, str);
    GET_STR(eGwsRevisionNumberConflict, str);
    GET_STR(eGwsLockRejected, str);
    GET_STR(eGwsLockedOffline, str);
    GET_STR(eGwsUnlockRejected, str);
    GET_STR(eGwsCannotUnlockOffline, str);
    GET_STR(eGwsFeatureAlreadyInEditSet, str);
    GET_STR(eGwsSkippedNewFeature, str);
    GET_STR(eGwsSkippedNotLockedFeature, str);
    GET_STR(eGwsFeatureNotSaved, str);
    GET_STR(eGwsCannotRemoveModifedFeatureOffline, str);
    GET_STR(eGwsNewFeatureCannotBeRemoved, str);
    GET_STR(eGwsNewFeatureRemoved, str);
    GET_STR(eGwsNotProcessed, str);
    GET_STR(eGwsRefreshFailedToReloadEditSetFeature, str);
    GET_STR(eGwsRefreshCacheFeatureIsUpTodate, str);
    GET_STR(eGwsRefreshDeletedFeatureInCache, str);
    GET_STR(eGwsRefreshReloadedFeatureInCache, str);
    GET_STR(eGwsRefreshFeatureNotFound, str);
    default:
        str = L"GWS feature status message is not defined.";
        break;
    }
}
