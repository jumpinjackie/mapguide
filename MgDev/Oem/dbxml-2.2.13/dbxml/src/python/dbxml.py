# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _dbxml

def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


try:
    from weakref import proxy as weakref_proxy
except:
    weakref_proxy = lambda x: x


DB_CREATE = _dbxml.DB_CREATE
DB_DIRTY_READ = _dbxml.DB_DIRTY_READ
DB_EXCL = _dbxml.DB_EXCL
DB_NOMMAP = _dbxml.DB_NOMMAP
DB_RDONLY = _dbxml.DB_RDONLY
DB_THREAD = _dbxml.DB_THREAD
DB_INIT_LOCK = _dbxml.DB_INIT_LOCK
DB_INIT_LOG = _dbxml.DB_INIT_LOG
DB_INIT_MPOOL = _dbxml.DB_INIT_MPOOL
DB_INIT_TXN = _dbxml.DB_INIT_TXN
DB_SALVAGE = _dbxml.DB_SALVAGE
DB_AGGRESSIVE = _dbxml.DB_AGGRESSIVE
DBXML_ADOPT_DBENV = _dbxml.DBXML_ADOPT_DBENV
DBXML_ALLOW_EXTERNAL_ACCESS = _dbxml.DBXML_ALLOW_EXTERNAL_ACCESS
DBXML_ALLOW_AUTO_OPEN = _dbxml.DBXML_ALLOW_AUTO_OPEN
DBXML_ALLOW_VALIDATION = _dbxml.DBXML_ALLOW_VALIDATION
DBXML_TRANSACTIONAL = _dbxml.DBXML_TRANSACTIONAL
DBXML_REVERSE_ORDER = _dbxml.DBXML_REVERSE_ORDER
DBXML_INDEX_VALUES = _dbxml.DBXML_INDEX_VALUES
DBXML_CACHE_DOCUMENTS = _dbxml.DBXML_CACHE_DOCUMENTS
DBXML_CHKSUM = _dbxml.DBXML_CHKSUM
DBXML_ENCRYPT = _dbxml.DBXML_ENCRYPT
DBXML_NO_INDEX_NODES = _dbxml.DBXML_NO_INDEX_NODES
DBXML_GEN_NAME = _dbxml.DBXML_GEN_NAME
DBXML_LAZY_DOCS = _dbxml.DBXML_LAZY_DOCS
DBXML_INDEX_NODES = _dbxml.DBXML_INDEX_NODES
LEVEL_NONE = _dbxml.LEVEL_NONE
LEVEL_DEBUG = _dbxml.LEVEL_DEBUG
LEVEL_INFO = _dbxml.LEVEL_INFO
LEVEL_WARNING = _dbxml.LEVEL_WARNING
LEVEL_ERROR = _dbxml.LEVEL_ERROR
LEVEL_ALL = _dbxml.LEVEL_ALL
CATEGORY_NONE = _dbxml.CATEGORY_NONE
CATEGORY_INDEXER = _dbxml.CATEGORY_INDEXER
CATEGORY_QUERY = _dbxml.CATEGORY_QUERY
CATEGORY_OPTIMIZER = _dbxml.CATEGORY_OPTIMIZER
CATEGORY_DICTIONARY = _dbxml.CATEGORY_DICTIONARY
CATEGORY_CONTAINER = _dbxml.CATEGORY_CONTAINER
CATEGORY_NODESTORE = _dbxml.CATEGORY_NODESTORE
CATEGORY_MANAGER = _dbxml.CATEGORY_MANAGER
CATEGORY_ALL = _dbxml.CATEGORY_ALL
metaDataNamespace_uri = _dbxml.metaDataNamespace_uri
metaDataNamespace_prefix = _dbxml.metaDataNamespace_prefix
metaDataName_name = _dbxml.metaDataName_name
metaDataName_root = _dbxml.metaDataName_root
class XmlManager(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlManager, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlManager, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlManager instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, XmlManager, 'this', _dbxml.new_XmlManager(*args))
        _swig_setattr(self, XmlManager, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlManager):
        try:
            if self.thisown: destroy(self)
        except: pass

    def setDefaultContainerFlags(*args): return _dbxml.XmlManager_setDefaultContainerFlags(*args)
    def getDefaultContainerFlags(*args): return _dbxml.XmlManager_getDefaultContainerFlags(*args)
    def setDefaultPageSize(*args): return _dbxml.XmlManager_setDefaultPageSize(*args)
    def getDefaultPageSize(*args): return _dbxml.XmlManager_getDefaultPageSize(*args)
    def setDefaultSequenceIncrement(*args): return _dbxml.XmlManager_setDefaultSequenceIncrement(*args)
    def getDefaultSequenceIncrement(*args): return _dbxml.XmlManager_getDefaultSequenceIncrement(*args)
    def setDefaultContainerType(*args): return _dbxml.XmlManager_setDefaultContainerType(*args)
    def getDefaultContainerType(*args): return _dbxml.XmlManager_getDefaultContainerType(*args)
    def getDbEnv(*args): return _dbxml.XmlManager_getDbEnv(*args)
    def getHome(*args): return _dbxml.XmlManager_getHome(*args)
    def registerResolver(*args): return _dbxml.XmlManager_registerResolver(*args)
    def existsContainer(*args): return _dbxml.XmlManager_existsContainer(*args)
    def removeContainer(*args): return _dbxml.XmlManager_removeContainer(*args)
    def renameContainer(*args): return _dbxml.XmlManager_renameContainer(*args)
    def upgradeContainer(*args): return _dbxml.XmlManager_upgradeContainer(*args)
    def reindexContainer(*args): return _dbxml.XmlManager_reindexContainer(*args)
    def createStdInInputStream(*args): return _dbxml.XmlManager_createStdInInputStream(*args)
    def createLocalFileInputStream(*args): return _dbxml.XmlManager_createLocalFileInputStream(*args)
    def createMemBufInputStream(*args): return _dbxml.XmlManager_createMemBufInputStream(*args)
    def createURLInputStream(*args): return _dbxml.XmlManager_createURLInputStream(*args)
    def createContainer(*args): return _dbxml.XmlManager_createContainer(*args)
    def openContainer(*args): return _dbxml.XmlManager_openContainer(*args)
    def dumpContainer(*args): return _dbxml.XmlManager_dumpContainer(*args)
    def loadContainer(*args): return _dbxml.XmlManager_loadContainer(*args)
    def verifyContainer(*args): return _dbxml.XmlManager_verifyContainer(*args)
    def createDocument(*args): return _dbxml.XmlManager_createDocument(*args)
    def createQueryContext(*args): return _dbxml.XmlManager_createQueryContext(*args)
    def createUpdateContext(*args): return _dbxml.XmlManager_createUpdateContext(*args)
    def prepare(*args): return _dbxml.XmlManager_prepare(*args)
    def query(*args): return _dbxml.XmlManager_query(*args)
    def createResults(*args): return _dbxml.XmlManager_createResults(*args)
    def createModify(*args): return _dbxml.XmlManager_createModify(*args)
    def createTransaction(*args): return _dbxml.XmlManager_createTransaction(*args)
    def createIndexLookup(*args): return _dbxml.XmlManager_createIndexLookup(*args)
    __swig_getmethods__["setLogLevel"] = lambda x: _dbxml.XmlManager_setLogLevel
    if _newclass:setLogLevel = staticmethod(_dbxml.XmlManager_setLogLevel)
    __swig_getmethods__["setLogCategory"] = lambda x: _dbxml.XmlManager_setLogCategory
    if _newclass:setLogCategory = staticmethod(_dbxml.XmlManager_setLogCategory)
    __swig_getmethods__["get_version_major"] = lambda x: _dbxml.XmlManager_get_version_major
    if _newclass:get_version_major = staticmethod(_dbxml.XmlManager_get_version_major)
    __swig_getmethods__["get_version_minor"] = lambda x: _dbxml.XmlManager_get_version_minor
    if _newclass:get_version_minor = staticmethod(_dbxml.XmlManager_get_version_minor)
    __swig_getmethods__["get_version_patch"] = lambda x: _dbxml.XmlManager_get_version_patch
    if _newclass:get_version_patch = staticmethod(_dbxml.XmlManager_get_version_patch)
    __swig_getmethods__["get_version_string"] = lambda x: _dbxml.XmlManager_get_version_string
    if _newclass:get_version_string = staticmethod(_dbxml.XmlManager_get_version_string)

class XmlManagerPtr(XmlManager):
    def __init__(self, this):
        _swig_setattr(self, XmlManager, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlManager, 'thisown', 0)
        _swig_setattr(self, XmlManager,self.__class__,XmlManager)
_dbxml.XmlManager_swigregister(XmlManagerPtr)

XmlManager_setLogLevel = _dbxml.XmlManager_setLogLevel

XmlManager_setLogCategory = _dbxml.XmlManager_setLogCategory

XmlManager_get_version_major = _dbxml.XmlManager_get_version_major

XmlManager_get_version_minor = _dbxml.XmlManager_get_version_minor

XmlManager_get_version_patch = _dbxml.XmlManager_get_version_patch

XmlManager_get_version_string = _dbxml.XmlManager_get_version_string

class XmlIndexLookup(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlIndexLookup, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlIndexLookup, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlIndexLookup instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    NONE = _dbxml.XmlIndexLookup_NONE
    EQ = _dbxml.XmlIndexLookup_EQ
    GT = _dbxml.XmlIndexLookup_GT
    GTE = _dbxml.XmlIndexLookup_GTE
    LT = _dbxml.XmlIndexLookup_LT
    LTE = _dbxml.XmlIndexLookup_LTE
    def __init__(self, *args):
        _swig_setattr(self, XmlIndexLookup, 'this', _dbxml.new_XmlIndexLookup(*args))
        _swig_setattr(self, XmlIndexLookup, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlIndexLookup):
        try:
            if self.thisown: destroy(self)
        except: pass

    def isNull(*args): return _dbxml.XmlIndexLookup_isNull(*args)
    def getIndex(*args): return _dbxml.XmlIndexLookup_getIndex(*args)
    def setIndex(*args): return _dbxml.XmlIndexLookup_setIndex(*args)
    def getNodeURI(*args): return _dbxml.XmlIndexLookup_getNodeURI(*args)
    def getNodeName(*args): return _dbxml.XmlIndexLookup_getNodeName(*args)
    def setNode(*args): return _dbxml.XmlIndexLookup_setNode(*args)
    def getParentURI(*args): return _dbxml.XmlIndexLookup_getParentURI(*args)
    def getParentName(*args): return _dbxml.XmlIndexLookup_getParentName(*args)
    def setParent(*args): return _dbxml.XmlIndexLookup_setParent(*args)
    def getLowBoundValue(*args): return _dbxml.XmlIndexLookup_getLowBoundValue(*args)
    def getLowBoundOperation(*args): return _dbxml.XmlIndexLookup_getLowBoundOperation(*args)
    def setLowBound(*args): return _dbxml.XmlIndexLookup_setLowBound(*args)
    def getHighBoundValue(*args): return _dbxml.XmlIndexLookup_getHighBoundValue(*args)
    def getHighBoundOperation(*args): return _dbxml.XmlIndexLookup_getHighBoundOperation(*args)
    def setHighBound(*args): return _dbxml.XmlIndexLookup_setHighBound(*args)
    def getContainer(*args): return _dbxml.XmlIndexLookup_getContainer(*args)
    def setContainer(*args): return _dbxml.XmlIndexLookup_setContainer(*args)
    def execute(*args): return _dbxml.XmlIndexLookup_execute(*args)

class XmlIndexLookupPtr(XmlIndexLookup):
    def __init__(self, this):
        _swig_setattr(self, XmlIndexLookup, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlIndexLookup, 'thisown', 0)
        _swig_setattr(self, XmlIndexLookup,self.__class__,XmlIndexLookup)
_dbxml.XmlIndexLookup_swigregister(XmlIndexLookupPtr)

class XmlContainer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlContainer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlContainer, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlContainer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    WholedocContainer = _dbxml.XmlContainer_WholedocContainer
    NodeContainer = _dbxml.XmlContainer_NodeContainer
    def __init__(self, *args):
        _swig_setattr(self, XmlContainer, 'this', _dbxml.new_XmlContainer(*args))
        _swig_setattr(self, XmlContainer, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlContainer):
        try:
            if self.thisown: destroy(self)
        except: pass

    def sync(*args): return _dbxml.XmlContainer_sync(*args)
    def close(*args): return _dbxml.XmlContainer_close(*args)
    def addAlias(*args): return _dbxml.XmlContainer_addAlias(*args)
    def removeAlias(*args): return _dbxml.XmlContainer_removeAlias(*args)
    def getManager(*args): return _dbxml.XmlContainer_getManager(*args)
    def getName(*args): return _dbxml.XmlContainer_getName(*args)
    def getContainerType(*args): return _dbxml.XmlContainer_getContainerType(*args)
    def getIndexNodes(*args): return _dbxml.XmlContainer_getIndexNodes(*args)
    def getPageSize(*args): return _dbxml.XmlContainer_getPageSize(*args)
    def setIndexSpecification(*args): return _dbxml.XmlContainer_setIndexSpecification(*args)
    def addIndex(*args): return _dbxml.XmlContainer_addIndex(*args)
    def deleteIndex(*args): return _dbxml.XmlContainer_deleteIndex(*args)
    def replaceIndex(*args): return _dbxml.XmlContainer_replaceIndex(*args)
    def addDefaultIndex(*args): return _dbxml.XmlContainer_addDefaultIndex(*args)
    def deleteDefaultIndex(*args): return _dbxml.XmlContainer_deleteDefaultIndex(*args)
    def replaceDefaultIndex(*args): return _dbxml.XmlContainer_replaceDefaultIndex(*args)
    def putDocument(*args): return _dbxml.XmlContainer_putDocument(*args)
    def deleteDocument(*args): return _dbxml.XmlContainer_deleteDocument(*args)
    def updateDocument(*args): return _dbxml.XmlContainer_updateDocument(*args)
    def getNumDocuments(*args): return _dbxml.XmlContainer_getNumDocuments(*args)
    def getIndexSpecification(*args): return _dbxml.XmlContainer_getIndexSpecification(*args)
    def getDocument(*args): return _dbxml.XmlContainer_getDocument(*args)
    def getAllDocuments(*args): return _dbxml.XmlContainer_getAllDocuments(*args)
    def lookupIndex(*args): return _dbxml.XmlContainer_lookupIndex(*args)
    def lookupStatistics(*args): return _dbxml.XmlContainer_lookupStatistics(*args)

class XmlContainerPtr(XmlContainer):
    def __init__(self, this):
        _swig_setattr(self, XmlContainer, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlContainer, 'thisown', 0)
        _swig_setattr(self, XmlContainer,self.__class__,XmlContainer)
_dbxml.XmlContainer_swigregister(XmlContainerPtr)

class XmlDocument(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlDocument, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlDocument, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlDocument instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, XmlDocument, 'this', _dbxml.new_XmlDocument(*args))
        _swig_setattr(self, XmlDocument, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlDocument):
        try:
            if self.thisown: destroy(self)
        except: pass

    def setName(*args): return _dbxml.XmlDocument_setName(*args)
    def getName(*args): return _dbxml.XmlDocument_getName(*args)
    def setContent(*args): return _dbxml.XmlDocument_setContent(*args)
    def getContentAsXmlInputStream(*args): return _dbxml.XmlDocument_getContentAsXmlInputStream(*args)
    def setContentAsXmlInputStream(*args): return _dbxml.XmlDocument_setContentAsXmlInputStream(*args)
    def fetchAllData(*args): return _dbxml.XmlDocument_fetchAllData(*args)
    def removeMetaData(*args): return _dbxml.XmlDocument_removeMetaData(*args)
    def setMetaData(*args): return _dbxml.XmlDocument_setMetaData(*args)
    def getContentAsString(*args): return _dbxml.XmlDocument_getContentAsString(*args)
    def getContent(*args): return _dbxml.XmlDocument_getContent(*args)
    def getMetaData(*args): return _dbxml.XmlDocument_getMetaData(*args)
    def getMetaDataIterator(*args): return _dbxml.XmlDocument_getMetaDataIterator(*args)

class XmlDocumentPtr(XmlDocument):
    def __init__(self, this):
        _swig_setattr(self, XmlDocument, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlDocument, 'thisown', 0)
        _swig_setattr(self, XmlDocument,self.__class__,XmlDocument)
_dbxml.XmlDocument_swigregister(XmlDocumentPtr)

class XmlMetaDataIterator(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlMetaDataIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlMetaDataIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlMetaDataIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, XmlMetaDataIterator, 'this', _dbxml.new_XmlMetaDataIterator(*args))
        _swig_setattr(self, XmlMetaDataIterator, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlMetaDataIterator):
        try:
            if self.thisown: destroy(self)
        except: pass

    def reset(*args): return _dbxml.XmlMetaDataIterator_reset(*args)
    def next(*args): return _dbxml.XmlMetaDataIterator_next(*args)
    def __iter__(self): return self


class XmlMetaDataIteratorPtr(XmlMetaDataIterator):
    def __init__(self, this):
        _swig_setattr(self, XmlMetaDataIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlMetaDataIterator, 'thisown', 0)
        _swig_setattr(self, XmlMetaDataIterator,self.__class__,XmlMetaDataIterator)
_dbxml.XmlMetaDataIterator_swigregister(XmlMetaDataIteratorPtr)

class XmlMetaData(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlMetaData, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlMetaData, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlMetaData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dbxml.delete_XmlMetaData):
        try:
            if self.thisown: destroy(self)
        except: pass

    def get_uri(*args): return _dbxml.XmlMetaData_get_uri(*args)
    def get_name(*args): return _dbxml.XmlMetaData_get_name(*args)
    def get_value(*args): return _dbxml.XmlMetaData_get_value(*args)
    def __init__(self, *args):
        _swig_setattr(self, XmlMetaData, 'this', _dbxml.new_XmlMetaData(*args))
        _swig_setattr(self, XmlMetaData, 'thisown', 1)

class XmlMetaDataPtr(XmlMetaData):
    def __init__(self, this):
        _swig_setattr(self, XmlMetaData, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlMetaData, 'thisown', 0)
        _swig_setattr(self, XmlMetaData,self.__class__,XmlMetaData)
_dbxml.XmlMetaData_swigregister(XmlMetaDataPtr)

class XmlQueryContext(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlQueryContext, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlQueryContext, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlQueryContext instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    DeadValues = _dbxml.XmlQueryContext_DeadValues
    LiveValues = _dbxml.XmlQueryContext_LiveValues
    Eager = _dbxml.XmlQueryContext_Eager
    Lazy = _dbxml.XmlQueryContext_Lazy
    def __init__(self, *args):
        _swig_setattr(self, XmlQueryContext, 'this', _dbxml.new_XmlQueryContext(*args))
        _swig_setattr(self, XmlQueryContext, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlQueryContext):
        try:
            if self.thisown: destroy(self)
        except: pass

    def setNamespace(*args): return _dbxml.XmlQueryContext_setNamespace(*args)
    def getNamespace(*args): return _dbxml.XmlQueryContext_getNamespace(*args)
    def removeNamespace(*args): return _dbxml.XmlQueryContext_removeNamespace(*args)
    def clearNamespaces(*args): return _dbxml.XmlQueryContext_clearNamespaces(*args)
    def setVariableValue(*args): return _dbxml.XmlQueryContext_setVariableValue(*args)
    def setBaseURI(*args): return _dbxml.XmlQueryContext_setBaseURI(*args)
    def getBaseURI(*args): return _dbxml.XmlQueryContext_getBaseURI(*args)
    def setReturnType(*args): return _dbxml.XmlQueryContext_setReturnType(*args)
    def getReturnType(*args): return _dbxml.XmlQueryContext_getReturnType(*args)
    def setEvaluationType(*args): return _dbxml.XmlQueryContext_setEvaluationType(*args)
    def getEvaluationType(*args): return _dbxml.XmlQueryContext_getEvaluationType(*args)
    def setDefaultCollection(*args): return _dbxml.XmlQueryContext_setDefaultCollection(*args)
    def getDefaultCollection(*args): return _dbxml.XmlQueryContext_getDefaultCollection(*args)
    def getVariableValue(*args): return _dbxml.XmlQueryContext_getVariableValue(*args)
    def getVariableValues(*args): return _dbxml.XmlQueryContext_getVariableValues(*args)

class XmlQueryContextPtr(XmlQueryContext):
    def __init__(self, this):
        _swig_setattr(self, XmlQueryContext, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlQueryContext, 'thisown', 0)
        _swig_setattr(self, XmlQueryContext,self.__class__,XmlQueryContext)
_dbxml.XmlQueryContext_swigregister(XmlQueryContextPtr)

class XmlResults(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlResults, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlResults, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlResults instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dbxml.delete_XmlResults):
        try:
            if self.thisown: destroy(self)
        except: pass

    def __init__(self, *args):
        _swig_setattr(self, XmlResults, 'this', _dbxml.new_XmlResults(*args))
        _swig_setattr(self, XmlResults, 'thisown', 1)
    def hasNext(*args): return _dbxml.XmlResults_hasNext(*args)
    def hasPrevious(*args): return _dbxml.XmlResults_hasPrevious(*args)
    def next(*args): return _dbxml.XmlResults_next(*args)
    def previous(*args): return _dbxml.XmlResults_previous(*args)
    def peek(*args): return _dbxml.XmlResults_peek(*args)
    def __iter__(self): return self

    def reset(*args): return _dbxml.XmlResults_reset(*args)
    def size(*args): return _dbxml.XmlResults_size(*args)
    def add(*args): return _dbxml.XmlResults_add(*args)

class XmlResultsPtr(XmlResults):
    def __init__(self, this):
        _swig_setattr(self, XmlResults, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlResults, 'thisown', 0)
        _swig_setattr(self, XmlResults,self.__class__,XmlResults)
_dbxml.XmlResults_swigregister(XmlResultsPtr)

class XmlUpdateContext(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlUpdateContext, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlUpdateContext, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlUpdateContext instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, XmlUpdateContext, 'this', _dbxml.new_XmlUpdateContext(*args))
        _swig_setattr(self, XmlUpdateContext, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlUpdateContext):
        try:
            if self.thisown: destroy(self)
        except: pass

    def setApplyChangesToContainers(*args): return _dbxml.XmlUpdateContext_setApplyChangesToContainers(*args)
    def getApplyChangesToContainers(*args): return _dbxml.XmlUpdateContext_getApplyChangesToContainers(*args)

class XmlUpdateContextPtr(XmlUpdateContext):
    def __init__(self, this):
        _swig_setattr(self, XmlUpdateContext, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlUpdateContext, 'thisown', 0)
        _swig_setattr(self, XmlUpdateContext,self.__class__,XmlUpdateContext)
_dbxml.XmlUpdateContext_swigregister(XmlUpdateContextPtr)

class XmlValue(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlValue, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlValue, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlValue instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    ELEMENT_NODE = _dbxml.XmlValue_ELEMENT_NODE
    ATTRIBUTE_NODE = _dbxml.XmlValue_ATTRIBUTE_NODE
    TEXT_NODE = _dbxml.XmlValue_TEXT_NODE
    CDATA_SECTION_NODE = _dbxml.XmlValue_CDATA_SECTION_NODE
    ENTITY_REFERENCE_NODE = _dbxml.XmlValue_ENTITY_REFERENCE_NODE
    ENTITY_NODE = _dbxml.XmlValue_ENTITY_NODE
    PROCESSING_INSTRUCTION_NODE = _dbxml.XmlValue_PROCESSING_INSTRUCTION_NODE
    COMMENT_NODE = _dbxml.XmlValue_COMMENT_NODE
    DOCUMENT_NODE = _dbxml.XmlValue_DOCUMENT_NODE
    DOCUMENT_TYPE_NODE = _dbxml.XmlValue_DOCUMENT_TYPE_NODE
    DOCUMENT_FRAGMENT_NODE = _dbxml.XmlValue_DOCUMENT_FRAGMENT_NODE
    NOTATION_NODE = _dbxml.XmlValue_NOTATION_NODE
    NONE = _dbxml.XmlValue_NONE
    NODE = _dbxml.XmlValue_NODE
    ANY_SIMPLE_TYPE = _dbxml.XmlValue_ANY_SIMPLE_TYPE
    ANY_URI = _dbxml.XmlValue_ANY_URI
    BASE_64_BINARY = _dbxml.XmlValue_BASE_64_BINARY
    BOOLEAN = _dbxml.XmlValue_BOOLEAN
    DATE = _dbxml.XmlValue_DATE
    DATE_TIME = _dbxml.XmlValue_DATE_TIME
    DAY_TIME_DURATION = _dbxml.XmlValue_DAY_TIME_DURATION
    DECIMAL = _dbxml.XmlValue_DECIMAL
    DOUBLE = _dbxml.XmlValue_DOUBLE
    DURATION = _dbxml.XmlValue_DURATION
    FLOAT = _dbxml.XmlValue_FLOAT
    G_DAY = _dbxml.XmlValue_G_DAY
    G_MONTH = _dbxml.XmlValue_G_MONTH
    G_MONTH_DAY = _dbxml.XmlValue_G_MONTH_DAY
    G_YEAR = _dbxml.XmlValue_G_YEAR
    G_YEAR_MONTH = _dbxml.XmlValue_G_YEAR_MONTH
    HEX_BINARY = _dbxml.XmlValue_HEX_BINARY
    NOTATION = _dbxml.XmlValue_NOTATION
    QNAME = _dbxml.XmlValue_QNAME
    STRING = _dbxml.XmlValue_STRING
    TIME = _dbxml.XmlValue_TIME
    YEAR_MONTH_DURATION = _dbxml.XmlValue_YEAR_MONTH_DURATION
    UNTYPED_ATOMIC = _dbxml.XmlValue_UNTYPED_ATOMIC
    BINARY = _dbxml.XmlValue_BINARY
    def __init__(self, *args):
        _swig_setattr(self, XmlValue, 'this', _dbxml.new_XmlValue(*args))
        _swig_setattr(self, XmlValue, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlValue):
        try:
            if self.thisown: destroy(self)
        except: pass

    def getType(*args): return _dbxml.XmlValue_getType(*args)
    def isNull(*args): return _dbxml.XmlValue_isNull(*args)
    def isType(*args): return _dbxml.XmlValue_isType(*args)
    def isNumber(*args): return _dbxml.XmlValue_isNumber(*args)
    def isString(*args): return _dbxml.XmlValue_isString(*args)
    def isBoolean(*args): return _dbxml.XmlValue_isBoolean(*args)
    def isBinary(*args): return _dbxml.XmlValue_isBinary(*args)
    def isNode(*args): return _dbxml.XmlValue_isNode(*args)
    def asNumber(*args): return _dbxml.XmlValue_asNumber(*args)
    def asString(*args): return _dbxml.XmlValue_asString(*args)
    def asBoolean(*args): return _dbxml.XmlValue_asBoolean(*args)
    def asBinary(*args): return _dbxml.XmlValue_asBinary(*args)
    def asDocument(*args): return _dbxml.XmlValue_asDocument(*args)
    def equals(*args): return _dbxml.XmlValue_equals(*args)
    def getNodeName(*args): return _dbxml.XmlValue_getNodeName(*args)
    def getNodeValue(*args): return _dbxml.XmlValue_getNodeValue(*args)
    def getNamespaceURI(*args): return _dbxml.XmlValue_getNamespaceURI(*args)
    def getPrefix(*args): return _dbxml.XmlValue_getPrefix(*args)
    def getLocalName(*args): return _dbxml.XmlValue_getLocalName(*args)
    def getNodeType(*args): return _dbxml.XmlValue_getNodeType(*args)
    def getParentNode(*args): return _dbxml.XmlValue_getParentNode(*args)
    def getFirstChild(*args): return _dbxml.XmlValue_getFirstChild(*args)
    def getLastChild(*args): return _dbxml.XmlValue_getLastChild(*args)
    def getPreviousSibling(*args): return _dbxml.XmlValue_getPreviousSibling(*args)
    def getNextSibling(*args): return _dbxml.XmlValue_getNextSibling(*args)
    def getAttributes(*args): return _dbxml.XmlValue_getAttributes(*args)
    def getOwnerElement(*args): return _dbxml.XmlValue_getOwnerElement(*args)
    __swig_getmethods__["setValue"] = lambda x: _dbxml.XmlValue_setValue
    if _newclass:setValue = staticmethod(_dbxml.XmlValue_setValue)

class XmlValuePtr(XmlValue):
    def __init__(self, this):
        _swig_setattr(self, XmlValue, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlValue, 'thisown', 0)
        _swig_setattr(self, XmlValue,self.__class__,XmlValue)
_dbxml.XmlValue_swigregister(XmlValuePtr)

XmlValue_setValue = _dbxml.XmlValue_setValue

class XmlIndexSpecification(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlIndexSpecification, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlIndexSpecification, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlIndexSpecification instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    UNIQUE_OFF = _dbxml.XmlIndexSpecification_UNIQUE_OFF
    UNIQUE_ON = _dbxml.XmlIndexSpecification_UNIQUE_ON
    PATH_NONE = _dbxml.XmlIndexSpecification_PATH_NONE
    PATH_NODE = _dbxml.XmlIndexSpecification_PATH_NODE
    PATH_EDGE = _dbxml.XmlIndexSpecification_PATH_EDGE
    NODE_NONE = _dbxml.XmlIndexSpecification_NODE_NONE
    NODE_ELEMENT = _dbxml.XmlIndexSpecification_NODE_ELEMENT
    NODE_ATTRIBUTE = _dbxml.XmlIndexSpecification_NODE_ATTRIBUTE
    NODE_METADATA = _dbxml.XmlIndexSpecification_NODE_METADATA
    KEY_NONE = _dbxml.XmlIndexSpecification_KEY_NONE
    KEY_PRESENCE = _dbxml.XmlIndexSpecification_KEY_PRESENCE
    KEY_EQUALITY = _dbxml.XmlIndexSpecification_KEY_EQUALITY
    KEY_SUBSTRING = _dbxml.XmlIndexSpecification_KEY_SUBSTRING
    def __init__(self, *args):
        _swig_setattr(self, XmlIndexSpecification, 'this', _dbxml.new_XmlIndexSpecification(*args))
        _swig_setattr(self, XmlIndexSpecification, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlIndexSpecification):
        try:
            if self.thisown: destroy(self)
        except: pass

    def addIndex(*args): return _dbxml.XmlIndexSpecification_addIndex(*args)
    def deleteIndex(*args): return _dbxml.XmlIndexSpecification_deleteIndex(*args)
    def replaceIndex(*args): return _dbxml.XmlIndexSpecification_replaceIndex(*args)
    def addDefaultIndex(*args): return _dbxml.XmlIndexSpecification_addDefaultIndex(*args)
    def deleteDefaultIndex(*args): return _dbxml.XmlIndexSpecification_deleteDefaultIndex(*args)
    def replaceDefaultIndex(*args): return _dbxml.XmlIndexSpecification_replaceDefaultIndex(*args)
    def reset(*args): return _dbxml.XmlIndexSpecification_reset(*args)
    def find(*args): return _dbxml.XmlIndexSpecification_find(*args)
    def next(*args): return _dbxml.XmlIndexSpecification_next(*args)
    def getDefaultIndex(*args): return _dbxml.XmlIndexSpecification_getDefaultIndex(*args)
    __swig_getmethods__["getValueType"] = lambda x: _dbxml.XmlIndexSpecification_getValueType
    if _newclass:getValueType = staticmethod(_dbxml.XmlIndexSpecification_getValueType)
    def __iter__(self): return self


class XmlIndexSpecificationPtr(XmlIndexSpecification):
    def __init__(self, this):
        _swig_setattr(self, XmlIndexSpecification, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlIndexSpecification, 'thisown', 0)
        _swig_setattr(self, XmlIndexSpecification,self.__class__,XmlIndexSpecification)
_dbxml.XmlIndexSpecification_swigregister(XmlIndexSpecificationPtr)

XmlIndexSpecification_getValueType = _dbxml.XmlIndexSpecification_getValueType

class XmlIndexDeclaration(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlIndexDeclaration, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlIndexDeclaration, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlIndexDeclaration instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dbxml.delete_XmlIndexDeclaration):
        try:
            if self.thisown: destroy(self)
        except: pass

    def get_uri(*args): return _dbxml.XmlIndexDeclaration_get_uri(*args)
    def get_name(*args): return _dbxml.XmlIndexDeclaration_get_name(*args)
    def get_index(*args): return _dbxml.XmlIndexDeclaration_get_index(*args)
    def __init__(self, *args):
        _swig_setattr(self, XmlIndexDeclaration, 'this', _dbxml.new_XmlIndexDeclaration(*args))
        _swig_setattr(self, XmlIndexDeclaration, 'thisown', 1)

class XmlIndexDeclarationPtr(XmlIndexDeclaration):
    def __init__(self, this):
        _swig_setattr(self, XmlIndexDeclaration, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlIndexDeclaration, 'thisown', 0)
        _swig_setattr(self, XmlIndexDeclaration,self.__class__,XmlIndexDeclaration)
_dbxml.XmlIndexDeclaration_swigregister(XmlIndexDeclarationPtr)

class XmlQueryExpression(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlQueryExpression, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlQueryExpression, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlQueryExpression instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, XmlQueryExpression, 'this', _dbxml.new_XmlQueryExpression(*args))
        _swig_setattr(self, XmlQueryExpression, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlQueryExpression):
        try:
            if self.thisown: destroy(self)
        except: pass

    def getQuery(*args): return _dbxml.XmlQueryExpression_getQuery(*args)
    def getQueryPlan(*args): return _dbxml.XmlQueryExpression_getQueryPlan(*args)
    def execute(*args): return _dbxml.XmlQueryExpression_execute(*args)

class XmlQueryExpressionPtr(XmlQueryExpression):
    def __init__(self, this):
        _swig_setattr(self, XmlQueryExpression, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlQueryExpression, 'thisown', 0)
        _swig_setattr(self, XmlQueryExpression,self.__class__,XmlQueryExpression)
_dbxml.XmlQueryExpression_swigregister(XmlQueryExpressionPtr)

class XmlInputStream(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlInputStream, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlInputStream, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlInputStream instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dbxml.delete_XmlInputStream):
        try:
            if self.thisown: destroy(self)
        except: pass

    def curPos(*args): return _dbxml.XmlInputStream_curPos(*args)
    def readBytes(*args): return _dbxml.XmlInputStream_readBytes(*args)
    def freeMemory(*args): return _dbxml.XmlInputStream_freeMemory(*args)
    def __init__(self, *args):
        if self.__class__ == XmlInputStream:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, XmlInputStream, 'this', _dbxml.new_XmlInputStream(*args))
        _swig_setattr(self, XmlInputStream, 'thisown', 1)
    def __disown__(self):
        self.thisown = 0
        _dbxml.disown_XmlInputStream(self)
        return weakref_proxy(self)

class XmlInputStreamPtr(XmlInputStream):
    def __init__(self, this):
        _swig_setattr(self, XmlInputStream, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlInputStream, 'thisown', 0)
        _swig_setattr(self, XmlInputStream,self.__class__,XmlInputStream)
_dbxml.XmlInputStream_swigregister(XmlInputStreamPtr)

class XmlResolver(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlResolver, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlResolver, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlResolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        if self.__class__ == XmlResolver:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, XmlResolver, 'this', _dbxml.new_XmlResolver(*args))
        _swig_setattr(self, XmlResolver, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlResolver):
        try:
            if self.thisown: destroy(self)
        except: pass

    def resolveDocument(*args): return _dbxml.XmlResolver_resolveDocument(*args)
    def resolveCollection(*args): return _dbxml.XmlResolver_resolveCollection(*args)
    def resolveSchema(*args): return _dbxml.XmlResolver_resolveSchema(*args)
    def resolveEntity(*args): return _dbxml.XmlResolver_resolveEntity(*args)
    def __disown__(self):
        self.thisown = 0
        _dbxml.disown_XmlResolver(self)
        return weakref_proxy(self)

class XmlResolverPtr(XmlResolver):
    def __init__(self, this):
        _swig_setattr(self, XmlResolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlResolver, 'thisown', 0)
        _swig_setattr(self, XmlResolver,self.__class__,XmlResolver)
_dbxml.XmlResolver_swigregister(XmlResolverPtr)

class XmlModify(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlModify, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlModify, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlModify instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    Element = _dbxml.XmlModify_Element
    Attribute = _dbxml.XmlModify_Attribute
    Text = _dbxml.XmlModify_Text
    ProcessingInstruction = _dbxml.XmlModify_ProcessingInstruction
    Comment = _dbxml.XmlModify_Comment
    def __init__(self, *args):
        _swig_setattr(self, XmlModify, 'this', _dbxml.new_XmlModify(*args))
        _swig_setattr(self, XmlModify, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlModify):
        try:
            if self.thisown: destroy(self)
        except: pass

    def addInsertBeforeStep(*args): return _dbxml.XmlModify_addInsertBeforeStep(*args)
    def addInsertAfterStep(*args): return _dbxml.XmlModify_addInsertAfterStep(*args)
    def addAppendStep(*args): return _dbxml.XmlModify_addAppendStep(*args)
    def addUpdateStep(*args): return _dbxml.XmlModify_addUpdateStep(*args)
    def addRemoveStep(*args): return _dbxml.XmlModify_addRemoveStep(*args)
    def addRenameStep(*args): return _dbxml.XmlModify_addRenameStep(*args)
    def setNewEncoding(*args): return _dbxml.XmlModify_setNewEncoding(*args)
    def execute(*args): return _dbxml.XmlModify_execute(*args)

class XmlModifyPtr(XmlModify):
    def __init__(self, this):
        _swig_setattr(self, XmlModify, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlModify, 'thisown', 0)
        _swig_setattr(self, XmlModify,self.__class__,XmlModify)
_dbxml.XmlModify_swigregister(XmlModifyPtr)

class XmlTransaction(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlTransaction, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlTransaction, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlTransaction instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dbxml.delete_XmlTransaction):
        try:
            if self.thisown: destroy(self)
        except: pass

    def __init__(self, *args):
        _swig_setattr(self, XmlTransaction, 'this', _dbxml.new_XmlTransaction(*args))
        _swig_setattr(self, XmlTransaction, 'thisown', 1)
    def abort(*args): return _dbxml.XmlTransaction_abort(*args)
    def createChild(*args): return _dbxml.XmlTransaction_createChild(*args)
    def commit(*args): return _dbxml.XmlTransaction_commit(*args)
    def getDbTxn(*args): return _dbxml.XmlTransaction_getDbTxn(*args)

class XmlTransactionPtr(XmlTransaction):
    def __init__(self, this):
        _swig_setattr(self, XmlTransaction, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlTransaction, 'thisown', 0)
        _swig_setattr(self, XmlTransaction,self.__class__,XmlTransaction)
_dbxml.XmlTransaction_swigregister(XmlTransactionPtr)

class XmlStatistics(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlStatistics, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlStatistics, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ XmlStatistics instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, XmlStatistics, 'this', _dbxml.new_XmlStatistics(*args))
        _swig_setattr(self, XmlStatistics, 'thisown', 1)
    def __del__(self, destroy=_dbxml.delete_XmlStatistics):
        try:
            if self.thisown: destroy(self)
        except: pass

    def getNumberOfIndexedKeys(*args): return _dbxml.XmlStatistics_getNumberOfIndexedKeys(*args)
    def getNumberOfUniqueKeys(*args): return _dbxml.XmlStatistics_getNumberOfUniqueKeys(*args)
    def getSumKeyValueSize(*args): return _dbxml.XmlStatistics_getSumKeyValueSize(*args)

class XmlStatisticsPtr(XmlStatistics):
    def __init__(self, this):
        _swig_setattr(self, XmlStatistics, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, XmlStatistics, 'thisown', 0)
        _swig_setattr(self, XmlStatistics,self.__class__,XmlStatistics)
_dbxml.XmlStatistics_swigregister(XmlStatisticsPtr)


