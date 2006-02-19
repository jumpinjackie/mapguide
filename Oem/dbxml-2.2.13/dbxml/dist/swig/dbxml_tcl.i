// Tcl-specific SWIG directives
%module dbxml_tcl
%{
extern "C" void *_NameToPtr(const char *);
#define NAME_TO_ENV(name) (DB_ENV *)_NameToPtr((name))
#define NAME_TO_TXN(name) (DB_TXN *)_NameToPtr((name))
%}

%typemap(tcl8, out) u_int32_t * {
	for (int i = 0; $1[i]; i++)
		Tcl_ListObjAppendElement(interp, $result, Tcl_NewIntObj($1[i]));
	delete [] $1;
}

%typemap(tcl8, in) DbEnv * {
	char *arg = Tcl_GetStringFromObj($input, NULL);
	DB_ENV *envp = NAME_TO_ENV(arg);
	$1 = envp ? DbEnv::wrap_DB_ENV(envp) : NULL;
}

%typemap(tcl8, in) DbTxn * {
	char *arg = Tcl_GetStringFromObj($input, NULL);
	DB_TXN *txnp = NAME_TO_TXN(arg);
	$1 = txnp ? DbTxn::wrap_DB_TXN(txnp) : 0;
}

%typemap(tcl8, in) XmlData *, XmlData & (XmlData xml_data) {
	int len;
	char *data = (char *)Tcl_GetByteArrayFromObj($input, &len);
  xml_data.set_data(data);
  xml_data.set_size(len);
	$1 = &xml_data;
}

%typemap(tcl8, out) const XmlData *, XmlData * {
	if ($1) {
		Tcl_SetObjResult(interp, Tcl_NewByteArrayObj((unsigned char *)$1->get_data(), $1->get_size()));
		delete $1; // done with new XmlData object
	} else
		Tcl_SetResult(interp, (char *) "", TCL_STATIC);
}

%typemap(tcl8, in) const void * {
	int len;
	char *data = (char *)Tcl_GetByteArrayFromObj($input, &len);
	$1 = data;
}

%typemap(tcl8, in) void * {
	int len;
	char *data = (char *)Tcl_GetByteArrayFromObj($input, &len);
	$1 = data;
}

/* 
 * gmf: in order to support function overloads that use DbTxn and DbEnv,
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


%exception {
	try {
		$function
	} catch (XmlException &e) {
		Tcl_SetResult(interp, (char *)e.what(), TCL_VOLATILE);
		return TCL_ERROR;
	}
}
