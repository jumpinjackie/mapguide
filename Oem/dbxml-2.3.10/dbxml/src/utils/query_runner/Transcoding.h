//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Transcoding.h,v 1.7 2006/10/30 17:46:06 bostic Exp $
//

#include <string>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUTF8Transcoder.hpp>

class CXXToXMLCh
{
public :

    CXXToXMLCh(const char* const toTranscode, bool adopt = false) : adopted(adopt)
    {
        fUnicodeForm = UTF8ToXMLCh(toTranscode);
    }

    CXXToXMLCh(std::string toTranscode, bool adopt = false) : adopted(adopt)
    {
        fUnicodeForm = UTF8ToXMLCh(toTranscode);
    }

    CXXToXMLCh(int toTranscode, bool adopt = false) : adopted(adopt)
    {
        std::string val = intToString(toTranscode);
        fUnicodeForm = UTF8ToXMLCh(val);
    }

    CXXToXMLCh(double toTranscode, bool adopt = false) : adopted(adopt)
    {
        std::string val = dtos(toTranscode);
        fUnicodeForm = UTF8ToXMLCh(val);
    }

    ~CXXToXMLCh()
    {
        if(!adopted) {
            //XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&fUnicodeForm);
            delete[] fUnicodeForm;
        }
    }

    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :

    XMLCh* UTF8ToXMLCh(std::string toTranscode) {
        XERCES_CPP_NAMESPACE_QUALIFIER XMLUTF8Transcoder t(0, 512);
    	size_t l = toTranscode.length();
	    XMLCh *tmp = new XMLCh[l + 1];
	    unsigned char *charSizes = new unsigned char[l + 1];
	    unsigned int count = 0;
	    t.transcodeFrom((XMLByte*)toTranscode.c_str(), l + 1, tmp, l + 1, count, charSizes);
	    delete [] charSizes;
        return tmp;
    }

     std::string dtos(double dbl){
        char buf[BUFSIZ];
        sprintf(buf, "%lf", dbl);
        return buf;
     }

     std::string intToString(int value){
        std::string number;
        int temp;
        bool neg= false;

        if ((value * (-1)) > value){
            value *=-1;
            neg=true;
        }

        for (int z = IntLen(value) ;z>=1;z--){
            temp = value % 10;
            number = ((char)(temp + 48))+ number;
            value = (value - temp) /10;
            temp *= 10 ;
        }
        if (neg==true){
            number="-" + number;
        }
        return number;
    }

    int IntLen(int value){
        int div=1;
        int len=1;
        do{
            div*=10;
            if ((int) ( (double)value / (double)(div)) ==0){
                return len;
            }
            len +=1;
        }while(true);
    }

    XMLCh*   fUnicodeForm;
    bool adopted;

};

class XMLChToCXX
{
public :
    XMLChToCXX(const XMLCh* const toTranscode, bool adopt = false) : adopted(adopt)
    {
        XERCES_CPP_NAMESPACE_QUALIFIER XMLUTF8Transcoder t(0, 512);
        size_t l =  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(toTranscode);
        const unsigned int needed = l * 3 + 1; // 3 bytes per XMLCh is the worst case, + '\0'
        fLocalForm = new XMLByte[needed];
        unsigned int charsEaten= 0;
        t.transcodeTo(toTranscode, l+1, fLocalForm, needed, charsEaten,  XERCES_CPP_NAMESPACE_QUALIFIER XMLTranscoder::UnRep_Throw);

    }

    ~XMLChToCXX()
    {
        if(!adopted) {
            delete[] fLocalForm;
        }
    }

    const std::string localForm() const
    {
        return (char *)fLocalForm;
    }

private :

    XMLByte*   fLocalForm;
    bool adopted;
};

