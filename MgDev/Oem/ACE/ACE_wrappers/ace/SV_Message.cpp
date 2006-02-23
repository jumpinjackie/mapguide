// SV_Message.cpp
// SV_Message.cpp,v 4.3 2003/07/27 20:48:27 dhinton Exp

#include "ace/SV_Message.h"

#if !defined (__ACE_INLINE__)
#include "ace/SV_Message.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(ace, SV_Message, "SV_Message.cpp,v 4.3 2003/07/27 20:48:27 dhinton Exp")

ACE_ALLOC_HOOK_DEFINE(ACE_SV_Message)

void
ACE_SV_Message::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_TRACE ("ACE_SV_Message::dump");
#endif /* ACE_HAS_DUMP */
}

