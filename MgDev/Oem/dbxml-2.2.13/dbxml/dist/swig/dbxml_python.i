/* Python integration added by Michael Droettboom */

%module(directors="1") "dbxml"

// Enable the "director" feature for allowing Java
// implementations of virtual behavior for 2 classes,
// XmlInputStream and XmlResolver
%feature("director") XmlInputStream;
%feature("director") XmlResolver;

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
%}

// if a director call fails (in python), throw
// an XmlException -- most of these calls originate
// in BDB XML proper, not Python.
%feature("director:except") {
	if ($error != NULL) {
		throwPyUserException();
	}
}

%exception {
	try {
		$action
	} catch (XmlException &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return NULL;
	}
}

%define PYTHON_ITERATOR(f)
%exception f {
	try {
		$action
		if (!result) {
			PyErr_SetString(PyExc_StopIteration,
					"End of results.");
			return NULL;
		}
	} catch (XmlException &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return NULL;
	}
}
%enddef

PYTHON_ITERATOR(XmlResults::next)
PYTHON_ITERATOR(XmlIndexSpecification::next)
PYTHON_ITERATOR(XmlMetaDataIterator::next)

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
	int major, minor, patch;
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
%}
