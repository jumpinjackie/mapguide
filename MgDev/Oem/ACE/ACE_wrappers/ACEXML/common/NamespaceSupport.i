// -*- C++ -*-  NamespaceSupport.i,v 1.4 2003/06/24 16:34:54 dhinton Exp

#include "ace/ACE.h"

ACE_TEMPLATE_SPECIALIZATION
ACEXML_INLINE int
ACE_Equal_To<ACEXML_String>::operator () (const ACEXML_String &lhs,
                                          const ACEXML_String &rhs) const
{
  return lhs == rhs;
}

ACE_TEMPLATE_SPECIALIZATION
ACE_INLINE u_long
ACE_Hash<ACEXML_String>::operator () (const ACEXML_String &t) const
{
  return ACE::hash_pjw (t.c_str ());
}
