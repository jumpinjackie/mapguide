/* -*- C++ -*- */
// FIFO_Send.i,v 4.4 2003/11/01 11:15:12 dhinton Exp

// FIFO_Send.i

#include "ace/ACE.h"

ASYS_INLINE ssize_t
ACE_FIFO_Send::send (const void *buf, size_t len)
{
  ACE_TRACE ("ACE_FIFO_Send::send");
  return ACE_OS::write (this->get_handle (), (const char *) buf, len);	
}

ASYS_INLINE ssize_t
ACE_FIFO_Send::send_n (const void *buf, size_t n)
{
  ACE_TRACE ("ACE_FIFO_Send::send_n");
  return ACE::send_n (this->get_handle (), buf, n);
}
