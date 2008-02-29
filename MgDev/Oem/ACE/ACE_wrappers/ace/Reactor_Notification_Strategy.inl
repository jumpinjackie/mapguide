/* -*- C++ -*- */
// Reactor_Notification_Strategy.inl,v 4.2 2001/12/11 01:02:12 bala Exp
ACE_INLINE ACE_Reactor *
ACE_Reactor_Notification_Strategy::reactor (void)
{
  return this->reactor_;
}

ACE_INLINE void
ACE_Reactor_Notification_Strategy::reactor (ACE_Reactor *r)
{
  this->reactor_ = r;
}
