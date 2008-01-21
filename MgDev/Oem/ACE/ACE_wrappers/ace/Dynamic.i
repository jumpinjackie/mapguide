/* -*- C++ -*- */
// Dynamic.i,v 4.4 2002/05/02 05:40:36 irfan Exp

// Dynamic.i

ACE_INLINE
ACE_Dynamic::~ACE_Dynamic (void)
{
  // ACE_TRACE ("ACE_Dynamic::~ACE_Dynamic");
}

ACE_INLINE void
ACE_Dynamic::set (void)
{
  // ACE_TRACE ("ACE_Dynamic::set");
  this->is_dynamic_ = 1;
}

ACE_INLINE int
ACE_Dynamic::is_dynamic ()
{
  // ACE_TRACE ("ACE_Dynamic::is_dynamic");
  return this->is_dynamic_;
}

ACE_INLINE void
ACE_Dynamic::reset (void)
{
  // ACE_TRACE ("ACE_Dynamic::reset");
  this->is_dynamic_ = 0;
}
