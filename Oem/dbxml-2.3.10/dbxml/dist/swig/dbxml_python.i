/* Python integration added by Michael Droettboom */

%module(directors="1") "dbxml"

// Enable the "director" feature for allowing Java
// implementations of virtual behavior for 2 classes,
// XmlInputStream and XmlResolver
%feature("director") XmlInputStream;
%feature("director") XmlResolver;

// Make sure that a DBEnv passed (python obj) gets an
// extra held reference from XmlManager
%pythonappend XmlManager::XmlManager(DbEnv *dbEnv, u_int32_t flags) %{
        if (len(args) > 1): self.env = args[0]
%}

// Put a reference on XmlManager in XmlContainer objects as they are
// create.  SWIG appears to require a pythonappend directive for 
// *every* overload to make this work
%pythonappend XmlManager::openContainer(XmlTransaction &txn, const std::string &name) %{
        val.mgr = args[0]
%}
%pythonappend XmlManager::openContainer(const std::string &name) %{
        val.mgr = args[0]
%}
%pythonappend XmlManager::openContainer(const std::string &name,u_int32_t flags) %{
        val.mgr = args[0]
%}
%pythonappend XmlManager::openContainer(XmlTransaction &txn, const std::string &name,u_int32_t flags) %{
        val.mgr = args[0]
%}
%pythonappend XmlManager::openContainer(XmlTransaction &txn, const std::string &name,u_int32_t flags, enum XmlContainer::ContainerType type) %{
        val.mgr = args[0]
%}
%pythonappend XmlManager::openContainer(const std::string &name,u_int32_t flags, enum XmlContainer::ContainerType type) %{
        val.mgr = args[0]
%}

// now createContainer
%pythonappend XmlManager::createContainer(XmlTransaction &txn, const std::string &name) %{
        val.mgr = args[0]
%}
%pythonappend XmlManager::createContainer(const std::string &name) %{
        val.mgr = args[0]
%}
%pythonappend XmlManager::createContainer(XmlTransaction &txn, const std::string &name,u_int32_t flags, enum XmlContainer::ContainerType type=XmlContainer::NodeContainer, int mode=0) %{
        val.mgr = args[0]
%}
%pythonappend XmlManager::createContainer(const std::string &name,u_int32_t flags, enum XmlContainer::ContainerType type=XmlContainer::NodeContainer, int mode=0) %{
        val.mgr = args[0]
%}

// TBD -- see if there's a better way to extract
// info from exception
%{
static void throwPyUserException() {
	PyObject *type = 0;
	PyObject *value = 0;
	PyObject *traceback = 0;
	PyErr_Fetch(&type, &value, &traceback);
	if (value) {
		char buf[1024];
		PyObject *str = PyObject_Str(value);
		Py_XINCREF(type);
		PyErr_Clear();
		PyOS_snprintf(buf, sizeof(buf), "Error from Python user code: %s", PyString_AsString(str));
		Py_DECREF(str);
		//PyErr_Print();
		throw XmlException(XmlException::INTERNAL_ERROR,
				   buf);
	}
}
// BDB XML Exceptions
static PyObject *XmlExceptionBase; // base -- all others derive from this
static PyObject *XmlContainerClosed;
static PyObject *XmlContainerExists;
static PyObject *XmlContainerOpen;
static PyObject *XmlDatabaseError;
static PyObject *XmlDocumentNotFound;
static PyObject *XmlInvalidValue;
static PyObject *XmlParserError;
static PyObject *XmlInternalError;
static PyObject *XmlVersionMismatch;
static PyObject *XmlLazyEvaluation;
static PyObject *XmlNoVariableBinding;
static PyObject *XmlUnknownIndex;
static PyObject *XmlQueryEvaluationError;
static PyObject *XmlQueryParserError;
static PyObject *XmlEventError;
static PyObject *XmlContainerNotFound;
static PyObject *XmlUniqueError;
static PyObject *XmlNoMemoryError;
static PyObject *XmlNullPointer;
static PyObject *XmlTransactionError;
static PyObject *XmlOperationTimeout;
static PyObject *XmlOperationInterrupted;

// create the appropriate exception object
static void makeXmlException(const XmlException &xe)
{
	PyObject *errReturn = NULL;
	PyObject *errTuple = NULL;
	switch (xe.getExceptionCode()) {
	case XmlException::INTERNAL_ERROR:
		errReturn = XmlInternalError; break;
	case XmlException::CONTAINER_OPEN:
		errReturn = XmlContainerOpen; break;
	case XmlException::CONTAINER_CLOSED:
		errReturn = XmlContainerClosed; break;
	case XmlException::CONTAINER_EXISTS:
		errReturn = XmlContainerExists; break;
	case XmlException::CONTAINER_NOT_FOUND:
		errReturn = XmlContainerNotFound; break;
	case XmlException::NULL_POINTER:
		errReturn = XmlNullPointer; break;
	case XmlException::INDEXER_PARSER_ERROR:
	case XmlException::DOM_PARSER_ERROR:
		errReturn = XmlParserError; break;
	case XmlException::DATABASE_ERROR:
		errReturn = XmlDatabaseError; break;
	case XmlException::XPATH_PARSER_ERROR:
		errReturn = XmlQueryParserError; break;
	case XmlException::XPATH_EVALUATION_ERROR:
		errReturn = XmlQueryEvaluationError; break;
	case XmlException::NO_VARIABLE_BINDING:
		errReturn = XmlNoVariableBinding; break;
	case XmlException::LAZY_EVALUATION:
		errReturn = XmlLazyEvaluation; break;
	case XmlException::UNKNOWN_INDEX:
		errReturn = XmlUnknownIndex; break;
	case XmlException::DOCUMENT_NOT_FOUND:
		errReturn = XmlDocumentNotFound; break;
	case XmlException::INVALID_VALUE:
		errReturn = XmlInvalidValue; break;
	case XmlException::VERSION_MISMATCH:
		errReturn = XmlVersionMismatch; break;
	case XmlException::TRANSACTION_ERROR:
		errReturn = XmlTransactionError; break;
	case XmlException::UNIQUE_ERROR:
		errReturn = XmlUniqueError; break;
	case XmlException::NO_MEMORY_ERROR:
		errReturn = XmlNoMemoryError; break;
	case XmlException::EVENT_ERROR:
		errReturn = XmlEventError; break;
	case XmlException::OPERATION_INTERRUPTED:
		errReturn = XmlOperationInterrupted; break;
	case XmlException::OPERATION_TIMEOUT:
		errReturn = XmlOperationTimeout; break;
	default:
		errReturn = XmlExceptionBase;
	}
	if (errReturn != NULL) {
		// set the value to an object with the code and text
		XmlException::ExceptionCode ec = xe.getExceptionCode();
		const char *what = xe.what();
		int dberr = xe.getDbErrno();
		errTuple = Py_BuildValue("(is)", ec, what);

		// set the actual error/exception object
		PyErr_SetObject(errReturn, errTuple);
		Py_DECREF(errTuple);

		// set attributes for error code, string, and if
		// a DATABASE_ERROR, a db error code
		// TBD: is Py_DECREF() on values the right thing?
		PyObject *val = Py_BuildValue("i", ec);
		PyObject_SetAttrString(errReturn, "ExceptionCode", val);
		Py_DECREF(val);
		val = Py_BuildValue("s", what);
		PyObject_SetAttrString(errReturn, "What", val);
		Py_DECREF(val);
		// Unconditionally set dberr and query error info,
		// even if it's meaningless
		val = Py_BuildValue("i", dberr);
		PyObject_SetAttrString(errReturn, "DBError", val);
		Py_DECREF(val);

		val = Py_BuildValue("i", xe.getQueryLine());
		PyObject_SetAttrString(errReturn, "QueryLine", val);
		Py_DECREF(val);
		
		val = Py_BuildValue("i", xe.getQueryColumn());
		PyObject_SetAttrString(errReturn, "QueryColumn", val);
		Py_DECREF(val);
	}
}
%}

// if a director call fails (in python), throw
// an XmlException -- most of these calls originate
// in BDB XML proper, not Python.
%feature("director:except") {
	if ($error != NULL) {
		throwPyUserException();
	}
}

// Encapsulate release/acquire of global interpreter lock in
// an exception-safe class
// GMF: Added this for possible use, but it appears that the
// SWIG macros used with the -threads directive work well enough.
// Leave it for now, just in case it comes in handy.
%{
#ifdef SWIG_PYTHON_USE_GIL
class dbxml_releaseGIL {
public:
	dbxml_releaseGIL() {
		_save = PyEval_SaveThread();
	}
	~dbxml_releaseGIL() {
		PyEval_RestoreThread(_save);
	}
	PyThreadState *_save;
};
#else
class dbxml_releaseGIL {
public:
	dbxml_releaseGIL() {}
};
#endif
%}

%exception {
	try {
		$action
	} catch (XmlException &e) {
		SWIG_PYTHON_THREAD_END_ALLOW;
		makeXmlException(e);
		return NULL;
	}
}

// Director calls don't have SWIG_PYTHON_THREAD_START_ALLOW,
// and it doesn't appear that it's possible to change
// %exception on a per-class basis, so enumerate the virtual methods
%exception XmlInputStream::curPos {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlInputStream::readBytes {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveDocument {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveCollection {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveSchema {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveEntity {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveModule {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}
%exception XmlResolver::resolveModuleLocation {
	try {
		$action
	} catch (XmlException &e) {
		makeXmlException(e);
		return NULL;
	}
}

%define PYTHON_ITERATOR(f)
%exception f {
	try {
		$action
		if (!result) {
			SWIG_PYTHON_THREAD_END_ALLOW;
			PyErr_SetString(PyExc_StopIteration,
					"End of results.");
			return NULL;
		}
	} catch (XmlException &e) {
		SWIG_PYTHON_THREAD_END_ALLOW;
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return NULL;
	}
}
%enddef

PYTHON_ITERATOR(XmlResults::next)
PYTHON_ITERATOR(XmlIndexSpecification::next)
PYTHON_ITERATOR(XmlMetaDataIterator::next)

%typemap(in) const unsigned char * {
  char *ret;
  if ($input == Py_None)
     ret = 0;
  else {
      ret = PyString_AsString($input);
      if (ret && (*ret == '\0')) ret = 0;
  }
  $1 = (unsigned char *)ret;
}

%typemap(in) Dbt *, Dbt & (Dbt dbt) {
  dbt.set_data(PyString_AsString($input));
  dbt.set_size(PyString_Size($input));
  $1 = &dbt;
}

%typemap(out) const Dbt * {
  if ($1)
    $result = PyString_FromStringAndSize((char *)$1->get_data(), $1->get_size());
  else
    $result = NULL;
}

%typemap(in) XmlData *, XmlData & (XmlData xml_data) {
	xml_data.set_data(PyString_AsString($input));
	xml_data.set_size(PyString_Size($input));
	$1 = &xml_data;
}

%typemap(out) const XmlData * {
	if ($1) {
		$result =
			PyString_FromStringAndSize((char *)$1->get_data(),
						   $1->get_size());
		delete $1; // done with new XmlData
	} else
		$result = NULL;
}

%{
/* Copied from bsddb3 source. This will break if bsddb3 changes, but
   that would be very unlikely, since this is pretty much the standard
   way to do Python wrappers ;) */

typedef struct {
    PyObject_HEAD
    DB_ENV*     db_env;
    int         flags;             /* saved flags from open() */
    int         closed;
    int         getReturnsNone;
} DBEnvObject;

typedef struct {
    PyObject_HEAD
    DB_TXN*         txn;
} DBTxnObject;
%}

%typemap(in) DbEnv* {
  $1 = ($input == Py_None) ? 
    NULL : DbEnv::wrap_DB_ENV(((DBEnvObject *)$input)->db_env);
}

%typemap(in) DbTxn* {
  $1 = ($input == Py_None) ?
    NULL : DbTxn::wrap_DB_TXN(((DBTxnObject *)$input)->txn);
}

/* 
 * gmf: in order to support function overloads that use DbTxn,
 * which is the subject of a typemap(in), above, a typemap(typecheck)
 * is required; otherwise, the generated code fails.  This is a general
 * rule for SWIG and overloads.  This check always succeeds.  Precedence is
 * set high (low).
 */
%typemap(typecheck, precedence=2000) DbTxn * {
 /* This would be a typecheck for a DbTxn *; just succeed */
 $1 = 1; 
}

%typemap(typecheck, precedence=2000) DbEnv * {
 /* This would be a typecheck for a DbEnv *; just succeed */
 $1 = 1; 
}

%init %{
	static const int dbMajorMin=4;
	static const int dbMinorMin=3;
	int major, minor, patch, dmajor, dminor, dpatch;
	(void) db_version(&major, &minor, &patch);
	if (major < dbMajorMin || minor < dbMinorMin) {
		char buf[512];
		PyOS_snprintf(buf, sizeof(buf), "Version mismatch: Berkeley DB XML %d.%d.%d requires Berkeley DB \n%d.%d or higher; version %d.%d.%d was found\n",
			      DBXML_VERSION_MAJOR, DBXML_VERSION_MINOR,
			      DBXML_VERSION_PATCH,
			      dbMajorMin, dbMinorMin, major, minor, patch);
		PyErr_SetString(PyExc_RuntimeError, buf);
		return;
	}
	(void) DbXml::dbxml_db_version(&dmajor, &dminor, &dpatch);
	// match major and minor numbers
	// TBD: warn if patch level is different
	if (major != dmajor || minor != dminor) {
		char buf[1024];
		PyOS_snprintf(buf, sizeof(buf), "Mismatched Berkeley DB Libraries.  Python linked with %d.%d.%d, and BDB XML is linked with %d.%d.%d\n",
			      major, minor, patch, dmajor, dminor, dpatch);
		PyErr_SetString(PyExc_RuntimeError, buf);
		return;
	}

	// initialize exception objects, and put them in module dictionary
	XmlExceptionBase =
		PyErr_NewException("_dbxml.XmlException", NULL, NULL);
	Py_INCREF(XmlExceptionBase);

#define MAKE_EX_CONSTANT(n) PyDict_SetItemString(d, #n, PyInt_FromLong(XmlException::n))
	MAKE_EX_CONSTANT(INTERNAL_ERROR);
	MAKE_EX_CONSTANT(CONTAINER_OPEN);
	MAKE_EX_CONSTANT(CONTAINER_CLOSED);
	MAKE_EX_CONSTANT(NULL_POINTER);
	MAKE_EX_CONSTANT(INDEXER_PARSER_ERROR);
	MAKE_EX_CONSTANT(DATABASE_ERROR);
	MAKE_EX_CONSTANT(XPATH_PARSER_ERROR);
	MAKE_EX_CONSTANT(DOM_PARSER_ERROR);
	MAKE_EX_CONSTANT(XPATH_EVALUATION_ERROR);
	MAKE_EX_CONSTANT(NO_VARIABLE_BINDING);
	MAKE_EX_CONSTANT(LAZY_EVALUATION);
	MAKE_EX_CONSTANT(DOCUMENT_NOT_FOUND);
	MAKE_EX_CONSTANT(CONTAINER_EXISTS);
	MAKE_EX_CONSTANT(UNKNOWN_INDEX);
	MAKE_EX_CONSTANT(INVALID_VALUE);
	MAKE_EX_CONSTANT(VERSION_MISMATCH);
	MAKE_EX_CONSTANT(EVENT_ERROR);
	MAKE_EX_CONSTANT(CONTAINER_NOT_FOUND);
	MAKE_EX_CONSTANT(TRANSACTION_ERROR);
	MAKE_EX_CONSTANT(UNIQUE_ERROR);
	MAKE_EX_CONSTANT(NO_MEMORY_ERROR);
	MAKE_EX_CONSTANT(OPERATION_INTERRUPTED);
	MAKE_EX_CONSTANT(OPERATION_TIMEOUT);

	PyDict_SetItemString(d, "XmlException_XQUERY_PARSER_ERROR",
			     PyInt_FromLong(XmlException::XPATH_PARSER_ERROR));
	PyDict_SetItemString(d, "XmlException_XQUERY_EVALUATION_ERROR",
			     PyInt_FromLong(XmlException::XPATH_EVALUATION_ERROR));
	PyModule_AddObject(m,"XmlException", XmlExceptionBase);
	// exceptions derive from XmlException
#define MAKE_EX(name) name=PyErr_NewException("_dbxml." #name, XmlExceptionBase, NULL); \
	PyModule_AddObject(m,#name, name);
	MAKE_EX(XmlContainerClosed);
	MAKE_EX(XmlContainerExists);
	MAKE_EX(XmlContainerOpen);
	MAKE_EX(XmlDatabaseError);
	MAKE_EX(XmlDocumentNotFound);
	MAKE_EX(XmlInvalidValue);
	MAKE_EX(XmlParserError);
	MAKE_EX(XmlInternalError);
	MAKE_EX(XmlVersionMismatch);
	MAKE_EX(XmlLazyEvaluation);
	MAKE_EX(XmlNoVariableBinding);
	MAKE_EX(XmlUnknownIndex);
	MAKE_EX(XmlQueryEvaluationError);
	MAKE_EX(XmlQueryParserError);
	MAKE_EX(XmlEventError);
	MAKE_EX(XmlContainerNotFound);
	MAKE_EX(XmlUniqueError);
	MAKE_EX(XmlNoMemoryError);
	MAKE_EX(XmlNullPointer);
	MAKE_EX(XmlTransactionError);
	MAKE_EX(XmlOperationInterrupted);
	MAKE_EX(XmlOperationTimeout);
	
%}

