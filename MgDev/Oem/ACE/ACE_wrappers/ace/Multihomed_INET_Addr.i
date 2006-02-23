/* -*- C++ -*- */
// Multihomed_INET_Addr.i,v 4.2 2003/11/18 01:06:01 bala Exp

// Multihomed_INET_Addr.i

// Accessor implementations
ACE_INLINE
size_t
ACE_Multihomed_INET_Addr::get_num_secondary_addresses() const
{
  return this->secondaries_.size();
}
