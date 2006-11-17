#include "Signature.h"

ACE_RCSID(src, Signature, "Signature.cpp,v 1.1 2002/02/20 22:05:12 mesnier_p Exp")

//-----------------------------------------------------------------------------

Signature::Signature (const ACE_CString &name)
  :name_(name),
   ref_count_ (1),
   used_ (0)
{
}

void 
Signature::used ()
{  
  used_++;
}

int 
Signature::used_count() const
{
  return used_;
}

const ACE_CString &
Signature::name() const
{
  return name_;
}

Signature *
Signature::dup()
{
  ref_count_++;
  return this;
}

void
Signature::release()
{
  if (--ref_count_ == 0)
    delete this;
}

