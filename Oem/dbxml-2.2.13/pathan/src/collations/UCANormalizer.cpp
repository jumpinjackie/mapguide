// UCANormalizer.cpp: implementation of the Normalizer class.
//
//////////////////////////////////////////////////////////////////////

#include "../config/pathan_config.h"
#include "UCANormalizer.hpp"
#include <pathan/XPath2Utils.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/XMLString.hpp>
#include <pathan/XPath2MemoryManager.hpp>

static const XMLCh NOT_COMPOSITE = 0xFFFF;

const XMLCh* Normalizer::getRecursiveDecomposition(bool bCanonical, XMLCh ch, XPath2MemoryManager* memMgr)
{
	XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf(1023, memMgr);

	wchar_t* decomp=g_decompose[ch];
	int tblIndex=ch/32;
	int bitIndex=ch%32;
	if(decomp!=0 && !(bCanonical && (g_isCompatibility[tblIndex] & (1<<bitIndex))!=0) )
		{
			for (int i = 0; decomp[i]!=0; ++i)
				buf.append(getRecursiveDecomposition(bCanonical,decomp[i], memMgr));
    } 
	else 
		{                    // if no decomp, append
			buf.append(XPath2Utils::asStr(ch, memMgr));
    }
	return memMgr->getPooledString(buf.getRawBuffer());
}

int Normalizer::getCanonicalClass(XMLCh ch)
{
	return g_canonicalClass[ch];
}

XMLCh Normalizer::getPairwiseComposition(XMLCh first, XMLCh second)
{
	int key=((first << 16) | second);
	std::map<int,int>::iterator it=g_composeMap.find(key);
	if(it!=g_composeMap.end())
		return (*it).second;
	return NOT_COMPOSITE;
}

const XMLCh* Normalizer::internalDecompose(bool bCanonical, const XMLCh* source, XPath2MemoryManager* memMgr)
{
	const XMLCh* target = 0;
	// first decompose it
	for (unsigned int i = 0; i < XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(source); ++i) 
	{
		const XMLCh* buffer=getRecursiveDecomposition(bCanonical,source[i], memMgr);

		// add all of the characters in the decomposition.
		// (may be just the original character, if there was
		// no decomposition mapping)

		for (unsigned int j = 0; j < XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(buffer); ++j) 
		{
			XMLCh ch = buffer[j];
			int chClass = getCanonicalClass(ch);
			int k = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(target); // insertion point
			if (chClass != 0) 
			{
				// bubble-sort combining marks as necessary
				for (; k > 0; --k) 
				{
					if (getCanonicalClass(target[k-1]) <= chClass) 
						break;
				}
			}
			target = XPath2Utils::insertData(target, ch, k, memMgr);
		}
	}

	return target;
}

XMLCh* Normalizer::internalCompose(const XMLCh* source, XPath2MemoryManager* memMgr)
{
    XMLCh* target = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(source);
    unsigned int starterPos = 0, compPos = 1;
    XMLCh starterCh = target[0];
    int lastClass = getCanonicalClass(starterCh);
    if (lastClass != 0) 
		lastClass = 256; // fix for irregular combining sequence
    
    // Loop on the decomposed characters, combining where possible
    for (unsigned int decompPos = 1; decompPos < XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(target); ++decompPos) 
	{
        XMLCh ch = target[decompPos];
        int chClass = getCanonicalClass(ch);
        XMLCh composite = getPairwiseComposition(starterCh, ch);
        if (composite != NOT_COMPOSITE && (lastClass < chClass || lastClass == 0)) 
		{
            target[starterPos] = composite;
            starterCh = composite;
        }
		else 
		{
            if (chClass == 0) 
			{
                starterPos = compPos;
                starterCh  = ch;
            }
            lastClass = chClass;
            target[compPos++] = ch;
        }
    }
    target[compPos] = 0;
	return target;
}

XMLCh* Normalizer::NormalizeC(const XMLCh* source, XPath2MemoryManager* memMgr)
{
	const XMLCh* target=internalDecompose(true,source, memMgr);
	XMLCh* result=internalCompose(target, memMgr);
	return result;
}

const XMLCh* Normalizer::NormalizeD(const XMLCh* source, XPath2MemoryManager* memMgr)
{
	return internalDecompose(true,source, memMgr);
}

XMLCh* Normalizer::NormalizeKC(const XMLCh* source, XPath2MemoryManager* memMgr)
{
	const XMLCh* target=internalDecompose(false,source, memMgr);
	XMLCh* result=internalCompose(target, memMgr);
	return result;
}

const XMLCh* Normalizer::NormalizeKD(const XMLCh* source, XPath2MemoryManager* memMgr)
{
	return internalDecompose(false,source, memMgr);
}
