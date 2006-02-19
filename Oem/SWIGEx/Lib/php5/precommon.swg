/*************************************************************** -*- c -*-
 * php5/precommon.swg
 *
 * Rename all exported symbols from common.swg, to avoid symbol
 * clashes if multiple interpreters are included
 *
 ************************************************************************/

#define SWIG_TypeRegister    SWIG_PHP5_TypeRegister
#define SWIG_TypeCheck       SWIG_PHP5_TypeCheck
#define SWIG_TypeCast        SWIG_PHP5_TypeCast
#define SWIG_TypeDynamicCast SWIG_PHP5_TypeDynamicCast
#define SWIG_TypeName        SWIG_PHP5_TypeName
#define SWIG_TypeQuery       SWIG_PHP5_TypeQuery
#define SWIG_TypeClientData  SWIG_PHP5_TypeClientData
#define SWIG_PackData        SWIG_PHP5_PackData 
#define SWIG_UnpackData      SWIG_PHP5_UnpackData 

