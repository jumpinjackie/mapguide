/* -*- C++ -*- */
// POSIX_CB_Proactor.i,v 4.3 2002/11/14 23:13:08 crodrigu Exp

#if defined (ACE_HAS_AIO_CALLS)

ACE_INLINE
ACE_POSIX_Proactor::Proactor_Type ACE_POSIX_CB_Proactor::get_impl_type (void)
{
  return PROACTOR_CB;
}

#endif /* ACE_HAS_AIO_CALLS */
