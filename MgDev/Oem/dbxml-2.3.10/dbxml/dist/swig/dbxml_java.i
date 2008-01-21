/* -*-java-*- */
%module(directors = "1") dbxml_java

%include "various.i"

// Enable the "director" feature for allowing Java
// implementations of virtual behavior for 2 classes,
// XmlInputStream and XmlResolver.
// NOTE: there are issues getting it to work properly
// with XmlInputStream, because of char * -> byte[]
// mappings.  This will need to be fixed later.  For now,
// it's not possible to implement XmlInputStream subclasses in Java
%feature("director") XmlResolver;

%{
class JavaException {};

class XmlInputStream_ReadBytes_InOutByteArray {
public:
	XmlInputStream_ReadBytes_InOutByteArray(JNIEnv *jenv, jbyteArray &jtoFill, char *toFill, unsigned int &result)
		: jenv_(jenv), jtoFill_(jtoFill), toFill_(toFill), result_(result)
	{
	}
	~XmlInputStream_ReadBytes_InOutByteArray() {
		if(jenv_->ExceptionOccurred()) {
			// This is caught in the exception handling routines
			throw JavaException();
		} else {
			jenv_->GetByteArrayRegion(jtoFill_, (jsize)0, (jsize)result_, (jbyte*)toFill_);
		}
	}
private:
	JNIEnv *jenv_;
	jbyteArray &jtoFill_;
	char *toFill_;
	unsigned int &result_;
};
%}

// this turns the char * into byte[] for Java
%apply signed char *INOUT {char *toFill};
%typemap(freearg) char *toFill ""
%typemap(directorin,descriptor="[B") char *toFill %{
	$input = jenv->NewByteArray((jsize)maxToRead);
	XmlInputStream_ReadBytes_InOutByteArray arg_guard$argnum(jenv, $input, $1, result);
%}

%feature("director") XmlInputStream;


%include "enumtypeunsafe.swg"
%javaconst(1);

// These two typemaps for string are directly copied from the std_string.i typemap
// in the java directory that comes with SWIG.  It is modified
// to map a null input into an empty string, rather than throwing
// an exception
%typemap(in) std::string
%{ 
const char *$1_pstr = "";
if($input) {
    $1_pstr = (const char *)jenv->GetStringUTFChars($input, 0);
}
if (!$1_pstr) return $null;
$1 =  std::string($1_pstr);
if ($input) {
    jenv->ReleaseStringUTFChars($input, $1_pstr);
}
%}

%typemap(in) const std::string &
%{ 
const char *$1_pstr = "";
if($input) {
    $1_pstr = (const char *)jenv->GetStringUTFChars($input, 0);
}
if (!$1_pstr) return $null;
std::string $1_str($1_pstr);
$1 = &$1_str;
if ($input) {
    jenv->ReleaseStringUTFChars($input, $1_pstr);
}
%}

//
// Pile of typemaps for unsigned char to be equivalent
// to signed char (Java String).  Not all may be required, but
// experimentation is necessary to eliminate any.
// NOTE: I tried using just these two, but they resulted in
// no compilation due to missing some explicit casts to JNI calls
//  %apply char {unsigned char};
//  %apply char * {unsigned char*};
//
%typemap(jni) unsigned char, const unsigned char & "jchar"
%typemap(jtype) unsigned char, const unsigned char & "char"
%typemap(jstype) unsigned char, const unsigned char & "char"
%typemap(jni) unsigned char *, unsigned char[ANY], unsigned char[]    "jstring"
%typemap(jtype) unsigned char *, unsigned char[ANY], unsigned char[]  "String"
%typemap(jstype) unsigned char *, unsigned char[ANY], unsigned char[] "String"
%typemap(javain) unsigned char, const unsigned char & "$javainput"
%typemap(javain) unsigned char *, unsigned char[ANY], unsigned char[] "$javainput"
%typemap(javaout) unsigned char, const unsigned char & { return $jnicall; }
%typemap(javaout) unsigned char *, unsigned char[ANY], unsigned char[] {
    return $jnicall;
  }

%typemap(in, noblock=1) unsigned char * {
 $1 = 0;
  if ($input) {
    $1 = ($1_ltype)JCALL2(GetStringUTFChars, jenv, $input, 0);
    if (!$1) return $null;
  }
}
%typemap(in, noblock=1) const unsigned char * {
 $1 = 0;
  if ($input) {
    $1 = ($1_ltype)JCALL2(GetStringUTFChars, jenv, $input, 0);
    if (!$1) return $null;
  }
}

%typemap(out, noblock=1) const unsigned char * { if ($1) $result = JCALL1(NewStringUTF, jenv, (const char *) $1); }

%typemap(out, noblock=1) unsigned char * { if ($1) $result = JCALL1(NewStringUTF, jenv, (char *) $1); }

// end pile of typemaps for unsigned char

%typemap(javaimports) SWIGTYPE %{
import com.sleepycat.db.*;
import com.sleepycat.db.internal.DbEnv;
import com.sleepycat.db.internal.DbConstants;
%}

%pragma(java) jniclassimports=%{
import com.sleepycat.db.DatabaseException;
%}

%pragma(java) jniclasscode=%{
  static {
    // An alternate library name can be specified via a property.
    //
    String override;

    if ((override = System.getProperty("sleepycat.dbxml.libfile")) != null) {
      System.load(override);
    }
    else if ((override = System.getProperty("sleepycat.dbxml.libname")) != null) {
      System.loadLibrary(override);
    }
    else {
        String os = System.getProperty("os.name");
        if (os != null && os.startsWith("Windows")) {
            // library name is "libdbxml_javaXX.dll" (for example) on Windows
	    String libname = "libdbxml_java" +
            XmlConstants.DBXML_VERSION_MAJOR + XmlConstants.DBXML_VERSION_MINOR;

            // first try to load the release version. If that fails try debug
            try {
                System.loadLibrary(libname);
            } catch (UnsatisfiedLinkError e) {
                try {
                    libname += "d";
                    System.loadLibrary(libname);
                } catch (UnsatisfiedLinkError e2) {
                    throw e;
                }
            }
      }
      else {
        // library name is "libdbxml_java-Major.Minor.so" (for example) on UNIX
        // Note: "dbxml_java" isn't good enough;
        // some Unixes require us to use the explicit SONAME.
        System.loadLibrary("dbxml_java-" +
                   XmlConstants.DBXML_VERSION_MAJOR + "." +
                   XmlConstants.DBXML_VERSION_MINOR);
      }
    }

    initialize();
  }

  static native final void initialize();
%}

%typemap(check) SWIGTYPE *self %{
	if (!$input) {
		jenv->Throw((jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
		    XmlException::INTERNAL_ERROR,
		    jenv->NewStringUTF("null object - call after object destroyed?"),
		    0, 0, 0, 0));					
		return $null;
	}
%}

%typemap(check) SWIGTYPE *jcls %{
	UNUSED($input);	/* just in case */
%}

/*
 * protect XmlManager methods that aren't publicly supported
 */
%javamethodmodifiers XmlManager::createContainer(
				 const std::string &name, 
				 u_int32_t flags,
				 enum XmlContainer::ContainerType type =
				 XmlContainer::NodeContainer,
				 int mode = 0) "protected";

%javamethodmodifiers XmlManager::createContainer(
				 XmlTransaction &txn,
				 const std::string &name, 
				 u_int32_t flags,
				 enum XmlContainer::ContainerType type =
				 XmlContainer::NodeContainer,
				 int mode = 0) "protected";

%javamethodmodifiers XmlManager::openContainer(const std::string &name, 
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlManager::openContainer(XmlTransaction &txn,
					       const std::string &name, 
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlManager::openContainer(XmlTransaction &txn,
					       const std::string &name, 
					       u_int32_t flags,
					       enum XmlContainer::ContainerType,
					       int mode = 0) "protected";

%javamethodmodifiers XmlManager::openContainer(const std::string &name, 
					       u_int32_t flags,
					       enum XmlContainer::ContainerType,
					       int mode = 0) "protected";

%javamethodmodifiers XmlManager::query(const std::string &query, 
				       XmlQueryContext &context,
				       u_int32_t flags) "protected";

%javamethodmodifiers XmlManager::query(XmlTransaction &txn,
				       const std::string &query, 
				       XmlQueryContext &context,
				       u_int32_t flags) "protected";

%javamethodmodifiers XmlManager::verifyContainer(const std::string &name, 
						const char *filename,
						u_int32_t flags) "protected";

%javamethodmodifiers XmlManager::reindexContainer(const std::string &name, 
						 XmlUpdateContext &uc,
						 u_int32_t flags = 0) "protected";
%javamethodmodifiers XmlManager::reindexContainer(XmlTransaction &txn, 
						  const std::string &name,
						  XmlUpdateContext &uc, 
						  u_int32_t flags = 0) "protected";


/*
 * Pieces of XmlManager and XmlTransaction are implemented
 * directly in Java, for better integration with DB.
 * Also, some global constants are moved to the XmlManager class in Java
 */

/*
 * Re-map delete() for XmlManager to close the Environment if necessary,
 * using closeInternal() (protected).
 * The basic code here comes from java.swg
 */
%typemap(javadestruct, methodname="delete") XmlManager {
    closeInternal();
    if(swigCPtr != 0 && swigCMemOwn) {
      swigCMemOwn = false;
      $jnicall;
    }
    swigCPtr = 0;
}

%typemap(javacode) XmlManager %{

    private Environment dbenv;
    private XmlContainerConfig defaultConfig;
    private boolean threaded = true; // default on if no Environment
    private boolean adopted = false;
    private XmlManagerConfig config = null;

    public XmlManager(final Environment dbenv,
		      XmlManagerConfig config)
	throws XmlException {
	this(dbxml_javaJNI.new_XmlManager(XmlHelper.getDbEnv(dbenv),
					  ((config == null ) ? 0 :
					   config.makeFlags())),true);
	this.dbenv = dbenv;
	this.config = config;
	if ((config != null) && (config.getAdoptEnvironment()))
	    this.adopted = true;
	try {
	    threaded =
		((XmlHelper.getDbEnv(dbenv).get_open_flags() &
		  DbConstants.DB_THREAD) != 0);
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
    }

    public XmlManager(XmlManagerConfig config)
	throws XmlException, java.io.FileNotFoundException {
	DbEnv newEnv = null;
	try {
	    newEnv = new DbEnv(0);
	    newEnv.set_cachesize(64 * 1024 * 1024, 1); // 64MB cache
	    newEnv.set_errpfx("BDB XML");
	    newEnv.open(null, DbConstants.DB_PRIVATE|
			DbConstants.DB_CREATE|DbConstants.DB_INIT_MPOOL|
			DbConstants.DB_THREAD, 0);
	    this.dbenv = XmlHelper.makeNewEnvironment(newEnv);
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	this.adopted = true;
	this.config = config;
	// do what the SWIG-generated ctor does, in order to
	// create a C++ XmlManager object.  Add ADOPT flag,
	// since the DbEnv was internally constructed.
	int flags = (config == null) ? 0 : config.makeFlags();
	swigCPtr = dbxml_javaJNI.new_XmlManager(newEnv, flags);
	swigCMemOwn = true;
    }

    public XmlManager()
	throws XmlException, java.io.FileNotFoundException {
	this(null);
    }

    public Environment getEnvironment() {
	return dbenv;
    }

    public XmlManagerConfig getManagerConfig() {
	return config;
    }

    public void setDefaultContainerConfig(XmlContainerConfig config) {
	this.defaultConfig = config;
    }

    public XmlContainerConfig getDefaultContainerConfig() {
	return defaultConfig;
    }

	public XmlInputStream createInputStream(java.io.InputStream is) {
		return new NativeXmlInputStream(is);
	}

    public XmlContainer createContainer(String name)
	throws XmlException {
	return createContainer(name, defaultConfig);
    }

    public XmlContainer createContainer(XmlTransaction txn, String name)
	throws XmlException {
	return createContainer(txn, name, defaultConfig);
    }

    public XmlContainer openContainer(String name)
	throws XmlException {
	return openContainer(name, defaultConfig);
    }

    public XmlContainer openContainer(XmlTransaction txn, String name)
	throws XmlException {
	return openContainer(txn, name, defaultConfig);
    }

    public XmlContainer createContainer(String name,
					XmlContainerConfig config)
	throws XmlException {
	int containerType;
	int flags = 0;
	if (config != null) {
	    flags = config.makeFlags(threaded);
	    containerType = (config.getNodeContainer()) ?
		XmlContainer.NodeContainer : XmlContainer.WholedocContainer;
	} else {
	    containerType = getDefaultContainerType();
	}
	XmlContainer cont = createContainer(name, flags, containerType, 0);
	cont.setContainerConfig(config);
	cont.setManager(this);
	return cont;
    }

    public XmlContainer createContainer(XmlTransaction txn,
					String name,
					XmlContainerConfig config)
	throws XmlException {
	int flags = 0;
	int containerType;
	if (config != null) {
	    flags = config.makeFlags(threaded);
	    containerType = (config.getNodeContainer()) ?
		XmlContainer.NodeContainer : XmlContainer.WholedocContainer;
	} else {
	    containerType = getDefaultContainerType();
	}
	XmlContainer cont = createContainer(txn, name, flags, containerType, 0);
	cont.setContainerConfig(config);
	cont.setManager(this);
	return cont;
    }

    public XmlContainer openContainer(String name,
				      XmlContainerConfig config)
	throws XmlException {
	int flags = 0;
	int containerType;
	if (config != null) {
	    flags = config.makeFlags(threaded);
	    containerType = (config.getNodeContainer()) ?
		XmlContainer.NodeContainer : XmlContainer.WholedocContainer;
	} else {
	    containerType = getDefaultContainerType();
	}

	XmlContainer cont = openContainer(name, flags, containerType, 0);
	cont.setContainerConfig(config);
	cont.setManager(this);
	return cont;
    }

    public XmlContainer openContainer(XmlTransaction txn,
				      String name,
				      XmlContainerConfig config)
	throws XmlException {
	int flags = 0;
	int containerType;
	if (config != null) {
	    flags = config.makeFlags(threaded);
	    containerType = (config.getNodeContainer()) ?
		XmlContainer.NodeContainer : XmlContainer.WholedocContainer;
	} else {
	    containerType = getDefaultContainerType();
	}

	XmlContainer cont = openContainer(txn, name, flags, containerType, 0);
	cont.setContainerConfig(config);
	cont.setManager(this);
	return cont;
    }

    public XmlTransaction createTransaction(com.sleepycat.db.Transaction toUse)
	throws XmlException {
	XmlTransaction txn = createTransaction(XmlHelper.getDbTxn(toUse));
	txn.setTransaction(toUse);
	return txn;
    }

    public XmlTransaction createTransaction(final Transaction parent,
					    TransactionConfig config)
	throws XmlException {
	Transaction newTxn = null;
	try {
	    newTxn = dbenv.beginTransaction(parent, config);
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	return createTransaction(newTxn);
    }

    public XmlTransaction createTransaction()
	throws XmlException {
	return createTransaction(null, null);
    }

    public XmlResults query(String query,
			    XmlQueryContext context,
			    XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return query(query, context, flags);
    }

    public XmlResults query(XmlTransaction txn,
			    String query,
			    XmlQueryContext context,
			    XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return query(txn, query, context, flags);
    }

    public void reindexContainer(String name,
				 XmlUpdateContext context,
				 XmlContainerConfig config)
	throws XmlException {
	int flags = 0;
	if (config != null)
	    flags = config.makeFlags(threaded);
	reindexContainer(name, context, flags);
    }

    public void reindexContainer(XmlTransaction txn,
				 String name,
				 XmlUpdateContext context,
				 XmlContainerConfig config)
	throws XmlException {
	int flags = 0;
	if (config != null)
	    flags = config.makeFlags(threaded);
	reindexContainer(txn, name, context, flags);
    }

    public void verifyContainer(String name,
				String filename,
				VerifyConfig config)
	throws XmlException {
	int flags = 0;
	if (config.getAggressive())
	    flags |= DbConstants.DB_AGGRESSIVE;
	if (config.getSalvage())
	    flags |= DbConstants.DB_SALVAGE;
	if (config.getPrintable())
	    flags |= DbConstants.DB_PRINTABLE;
	if (config.getOrderCheckOnly())
	    flags |= DbConstants.DB_ORDERCHKONLY;
	if (config.getNoOrderCheck())
	    flags |= DbConstants.DB_NOORDERCHK;
	verifyContainer(name, filename, flags);
    }

    protected void closeInternal() {
	if (adopted) {
	    try {
		dbenv.close();
	    } catch (DatabaseException de) {}
	}
	adopted = false;
	this.dbenv = null;
	this.config = null;
    }

    public void close() {
	delete();
    }

    public final static int LEVEL_NONE = dbxml_java.LEVEL_NONE;
    public final static int LEVEL_DEBUG = dbxml_java.LEVEL_DEBUG;
    public final static int LEVEL_INFO = dbxml_java.LEVEL_INFO;
    public final static int LEVEL_WARNING = dbxml_java.LEVEL_WARNING;
    public final static int LEVEL_ERROR = dbxml_java.LEVEL_ERROR;
    public final static int LEVEL_ALL = dbxml_java.LEVEL_ALL;
    public final static int CATEGORY_NONE = dbxml_java.CATEGORY_NONE;
    public final static int CATEGORY_INDEXER = dbxml_java.CATEGORY_INDEXER;
    public final static int CATEGORY_QUERY = dbxml_java.CATEGORY_QUERY;
    public final static int CATEGORY_OPTIMIZER = dbxml_java.CATEGORY_OPTIMIZER;
    public final static int CATEGORY_DICTIONARY = dbxml_java.CATEGORY_DICTIONARY;
    public final static int CATEGORY_CONTAINER = dbxml_java.CATEGORY_CONTAINER;
    public final static int CATEGORY_NODESTORE = dbxml_java.CATEGORY_NODESTORE;
    public final static int CATEGORY_MANAGER = dbxml_java.CATEGORY_MANAGER;
    public final static int CATEGORY_ALL = dbxml_java.CATEGORY_ALL;
    public final static String metaDataNamespace_uri = dbxml_javaConstants.metaDataNamespace_uri;
    public final static String metaDataNamespace_prefix = dbxml_javaConstants.metaDataNamespace_prefix;
    public final static String metaDataName_name = dbxml_javaConstants.metaDataName_name;
    public final static String metaDataName_root = dbxml_javaConstants.metaDataName_root;
    %}


/*
 * XmlTransaction is mostly Java
 */
%typemap(javacode) XmlTransaction %{
    Transaction txn;

    public void setTransaction(final com.sleepycat.db.Transaction txn) {
	this.txn = txn;
    }

    public Transaction getTransaction() {
	return this.txn;
    }

    public void abort()
	throws XmlException {
	try {
	    txn.abort();
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	delete();
    }

    public void commit()
	throws XmlException {
	try {
	    txn.commit();
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	delete();
    }

    public void commitSync()
	throws XmlException {
	try {
	    txn.commitSync();
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	delete();
    }

    public void commitNoSync()
	throws XmlException {
	try {
	    txn.commitNoSync();
	} catch (DatabaseException de) {
	    throw new XmlException(XmlException.DATABASE_ERROR,
				   de.toString(), de,
				   de.getErrno(), 0, 0);
	}
	delete();
    }
%}

/* N.B. For some reason, these two javamethodmodifiers directives are no-ops */
%javamethodmodifiers XmlDocument::setContentAsXmlInputStream_java(XmlInputStream adopted) "protected";

%javamethodmodifiers XmlDocument::setContentAsEventReader_java(XmlEventReader reader) "protected";


/*
 * Create a java.io.InputStream from document content
 */
%typemap(javacode) XmlDocument %{

    public void setContentAsEventReader(XmlEventReader reader) 
	throws XmlException {
	setContentAsEventReader_java(reader);
	reader.delete(); /* disallow allow further access */
    }
    public InputStream getContentAsInputStream() throws XmlException {
	return new XmlInputStreamWrap(getContentAsXmlInputStream());
    }

    public void setContentAsXmlInputStream(XmlInputStream adopted) throws XmlException {
	dbxml_javaJNI.XmlDocument_setContentAsXmlInputStream_java(swigCPtr, 
								  XmlInputStream.getCPtr(adopted));
	adopted.swigReleaseOwnership();
    }
%}

%typemap(javaimports) XmlDocument %{
import com.sleepycat.db.*;
import com.sleepycat.db.internal.DbEnv;
import com.sleepycat.db.internal.DbConstants;
import java.io.InputStream;
%}

/*
 * protect XmlContainer methods that aren't publicly supported
 */
%javamethodmodifiers XmlContainer::putDocument(XmlDocument &,
					       XmlUpdateContext &,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(XmlTransaction &,
					       XmlDocument &,
					       XmlUpdateContext &,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(const std::string &name,
					       XmlInputStream *input,
					       XmlUpdateContext &context,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(XmlTransaction &,
					       const std::string &name,
					       XmlInputStream *input,
					       XmlUpdateContext &context,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(XmlTransaction &,
					       const std::string &name, 
					       XmlEventReader &reader,
					       XmlUpdateContext &context, 
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(const std::string &name, 
					       XmlEventReader &reader,
					       XmlUpdateContext &context, 
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(const std::string &name,
					       const std::string &contents,
					       XmlUpdateContext &context,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocument(XmlTransaction &,
					       const std::string &name,
					       const std::string &contents,
					       XmlUpdateContext &context,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocumentAsEventWriter(XmlTransaction &,
							    XmlDocument &document,
							    XmlUpdateContext &context,
							    u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::putDocumentAsEventWriter(XmlDocument &document,
							    XmlUpdateContext &context,
							    u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::getDocument(const std::string &name,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::getDocument(XmlTransaction &,
					       const std::string &name,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::getAllDocuments(u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::getAllDocuments(XmlTransaction &,
						   u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::getIndexSpecification(XmlTransaction &,
							 u_int32_t flags) const "protected";

%javamethodmodifiers XmlContainer::lookupIndex(
					       XmlQueryContext &context, 
					       const std::string &uri,
					       const std::string &name, 
					       const std::string &index,
					       const XmlValue &value,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::lookupIndex(
					       XmlQueryContext &context, 
					       const std::string &uri,
					       const std::string &name, 
					       const std::string &parent_uri,
					       const std::string &parent_name, 
					       const std::string &index,
					       const XmlValue &value,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::lookupIndex(XmlTransaction &,
					       XmlQueryContext &context, 
					       const std::string &uri,
					       const std::string &name, 
					       const std::string &index,
					       const XmlValue &value,
					       u_int32_t flags) "protected";

%javamethodmodifiers XmlContainer::lookupIndex(XmlTransaction &,
					       XmlQueryContext &context, 
					       const std::string &uri,
					       const std::string &name, 
					       const std::string &parent_uri,
					       const std::string &parent_name, 
					       const std::string &index,
					       const XmlValue &value,
					       u_int32_t flags) "protected";

%typemap(javacode) XmlContainer %{
    private XmlContainerConfig config;
    private XmlManager manager;

    public XmlContainerConfig getContainerConfig() {
	return config;
    }

    public XmlManager getManager() {
	return manager;
    }

    public void close() {
	delete(); // delete the C++ object, as well
    }

    public void putDocument(XmlDocument document,
			    XmlUpdateContext context,
			    XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	putDocument(document, context, flags);
    }

    public void putDocument(XmlTransaction txn, XmlDocument document,
			    XmlUpdateContext context,
			    XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	putDocument(txn, document, context, flags);
    }

    public String putDocument(String name,
			      XmlInputStream input,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return putDocument(name, input, context, flags);
    }

    public String putDocument(XmlTransaction txn,
			      String name,
			      XmlInputStream input,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return putDocument(txn, name, input, context, flags);
    }

    public String putDocument(String name,
			      String content,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return putDocument(name, content, context, flags);
    }

    public String putDocument(XmlTransaction txn,
			      String name,
			      String content,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return putDocument(txn, name, content, context, flags);
    }

    public String putDocument(String name,
			      XmlEventReader reader,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	String ret = putDocument(name, reader, context, flags);
	reader.delete(); /* no further access */
	return ret;
    }

    public String putDocument(String name,
			      XmlEventReader reader,
			      XmlUpdateContext context)
	throws XmlException {
	return putDocument(name, reader, context, null);
    }

    public String putDocument(XmlTransaction txn,
			      String name,
			      XmlEventReader reader,
			      XmlUpdateContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	String ret = putDocument(txn, name, reader, context, flags);
	reader.delete(); /* no further access */
	return ret;
    }

    public String putDocument(XmlTransaction txn, 
			      String name,
			      XmlEventReader reader,
			      XmlUpdateContext context)
	throws XmlException {
	return putDocument(txn, name, reader, context, null);
    }

    public XmlEventWriter putDocumentAsEventWriter(XmlDocument document,
						   XmlUpdateContext context,
						   XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return putDocumentAsEventWriter(document, context, flags);
    }

    public XmlEventWriter putDocumentAsEventWriter(XmlTransaction txn,
						   XmlDocument document,
						   XmlUpdateContext context,
						   XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return putDocumentAsEventWriter(txn, document, context, flags);
    }

    public XmlDocument getDocument(String name)
	throws XmlException {
	return getDocument(name, null);
    }

    public XmlDocument getDocument(String name,
				   XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return getDocument(name, flags);
    }

    public XmlDocument getDocument(XmlTransaction txn,
				   String name)
	throws XmlException {
	return getDocument(txn, name, null);
    }

    public XmlDocument getDocument(XmlTransaction txn,
				   String name,
				   XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return getDocument(txn, name, flags);
    }

    public XmlResults getAllDocuments(XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return getAllDocuments(flags);
    }

    public XmlResults getAllDocuments(XmlTransaction txn,
				      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return getAllDocuments(txn, flags);
    }


    public XmlIndexSpecification getIndexSpecification(XmlTransaction txn,
						       XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return getIndexSpecification(txn, flags);
    }

    public XmlResults lookupIndex(XmlQueryContext context,
				  String uri, String name,
				  String index,
				  XmlValue value,
				  XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return lookupIndex(context, uri, name, index, value, flags);
    }

    public XmlResults lookupIndex(XmlTransaction txn,
				  XmlQueryContext context,
				  String uri, String name,
				  String index,
				  XmlValue value,
				  XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return lookupIndex(txn, context, uri, name, index, value, flags);
    }

    public XmlResults lookupIndex(XmlQueryContext context,
				  String uri, String name,
				  String parentUri, String parentName,
				  String index,
				  XmlValue value,
				  XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return lookupIndex(context, uri, name,
			   parentUri, parentName, index, value, flags);
    }

    public XmlResults lookupIndex(XmlTransaction txn,
				  XmlQueryContext context,
				  String uri, String name,
				  String parentUri, String parentName,
				  String index,
				  XmlValue value,
				  XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return lookupIndex(txn, context, uri, name,
			   parentUri, parentName, index, value, flags);
    }

    /* package */
    void setContainerConfig(XmlContainerConfig config) {
	this.config = config;
    }

    /* package */
    void setManager(XmlManager manager) {
	this.manager = manager;
    }

%}

/*
 * protect XmlQueryExpression methods that aren't publicly supported
 */

%javamethodmodifiers XmlQueryExpression::execute(XmlQueryContext &,
						 u_int32_t) const "protected";

%javamethodmodifiers XmlQueryExpression::execute(const XmlValue &, 
						 XmlQueryContext &,
						 u_int32_t) const "protected";

%javamethodmodifiers XmlQueryExpression::execute(XmlTransaction &,
						 XmlQueryContext &,
						 u_int32_t) const "protected";

%javamethodmodifiers XmlQueryExpression::execute(XmlTransaction &,
						 const XmlValue &, 
						 XmlQueryContext &,
						 u_int32_t) const "protected";

%typemap(javacode) XmlQueryExpression %{
    public XmlResults execute(XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(context, flags);
    }
    public XmlResults execute(XmlValue contextItem,
			      XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(contextItem, context, flags);
    }

    public XmlResults execute(XmlTransaction txn,
			      XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(txn, context, flags);
    }

    public XmlResults execute(XmlTransaction txn,
			      XmlValue contextItem,
			      XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(txn, contextItem, context, flags);
    }
%}

// protect non-exported XmlIndexLookup methods
%javamethodmodifiers XmlIndexLookup::execute(XmlQueryContext &,
					     u_int32_t) const "protected";
%javamethodmodifiers XmlIndexLookup::execute(XmlTransaction &,
					     XmlQueryContext &,
					     u_int32_t) const "protected";

%typemap(javacode) XmlIndexLookup %{
    public XmlResults execute(XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(context, flags);
    }
    public XmlResults execute(XmlTransaction txn,
			      XmlQueryContext context,
			      XmlDocumentConfig config)
	throws XmlException {
	int flags = (config == null) ? 0 : config.makeFlags();
	return execute(txn, context, flags);
    }
%}

%javamethodmodifiers XmlEventReader::closeInternal() "protected";

%javamethodmodifiers XmlEventWriter::closeInternal() "protected";


%typemap(javacode) XmlEventReader %{
    public void close()
	/* wrapper for close that protects from double-close */
	throws XmlException {
	if (swigCPtr != 0) {
	    closeInternal();
	    delete();
	}
    }
%}

%typemap(javacode) XmlEventWriter %{
    public void close()
	/* wrapper for close that protects from double-close */
	throws XmlException {
	if (swigCPtr != 0) {
	    closeInternal();
	    delete();
	}
    }
%}

/*
 * This is a hack to the typemap from java.swg for construction
 * that makes the writer, and optionally the reader inaccessible
 * after construction of XmlEventReaderToWriter.  It does not
 * appear possible to use %rename on constructors.
 */
%typemap(javaconstruct,directorconnect="\n $moduleJNI.$javaclassname_director_connect(this, swigCPtr, swigCMemOwn, WEAKREF);") XmlEventReaderToWriter { 
    this($imcall, true);$directorconnect
    writer.delete();
    if (ownsReader)
	reader.delete();
}

%{
/* don't use SWIG's array handling - save code space */
#define SWIG_NOINCLUDE 1

#if DB_VERSION_MAJOR > 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR >= 4)
#define DB_EXTRA_EXC 1
#endif

// XML classes
static jclass xml_indexdecl_class;
static jclass xmlex_class;
static jclass np_class;
// DB classes
static jclass dbenv_class;
static jclass dbtxn_class;
// DB exception classes
static jclass dbex_class, deadex_class, lockex_class, memex_class;
static jclass runrecex_class, rephandledeadex_class;
#ifdef DB_EXTRA_EXC
// DB 4.4 TBD...
static jclass repdupmasterex_class;
static jclass repholdelectionex_class, repunavailex_class;
static jclass versionex_class;
#endif


// fields
jfieldID dbenv_private_fid;
jfieldID dbtxn_private_fid;

// method IDs
static jmethodID xml_indexdecl_construct;
static jmethodID xmlex_construct;
// DB exception method ids
static jmethodID dbex_construct, deadex_construct, lockex_construct;
static jmethodID memex_construct;
static jmethodID rephandledeadex_construct;
static jmethodID runrecex_construct;
#ifdef DB_EXTRA_EXC
// DB 4.4. TBD
static jmethodID repdupmasterex_construct, memex_update_method;
static jmethodID repholdelectionex_construct, repunavailex_construct;
static jmethodID versionex_construct;
#endif

#define DB_PKG "com/sleepycat/db/"
#define DB_PKG_INT "com/sleepycat/db/internal/"
#define XML_PKG "com/sleepycat/dbxml/"

/* Forward declarations */
static jthrowable __dbj_get_except(JNIEnv *jenv,
   int err, const char *msg, jobject obj, jobject jdbenv);

static jthrowable make_db_except(JNIEnv *jenv,
   int err, const char *msg, jobject obj, jobject jdbenv);

// all classes
const struct {
    jclass *cl;
    const char *name;
} all_classes[] = {
    // XML
    { &xml_indexdecl_class, XML_PKG "XmlIndexDeclaration" },
    { &xmlex_class, XML_PKG "XmlException" },
    // DB
    { &dbenv_class, DB_PKG_INT "DbEnv" },
    { &dbtxn_class, DB_PKG_INT "DbTxn" },
    { &dbex_class, DB_PKG "DatabaseException" },
    { &deadex_class, DB_PKG "DeadlockException" },
    { &lockex_class, DB_PKG "LockNotGrantedException" },
    { &memex_class, DB_PKG "MemoryException" },
    { &rephandledeadex_class, DB_PKG "ReplicationHandleDeadException" },
    { &runrecex_class, DB_PKG "RunRecoveryException" },
#ifdef DB_EXTRA_EXC
    { &repdupmasterex_class, DB_PKG "ReplicationDuplicateMasterException" },
    { &repholdelectionex_class, DB_PKG "ReplicationHoldElectionException" },
    { &repunavailex_class, DB_PKG "ReplicationSiteUnavailableException" },
    { &versionex_class, DB_PKG "VersionMismatchException" },
#endif
    // Misc
    { &np_class, "java/lang/NullPointerException" }
};

const struct {
	jmethodID *mid;
	jclass *cl;
	const char *name;
	const char *sig;
} all_methods[] = {
    // XML methods
    { &xml_indexdecl_construct, &xml_indexdecl_class, "<init>",
      "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"  },
    { &xmlex_construct, &xmlex_class, "<init>",
      "(ILjava/lang/String;L" DB_PKG "DatabaseException;III)V"  },
    // DB methods
    { &dbex_construct, &dbex_class, "<init>",
      "(Ljava/lang/String;I)V" },
    { &deadex_construct, &deadex_class, "<init>",
      "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" },
    { &lockex_construct, &lockex_class, "<init>",
      "(Ljava/lang/String;IIL" DB_PKG "DatabaseEntry;L" DB_PKG "internal/DbLock;IL" DB_PKG "internal/DbEnv;)V" },
    { &memex_construct, &memex_class, "<init>",
      "(Ljava/lang/String;L" DB_PKG "DatabaseEntry;IL" DB_PKG "internal/DbEnv;)V" },
    { &rephandledeadex_construct, &rephandledeadex_class, "<init>",
      "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" },
    { &runrecex_construct, &runrecex_class, "<init>",
      "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" }
#ifdef DB_EXTRA_EXC
    ,{ &memex_update_method, &memex_class, "updateDatabaseEntry",
      "(L" DB_PKG "DatabaseEntry;)V" },
    { &repdupmasterex_construct, &repdupmasterex_class, "<init>",
      "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" },
    { &repholdelectionex_construct, &repholdelectionex_class, "<init>",
      "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" },
    { &repunavailex_construct, &repunavailex_class, "<init>",
      "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" },
    { &versionex_construct, &versionex_class, "<init>",
      "(Ljava/lang/String;IL" DB_PKG "internal/DbEnv;)V" }
#endif
};

#define NELEM(x) (sizeof (x) / sizeof (x[0]))

static bool checkDbVersion()
{
	int major, minor, patch, dmajor, dminor, dpatch;
	(void) db_version(&major, &minor, &patch);
	(void) DbXml::dbxml_db_version(&dmajor, &dminor, &dpatch);
	// match major and minor numbers
	// TBD: warn if patch level is different
	if (major != dmajor || minor != dminor)
		return false;
	return true;
}

extern "C"
JNIEXPORT void JNICALL Java_com_sleepycat_dbxml_dbxml_1javaJNI_initialize
  (JNIEnv *jenv, jclass clazz)
{
    (void)clazz; /* unused */
    jclass cl;
    unsigned int i, j;

    if (checkDbVersion() == false) {
	    fprintf(stderr,
		    "Mismatched Berkeley DB libraries");
	    return;
    }
    // This initialization code comes from DB's java_util.i file
    for (i = 0; i < NELEM(all_classes); i++) {
	cl = (jenv)->FindClass(all_classes[i].name);
	if (cl == NULL) {
	    fprintf(stderr,
		    "Failed to load class %s - check CLASSPATH\n",
		    all_classes[i].name);
	    return;
	}
	/*
	 * Wrap classes in GlobalRefs so we keep the reference between
	 * calls.
	 */
	*all_classes[i].cl = (jclass)(jenv)->NewGlobalRef(cl);

	if (*all_classes[i].cl == NULL) {
	    fprintf(stderr,
		    "Failed to create a global reference for %s\n",
		    all_classes[i].name);
	    return;
	}
    }

    // get some field ids
#if DB_VERSION_MAJOR < 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR <= 1)
    dbenv_private_fid = jenv->GetFieldID(dbenv_class, "private_dbobj_", "J");
    dbtxn_private_fid = jenv->GetFieldID(dbtxn_class, "private_dbobj_", "J");
#else
    dbenv_private_fid = jenv->GetFieldID(dbenv_class, "swigCPtr", "J");
    dbtxn_private_fid = jenv->GetFieldID(dbtxn_class, "swigCPtr", "J");
#endif
    /* Get method IDs */
    for (i = 0; i < NELEM(all_methods); i++) {
	*all_methods[i].mid = (jenv)->
	    GetMethodID(*all_methods[i].cl, all_methods[i].name,
			all_methods[i].sig);

	if (*all_methods[i].mid == NULL) {
	    for (j = 0; j < NELEM(all_classes); j++)
		if (all_methods[i].cl == all_classes[j].cl)
		    break;
	    fprintf(stderr,
		    "Failed to look up method %s.%s with sig %s\n",
		    all_classes[j].name, all_methods[i].name,
		    all_methods[i].sig);
	    return;
	}
    }
}

/* Union to convert longs to pointers (see {get,set}_private_dbobj). */
typedef union {
	jlong java_long;
	void *ptr;
} long_to_ptr;

static DbEnv *get_DbEnv(JNIEnv *jenv, jobject jdbenv)
{
	if (jdbenv == NULL)
		return NULL;
	else {
#if DB_VERSION_MAJOR < 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR <= 1)
		long_to_ptr lp;
		lp.java_long = jenv->GetLongField(jdbenv, dbenv_private_fid);
		return DbEnv::wrap_DB_ENV((DB_ENV *)lp.ptr);
#else
		jlong swigCPtr = jenv->GetLongField(jdbenv, dbenv_private_fid);
		return DbEnv::wrap_DB_ENV(*(DB_ENV **)&swigCPtr);
#endif
	}
}

static DbTxn *get_DbTxn(JNIEnv *jenv, jobject jtxn)
{
	if (jtxn == NULL)
		return NULL;
	else {
#if DB_VERSION_MAJOR < 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR <= 1)
		long_to_ptr lp;
		lp.java_long = jenv->GetLongField(jtxn, dbtxn_private_fid);
		return DbTxn::wrap_DB_TXN((DB_TXN *)lp.ptr);
#else
		jlong swigCPtr = jenv->GetLongField(jtxn, dbtxn_private_fid);
		return DbTxn::wrap_DB_TXN(*(DB_TXN **)&swigCPtr);
#endif
	}
}

struct DBT_INFO {
	Dbt dbt;
	jbyte *orig_data;
};

%}

%exception {
	jthrowable t = NULL;
	try {
		$action
	}
	catch (XmlException &xe) {
	    // use 0 for dberr; otherwise, DatabaseException::toString
	    // prints redundant information.  The DB error information
	    // is already in the what() string.
	    if (xe.getExceptionCode() == XmlException::NULL_POINTER) {
		// throw null pointer exception
		jenv->ThrowNew(np_class, xe.what());
	    } else {
		jthrowable dt = 0;
		const char *msg = xe.what();
		XmlException::ExceptionCode ecode =
			xe.getExceptionCode();
		int qLine = xe.getQueryLine();
		int qCol = xe.getQueryColumn();
		if (ecode == XmlException::DATABASE_ERROR) {
		    dt = __dbj_get_except(jenv, xe.getDbErrno(),
					  msg, NULL, NULL);
		}
		t = (jthrowable)jenv->
		    NewObject(xmlex_class, xmlex_construct,
			      ecode, jenv->NewStringUTF(msg), dt, 0, qLine, qCol);
	    }
	}
	catch (DbException &de) {
	    t = make_db_except(jenv, de.get_errno(),
				 de.what(), NULL, NULL);
	}
	catch (std::exception &se) {
	    std::string emsg = "std::exception thrown from C++ API: ";
	    emsg += se.what();
	    t = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
		    XmlException::INTERNAL_ERROR,
		    jenv->NewStringUTF(emsg.c_str()), 0, 0, 0, 0);
	}
	catch(JavaException & /* je */) {
		// This means there's already an exception waiting in the JVM
		return $null;
	}
	catch (...) {
	    t = (jthrowable)jenv->NewObject(xmlex_class, xmlex_construct,
		    XmlException::INTERNAL_ERROR,
		    jenv->NewStringUTF("Uncaught exception from C++ API"), 0, 0, 0, 0);
	}

	if (t) {
	    jenv->Throw(t);
	    return $null;
	}
}

// Typemaps
%define JAVA_TYPEMAP(_ctype, _jtype, _jnitype)
%typemap(jstype) _ctype #_jtype
%typemap(jtype) _ctype #_jtype
%typemap(jni) _ctype #_jnitype
%typemap(javain) _ctype "$javainput"
%typemap(javaout) _ctype { return $jnicall; }
%enddef

JAVA_TYPEMAP(u_int32_t, int, jint)
JAVA_TYPEMAP(size_t, int, jint)

%typemap(in) bool %{
	$1 = ($input != JNI_FALSE);
%}

%typemap(out) bool %{
	$result = $1 ? JNI_TRUE : JNI_FALSE;
%}


JAVA_TYPEMAP(DbEnv *, com.sleepycat.db.internal.DbEnv, jobject)
%typemap(in) DbEnv * {
    $1 = get_DbEnv(jenv, $input);
}

JAVA_TYPEMAP(DbTxn *, com.sleepycat.db.internal.DbTxn, jobject)
%typemap(in) DbTxn * {
    $1 = get_DbTxn(jenv, $input);
}

//JAVA_TYPEMAP(char *, byte[], jbyteArray)
/*
 * Fake DBT and XmlData * translation to work around SWIG problems
 * with (void *, size_t) typemaps for Java.
 */
JAVA_TYPEMAP(Dbt &, byte[], jbyteArray)
JAVA_TYPEMAP(Dbt *, byte[], jbyteArray)

JAVA_TYPEMAP(XmlData &, byte[], jbyteArray)
JAVA_TYPEMAP(XmlData *, byte[], jbyteArray)

%typemap(in) Dbt & (struct DBT_INFO ldbt) %{
    if ($input != NULL) {
        ldbt.dbt.set_data(ldbt.orig_data = jenv->GetByteArrayElements($input, (jboolean *)0));
        ldbt.dbt.set_size(jenv->GetArrayLength($input));
    }
    ldbt.dbt.set_flags(DB_DBT_MALLOC);
    $1 = &ldbt.dbt;
%}

%typemap(freearg) Dbt & %{
    if ($input != NULL)
        jenv->ReleaseByteArrayElements($input, ldbt$argnum.orig_data, 0);
%}

%typemap(out) const Dbt * %{
    $result = jenv->NewByteArray($1->get_size());
    jenv->SetByteArrayRegion($result, 0, $1->get_size(), (jbyte *)$1->get_data());
%}

%typemap(in) XmlData *, XmlData & (XmlData xml_data) {
    if ($input != NULL) {
        xml_data.set_data(jenv->GetByteArrayElements($input, (jboolean *)0));
        xml_data.set_size(jenv->GetArrayLength($input));
    }
    $1 = &xml_data;
}

%typemap(out) const XmlData *, XmlData * %{
    if ($1) {
	$result = jenv->NewByteArray($1->get_size());
	jenv->SetByteArrayRegion($result, 0, $1->get_size(),
				 (jbyte *)$1->get_data());
	delete $1; // done with new XmlData object
    } else {
	$result = NULL;
    }
%}

JAVA_TYPEMAP(XmlIndexDeclaration *, XmlIndexDeclaration, jobject)
%typemap(out) XmlIndexDeclaration * %{
    if ($1 == NULL)
        $result = NULL;
    else {
        $result = jenv->NewObject(xml_indexdecl_class, xml_indexdecl_construct,
            jenv->NewStringUTF($1->uri.c_str()),
            jenv->NewStringUTF($1->name.c_str()),
            jenv->NewStringUTF($1->index.c_str()));
        delete $1;
    }
%}


// _dbj_* are directly from DB's java_except.i file

%{
static jthrowable make_db_except(JNIEnv *jenv,
				 int err, const char *msg, jobject obj,
				 jobject jdbenv) {

    jthrowable dt = __dbj_get_except(jenv, err, msg, obj, jdbenv);
    return (jthrowable)jenv->
	NewObject(xmlex_class, xmlex_construct,
		  XmlException::DATABASE_ERROR,
		  jenv->NewStringUTF(msg), dt, 0, 0, 0, 0);
}

static jthrowable __dbj_get_except(
   JNIEnv *jenv,
   int err, const char *msg, jobject obj, jobject jdbenv) {
    jobject jmsg;

    if (msg == NULL)
	msg = db_strerror(err);

    jmsg = jenv->NewStringUTF(msg);

    switch (err) {
#if 0
    // don't map these (yet?)
    case EINVAL:
	return (jthrowable)jenv->
	    NewObject(
		      illegalargex_class, illegalargex_construct, jmsg);

    case ENOENT:
	return (jthrowable)jenv->
	    NewObject(filenotfoundex_class, filenotfoundex_construct, jmsg);

    case ENOMEM:
	return (jthrowable)jenv->
	    NewObject(outofmemerr_class, outofmemerr_construct, jmsg);
#endif
    case DB_BUFFER_SMALL:
	return (jthrowable)jenv->
	    NewObject(memex_class, memex_construct, jmsg, obj, err, jdbenv);

    case DB_REP_HANDLE_DEAD:
	return (jthrowable)jenv->
	    NewObject(rephandledeadex_class, rephandledeadex_construct,
		      jmsg, err, jdbenv);
    case DB_RUNRECOVERY:
	return (jthrowable)jenv->
	    NewObject(runrecex_class, runrecex_construct, jmsg, err, jdbenv);

    case DB_LOCK_DEADLOCK:
	return (jthrowable)jenv->
	    NewObject(deadex_class, deadex_construct, jmsg, err, jdbenv);

    case DB_LOCK_NOTGRANTED:
	return (jthrowable)jenv->
	    NewObject(lockex_class,lockex_construct,
		      jmsg, 0, 0, NULL, NULL, 0, jdbenv);

#ifdef DB_EXTRA_EXC
    // These are from DB 4.4, TBD...
    case DB_REP_DUPMASTER:
	return (jthrowable)jenv->
	    NewObject(repdupmasterex_class, repdupmasterex_construct,
		      jmsg, err, jdbenv);

    case DB_REP_HOLDELECTION:
	return (jthrowable)jenv->
	    NewObject(repholdelectionex_class, repholdelectionex_construct,
		      jmsg, err, jdbenv);

    case DB_REP_UNAVAIL:
	return (jthrowable)jenv->
	    NewObject(repunavailex_class, repunavailex_construct,
		      jmsg, err, jdbenv);

    case DB_VERSION_MISMATCH:
	return (jthrowable)jenv->
	    NewObject(versionex_class,
		      versionex_construct, jmsg, 0, 0, NULL, NULL, 0, jdbenv);
#endif //  DB 4.4
    default:
    return (jthrowable)jenv->
	NewObject(dbex_class,
		  dbex_construct, jmsg, err, jdbenv);
    }
}

%}
