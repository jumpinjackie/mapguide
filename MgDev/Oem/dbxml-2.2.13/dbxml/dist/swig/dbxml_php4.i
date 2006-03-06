%module dbxml_php4

%include db-minimal.i

/* general-purpose exception handler for XmlException */
%exception {
  try {
    $action
  } catch (XmlException &e) {
    zend_error(E_ERROR, e.what());
    /* I think zend_error aborts the function */
    /* ZVAL_BOOL(return_value,0); */
  }
}

/*
 * Need to typemap some objects -- std::string and
 * DB-specific objects. Same as python.
 */
#include <string>
namespace std {
    class string;
    %typemap(in) string {
        convert_to_string_ex($input);
        $1 = std::string(Z_STRVAL_PP($input));
    }
    %typemap(in) const string & (std::string temp) {
        convert_to_string_ex($input);
        temp = std::string(Z_STRVAL_PP($input));
        $1 = &temp;
    }
    %typemap(out) string {
        ZVAL_STRINGL($result,const_cast<char*>($1.c_str()),$1.length(),1);
    }
    %typemap(out) const string & {
        ZVAL_STRINGL($result,const_cast<char*>($1->c_str()),$1->length(),1);
    }
}

%typemap(in) Dbt *, Dbt & (Dbt dbt) {
  convert_to_string_ex($input);
  dbt.set_data(Z_STRVAL_PP($input));
  dbt.set_size(Z_STRLEN_PP($input));
  $1 = &dbt;
}

%typemap(out) const Dbt * {
  if ($1) {
    ZVAL_STRINGL($result,(char *)$1->get_data(),$1->get_size(), 1);
  } else {
    $result = NULL;
  }
}

%typemap(in) XmlData *, XmlData & (XmlData xml_data) {
  convert_to_string_ex($input);
  xml_data.set_data(Z_STRVAL_PP($input));
  xml_data.set_size(Z_STRLEN_PP($input));
  $1 = &dbt;
}

%typemap(out) const XmlData * {
  if ($1) {
    ZVAL_STRINGL($result,(char *)$1->get_data(),$1->get_size(), 1);
  } else {
    $result = NULL;
  }
}
