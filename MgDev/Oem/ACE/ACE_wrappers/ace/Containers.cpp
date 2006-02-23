// Containers.cpp,v 4.60 2002/04/10 17:44:15 ossama Exp

#include "ace/Containers.h"

ACE_RCSID (ace,
           Containers,
           "Containers.cpp,v 4.60 2002/04/10 17:44:15 ossama Exp")

#if !defined (__ACE_INLINE__)
#include "ace/Containers.i"
#endif /* __ACE_INLINE__ */

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Double_Linked_List<ACE_DLList_Node>;
template class ACE_Double_Linked_List_Iterator_Base<ACE_DLList_Node>;
template class ACE_Double_Linked_List_Iterator<ACE_DLList_Node>;
template class ACE_Double_Linked_List_Reverse_Iterator<ACE_DLList_Node>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Double_Linked_List<ACE_DLList_Node>
#pragma instantiate ACE_Double_Linked_List_Iterator_Base<ACE_DLList_Node>
#pragma instantiate ACE_Double_Linked_List_Iterator<ACE_DLList_Node>
#pragma instantiate ACE_Double_Linked_List_Reverse_Iterator<ACE_DLList_Node>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */

