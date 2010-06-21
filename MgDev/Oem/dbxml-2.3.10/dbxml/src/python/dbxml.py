# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _dbxml
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


try:
    import weakref
    weakref_proxy = weakref.proxy
except:
    weakref_proxy = lambda x: x


DB_CREATE = _dbxml.DB_CREATE
DB_READ_UNCOMMITTED = _dbxml.DB_READ_UNCOMMITTED
DB_DIRTY_READ = _dbxml.DB_DIRTY_READ
DB_EXCL = _dbxml.DB_EXCL
DB_NOMMAP = _dbxml.DB_NOMMAP
DB_RDONLY = _dbxml.DB_RDONLY
DB_THREAD = _dbxml.DB_THREAD
DB_READ_COMMITTED = _dbxml.DB_READ_COMMITTED
DB_DEGREE_2 = _dbxml.DB_DEGREE_2
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
DBXML_CHKSUM = _dbxml.DBXML_CHKSUM
DBXML_ENCRYPT = _dbxml.DBXML_ENCRYPT
DBXML_INDEX_NODES = _dbxml.DBXML_INDEX_NODES
DBXML_NO_INDEX_NODES = _dbxml.DBXML_NO_INDEX_NODES
DBXML_REVERSE_ORDER = _dbxml.DBXML_REVERSE_ORDER
DBXML_INDEX_VALUES = _dbxml.DBXML_INDEX_VALUES
DBXML_CACHE_DOCUMENTS = _dbxml.DBXML_CACHE_DOCUMENTS
DBXML_LAZY_DOCS = _dbxml.DBXML_LAZY_DOCS
DBXML_WELL_FORMED_ONLY = _dbxml.DBXML_WELL_FORMED_ONLY
DBXML_GEN_NAME = _dbxml.DBXML_GEN_NAME
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
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _dbxml.new_XmlManager(*args)
        try: self.this.append(this)
        except: self.this = this
        
        if (len(args) > 1): self.env = args[0]


    __swig_destroy__ = _dbxml.delete_XmlManager
    __del__ = lambda self : None;
    def setDefaultContainerFlags(*args): return _dbxml.XmlManager_setDefaultContainerFlags(*args)
    def getDefaultContainerFlags(*args): return _dbxml.XmlManager_getDefaultContainerFlags(*args)
    def getFlags(*args): return _dbxml.XmlManager_getFlags(*args)
    def setDefaultPageSize(*args): return _dbxml.XmlManager_setDefaultPageSize(*args)
    def getDefaultPageSize(*args): return _dbxml.XmlManager_getDefaultPageSize(*args)
    def setDefaultSequenceIncrement(*args): return _dbxml.XmlManager_setDefaultSequenceIncrement(*args)
    def getDefaultSequenceIncrement(*args): return _dbxml.XmlManager_getDefaultSequenceIncrement(*args)
    def setDefaultContainerType(*args): return _dbxml.XmlManager_setDefaultContainerType(*args)
    def getDefaultContainerType(*args): return _dbxml.XmlManager_getDefaultContainerType(*args)
    def getDbEnv(*args): return _dbxml.XmlManager_getDbEnv(*args)
    def getHome(*args): return _dbxml.XmlManager_getHome(*args)
    def registerResolver(*args): return _dbxml.XmlManager_registerResolver(*args)
    def getImplicitTimezone(*args): return _dbxml.XmlManager_getImplicitTimezone(*args)
    def setImplicitTimezone(*args): return _dbxml.XmlManager_setImplicitTimezone(*args)
    def existsContainer(*args): return _dbxml.XmlManager_existsContainer(*args)
    def removeContainer(*args): return _dbxml.XmlManager_removeContainer(*args)
    def renameContainer(*args): return _dbxml.XmlManager_renameContainer(*args)
    def upgradeContainer(*args): return _dbxml.XmlManager_upgradeContainer(*args)
    def compactContainer(*args): return _dbxml.XmlManager_compactContainer(*args)
    def truncateContainer(*args): return _dbxml.XmlManager_truncateContainer(*args)
    def reindexContainer(*args): return _dbxml.XmlManager_reindexContainer(*args)
    def createStdInInputStream(*args): return _dbxml.XmlManager_createStdInInputStream(*args)
    def createLocalFileInputStream(*args): return _dbxml.XmlManager_createLocalFileInputStream(*args)
    def createMemBufInputStream(*args): return _dbxml.XmlManager_createMemBufInputStream(*args)
    def createURLInputStream(*args): return _dbxml.XmlManager_createURLInputStream(*args)
    def createContainer(*args):
        val = _dbxml.XmlManager_createContainer(*args)
        
        val.mgr = args[0]

        return val

    def openContainer(*args):
        val = _dbxml.XmlManager_openContainer(*args)
        
        val.mgr = args[0]

        return val

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
XmlManager_swigregister = _dbxml.XmlManager_swigregister
XmlManager_swigregister(XmlManager)
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
    __repr__ = _swig_repr
    NONE = _dbxml.XmlIndexLookup_NONE
    EQ = _dbxml.XmlIndexLookup_EQ
    GT = _dbxml.XmlIndexLookup_GT
    GTE = _dbxml.XmlIndexLookup_GTE
    LT = _dbxml.XmlIndexLookup_LT
    LTE = _dbxml.XmlIndexLookup_LTE
    def __init__(self, *args): 
        this = _dbxml.new_XmlIndexLookup(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlIndexLookup
    __del__ = lambda self : None;
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
XmlIndexLookup_swigregister = _dbxml.XmlIndexLookup_swigregister
XmlIndexLookup_swigregister(XmlIndexLookup)

class XmlContainer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlContainer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlContainer, name)
    __repr__ = _swig_repr
    WholedocContainer = _dbxml.XmlContainer_WholedocContainer
    NodeContainer = _dbxml.XmlContainer_NodeContainer
    def __init__(self, *args): 
        this = _dbxml.new_XmlContainer(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlContainer
    __del__ = lambda self : None;
    def sync(*args): return _dbxml.XmlContainer_sync(*args)
    def close(*args): return _dbxml.XmlContainer_close(*args)
    def getFlags(*args): return _dbxml.XmlContainer_getFlags(*args)
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
    def putDocumentAsEventWriter(*args): return _dbxml.XmlContainer_putDocumentAsEventWriter(*args)
    def deleteDocument(*args): return _dbxml.XmlContainer_deleteDocument(*args)
    def updateDocument(*args): return _dbxml.XmlContainer_updateDocument(*args)
    def getNumDocuments(*args): return _dbxml.XmlContainer_getNumDocuments(*args)
    def getIndexSpecification(*args): return _dbxml.XmlContainer_getIndexSpecification(*args)
    def getDocument(*args): return _dbxml.XmlContainer_getDocument(*args)
    def getNode(*args): return _dbxml.XmlContainer_getNode(*args)
    def getAllDocuments(*args): return _dbxml.XmlContainer_getAllDocuments(*args)
    def lookupIndex(*args): return _dbxml.XmlContainer_lookupIndex(*args)
    def lookupStatistics(*args): return _dbxml.XmlContainer_lookupStatistics(*args)
XmlContainer_swigregister = _dbxml.XmlContainer_swigregister
XmlContainer_swigregister(XmlContainer)

class XmlDocument(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlDocument, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlDocument, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _dbxml.new_XmlDocument(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlDocument
    __del__ = lambda self : None;
    def setName(*args): return _dbxml.XmlDocument_setName(*args)
    def getName(*args): return _dbxml.XmlDocument_getName(*args)
    def setContent(*args): return _dbxml.XmlDocument_setContent(*args)
    def getContentAsXmlInputStream(*args): return _dbxml.XmlDocument_getContentAsXmlInputStream(*args)
    def setContentAsXmlInputStream(*args): return _dbxml.XmlDocument_setContentAsXmlInputStream(*args)
    def getContentAsEventReader(*args): return _dbxml.XmlDocument_getContentAsEventReader(*args)
    def setContentAsEventReader(*args): return _dbxml.XmlDocument_setContentAsEventReader(*args)
    def getContentAsEventWriter(*args): return _dbxml.XmlDocument_getContentAsEventWriter(*args)
    def fetchAllData(*args): return _dbxml.XmlDocument_fetchAllData(*args)
    def removeMetaData(*args): return _dbxml.XmlDocument_removeMetaData(*args)
    def setMetaData(*args): return _dbxml.XmlDocument_setMetaData(*args)
    def getContentAsString(*args): return _dbxml.XmlDocument_getContentAsString(*args)
    def getContent(*args): return _dbxml.XmlDocument_getContent(*args)
    def getMetaData(*args): return _dbxml.XmlDocument_getMetaData(*args)
    def getMetaDataIterator(*args): return _dbxml.XmlDocument_getMetaDataIterator(*args)
    def equals(*args): return _dbxml.XmlDocument_equals(*args)
XmlDocument_swigregister = _dbxml.XmlDocument_swigregister
XmlDocument_swigregister(XmlDocument)

class XmlMetaDataIterator(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlMetaDataIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlMetaDataIterator, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _dbxml.new_XmlMetaDataIterator(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlMetaDataIterator
    __del__ = lambda self : None;
    def reset(*args): return _dbxml.XmlMetaDataIterator_reset(*args)
    def next(*args): return _dbxml.XmlMetaDataIterator_next(*args)
    def __iter__(self): return self

XmlMetaDataIterator_swigregister = _dbxml.XmlMetaDataIterator_swigregister
XmlMetaDataIterator_swigregister(XmlMetaDataIterator)

class XmlMetaData(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlMetaData, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlMetaData, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _dbxml.delete_XmlMetaData
    __del__ = lambda self : None;
    def get_uri(*args): return _dbxml.XmlMetaData_get_uri(*args)
    def get_name(*args): return _dbxml.XmlMetaData_get_name(*args)
    def get_value(*args): return _dbxml.XmlMetaData_get_value(*args)
    def __init__(self, *args): 
        this = _dbxml.new_XmlMetaData(*args)
        try: self.this.append(this)
        except: self.this = this
XmlMetaData_swigregister = _dbxml.XmlMetaData_swigregister
XmlMetaData_swigregister(XmlMetaData)

class XmlQueryContext(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlQueryContext, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlQueryContext, name)
    __repr__ = _swig_repr
    DeadValues = _dbxml.XmlQueryContext_DeadValues
    LiveValues = _dbxml.XmlQueryContext_LiveValues
    Eager = _dbxml.XmlQueryContext_Eager
    Lazy = _dbxml.XmlQueryContext_Lazy
    def __init__(self, *args): 
        this = _dbxml.new_XmlQueryContext(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlQueryContext
    __del__ = lambda self : None;
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
    def interruptQuery(*args): return _dbxml.XmlQueryContext_interruptQuery(*args)
    def setQueryTimeoutSeconds(*args): return _dbxml.XmlQueryContext_setQueryTimeoutSeconds(*args)
    def getQueryTimeoutSeconds(*args): return _dbxml.XmlQueryContext_getQueryTimeoutSeconds(*args)
    def getVariableValue(*args): return _dbxml.XmlQueryContext_getVariableValue(*args)
    def getVariableValues(*args): return _dbxml.XmlQueryContext_getVariableValues(*args)
XmlQueryContext_swigregister = _dbxml.XmlQueryContext_swigregister
XmlQueryContext_swigregister(XmlQueryContext)

class XmlResults(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlResults, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlResults, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _dbxml.delete_XmlResults
    __del__ = lambda self : None;
    def __init__(self, *args): 
        this = _dbxml.new_XmlResults(*args)
        try: self.this.append(this)
        except: self.this = this
    def hasNext(*args): return _dbxml.XmlResults_hasNext(*args)
    def hasPrevious(*args): return _dbxml.XmlResults_hasPrevious(*args)
    def getEvaluationType(*args): return _dbxml.XmlResults_getEvaluationType(*args)
    def next(*args): return _dbxml.XmlResults_next(*args)
    def previous(*args): return _dbxml.XmlResults_previous(*args)
    def peek(*args): return _dbxml.XmlResults_peek(*args)
    def __iter__(self): return self

    def reset(*args): return _dbxml.XmlResults_reset(*args)
    def size(*args): return _dbxml.XmlResults_size(*args)
    def add(*args): return _dbxml.XmlResults_add(*args)
XmlResults_swigregister = _dbxml.XmlResults_swigregister
XmlResults_swigregister(XmlResults)

class XmlUpdateContext(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlUpdateContext, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlUpdateContext, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _dbxml.new_XmlUpdateContext(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlUpdateContext
    __del__ = lambda self : None;
    def setApplyChangesToContainers(*args): return _dbxml.XmlUpdateContext_setApplyChangesToContainers(*args)
    def getApplyChangesToContainers(*args): return _dbxml.XmlUpdateContext_getApplyChangesToContainers(*args)
XmlUpdateContext_swigregister = _dbxml.XmlUpdateContext_swigregister
XmlUpdateContext_swigregister(XmlUpdateContext)

class XmlValue(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlValue, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlValue, name)
    __repr__ = _swig_repr
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
        this = _dbxml.new_XmlValue(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlValue
    __del__ = lambda self : None;
    def getType(*args): return _dbxml.XmlValue_getType(*args)
    def getTypeURI(*args): return _dbxml.XmlValue_getTypeURI(*args)
    def getTypeName(*args): return _dbxml.XmlValue_getTypeName(*args)
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
    def asEventReader(*args): return _dbxml.XmlValue_asEventReader(*args)
    def getNodeHandle(*args): return _dbxml.XmlValue_getNodeHandle(*args)
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
XmlValue_swigregister = _dbxml.XmlValue_swigregister
XmlValue_swigregister(XmlValue)
XmlValue_setValue = _dbxml.XmlValue_setValue

class XmlIndexSpecification(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlIndexSpecification, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlIndexSpecification, name)
    __repr__ = _swig_repr
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
        this = _dbxml.new_XmlIndexSpecification(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlIndexSpecification
    __del__ = lambda self : None;
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

XmlIndexSpecification_swigregister = _dbxml.XmlIndexSpecification_swigregister
XmlIndexSpecification_swigregister(XmlIndexSpecification)
XmlIndexSpecification_getValueType = _dbxml.XmlIndexSpecification_getValueType

class XmlIndexDeclaration(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlIndexDeclaration, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlIndexDeclaration, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _dbxml.delete_XmlIndexDeclaration
    __del__ = lambda self : None;
    def get_uri(*args): return _dbxml.XmlIndexDeclaration_get_uri(*args)
    def get_name(*args): return _dbxml.XmlIndexDeclaration_get_name(*args)
    def get_index(*args): return _dbxml.XmlIndexDeclaration_get_index(*args)
    def __init__(self, *args): 
        this = _dbxml.new_XmlIndexDeclaration(*args)
        try: self.this.append(this)
        except: self.this = this
XmlIndexDeclaration_swigregister = _dbxml.XmlIndexDeclaration_swigregister
XmlIndexDeclaration_swigregister(XmlIndexDeclaration)

class XmlQueryExpression(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlQueryExpression, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlQueryExpression, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _dbxml.new_XmlQueryExpression(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlQueryExpression
    __del__ = lambda self : None;
    def getQuery(*args): return _dbxml.XmlQueryExpression_getQuery(*args)
    def getQueryPlan(*args): return _dbxml.XmlQueryExpression_getQueryPlan(*args)
    def execute(*args): return _dbxml.XmlQueryExpression_execute(*args)
XmlQueryExpression_swigregister = _dbxml.XmlQueryExpression_swigregister
XmlQueryExpression_swigregister(XmlQueryExpression)

class XmlInputStream(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlInputStream, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlInputStream, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _dbxml.delete_XmlInputStream
    __del__ = lambda self : None;
    def curPos(*args): return _dbxml.XmlInputStream_curPos(*args)
    def readBytes(*args): return _dbxml.XmlInputStream_readBytes(*args)
    def freeMemory(*args): return _dbxml.XmlInputStream_freeMemory(*args)
    def __init__(self, *args): 
        if self.__class__ == XmlInputStream:
            args = (None,) + args
        else:
            args = (self,) + args
        this = _dbxml.new_XmlInputStream(*args)
        try: self.this.append(this)
        except: self.this = this
    def __disown__(self):
        self.this.disown()
        _dbxml.disown_XmlInputStream(self)
        return weakref_proxy(self)
XmlInputStream_swigregister = _dbxml.XmlInputStream_swigregister
XmlInputStream_swigregister(XmlInputStream)

class XmlResolver(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlResolver, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlResolver, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        if self.__class__ == XmlResolver:
            args = (None,) + args
        else:
            args = (self,) + args
        this = _dbxml.new_XmlResolver(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlResolver
    __del__ = lambda self : None;
    def resolveDocument(*args): return _dbxml.XmlResolver_resolveDocument(*args)
    def resolveCollection(*args): return _dbxml.XmlResolver_resolveCollection(*args)
    def resolveSchema(*args): return _dbxml.XmlResolver_resolveSchema(*args)
    def resolveEntity(*args): return _dbxml.XmlResolver_resolveEntity(*args)
    def resolveModuleLocation(*args): return _dbxml.XmlResolver_resolveModuleLocation(*args)
    def resolveModule(*args): return _dbxml.XmlResolver_resolveModule(*args)
    def __disown__(self):
        self.this.disown()
        _dbxml.disown_XmlResolver(self)
        return weakref_proxy(self)
XmlResolver_swigregister = _dbxml.XmlResolver_swigregister
XmlResolver_swigregister(XmlResolver)

class XmlModify(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlModify, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlModify, name)
    __repr__ = _swig_repr
    Element = _dbxml.XmlModify_Element
    Attribute = _dbxml.XmlModify_Attribute
    Text = _dbxml.XmlModify_Text
    ProcessingInstruction = _dbxml.XmlModify_ProcessingInstruction
    Comment = _dbxml.XmlModify_Comment
    def __init__(self, *args): 
        this = _dbxml.new_XmlModify(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlModify
    __del__ = lambda self : None;
    def addInsertBeforeStep(*args): return _dbxml.XmlModify_addInsertBeforeStep(*args)
    def addInsertAfterStep(*args): return _dbxml.XmlModify_addInsertAfterStep(*args)
    def addAppendStep(*args): return _dbxml.XmlModify_addAppendStep(*args)
    def addUpdateStep(*args): return _dbxml.XmlModify_addUpdateStep(*args)
    def addRemoveStep(*args): return _dbxml.XmlModify_addRemoveStep(*args)
    def addRenameStep(*args): return _dbxml.XmlModify_addRenameStep(*args)
    def setNewEncoding(*args): return _dbxml.XmlModify_setNewEncoding(*args)
    def execute(*args): return _dbxml.XmlModify_execute(*args)
XmlModify_swigregister = _dbxml.XmlModify_swigregister
XmlModify_swigregister(XmlModify)

class XmlTransaction(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlTransaction, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlTransaction, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _dbxml.delete_XmlTransaction
    __del__ = lambda self : None;
    def __init__(self, *args): 
        this = _dbxml.new_XmlTransaction(*args)
        try: self.this.append(this)
        except: self.this = this
    def abort(*args): return _dbxml.XmlTransaction_abort(*args)
    def createChild(*args): return _dbxml.XmlTransaction_createChild(*args)
    def commit(*args): return _dbxml.XmlTransaction_commit(*args)
    def getDbTxn(*args): return _dbxml.XmlTransaction_getDbTxn(*args)
XmlTransaction_swigregister = _dbxml.XmlTransaction_swigregister
XmlTransaction_swigregister(XmlTransaction)

class XmlStatistics(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlStatistics, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlStatistics, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _dbxml.new_XmlStatistics(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlStatistics
    __del__ = lambda self : None;
    def getNumberOfIndexedKeys(*args): return _dbxml.XmlStatistics_getNumberOfIndexedKeys(*args)
    def getNumberOfUniqueKeys(*args): return _dbxml.XmlStatistics_getNumberOfUniqueKeys(*args)
    def getSumKeyValueSize(*args): return _dbxml.XmlStatistics_getSumKeyValueSize(*args)
XmlStatistics_swigregister = _dbxml.XmlStatistics_swigregister
XmlStatistics_swigregister(XmlStatistics)

class XmlEventReader(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlEventReader, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlEventReader, name)
    __repr__ = _swig_repr
    StartElement = _dbxml.XmlEventReader_StartElement
    EndElement = _dbxml.XmlEventReader_EndElement
    Characters = _dbxml.XmlEventReader_Characters
    CDATA = _dbxml.XmlEventReader_CDATA
    Comment = _dbxml.XmlEventReader_Comment
    Whitespace = _dbxml.XmlEventReader_Whitespace
    StartDocument = _dbxml.XmlEventReader_StartDocument
    EndDocument = _dbxml.XmlEventReader_EndDocument
    StartEntityReference = _dbxml.XmlEventReader_StartEntityReference
    EndEntityReference = _dbxml.XmlEventReader_EndEntityReference
    ProcessingInstruction = _dbxml.XmlEventReader_ProcessingInstruction
    DTD = _dbxml.XmlEventReader_DTD
    __swig_destroy__ = _dbxml.delete_XmlEventReader
    __del__ = lambda self : None;
    def close(*args): return _dbxml.XmlEventReader_close(*args)
    def setReportEntityInfo(*args): return _dbxml.XmlEventReader_setReportEntityInfo(*args)
    def getReportEntityInfo(*args): return _dbxml.XmlEventReader_getReportEntityInfo(*args)
    def setExpandEntities(*args): return _dbxml.XmlEventReader_setExpandEntities(*args)
    def getExpandEntities(*args): return _dbxml.XmlEventReader_getExpandEntities(*args)
    def next(*args): return _dbxml.XmlEventReader_next(*args)
    def nextTag(*args): return _dbxml.XmlEventReader_nextTag(*args)
    def hasNext(*args): return _dbxml.XmlEventReader_hasNext(*args)
    def getEventType(*args): return _dbxml.XmlEventReader_getEventType(*args)
    def getNamespaceURI(*args): return _dbxml.XmlEventReader_getNamespaceURI(*args)
    def getLocalName(*args): return _dbxml.XmlEventReader_getLocalName(*args)
    def getPrefix(*args): return _dbxml.XmlEventReader_getPrefix(*args)
    def getValue(*args): return _dbxml.XmlEventReader_getValue(*args)
    def getValueLength(*args): return _dbxml.XmlEventReader_getValueLength(*args)
    def getAttributeCount(*args): return _dbxml.XmlEventReader_getAttributeCount(*args)
    def isAttributeSpecified(*args): return _dbxml.XmlEventReader_isAttributeSpecified(*args)
    def getAttributeLocalName(*args): return _dbxml.XmlEventReader_getAttributeLocalName(*args)
    def getAttributeNamespaceURI(*args): return _dbxml.XmlEventReader_getAttributeNamespaceURI(*args)
    def getAttributePrefix(*args): return _dbxml.XmlEventReader_getAttributePrefix(*args)
    def getAttributeValue(*args): return _dbxml.XmlEventReader_getAttributeValue(*args)
    def getEncoding(*args): return _dbxml.XmlEventReader_getEncoding(*args)
    def getVersion(*args): return _dbxml.XmlEventReader_getVersion(*args)
    def getSystemId(*args): return _dbxml.XmlEventReader_getSystemId(*args)
    def isStandalone(*args): return _dbxml.XmlEventReader_isStandalone(*args)
    def standaloneSet(*args): return _dbxml.XmlEventReader_standaloneSet(*args)
    def encodingSet(*args): return _dbxml.XmlEventReader_encodingSet(*args)
    def hasEntityEscapeInfo(*args): return _dbxml.XmlEventReader_hasEntityEscapeInfo(*args)
    def needsEntityEscape(*args): return _dbxml.XmlEventReader_needsEntityEscape(*args)
    def hasEmptyElementInfo(*args): return _dbxml.XmlEventReader_hasEmptyElementInfo(*args)
    def isEmptyElement(*args): return _dbxml.XmlEventReader_isEmptyElement(*args)
    def isWhiteSpace(*args): return _dbxml.XmlEventReader_isWhiteSpace(*args)
    def __init__(self, *args): 
        this = _dbxml.new_XmlEventReader(*args)
        try: self.this.append(this)
        except: self.this = this
XmlEventReader_swigregister = _dbxml.XmlEventReader_swigregister
XmlEventReader_swigregister(XmlEventReader)

class XmlEventWriter(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlEventWriter, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlEventWriter, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _dbxml.delete_XmlEventWriter
    __del__ = lambda self : None;
    def close(*args): return _dbxml.XmlEventWriter_close(*args)
    def writeAttribute(*args): return _dbxml.XmlEventWriter_writeAttribute(*args)
    def writeText(*args): return _dbxml.XmlEventWriter_writeText(*args)
    def writeProcessingInstruction(*args): return _dbxml.XmlEventWriter_writeProcessingInstruction(*args)
    def writeStartElement(*args): return _dbxml.XmlEventWriter_writeStartElement(*args)
    def writeEndElement(*args): return _dbxml.XmlEventWriter_writeEndElement(*args)
    def writeDTD(*args): return _dbxml.XmlEventWriter_writeDTD(*args)
    def writeStartDocument(*args): return _dbxml.XmlEventWriter_writeStartDocument(*args)
    def writeEndDocument(*args): return _dbxml.XmlEventWriter_writeEndDocument(*args)
    def writeStartEntity(*args): return _dbxml.XmlEventWriter_writeStartEntity(*args)
    def writeEndEntity(*args): return _dbxml.XmlEventWriter_writeEndEntity(*args)
    def __init__(self, *args): 
        this = _dbxml.new_XmlEventWriter(*args)
        try: self.this.append(this)
        except: self.this = this
XmlEventWriter_swigregister = _dbxml.XmlEventWriter_swigregister
XmlEventWriter_swigregister(XmlEventWriter)

class XmlEventReaderToWriter(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XmlEventReaderToWriter, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XmlEventReaderToWriter, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _dbxml.new_XmlEventReaderToWriter(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _dbxml.delete_XmlEventReaderToWriter
    __del__ = lambda self : None;
    def start(*args): return _dbxml.XmlEventReaderToWriter_start(*args)
XmlEventReaderToWriter_swigregister = _dbxml.XmlEventReaderToWriter_swigregister
XmlEventReaderToWriter_swigregister(XmlEventReaderToWriter)



