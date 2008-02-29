/* -*- C++ -*- */
// SV_Message.i,v 4.1 2003/11/01 11:15:17 dhinton Exp

// SV_Message.i

#include "ace/Global_Macros.h"

ACE_INLINE
ACE_SV_Message::ACE_SV_Message (long t)
  : type_ (t)
{
  ACE_TRACE ("ACE_SV_Message::ACE_SV_Message");
}

ACE_INLINE
ACE_SV_Message::~ACE_SV_Message (void)
{
  ACE_TRACE ("ACE_SV_Message::~ACE_SV_Message");
}

ACE_INLINE long
ACE_SV_Message::type (void) const
{
  ACE_TRACE ("ACE_SV_Message::type");
  return this->type_;
}

ACE_INLINE void
ACE_SV_Message::type (long t)
{
  ACE_TRACE ("ACE_SV_Message::type");
  this->type_ = t;
}
