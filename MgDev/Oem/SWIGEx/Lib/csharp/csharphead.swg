/* -----------------------------------------------------------------------------
 * csharphead.swg
 *
 * CSharp support code
 * ----------------------------------------------------------------------------- */

%insert(runtime) %{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#if defined(_WIN32) || defined(__CYGWIN32__)
#  define DllExport   __declspec( dllexport )
#  define SWIGSTDCALL __stdcall
#else
#  define DllExport  
#  define SWIGSTDCALL
#endif 
%}

%insert(runtime) %{
/* Support for throwing C# custom exceptions from C/C++ */
typedef void (SWIGSTDCALL* SWIG_CSharpCustomExceptionCallback_t)(const char*, const void* cptr);
static SWIG_CSharpCustomExceptionCallback_t custom_exceptions_callback;

%}

%insert(runtime) %{
#ifdef __cplusplus
extern "C" 
#endif

DllExport void SWIGSTDCALL SWIGRegisterCustomExceptionCallbacks_$module(SWIG_CSharpCustomExceptionCallback_t callback) {
    custom_exceptions_callback = callback;
}

%}

%pragma(csharp) imclasscode=%{
  class SWIGExceptionHelper {

    public delegate void SWIGCustomExceptionDelegate(string className, IntPtr cptr);


    static SWIGCustomExceptionDelegate customExceptionDelegate = new SWIGCustomExceptionDelegate(ThrowCustomException);

    [DllImport("$dllname", EntryPoint="SWIGRegisterCustomExceptionCallbacks_$module")]
    public static extern void SWIGRegisterCustomExceptionCallbacks_$module(
      SWIGCustomExceptionDelegate customExceptionDelegate);

    static void ThrowCustomException(string className, IntPtr cptr) {

        object exception = null;
        try
        {
            object[] parameters = new object[2];
            parameters[0] = cptr;
            parameters[1] = true;

            exception = Activator.CreateInstance(Type.GetType(className), parameters);
           
        }
        catch(Exception e)
        {
            Debug.WriteLine(e.Message);
        }

        if(exception != null)
            throw (Exception)exception;

    }

    static SWIGExceptionHelper() {
      SWIGRegisterCustomExceptionCallbacks_$module(customExceptionDelegate);
    }
  }

  static SWIGExceptionHelper exceptionHelper = new SWIGExceptionHelper();
%}

%insert(runtime) %{
/* Callback for returning strings to C# without leaking memory */
typedef void * (SWIGSTDCALL* SWIG_CSharpStringHelperCallback)(int);
static SWIG_CSharpStringHelperCallback SWIG_csharp_string_callback = NULL;
%}

%pragma(csharp) imclasscode=%{
  class SWIGStringHelper {

    public delegate IntPtr SWIGStringDelegate(int len);
    static SWIGStringDelegate stringDelegate = new SWIGStringDelegate(CreateString);

    [DllImport("$dllname", EntryPoint="SWIGRegisterStringCallback_$module")]
    public static extern void SWIGRegisterStringCallback_$module(SWIGStringDelegate stringDelegate);

    static IntPtr CreateString(int len) {
      return Marshal.AllocCoTaskMem(len);
    }

    static SWIGStringHelper() {
      SWIGRegisterStringCallback_$module(stringDelegate);
    }
  }

  static SWIGStringHelper stringHelper = new SWIGStringHelper();
%}

%insert(runtime) %{
#ifdef __cplusplus
extern "C" 
#endif
DllExport void SWIGSTDCALL SWIGRegisterStringCallback_$module(SWIG_CSharpStringHelperCallback callback) {
  SWIG_csharp_string_callback = callback;
}

/* Contract support */

#define SWIG_contract_assert(nullreturn, expr, msg) if (!(expr)) {SWIG_CSharpThrowException(SWIG_CSharpArgumentOutOfRangeException, msg); return nullreturn; } else
%}

%pragma(csharp) imclasscode=%{
  static $modulePINVOKE()
  {
      classMap = new Hashtable();
$initMap
  }
  static public object createObject(int id, IntPtr cPtr, bool ownMemory)
  {
      Type type = classMap[id] as Type;
      if(type == null)
          return null;
      object[] args = new object[2] { cPtr, ownMemory };
      return Activator.CreateInstance(type, args);
  }
  protected static Hashtable classMap;
%}
    
%insert(runtime) %{
/* Support for throwing C# exceptions from C/C++ */
typedef enum {
  SWIG_CSharpException,
  SWIG_CSharpOutOfMemoryException,
  SWIG_CSharpIndexOutOfRangeException,
  SWIG_CSharpDivideByZeroException,
  SWIG_CSharpArgumentOutOfRangeException,
  SWIG_CSharpNullReferenceException
} SWIG_CSharpExceptionCodes;

typedef void (SWIGSTDCALL* SWIG_CSharpExceptionCallback_t)(const char *);

typedef struct {
  SWIG_CSharpExceptionCodes code;
  SWIG_CSharpExceptionCallback_t callback;
} SWIG_CSharpExceptions_t;

static SWIG_CSharpExceptions_t SWIG_csharp_exceptions[] = {
  { SWIG_CSharpException, NULL },
  { SWIG_CSharpOutOfMemoryException, NULL },
  { SWIG_CSharpIndexOutOfRangeException, NULL },
  { SWIG_CSharpDivideByZeroException, NULL },
  { SWIG_CSharpArgumentOutOfRangeException, NULL },
  { SWIG_CSharpNullReferenceException, NULL } };

static void SWIG_CSharpThrowException(SWIG_CSharpExceptionCodes code, const char *msg) {
  SWIG_CSharpExceptionCallback_t callback = SWIG_csharp_exceptions[SWIG_CSharpException].callback;
  if (code >=0 && (size_t)code < sizeof(SWIG_csharp_exceptions)/sizeof(SWIG_CSharpExceptionCodes)) {
    callback = SWIG_csharp_exceptions[code].callback;
  }
  callback(msg);
}
%}

%insert(runtime) %{
#ifdef __cplusplus
extern "C" 
#endif
DllExport void SWIGSTDCALL SWIGRegisterExceptionCallbacks_$module(SWIG_CSharpExceptionCallback_t systemException,
                                                          SWIG_CSharpExceptionCallback_t outOfMemory, 
                                                          SWIG_CSharpExceptionCallback_t indexOutOfRange, 
                                                          SWIG_CSharpExceptionCallback_t divideByZero, 
                                                          SWIG_CSharpExceptionCallback_t argumentOutOfRange,
                                                          SWIG_CSharpExceptionCallback_t nullReference) {
  SWIG_csharp_exceptions[SWIG_CSharpException].callback = systemException;
  SWIG_csharp_exceptions[SWIG_CSharpOutOfMemoryException].callback = outOfMemory;
  SWIG_csharp_exceptions[SWIG_CSharpIndexOutOfRangeException].callback = indexOutOfRange;
  SWIG_csharp_exceptions[SWIG_CSharpDivideByZeroException].callback = divideByZero;
  SWIG_csharp_exceptions[SWIG_CSharpArgumentOutOfRangeException].callback = argumentOutOfRange;
  SWIG_csharp_exceptions[SWIG_CSharpNullReferenceException].callback = nullReference;
}
%}

%pragma(csharp) imclasscode=%{
  class SWIGStandardExceptionHelper {

    public delegate void SWIGExceptionDelegate(string message);

    static SWIGExceptionDelegate systemDelegate = new SWIGExceptionDelegate(ThrowSystemException);
    static SWIGExceptionDelegate outOfMemoryDelegate = new SWIGExceptionDelegate(ThrowOutOfMemoryException);
    static SWIGExceptionDelegate indexOutOfRangeDelegate = new SWIGExceptionDelegate(ThrowIndexOutOfRangeException);
    static SWIGExceptionDelegate divideByZeroDelegate = new SWIGExceptionDelegate(ThrowDivideByZeroException);
    static SWIGExceptionDelegate argumentOutOfRangeDelegate = new SWIGExceptionDelegate(ThrowArgumentOutOfRangeException);
    static SWIGExceptionDelegate nullReferenceDelegate = new SWIGExceptionDelegate(ThrowNullReferenceException);

    [DllImport("$dllname", EntryPoint="SWIGRegisterExceptionCallbacks_$module")]
    public static extern void SWIGRegisterExceptionCallbacks_$module(
      SWIGExceptionDelegate systemExceptionDelegate,
      SWIGExceptionDelegate outOfMemoryDelegate, 
      SWIGExceptionDelegate indexOutOfRangeDelegate, 
      SWIGExceptionDelegate divideByZeroDelegate, 
      SWIGExceptionDelegate argumentOutOfRangeDelegate,
      SWIGExceptionDelegate nullReferenceDelegate);

    static void ThrowSystemException(string message) {
      throw new System.SystemException(message);
    }

    static void ThrowOutOfMemoryException(string message) {
      throw new System.OutOfMemoryException(message);
    }

    static void ThrowIndexOutOfRangeException(string message) {
      throw new System.IndexOutOfRangeException(message);
    }

    static void ThrowDivideByZeroException(string message) {
      throw new System.DivideByZeroException(message);
    }

    static void ThrowArgumentOutOfRangeException(string message) {
      throw new System.ArgumentOutOfRangeException(message);
    }

    static void ThrowNullReferenceException(string message) {
      throw new System.NullReferenceException(message);
    }

    static SWIGStandardExceptionHelper() {
      SWIGRegisterExceptionCallbacks_$module(systemDelegate, outOfMemoryDelegate, indexOutOfRangeDelegate, divideByZeroDelegate, argumentOutOfRangeDelegate, nullReferenceDelegate);
    }
  }

  static SWIGStandardExceptionHelper standardExceptionHelper = new SWIGStandardExceptionHelper();
%}
