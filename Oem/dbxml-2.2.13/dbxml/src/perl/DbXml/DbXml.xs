
//#include <iostream>
#include <fstream>
#include <math.h> //for isnan
#include "dbxml/DbXml.hpp"

using namespace std;
using namespace DbXml;

extern "C" {

  #include "EXTERN.h"
  #include "perl.h"
  #include "XSUB.h"
  
  //#include "../ppport.h"
  
  #include "const-c.inc"

}

#include "../common.h"

typedef Dbt DbtPtr ;


typedef std::string	std__string;

typedef	XmlQueryContext::ReturnType XmlQueryContext__ReturnType;
typedef	XmlQueryContext::EvaluationType XmlQueryContext__EvaluationType;
typedef	XmlException::ExceptionCode XmlException__ExceptionCode;
typedef	XmlValue::Type XmlValue__Type;
typedef	XmlContainer::ContainerType XmlContainer__ContainerType;
//typedef	XmlModify::ModificationType XmlModify__ModificationType;
typedef	XmlModify::XmlObject XmlModify__XmlObject;

typedef Dbt	Dbt_or_pv;
typedef	XmlValue XmlValueOrScalar;
typedef XmlIndexLookup::Operation XmlIndexLookup__Operation;


#if 0
	catch(XmlException & e)
	{
	    cout << "Caught Exception in XSUB " << e.what() << endl ;

	 if (1)
	 {
	    // Take a copy of the C++ exception
	    XmlException * e_copy = new XmlException(e);

	    // Store it in an SV
            SV * except = sv_newmortal();
            sv_setref_pv(except, "XmlException", (void*)e_copy);
            SV * errsv = get_sv("@", TRUE);
	    sv_setsv(errsv, except);
	    croak(Nullch);
	}
	if (0)
	{
	    text = (char*)e.what();
	    value = (int)e.getExceptionCode();
	    dberr = e.getDbErrno();

	    //HV * hash = newHV();
	    //my_hv_store(hash, "-text", newSVpv(e.what(), 0));
	    //my_hv_store(hash, "-value", newSViv((IV)e.getExceptionCode()));
	    //my_hv_store(hash, "-dberrno", newSViv(e.getDbErrno()));
	    //exc = newRV((SV*)hash);
	}
	    if (0)
	    {
	    HV * hash = newHV();


	    //my_hv_store(hash, "-text", newSVpv(e.what(), 0));
	    my_hv_store(hash, "-value", newSViv((IV)e.getExceptionCode()));
	    my_hv_store(hash, "-dberrno", newSViv(e.getDbErrno()));
	    //my_hv_store(except, "__INNER__", newSViv(e_copy)));

	    SV * except = newRV((SV*)hash);

	    sv_bless((SV*)except, gv_stashpv("XmlException", FALSE));
	    //newSVrv((SV*)except, "DbXml::XmlException");

	    // do a die $object
            SV * errsv = get_sv("@", TRUE);
	    sv_setsv(errsv, (SV*)except);
	    croak(Nullch);
	    }
	}
#endif

#if 0

XmlContainer*
XmlContainer::new(...)
	INIT:
	    MY_TRY
	CODE:
	{
		//XmlContainer::new(char * name, u_int32_t flags=0)
	    // Valid forms are
	    //    new(env, name, flags)
	    //    new(env, name)
	    //    new(name)
	    //    new(name, flags)
	    //
	    if (items < 2 || items > 4)
	        Perl_croak(aTHX_ "Usage");

	    DbEnv * env = NULL;
	    u_int32_t flags = 0;
	    int index = 1;

	    // Deal with optional environment first
	    if (Sv_ISA(1, "DbEnv")) {
	        env = GetObjPointer(0, DbEnv);
		++ index ;
	    }

	    if (items < 1)
	        Perl_croak(aTHX_ "Usage");

	    // next thing must be a the name
	    const char* name = GetStringPointer(index) ;

	    if (items == 2)
	        flags = GetUV(index+1, u_int32_t);

	    RETVAL = new XmlContainer(env, name, flags);
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

#endif


#if 0
class MyResolver: public DbXml::XmlResolver
{

  public:
    MyResolver(char* classname): classname(classname)
    {
    }

    virtual ~MyResolver() {}

    virtual bool resolveDocument(XmlTransaction *txn, XmlManager &mgr,
                         const std::string &uri, XmlValue &result) const
    {
    }

    virtual bool resolveCollection(XmlTransaction *txn, XmlManager &mgr,
                           const std::string &uri, XmlResults &result) const
    {
    }

    virtual XmlInputStream *resolveSchema(XmlTransaction *txn, XmlManager &mgr,
                                  const std::string &schemaLocation,
                                  const std::string &nameSpace) const
    {
    }

    virtual XmlInputStream *resolveEntity(XmlTransaction *txn, XmlManager &mgr,
                                  const std::string &systemId,
                                  const std::string &publicId) const 
    {
    }

  private:
    std::string classname ;
};
#endif

static void
dumpXmlValue(XmlValue & value)
{
    if (value.isNull())
        cout << "value is NULL\n" ;
    else
    {
	XmlValue::Type type = value.getType();
	if (type == XmlValue::BOOLEAN)
	    cout << "Type is Boolean, value is " << value.asBoolean() << endl;
	//else if (type == XmlValue::NUMBER)
	    //cout << "Type is Number, value is " << value.asNumber() << endl;
	else if (type == XmlValue::STRING)
	    cout << "Type is String, value is " << value.asString() << endl;
	//else if (type == XmlValue::DOCUMENT) {
	    //cout << "Type is Document, value is "<<  value.asString() << endl;

	//}
	else if (type == XmlValue::NODE)
	    cout << "Type is Node"  << endl;
	//else if (type == XmlValue::VARIABLE)
	    //cout << "Type is Variable"  << endl;
	//else if (type == XmlValue::BINARY)
	    //cout << "Type is Binary"  << endl;
	else if (type == XmlValue::NONE)
	    cout << "Type is None"  << endl;
	else 
	    cout << "Type is Unknown"  << endl;
    }

}


MODULE = DbXml		PACKAGE = DbXml		

INCLUDE: const-xs.inc

MODULE = DbXml		PACKAGE = DbXml		

void
setLogLevel(int level, bool enabled)
	INIT:
	    MY_TRY
	CODE:
	{
	    setLogLevel((LogLevel)level, enabled);
	}
	CLEANUP:
	    MY_CATCH

void
setLogCategory(int category, bool enabled)
	INIT:
	    MY_TRY
	CODE:
	{
	    setLogCategory((LogCategory)category, enabled);
	}
	CLEANUP:
	    MY_CATCH

const char*
dbxml_version(int major, int minor, int patch)
	INIT:
	    MY_TRY
	CODE:
	{
            RETVAL = dbxml_version(&major, &minor, &patch);
	}
	OUTPUT:
	    major
	    minor
	    patch
	    RETVAL
	CLEANUP:
	    MY_CATCH


const char*
xerces_version()
	CODE:
	{
            RETVAL = "whatever";
            //RETVAL = gXercesFullVersionStr;
	}
	OUTPUT:
	    RETVAL

MODULE = XmlManager	PACKAGE = XmlManager

XmlManager*
XmlManager::new(...)
    SV * PARENT = NULL;
	INIT:
	    MY_TRY
    CODE:
	{
	    if (items == 1)
	        RETVAL = new XmlManager();
	    else if (items == 2)
	    {
            if (sv_derived_from(ST(1), "DbEnv"))
            {
                PARENT = ST(1) ;
                DbEnv *dbEnv = GetObjPointer(ST(1), DbEnv);
                RETVAL = new XmlManager(dbEnv);
            }
            else
            {
                RETVAL = new XmlManager((u_int32_t)SvUV(ST(1)));
            }
	    }
	    else if (items == 3)
	    {
            PARENT = ST(1) ;
            DbEnv *dbEnv = GetObjPointer(ST(1), DbEnv);
	        RETVAL = new XmlManager(dbEnv, (u_int32_t)SvUV(ST(2)));
	    }
	    else 
	    {
	        croak("XmlManager::new expects between 0 & 2 parameters");
	    }
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlManager::DESTROY()
	INIT:
        //warn("XmlManager::DESTROY\n");
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void 
XmlManager::setDefaultContainerFlags(u_int32_t flags)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

u_int32_t 
XmlManager::getDefaultContainerFlags()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void 
XmlManager::setDefaultContainerType(XmlContainer::ContainerType type)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

XmlContainer::ContainerType 
XmlManager::getDefaultContainerType()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void 
XmlManager::setDefaultPageSize(u_int32_t pageSize)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

u_int32_t 
XmlManager::getDefaultPageSize()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void 
XmlManager::setDefaultSequenceIncrement(u_int32_t incr)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

u_int32_t
XmlManager::getDefaultSequenceIncrement()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


void 
XmlManager::registerResolver(char *classname)
	INIT:
	    MY_TRY
	CODE:
	{
        //MyResolver res(classname);
	  //  THIS->registerResolver(res);
	}
	CLEANUP:
	    MY_CATCH

int
XmlManager::existsContainer(string name)
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void 
XmlManager::_reindexContainer(XmlTransaction *txn, string name, XmlUpdateContext *context, u_int32_t flags = 0)
    INIT:
        MY_TRY
    CODE:
    {
        if (txn)
            THIS->reindexContainer(*txn, name, *context, flags);
        else
            THIS->reindexContainer(name, *context, flags);
    }
    CLEANUP:
        MY_CATCH

XmlIndexLookup *
XmlManager::createIndexLookup( XmlContainer *container, string uri, string name, string index, SV *value = NULL, XmlIndexLookup::Operation op = XmlIndexLookup::EQ)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlIndexLookup";
	INIT:
	    MY_TRY
	CODE:
    {
        XmlValue * v ;
        getXmlValue(v, value, lex);

        RETVAL = new XmlIndexLookup(
                        THIS->createIndexLookup(*container, uri, 
                                    name, index, *v, op));
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlContainer *
XmlManager::_createContainer(XmlTransaction *txn, string name, u_int32_t flags=THIS->getDefaultContainerFlags(), XmlContainer::ContainerType type=THIS->getDefaultContainerType(), int mode=0)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlContainer";
	INIT:
	    MY_TRY
    CODE:
	{
        if (txn)
            RETVAL = new XmlContainer(
                THIS->createContainer(*txn, name, flags, type, mode));
        else
            RETVAL = new XmlContainer(
                THIS->createContainer(name, flags, type, mode));
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH




XmlContainer *
XmlManager::_openContainer(...)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlContainer";
	INIT:
	    MY_TRY
	CODE:
    {
        bool ok = true ;

        if (SvOK(ST(1)))
        {
            XmlTransaction * txn = GetObjPointer(ST(1), XmlTransaction);

            if (items == 3)
            {
                RETVAL = new XmlContainer(
                            THIS->openContainer(*txn, SvPV_nolen(ST(2))));
            }
            else if (items == 4)
            {
                RETVAL = new XmlContainer(
                            THIS->openContainer(*txn, SvPV_nolen(ST(2)),
                                                    SvUV(ST(3))));
            }
            else
            {
                ok = false ;
            }
        }
        else
        {
            if (items == 3)
            {
                RETVAL = new XmlContainer(
                            THIS->openContainer(SvPV_nolen(ST(2))));
            }
            else if (items == 4)
            {
                RETVAL = new XmlContainer(
                            THIS->openContainer(SvPV_nolen(ST(2)),
                                                    SvUV(ST(3))));
            }
            else
            {
                ok = false ;
            }
        }

	    if (! ok)
	    {
	        croak("XmlManager::createContainer expects between 1 & 5 parameters");
	    }
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


void 
XmlManager::_removeContainer(XmlTransaction* txn, string name)
	INIT:
	    MY_TRY
	CODE:
    {
        if (txn)
            THIS->removeContainer(*txn, name);
        else
            THIS->removeContainer(name);
    }
	CLEANUP:
	    MY_CATCH

void 
XmlManager::_renameContainer(XmlTransaction* txn, string oldName, string newName)
	INIT:
	    MY_TRY
	CODE:
    {
        if (txn)
            THIS->renameContainer(*txn, oldName, newName);
        else
            THIS->renameContainer(oldName, newName);
    }
	CLEANUP:
	    MY_CATCH


void 
XmlManager::dumpContainer(string name, char *out)
	INIT:
	    MY_TRY
	CODE:
	{
        std::ofstream out_stream(out) ;
	    THIS->dumpContainer(name, &out_stream);
	}
	CLEANUP:
	    MY_CATCH

void 
XmlManager::loadContainer(string name, char *filename, SV*lineno=0, XmlUpdateContext *context=0)
	INIT:
	    MY_TRY
	CODE:
	{
        unsigned long line = 0;
        LexPointer<XmlUpdateContext> lex ;   

        if (lineno)
            line = (unsigned long)SvUV(lineno);

        if (! context)
        {
    line = 0 ;
            context = new XmlUpdateContext(THIS->createUpdateContext()) ;
            lex.setPtr(context);
        }

        std::ifstream in(filename) ;
        THIS->loadContainer(name, &in, &line, *context);

        if (lineno)
            sv_setuv(lineno, (UV)line);

	}
	CLEANUP:
	    MY_CATCH

void 
XmlManager::verifyContainer(string name, char *out, u_int32_t flags = 0)
	INIT:
	    MY_TRY
	CODE:
	{
        std::ofstream out_stream(out) ;
	    THIS->verifyContainer(name, &out_stream, flags);
	}
	CLEANUP:
	    MY_CATCH

void 
XmlManager::upgradeContainer(string name, XmlUpdateContext * context=0)
	INIT:
	    MY_TRY
	CODE:
	{
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->createUpdateContext()) ;
            lex.setPtr(context);
        }

        THIS->upgradeContainer(name, *context);

	}
	CLEANUP:
	    MY_CATCH


XmlDocument *
XmlManager::createDocument()
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlDocument";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = new XmlDocument(THIS->createDocument());
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlQueryContext *
XmlManager::createQueryContext( XmlQueryContext::ReturnType rt = XmlQueryContext::LiveValues, XmlQueryContext::EvaluationType et = XmlQueryContext::Eager)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlQueryContext";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = new XmlQueryContext(
			    THIS->createQueryContext(rt, et));
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlUpdateContext *
XmlManager::createUpdateContext()
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlUpdateContext";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = new XmlUpdateContext(
			    THIS->createUpdateContext());
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlQueryExpression *
XmlManager::_prepare(XmlTransaction *txn, string queryString, XmlQueryContext *context=NULL)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlQueryExpression";
	INIT:
	    MY_TRY
	CODE:
	{
        LexPointer<XmlQueryContext> lex ;   

        if (! context)
        {
            context = new XmlQueryContext(THIS->createQueryContext()) ;
            lex.setPtr(context);
        }

        if (txn)
        {
            RETVAL = new XmlQueryExpression(
                    THIS->prepare(*txn, queryString, *context));
        }
        else
        {
            RETVAL = new XmlQueryExpression(
                    THIS->prepare(queryString, *context));
        }

	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlResults *
XmlManager::_query(XmlTransaction* txn, string queryString, XmlQueryContext *context=NULL, u_int32_t flags = 0)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlResults";
	INIT:
	    MY_TRY
	CODE:
	{
        LexPointer<XmlQueryContext> lex ;   
        
        if (! context)
        {
            context = new XmlQueryContext(THIS->createQueryContext()) ;
            lex.setPtr(context);
        }

        if (txn)
        {
            RETVAL = new XmlResults(
                        THIS->query(*txn, queryString, *context, flags));
        }
        else
        {
            RETVAL = new XmlResults(
                        THIS->query(queryString, *context, flags));
        }

	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlResults *
XmlManager::createResults()
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlResults";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = new XmlResults(
			    THIS->createResults());
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlModify *
XmlManager::createModify()
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlModify";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = new XmlModify(
			    THIS->createModify());
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlTransaction *
XmlManager::createTransaction(...)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlTransaction";
	INIT:
	    MY_TRY
	CODE:
	{
	    if (items == 1)
        {
	        RETVAL = new XmlTransaction(
                    THIS->createTransaction());
        }
        else if (items == 2)
        {
            SV* toUse = ST(1) ;
            if (sv_derived_from(toUse, "DbTxn"))
            {
                DbTxn * txn_ptr = GetObjPointer(toUse, DbTxn);
                RETVAL = new XmlTransaction(
                        THIS->createTransaction(txn_ptr));
            }
            else
            {
                RETVAL = new XmlTransaction(
                        THIS->createTransaction(SvUV(toUse)));
            }
        }
        else
	        croak("createTransaction expects 0 or 1 parameter");
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


DbEnv *
XmlManager::getDbEnv()
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"DbEnv";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = THIS->getDbEnv();
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

string
XmlManager::getHome() 
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


XmlInputStream *
XmlManager::createStdInInputStream()
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlInputStream";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = THIS->createStdInInputStream();
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlInputStream *
XmlManager::createLocalFileInputStream(string filename)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlInputStream";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = THIS->createLocalFileInputStream(filename);
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlInputStream *
XmlManager::createURLInputStream(char* baseId, char* systemId, char* publicId=NULL)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlInputStream";
	INIT:
	    MY_TRY
	CODE:
	{
        if (publicId == NULL)
            RETVAL = THIS->createURLInputStream(baseId, systemId);
        else
            RETVAL = THIS->createURLInputStream(baseId, systemId, publicId);
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


MODULE = XmlContainer	PACKAGE = XmlContainer

void
XmlContainer::DESTROY()
	INIT:
        //warn("XmlContainer::DESTROY\n");
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
XmlContainer::sync()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


XmlContainer::ContainerType 
XmlContainer::getContainerType()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

bool
XmlContainer::getIndexNodes()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlManager *
XmlContainer::getManager()
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlManager";
	INIT:
	    MY_TRY
	CODE:
	{
        RETVAL = new XmlManager(
                THIS->getManager());
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlContainer::deleteDocument_1(XmlTransaction* txn, XmlDocument* document, XmlUpdateContext *context=0)
	INIT:
	    MY_TRY
	CODE:
	{
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }

        if (txn)
            THIS->deleteDocument(*txn, *document, *context);
        else
            THIS->deleteDocument(*document, *context);

	}
	CLEANUP:
	    MY_CATCH

void
XmlContainer::deleteDocument_2(XmlTransaction* txn, string name , XmlUpdateContext *context=0)
	INIT:
	    MY_TRY
	CODE:
	{        
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }


        if (txn)
            THIS->deleteDocument(*txn, name, *context);
        else
            THIS->deleteDocument(name, *context);

	}
	CLEANUP:
	    MY_CATCH

void
XmlContainer::_updateDocument(XmlTransaction* txn, XmlDocument* document, XmlUpdateContext *context=0)
	INIT:
	    MY_TRY
	CODE:
	{
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }

        if (txn)
            THIS->updateDocument(*txn, *document, *context);
        else
            THIS->updateDocument(*document, *context);
	}
	CLEANUP:
	    MY_CATCH


void
XmlContainer::_setIndexSpecification(XmlTransaction *txn, XmlIndexSpecification * index, XmlUpdateContext *context=0)
	INIT:
	    MY_TRY
	CODE:
	{        
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }

        if (txn)
            THIS->setIndexSpecification(*txn, *index, *context);
        else
            THIS->setIndexSpecification(*index, *context);
	}
	CLEANUP:
	    MY_CATCH

XmlIndexSpecification *
XmlContainer::_getIndexSpecification(XmlTransaction *txn, u_int32_t flags = 0)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlIndexSpecification";
	INIT:
	    MY_TRY
	CODE:
	{
        if (txn)
            RETVAL = new XmlIndexSpecification(
                    THIS->getIndexSpecification(*txn, flags));
        else
            RETVAL = new XmlIndexSpecification(
                    THIS->getIndexSpecification());
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void	    
XmlContainer::_addIndex(XmlTransaction * txn, string uri, string name, string index, XmlUpdateContext *context=0)
	INIT:
	    MY_TRY
	CODE:
	{
        // TODO --     void addIndex(XmlTransaction &txn,
        //                    const std::string &uri, const std::string &name,
        //                    XmlIndexSpecification::Type indexType,
        //                    XmlValue::Type syntaxType, XmlUpdateContext &context);
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }

        if (txn)
            THIS->addIndex(*txn, uri, name, index, *context);
        else
            THIS->addIndex(uri, name, index, *context);
	}
	CLEANUP:
	    MY_CATCH

void	    
XmlContainer::_deleteIndex(XmlTransaction * txn, string uri, string name, string index,  XmlUpdateContext *context=0)
	INIT:
	    MY_TRY
	CODE:
	{        
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }


        if (txn)
            THIS->deleteIndex(*txn, uri, name, index, *context);
        else
            THIS->deleteIndex(uri, name, index, *context);
	}
	CLEANUP:
	    MY_CATCH

void	    
XmlContainer::_replaceIndex(XmlTransaction * txn, string uri, string name, string index, XmlUpdateContext *context=0)
	INIT:
	    MY_TRY
	CODE:
	{        
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }

        if (txn)
            THIS->replaceIndex(*txn, uri, name, index, *context);
        else
            THIS->replaceIndex(uri, name, index, *context);

	}
	CLEANUP:
	    MY_CATCH


void	    
XmlContainer::_addDefaultIndex(XmlTransaction * txn, string index, XmlUpdateContext *context=0)
	INIT:
	    MY_TRY
	CODE:
	{        
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }

        if (txn)
            THIS->addDefaultIndex(*txn, index, *context);
        else
            THIS->addDefaultIndex(index, *context);

	}
	CLEANUP:
	    MY_CATCH



void	    
XmlContainer::_deleteDefaultIndex(XmlTransaction * txn, string index, XmlUpdateContext *context=0)
	INIT:
	    MY_TRY
	CODE:
	{
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }

        if (txn)
            THIS->deleteDefaultIndex(*txn, index, *context);
        else
            THIS->deleteDefaultIndex(index, *context);

	}
	CLEANUP:
	    MY_CATCH



void	    
XmlContainer::_replaceDefaultIndex(XmlTransaction * txn, string index, XmlUpdateContext *context=0)
	INIT:
	    MY_TRY
	CODE:
	{
        LexPointer<XmlUpdateContext> lex ;   

        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }

        if (txn)
            THIS->replaceDefaultIndex(*txn, index, *context);
        else
            THIS->replaceDefaultIndex(index, *context);
	}
	CLEANUP:
	    MY_CATCH


string
XmlContainer::getName()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlDocument* 	
XmlContainer::_getDocument(XmlTransaction* txn, string name, u_int32_t flags=0)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlDocument";
	INIT:
	    MY_TRY
	CODE:
	{
        if (txn)
            RETVAL = new XmlDocument(THIS->getDocument(*txn, name, flags));
        else
            RETVAL = new XmlDocument(THIS->getDocument(name, flags));
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


void
XmlContainer::_putDocument_1(XmlTransaction* txn, XmlDocument * document, XmlUpdateContext *context=0, u_int32_t flags=0)
	INIT:
	  MY_TRY
	CODE:
	{
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }
        
        if (txn)
            THIS->putDocument(*txn, *document, *context, flags);
        else
            THIS->putDocument(*document, *context, flags);
	}
	CLEANUP:
	    MY_CATCH

string
XmlContainer::_putDocument_2(XmlTransaction* txn, string name, SV* contents, XmlUpdateContext *context=0, u_int32_t flags=0)
	INIT:
	  MY_TRY
	CODE:
	{
        LexPointer<XmlUpdateContext> lex ;   
        
        if (! context)
        {
            context = new XmlUpdateContext(THIS->getManager().createUpdateContext()) ;
            lex.setPtr(context);
        }
        
        if (! sv_isobject(contents)) {
            STRLEN len ;
            const char * ptr = SvPV(contents, len);
            string s(ptr, len);

            if (txn)
                RETVAL = THIS->putDocument(*txn, name, s, *context, flags);
            else
                RETVAL = THIS->putDocument(name, s, *context, flags);
        }
        else if (sv_derived_from(contents, "XmlInputStream"))
        {
            XmlInputStream *s = GetObjPointer(contents, XmlInputStream);
            if (txn)
                RETVAL = THIS->putDocument(*txn, name, s, *context, flags);
            else
                RETVAL = THIS->putDocument(name, s, *context, flags);
            ClearObjPointer(contents);
        }
        else 
            croak("putDocument parameter 2 should be an XmlInputStream or a string");
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


XmlResults* 	
XmlContainer::_lookupIndex_1(XmlTransaction *txn, XmlQueryContext *context, string uri, string name, string index, SV *value = NULL, u_int32_t flags = 0)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlResults";
	INIT:
	    MY_TRY
	CODE:
	{
        XmlValue * v ;
        getXmlValue(v, value, lex);

        if (txn)
            RETVAL = new XmlResults(THIS->lookupIndex(*txn, *context, uri, 
                                    name, index, *v, flags));
        else
            RETVAL = new XmlResults(THIS->lookupIndex(*context, uri, name, 
                                    index, *v, flags));
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlResults* 	
XmlContainer::_lookupIndex_2(XmlTransaction *txn, XmlQueryContext *context, string uri, string name, string parent_uri, string parent_name, string index, SV *value = NULL, u_int32_t flags = 0)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlResults";
	INIT:
	    MY_TRY
	CODE:
	{
        XmlValue * v ;
        getXmlValue(v, value, lex);

        if (txn)
            RETVAL = new XmlResults(THIS->lookupIndex(*txn, *context, uri, 
                    name, parent_uri, parent_name, index, *v, flags));
        else
            RETVAL = new XmlResults(THIS->lookupIndex(*context, uri, name, 
                    parent_uri, parent_name, index, *v, flags));
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlStatistics* 	
XmlContainer::_lookupStatistics_1(XmlTransaction *txn, string uri, string name, string index, SV *value = NULL)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlStatistics";
	INIT:
	    MY_TRY
	CODE:
	{
        XmlValue * v ;
        getXmlValue(v, value, lex);

        if (txn)
            RETVAL = new XmlStatistics(THIS->lookupStatistics(*txn, uri, 
                                        name, index, *v));
        else
            RETVAL = new XmlStatistics(THIS->lookupStatistics(uri, name, 
                                            index, *v));
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlStatistics* 	
XmlContainer::_lookupStatistics_2(XmlTransaction *txn, string uri, string name, string parent_uri, string parent_name, string index, SV *value = NULL)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlStatistics";
	INIT:
	    MY_TRY
	CODE:
	{
        XmlValue * v ;
        getXmlValue(v, value, lex);

        if (txn)
            RETVAL = new XmlStatistics(THIS->lookupStatistics(*txn, uri, 
                        name, parent_uri, parent_name, index, *v));
        else
            RETVAL = new XmlStatistics(THIS->lookupStatistics(uri, name, 
                        parent_uri, parent_name, index, *v));
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

bool
XmlContainer::addAlias(string alias)
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

bool
XmlContainer::removeAlias(string alias)
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


XmlResults*
XmlContainer::_getAllDocuments(XmlTransaction *txn, u_int32_t flags=0)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlResults";
	INIT:
	    MY_TRY
	CODE:
	{
        if (txn)
            RETVAL = new XmlResults(THIS->getAllDocuments(*txn, flags));
        else
            RETVAL = new XmlResults(THIS->getAllDocuments(flags));
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


size_t
XmlContainer::_getNumDocuments(XmlTransaction *txn)
	INIT:
	    MY_TRY
	CODE:
	{
        if (txn)
            RETVAL = THIS->getNumDocuments(*txn);
        else
            RETVAL = THIS->getNumDocuments();
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


MODULE = XmlDocument	PACKAGE = XmlDocument

void
XmlDocument::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
XmlDocument::setContent(string content)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


void
XmlDocument::setName(string name)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

string
XmlDocument::getContent()
	INIT:
	    MY_TRY
	CODE:
	{
	    THIS->getContent(RETVAL);
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlInputStream *
XmlDocument::getContentAsXmlInputStream()
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlInputStream";
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlDocument::setContentAsXmlInputStream(XmlInputStream *adopted_str)
	INIT:
	    MY_TRY
	CLEANUP:
        ClearObjPointer(ST(1));
	    MY_CATCH

string
XmlDocument::getName()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlDocument::fetchAllData()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
XmlDocument::setMetaData(string uri, string name, SV* value)
    INIT:
        MY_TRY
    CODE:
    {
        if (! sv_isobject(value)) {
            std::string s;
            //StringfromSV(s, value);
            //setXmlDatafromSV(xdata, value);
            //setXmlValuefromSV(xdata, value);
            //XmlValue xdata(XmlValue::STRING, s);
            XmlValue xdata(XmlValue::STRING, (const char*)SvPV_nolen(value));
            THIS->setMetaData(uri, name, xdata);
        }
        else if (sv_derived_from(value, "XmlValue"))
            THIS->setMetaData(uri, name, *GetObjPointer(value, XmlValue));
        else
            croak("setMetaData parameter 2 should be an XmlValue or a string");
    }
    CLEANUP:
        MY_CATCH


void
XmlDocument::removeMetaData(string uri, string name)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


bool
XmlDocument::getMetaData(string uri, string name, SV* value)
	INIT:
	    MY_TRY
	CODE:
	{
	    if (! sv_isobject(value)) {
            XmlData v ;
            if (RETVAL = THIS->getMetaData(uri, name, v))
                setSVfromXmlData(value, v);
	    }
	    else if (sv_derived_from(value, "XmlValue"))
            RETVAL = THIS->getMetaData(uri, name, *GetObjPointer(value, XmlValue));
	    else 
	        croak("getMetaData parameter 2 should be an XmlValue or a string");
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlMetaDataIterator *
XmlDocument::getMetaDataIterator()
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlMetaDataIterator";
	INIT:
	    MY_TRY
	CODE:
	{
        RETVAL = new XmlMetaDataIterator(THIS->getMetaDataIterator()) ;
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


MODULE = XmlQueryContext	PACKAGE = XmlQueryContext


void
XmlQueryContext::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
XmlQueryContext::clearNamespaces()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH
	
void
XmlQueryContext::setNamespace(string prefix, string uri)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH
	
string
XmlQueryContext::getNamespace(string prefix)
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlQueryContext::removeNamespace(string prefix)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH
	
void
XmlQueryContext::setEvaluationType(XmlQueryContext::EvaluationType type)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH
	
XmlQueryContext::EvaluationType
XmlQueryContext::getEvaluationType()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


void
XmlQueryContext::setReturnType(XmlQueryContext::ReturnType type);
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH
	
XmlQueryContext::ReturnType
XmlQueryContext::getReturnType()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlQueryContext::setDefaultCollection(string uri)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


string
XmlQueryContext::getDefaultCollection()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


void
XmlQueryContext::setVariableValue(string name, SV * value);
	INIT:
	    MY_TRY
	CODE:
	{
        if (value == NULL || ! SvOK(value))
        {
            XmlValue v ;
            THIS->setVariableValue(name, v);
        }
	    else if (SvOK(value) && sv_derived_from(value, "XmlValue"))
        {
            XmlValue * ptr = GetObjPointer(value, XmlValue);
            THIS->setVariableValue(name, *ptr);
        }
	    else if (SvOK(value) && sv_derived_from(value, "XmlResults"))
        {
            XmlResults * ptr = GetObjPointer(value, XmlResults);
            THIS->setVariableValue(name, *ptr);
        }
        else //if (! sv_isobject(value) )
        {
            string s(SvPV_nolen(value), SvCUR(value));
            XmlValue v(s) ;
            THIS->setVariableValue(name, v);
        }
	}
	CLEANUP:
	    MY_CATCH

bool
XmlQueryContext::getVariableValue(string name, SV * value);
	INIT:
	    MY_TRY
	CODE:
	{
	    if (SvOK(value) && sv_derived_from(value, "XmlValue"))
        {
            XmlValue * ptr = GetObjPointer(value, XmlValue);
            RETVAL = THIS->getVariableValue(name, *ptr);
        }
	    else if (SvOK(value) && sv_derived_from(value, "XmlResults"))
        {
            XmlResults * ptr = GetObjPointer(value, XmlResults);
            RETVAL = THIS->getVariableValue(name, *ptr);
        }
	    else 
        {
            XmlValue v ;
            RETVAL = THIS->getVariableValue(name, v);
            if (RETVAL)
                setSVfromXmlValue(value, v);
        }
	}
	CLEANUP:
	    MY_CATCH


void
XmlQueryContext::setBaseURI(string baseURI);
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

string
XmlQueryContext::getBaseURI()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


MODULE = XmlValue	PACKAGE = XmlValue

XmlValue*
XmlValue::new(...)
    SV * PARENT = NULL;
	INIT:
	    MY_TRY
	CODE:
	{
	    if (items == 1)
	        RETVAL = new XmlValue();
	    else if (items == 2)
	    {
            if (sv_derived_from(ST(1), "XmlDocument"))
            {
                XmlDocument * ptr = GetObjPointer(ST(1), XmlDocument);
                RETVAL = new XmlValue(*ptr);
            }
            else if (sv_derived_from(ST(1), "XmlValue"))
            {
                XmlValue * ptr = GetObjPointer(ST(1), XmlValue);
                RETVAL = new XmlValue(*ptr);
            }
            else
            {
                STRLEN len;
                char* ptr = SvPV(ST(1), len);
                const XmlData str(ptr, len);
                RETVAL = new XmlValue(XmlValue::STRING, str);
            }
	    }
	    else if (items == 3)
	    {
	        XmlValue::Type type = (XmlValue::Type)SvUV(ST(1));
	        SV * value = ST(2);

	        if (type == XmlValue::BOOLEAN)
		    // The Solaris C++ compiler doesn't like this
		    // so create a boolean in a more roundabout way.
	            //RETVAL = new XmlValue((bool)SvIV(value));
	            RETVAL = new XmlValue(type, SvIV(value) ? "true" : "false");
	        else if (type == XmlValue::DOUBLE)
	            RETVAL = new XmlValue((double)SvNV(value));
	        else {
	            STRLEN len;
	            char* ptr = SvPV(value, len);
	            const XmlData str(ptr, len);
	            RETVAL = new XmlValue(type, str);
	        }
	    }
	    else 
	    {
	        croak("XmlValue::new expects between 0 & 2 parameters");
	    }
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlValue::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

XmlValue::Type
XmlValue::getType()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

string
XmlValue::getNodeName()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

string
XmlValue::getNodeValue()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

string
XmlValue::getNamespaceURI()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

string
XmlValue::getPrefix()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

string
XmlValue::getLocalName()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

short
XmlValue::getNodeType()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlValue *
XmlValue::getParentNode()
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlValue";
	INIT:
	    MY_TRY
    CODE:
    {
        RETVAL = new XmlValue(THIS->getParentNode());
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlValue *
XmlValue::getFirstChild()
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlValue";
	INIT:
	    MY_TRY
    CODE:
    {
        RETVAL = new XmlValue(THIS->getFirstChild());
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlValue *
XmlValue::getLastChild()
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlValue";
	INIT:
	    MY_TRY
    CODE:
    {
        RETVAL = new XmlValue(THIS->getLastChild());
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlValue *
XmlValue::getPreviousSibling()
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlValue";
	INIT:
	    MY_TRY
    CODE:
    {
        RETVAL = new XmlValue(THIS->getPreviousSibling());
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlValue *
XmlValue::getNextSibling()
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlValue";
	INIT:
	    MY_TRY
    CODE:
    {
        RETVAL = new XmlValue(THIS->getNextSibling());
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlResults *
XmlValue::getAttributes()
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlResults";
	INIT:
	    MY_TRY
    CODE:
    {
        RETVAL = new XmlResults(THIS->getAttributes());
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlValue *
XmlValue::getOwnerElement()
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlValue";
	INIT:
	    MY_TRY
    CODE:
    {
        RETVAL = new XmlValue(THIS->getOwnerElement());
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

bool
XmlValue::isNumber()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

bool
XmlValue::isString()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

bool
XmlValue::isBinary()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

bool
XmlValue::isBoolean()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

bool
XmlValue::isNode()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


bool
XmlValue::isNull()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

bool
XmlValue::isType(XmlValue::Type type)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

SV*
XmlValue::asNumber()
	INIT:
	    MY_TRY
	CODE:
	{
	    double num = THIS->asNumber();
	    if (isnan(num))
		RETVAL = &PL_sv_undef ;
	    else
                RETVAL = newSVnv(num);

	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

string
XmlValue::asString()
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = THIS->asString();
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

bool
XmlValue::asBoolean()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

XmlDocument*
XmlValue::asDocument()
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlDocument";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = new XmlDocument(THIS->asDocument());
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

SV*
XmlValue::asBinary()
	INIT:
	    MY_TRY
	CODE:
	{
        //RETVAL = setSVfromXmlData(RETVAL, THIS->asBinary());
        RETVAL = newSVfromXmlData(THIS->asBinary());
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


bool 
XmlValue::equals(XmlValue *v) 
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = THIS->equals(*v);
	}
	CLEANUP:
	    MY_CATCH


MODULE = XmlResults	PACKAGE = XmlResults


void
XmlResults::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

bool
XmlResults::next(SV * value)
	INIT:
	    MY_TRY
	CODE:
	{
	    if (SvOK(value) && sv_derived_from(value, "XmlValue"))
        {
            XmlValue * ptr = GetObjPointer(value, XmlValue);
            RETVAL = THIS->next(*ptr);
        }
	    else if (SvOK(value) && sv_derived_from(value, "XmlDocument"))
        {
            XmlDocument * ptr = GetObjPointer(value, XmlDocument);
            RETVAL = THIS->next(*ptr);
        }
	    else 
        {
            XmlValue v ;
            RETVAL = THIS->next(v);
            if (RETVAL)
                setSVfromXmlValue(value, v);
        }
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

bool
XmlResults::previous(SV * value)
	INIT:
	    MY_TRY
	CODE:
	{
	    if (sv_derived_from(value, "XmlValue"))
        {
            XmlValue * ptr = GetObjPointer(value, XmlValue);
            RETVAL = THIS->previous(*ptr);
        }
	    else if (sv_derived_from(value, "XmlDocument"))
        {
            XmlDocument * ptr = GetObjPointer(value, XmlDocument);
            RETVAL = THIS->previous(*ptr);
        }
	    else 
        {
            XmlValue v ;
            RETVAL = THIS->previous(v);
            if (RETVAL)
                setSVfromXmlValue(value, v);
        }

	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

bool
XmlResults::peek(SV * value)
	INIT:
	    MY_TRY
	CODE:
	{
	    if (sv_derived_from(value, "XmlValue"))
        {
            XmlValue * ptr = GetObjPointer(value, XmlValue);
            RETVAL = THIS->peek(*ptr);
        }
	    else if (sv_derived_from(value, "XmlDocument"))
        {
            XmlDocument * ptr = GetObjPointer(value, XmlDocument);
            RETVAL = THIS->peek(*ptr);
        }
	    else 
        {
            XmlValue v ;
            RETVAL = THIS->peek(v);
            if (RETVAL)
                setSVfromXmlValue(value, v);
        }
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH


size_t
XmlResults::size()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
XmlResults::reset()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

bool
XmlResults::hasNext()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

bool
XmlResults::hasPrevious()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
XmlResults::add(XmlValueOrScalar * value)
	INIT:
	    MY_TRY
    CODE:
    {
        THIS->add(value) ;
    }
	CLEANUP:
	    MY_CATCH

MODULE = XmlUpdateContext	PACKAGE = XmlUpdateContext

void
XmlUpdateContext::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
XmlUpdateContext::setApplyChangesToContainers(bool applyChanges)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

bool
XmlUpdateContext::getApplyChangesToContainers()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

MODULE = XmlException	PACKAGE = XmlException

#define XmlException    MyXmlException

void
XmlException::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int
XmlException::getDbErrno()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

XmlException::ExceptionCode
XmlException::getExceptionCode()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

MODULE = XmlIndexSpecification	PACKAGE = XmlIndexSpecification

XmlIndexSpecification*
XmlIndexSpecification::new()
    SV * PARENT = NULL;
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void
XmlIndexSpecification::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void	    
XmlIndexSpecification::addIndex(string uri, string name, string index)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void	    
XmlIndexSpecification::deleteIndex(string uri, string name, string index)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void	    
XmlIndexSpecification::replaceIndex(string uri, string name, string index)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void	    
XmlIndexSpecification::addDefaultIndex(string index)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void	    
XmlIndexSpecification::deleteDefaultIndex(string index)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void	    
XmlIndexSpecification::replaceDefaultIndex(string index)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

string	    
XmlIndexSpecification::getDefaultIndex()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

bool	    
XmlIndexSpecification::find(uri, name, index)
	INPUT:
	    string uri
	    string name
	    string index = NO_INIT
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	    index
	CLEANUP:
	    MY_CATCH

bool	    
XmlIndexSpecification::next(uri, name, index)
	INPUT:
	    string uri = NO_INIT
	    string name = NO_INIT
	    string index = NO_INIT
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	    uri
	    name
	    index
	CLEANUP:
	    MY_CATCH

void	    
XmlIndexSpecification::reset()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

int	    
getValueType(string index)
	INIT:
	    MY_TRY
    CODE:
    {
        RETVAL = (int)XmlIndexSpecification::getValueType(index);
    }
    OUTPUT:
        RETVAL
	CLEANUP:
	    MY_CATCH

MODULE = XmlQueryExpression	PACKAGE = XmlQueryExpression

void
XmlQueryExpression::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

string
XmlQueryExpression::getQuery()
        INIT:
            MY_TRY
        OUTPUT:
            RETVAL
        CLEANUP:
            MY_CATCH

string
XmlQueryExpression::getQueryPlan()
        INIT:
            MY_TRY
        OUTPUT:
            RETVAL
        CLEANUP:
            MY_CATCH


XmlResults *
XmlQueryExpression::_execute1(XmlTransaction *txn, XmlValueOrScalar *contextItem, XmlQueryContext *context, u_int32_t flags = 0)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlResults";
        INIT:
            MY_TRY
        CODE:
        {
            if (txn)
            {
                RETVAL = new XmlResults(THIS->execute(*txn, *contextItem, *context, flags));
            }
            else
            {
                RETVAL = new XmlResults(THIS->execute(*contextItem, *context, flags));
            }
        }
        OUTPUT:
            RETVAL
        CLEANUP:
            MY_CATCH


XmlResults *
XmlQueryExpression::_execute2(XmlTransaction *txn, XmlQueryContext *context, u_int32_t flags = 0)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlResults";
        INIT:
            MY_TRY
        CODE:
        {
            if (txn)
            {
                RETVAL = new XmlResults(THIS->execute(*txn, *context, flags));
            }
            else
            {
                RETVAL = new XmlResults(THIS->execute(*context, flags));
            }
            
        }
        OUTPUT:
            RETVAL
        CLEANUP:
            MY_CATCH



MODULE = XmlModify	PACKAGE = XmlModify

void
XmlModify::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


void
XmlModify::setNewEncoding(string newEncoding)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void 
XmlModify::addInsertBeforeStep(XmlQueryExpression *selectionExpr, XmlModify::XmlObject type, string name, string content);
	INIT:
	    MY_TRY
	CODE:
	{
	    THIS->addInsertBeforeStep(*selectionExpr, type, name, content);
	}
	CLEANUP:
	    MY_CATCH

void 
XmlModify::addInsertAfterStep(XmlQueryExpression *selectionExpr, XmlModify::XmlObject type, string name, string content);
	INIT:
	    MY_TRY
	CODE:
	{
	    THIS->addInsertAfterStep(*selectionExpr, type, name, content);
	}
	CLEANUP:
	    MY_CATCH


void 
XmlModify::addAppendStep(XmlQueryExpression *selectionExpr, XmlModify::XmlObject type, string name, string content, int location = -1);
	INIT:
	    MY_TRY
	CODE:
	{
	    THIS->addAppendStep(*selectionExpr, type, name, content, location);
	}
	CLEANUP:
	    MY_CATCH


void 
XmlModify::addUpdateStep(XmlQueryExpression *selectionExpr, string content);
	INIT:
	    MY_TRY
	CODE:
	{
	    THIS->addUpdateStep(*selectionExpr, content);
	}
	CLEANUP:
	    MY_CATCH


void 
XmlModify::addRemoveStep(XmlQueryExpression *selectionExpr);
	INIT:
	    MY_TRY
	CODE:
	{
	    THIS->addRemoveStep(*selectionExpr);
	}
	CLEANUP:
	    MY_CATCH


void 
XmlModify::addRenameStep(XmlQueryExpression *selectionExpr, string newName);
	INIT:
	    MY_TRY
	CODE:
	{
	    THIS->addRenameStep(*selectionExpr, newName);
	}
	CLEANUP:
	    MY_CATCH


	    
unsigned int
XmlModify::_execute1(XmlTransaction *txn, XmlValueOrScalar *toModify, XmlQueryContext *context, XmlUpdateContext *uc)
    INIT:
        MY_TRY
    CODE:
    {
        if (txn)
            RETVAL = THIS->execute(*txn, *toModify, *context, *uc);
        else
            RETVAL = THIS->execute(*toModify, *context, *uc);

    }
    OUTPUT:
        RETVAL
    CLEANUP:
            MY_CATCH

unsigned int
XmlModify::_execute2(XmlTransaction *txn, XmlResults *toModify, XmlQueryContext *context, XmlUpdateContext *uc)
    INIT:
        MY_TRY
    CODE:
    {
        if (txn)
            RETVAL = THIS->execute(*txn, *toModify, *context, *uc);
        else
            RETVAL = THIS->execute(*toModify, *context, *uc);
    }
    OUTPUT:
        RETVAL
    CLEANUP:
        MY_CATCH

MODULE = XmlInputStream	PACKAGE = XmlInputStream

void
XmlInputStream::DESTROY()
	INIT:
	    MY_TRY
    CODE:
        if (THIS)
            delete THIS;
	CLEANUP:
	    MY_CATCH

unsigned int 
XmlInputStream::curPos() 
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

MODULE = XmlMetaDataIterator	PACKAGE = XmlMetaDataIterator

void
XmlMetaDataIterator::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

bool 
XmlMetaDataIterator::next(uri, name, value)
	    string uri = NO_INIT
	    string name = NO_INIT
	    SV* value 
        INIT:
            MY_TRY
        CODE:
        {
            if (! sv_isobject(value)) {
                XmlValue v ;
                if (RETVAL = THIS->next(uri, name, v))
                    setSVfromXmlValue(value, v);
            }
            else
                RETVAL = THIS->next(uri, name, *GetObjPointer(value, XmlValue));
        }
        OUTPUT:
            RETVAL
            uri
            name
        CLEANUP:
            MY_CATCH

void 
XmlMetaDataIterator::reset()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

MODULE = XmlStatistics	PACKAGE = XmlStatistics

void
XmlStatistics::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

double 
XmlStatistics::getNumberOfIndexedKeys()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

double 
XmlStatistics::getNumberOfUniqueKeys()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

double 
XmlStatistics::getSumKeyValueSize()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH


MODULE = XmlTransaction	PACKAGE = XmlTransaction

void
XmlTransaction::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void 
XmlTransaction::abort()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

void 
XmlTransaction::commit(u_int32_t flags = 0)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

XmlTransaction *
XmlTransaction::createChild(u_int32_t flags = 0);
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlTransaction";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = new XmlTransaction(THIS->createChild(flags));
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

DbTxn *
XmlTransaction::getDbTxn();
    SV * PARENT = NULL;
	char *	CLASS = (char *)"DbTxn";
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH



MODULE = XmlResolver	PACKAGE = XmlResolver

void
XmlResolver::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

bool 
XmlResolver::resolveDocument(XmlTransaction *txn, XmlManager *mgr, string uri, XmlValue *result) 
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = THIS->resolveDocument(txn, *mgr, uri, *result);
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

bool 
XmlResolver::resolveCollection(XmlTransaction *txn, XmlManager *mgr, string uri, XmlResults *result) 
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = THIS->resolveCollection(txn, *mgr, uri, *result);
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlInputStream *
XmlResolver::resolveSchema(XmlTransaction *txn, XmlManager *mgr, string schemaLocation, string nameSpace)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlInputStream";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = THIS->resolveSchema(txn, *mgr, schemaLocation, nameSpace);
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlInputStream *
XmlResolver::resolveEntity(XmlTransaction *txn, XmlManager *mgr, string systemId, string publicId)
    SV * PARENT = ST(0);
	char *	CLASS = (char *)"XmlInputStream";
	INIT:
	    MY_TRY
	CODE:
	{
	    RETVAL = THIS->resolveEntity(txn, *mgr, systemId, publicId);
	}
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

MODULE = XmlIndexLookup	PACKAGE = XmlIndexLookup

void
XmlIndexLookup::DESTROY()
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

string
XmlIndexLookup::getIndex()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlIndexLookup::setIndex(string index)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

string
XmlIndexLookup::getNodeURI()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

string
XmlIndexLookup::getNodeName()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlIndexLookup::setNode(string uri, string name)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

string
XmlIndexLookup::getParentURI()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

string
XmlIndexLookup::getParentName()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlIndexLookup::setParent(string uri, string name)
	INIT:
	    MY_TRY
	CLEANUP:
	    MY_CATCH

XmlValue*
XmlIndexLookup::getLowBoundValue()
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlValue";
	INIT:
	    MY_TRY
    CODE:
    {
        RETVAL = new XmlValue(THIS->getLowBoundValue());
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlIndexLookup::Operation
XmlIndexLookup::getLowBoundOperation()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlIndexLookup::setLowBound(XmlValueOrScalar* value, XmlIndexLookup::Operation op)
	INIT:
	    MY_TRY
    CODE:
    {
        THIS->setLowBound(*value, op);
    }
	CLEANUP:
	    MY_CATCH

XmlValue*
XmlIndexLookup::getHighBoundValue()
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlValue";
	INIT:
	    MY_TRY
    CODE:
    {
        RETVAL = new XmlValue(THIS->getHighBoundValue());
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

XmlIndexLookup::Operation
XmlIndexLookup::getHighBoundOperation()
	INIT:
	    MY_TRY
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlIndexLookup::setHighBound(XmlValueOrScalar* value, XmlIndexLookup::Operation op)
	INIT:
	    MY_TRY
    CODE:
    {
        THIS->setHighBound(*value, op);
    }
	CLEANUP:
	    MY_CATCH

XmlContainer*
XmlIndexLookup::getContainer()
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlContainer";
	INIT:
	    MY_TRY
    CODE:
    {
        RETVAL = new XmlContainer(THIS->getContainer());
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

void
XmlIndexLookup::setContainer(XmlContainer* container)
	INIT:
	    MY_TRY
    CODE:
    {
        THIS->setContainer(*container);
    }
	CLEANUP:
	    MY_CATCH

XmlResults*
XmlIndexLookup::_execute(XmlTransaction* txn, XmlQueryContext* context, u_int32_t flags = 0)
    SV * PARENT = NULL;
	char *	CLASS = (char *)"XmlResults";
	INIT:
	    MY_TRY
    CODE:
    {
        if (txn)
            RETVAL = new XmlResults(THIS->execute(*txn, *context, flags));
        else
            RETVAL = new XmlResults(THIS->execute(*context, flags));
    }
	OUTPUT:
	    RETVAL
	CLEANUP:
	    MY_CATCH

MODULE = Sleepycat::DbXml		PACKAGE = Sleepycat::DbXml		



BOOT:
{
  int Major, Minor, Patch;
  (void) db_version(&Major, &Minor, &Patch);
  static const int minMajor = 4;
  static const int minMinor = 3;
  if (Major < minMajor || Minor < minMinor) {
    croak("\nBerkeley DB XML requires at least version %d.%d of Berkeley DB.\n\tYou have version %d.%d.%d loaded.\n",
	  minMajor, minMinor, Major, Minor, Patch);
  }
}
