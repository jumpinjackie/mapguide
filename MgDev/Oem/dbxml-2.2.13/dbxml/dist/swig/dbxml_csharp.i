%module dbxml_dotnet

// Typemaps
%define STD_TYPEMAP(_ctype, _jtype, _jnitype)
%typemap(jstype) _ctype #_jtype
%typemap(jtype) _ctype #_jtype
%typemap(jni) _ctype #_jnitype
%typemap(out) _ctype %{ $result = (_jnitype)$1; %}
%typemap(javain) _ctype "$javainput"
%typemap(javaout) _ctype { return $jnicall; }
%enddef

STD_TYPEMAP(db_timeout_t, long, jlong)

%include db-minimal.i