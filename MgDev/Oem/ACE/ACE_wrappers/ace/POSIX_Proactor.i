/* -*- C++ -*- */
// POSIX_Proactor.i,v 4.6 2002/11/14 23:13:08 crodrigu Exp

ACE_INLINE 
ACE_POSIX_Proactor::Proactor_Type ACE_POSIX_Proactor::get_impl_type (void)
{
  return PROACTOR_POSIX;
} 

ACE_INLINE 
ACE_POSIX_Proactor::Proactor_Type ACE_POSIX_AIOCB_Proactor::get_impl_type (void)
{
  return PROACTOR_AIOCB;
} 

#if defined(ACE_HAS_POSIX_REALTIME_SIGNALS)

ACE_INLINE 
ACE_POSIX_Proactor::Proactor_Type ACE_POSIX_SIG_Proactor::get_impl_type (void)
{
  return PROACTOR_SIG;
} 

#endif /* ACE_HAS_POSIX_REALTIME_SIGNALS */

ACE_INLINE 
ACE_Asynch_Pseudo_Task& ACE_POSIX_Proactor::get_asynch_pseudo_task (void)
{
  return this->pseudo_task_;
}
