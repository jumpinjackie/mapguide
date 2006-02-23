# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000-2005
#	Sleepycat Software.  All rights reserved.
#
# $Id: globals.tcl,v 1.32 2005/09/16 19:44:41 gmf Exp $
#
# Shared global variables and constants used in dbxml tests.

global testdir
set testdir ./TESTDIR

global dict
global util_path xmlutil_path
global encrypt passwd

global is_hp_test is_qnx_test is_windows_test is_je_test

global dbxml_err dbxml_pending

# Constants / enumerated values
global DB_CREATE DB_DIRTY_READ DB_EXCL DB_NOMMAP
global DB_SALVAGE DB_AGGRESSIVE
global DB_RDONLY DB_THREAD
global DB_INIT_LOCK DB_INIT_LOG DB_INIT_MPOOL DB_INIT_TXN

global DBXML_GEN_NAME DBXML_LAZY_DOCS DBXML_INDEX_NODES DBXML_NO_INDEX_NODES DBXML_TRANSACTIONAL DBXML_ADOPT_DBENV
global DBXML_ALLOW_VALIDATION DBXML_ALLOW_AUTO_OPEN DBXML_ALLOW_EXTERNAL_ACCESS
global DBXML_REVERSE_ORDER DBXML_INDEX_VALUES
global XmlIndexLookup_NONE XmlIndexLookup_EQ XmlIndexLookup_GT XmlIndexLookup_GTE XmlIndexLookup_LT XmlIndexLookup_LTE
global LEVEL_NONE LEVEL_DEBUG LEVEL_INFO LEVEL_WARNING LEVEL_ERROR LEVEL_ALL

global CATEGORY_NONE CATEGORY_INDEXER CATEGORY_QUERY
global CATEGORY_OPTIMIZER CATEGORY_DICTIONARY CATEGORY_CONTAINER
global CATEGORY_ALL CATEGORY_NODESTORE CATEGORY_MANAGER

global XmlDocument_NONE XmlDocument_UTF8 XmlDocument_UTF16
global XmlDocument_UCS4 XmlDocument_UCS4_ASCII XmlDocument_EBCDIC

global XmlQueryContext_DeadValues XmlQueryContext_LiveValues
global XmlQueryContext_Eager XmlQueryContext_Lazy

global XmlContainer_WholedocContainer XmlContainer_NodeContainer
global metaDataNamespace_uri metaDataName_name

global XmlValue_NONE 
global XmlValue_DOCUMENT XmlValue_NODE XmlValue_VARIABLE
global XmlValue_ANY_URI XmlValue_BASE_64_BINARY XmlValue_BOOLEAN XmlValue_DATE XmlValue_DATE_TIME XmlValue_DAY_TIME_DURATION XmlValue_DECIMAL XmlValue_DOUBLE XmlValue_DURATION XmlValue_FLOAT XmlValue_G_DAY XmlValue_G_MONTH XmlValue_G_MONTH_DAY XmlValue_G_YEAR XmlValue_G_YEAR_MONTH XmlValue_HEX_BINARY XmlValue_NOTATION XmlValue_QNAME XmlValue_STRING XmlValue_TIME XmlValue_YEAR_MONTH_DURATION XmlValue_BINARY

global XmlModify_InsertAfter XmlModify_InsertBefore XmlModify_Remove XmlModify_Rename XmlModify_Append XmlModify_Update
global XmlModify_Element XmlModify_Attribute XmlModify_ProcessingInstruction  XmlModify_Text  XmlModify_Comment XmlModify_None

# container type - assign at the start of each group of tests
global global_container_type
global global_container_flags

# keywords in data-driven tests
global key_groupid
set key_groupid TEST
global key_grouptitle
set key_grouptitle TITLE
global key_index
set key_index INDEX
global key_variable
set key_variable VARIABLE
global key_returntype
set key_returntype RETURNTYPE
global key_evalulationtype
set key_evalulationtype EVALUATIONTYPE
global key_skip
set key_skip SKIP
