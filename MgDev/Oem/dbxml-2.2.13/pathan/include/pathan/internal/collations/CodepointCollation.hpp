// CodepointCollation.h: interface for the CodepointCollation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODEPOINTCOLLATION_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_)
#define AFX_CODEPOINTCOLLATION_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_

#include <pathan/Pathan.hpp>
#include "CollationImpl.hpp"

class XPath2MemoryManager;

class PATHAN_EXPORT CodepointCollation : public CollationHelper {
public:
	CodepointCollation();

  static const XMLCh* getCodepointCollationName();
	virtual const XMLCh* getCollationName() const;
	virtual int compare(const XMLCh* string1, const XMLCh* string2) const;
};

#endif // !defined(AFX_CODEPOINTCOLLATION_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_)
