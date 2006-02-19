// Collation.h: interface for the Collation interface.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLATION_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_)
#define AFX_COLLATION_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_

#include <pathan/Pathan.hpp>
#include <xercesc/util/XercesDefs.hpp>

class Sequence;
class DynamicContext;

class PATHAN_EXPORT Collation
{
public:
  virtual ~Collation() {};

  virtual const XMLCh* getCollationName() const = 0;
  virtual Sequence sort(Sequence data, const DynamicContext* context) const = 0;
  virtual int compare(const XMLCh* const string1, const XMLCh* const string2) const = 0;

protected:
  Collation() {}
};

#endif // !defined(AFX_COLLATION_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_)
