/* -*- C++ -*- */
//Refcountable.inl,v 4.1 2001/12/10 21:44:43 bala Exp
ACE_INLINE
ACE_Refcountable::ACE_Refcountable (int refcount)
  : refcount_ (refcount)
{
}

ACE_INLINE
ACE_Refcountable::~ACE_Refcountable (void)
{
}

ACE_INLINE int
ACE_Refcountable::increment (void)
{
  return ++this->refcount_;
}

ACE_INLINE int
ACE_Refcountable::decrement (void)
{
  return --this->refcount_;
}

ACE_INLINE int
ACE_Refcountable::refcount (void) const
{
  return this->refcount_;
}
