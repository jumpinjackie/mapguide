//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

%typemap(jni) STRINGPARAM "jstring"
%typemap(jtype) STRINGPARAM "String"
%typemap(jstype) STRINGPARAM "String"
%typemap(javain) STRINGPARAM "$javainput"
%typemap(in) STRINGPARAM
{
    if($input == NULL)
       $1 = std::wstring(L"");
    else
    {
        int len = jenv->GetStringLength($input);
        const jchar *pstr = jenv->GetStringChars($input, 0);
        if (!pstr)
            $1 = std::wstring(L"ERROR: SWIG string conversion"); 
        else
        {
#ifdef WIN32
            $1 =  std::wstring((const wchar_t*)pstr, len);
#else
            xstring u16String = xstring((XMLCh*)pstr, len);
            lstring u32String;
            UnicodeString::UTF16toUTF32(u16String.c_str(),u32String);
            $1 =  std::wstring((const wchar_t*) u32String.c_str());
#endif
            jenv->ReleaseStringChars($input, pstr);
        }
    }
}

%typemap(jni) STRING "jstring"
%typemap(jtype) STRING "String"
%typemap(jstype) STRING "String"
%typemap(javaout) STRING
{
  { return $jnicall; }
}
%typemap(out) STRING
{
#ifdef WIN32
    jresult = jenv->NewString((const jchar *)result.c_str(), (jsize)result.length());
#else
    xstring u16String;
    UnicodeString::UTF32toUTF16((const LCh*) result.c_str(), u16String);
    jresult = jenv->NewString((const jchar *)u16String.c_str(), (jsize)u16String.length());
#endif
}

%typemap(jni) BYTE_ARRAY_IN "jbyteArray"
%typemap(jtype) BYTE_ARRAY_IN "byte[]"
%typemap(jstype) BYTE_ARRAY_IN "byte[]"
%typemap(in) BYTE_ARRAY_IN (jbyte* bytes)
{
    bytes = jenv->GetByteArrayElements($input, 0);
    if (!bytes)
        return $null;
    $1 = (BYTE_ARRAY_IN)bytes;
}
%typemap(freearg) BYTE_ARRAY_IN
{
    jenv->ReleaseByteArrayElements($input, bytes$argnum, 0);
}
%typemap(javain) BYTE_ARRAY_IN "$javainput"


%typemap(jni) BYTE_ARRAY_OUT "jbyteArray"
%typemap(jtype) BYTE_ARRAY_OUT "byte[]"
%typemap(jstype) BYTE_ARRAY_OUT "byte[]"
%typemap(in) BYTE_ARRAY_OUT (jbyte* bytes)
{
    bytes = jenv->GetByteArrayElements($input, 0);
    if (!bytes)
        return $null;
    $1 = (BYTE_ARRAY_OUT)bytes;
}
%typemap(freearg) BYTE_ARRAY_OUT
{
    jenv->ReleaseByteArrayElements($input, bytes$argnum, JNI_COMMIT);
}
%typemap(javain) BYTE_ARRAY_OUT "$javainput"

%include "javaextensions.i"
