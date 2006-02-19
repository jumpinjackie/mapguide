
/*! 
    \file vlist.h
    \brief A documented header file containing the vlist 
*/


#ifndef VLIST_H
#define VLIST_H


#include <stdlib.h>
 
#ifdef _WIN32_WCE
#define VLIST_CDECL __cdecl
#else
#define VLIST_CDECL 
#endif
 
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
	The vlist is single linked list that stores void pointers.
	It has a cursor to traverse the list. 
*/
typedef struct vlist_node_s{
	void * item;
	struct vlist_node_s * next;
} vlist_node_t;

typedef struct vlist_s{
	struct vlist_node_s * head;
	struct vlist_node_s * tail;
	struct vlist_node_s * cursor;
	unsigned int cursor_index;
	unsigned int count;
	void *(VLIST_CDECL *malloc) (size_t);
	void (VLIST_CDECL *free) (void*);
} vlist_t;


/*!
    \brief This function creates and initalises a vlist structure.
	\var vlist_malloc A malloc function for vlist to use	
	\var vlist_free A free function for vlist to use	
	\return The vlist structure 
*/
vlist_t* new_vlist(
	void *  (VLIST_CDECL *  vlist_malloc) (size_t),
	void (VLIST_CDECL *vlist_free) (void *));

/*!
    \brief This function destroys and cleans up a vlist structure.
	\var vlist The list to delete
*/
void delete_vlist( 
	vlist_t* vlist); 

/*!
    \brief This function adds an item to the head of a vlist structure.
	\var vlist The list operate on
	\var item A void pointer to store 
*/
void vlist_add_first( 
	vlist_t* vlist, 
	void* item);

/*!
    \brief This function adds an item to the tail of a vlist structure.
	\var vlist The list operate on
	\var item A void pointer to store 
*/
void vlist_add_last( 
	vlist_t* vlist, 
	void* item);

/*!
    \brief This function adds an item after the cursor to the vlist structure.
	\var vlist The list operate on
	\var item A void pointer to store 
*/
void vlist_add_after_cursor(
	vlist_t* vlist,
	void *item);

/*!
    \brief This function removes an item from the vlist structure.
	\var vlist The list operate on
	\var item A void pointer to remove from the list 
	\return 0 if item not found ~0 if found
*/
int vlist_remove(
	vlist_t* vlist, 
	void* item);

/*!
    \brief This function removes the first item from the vlist structure.
	\var vlist The list operate on
	\return 0 if vlist is empty or first item pointer
*/
void * vlist_remove_first(
	vlist_t* vlist);

/*!
    \brief This function removes the next item after the cursor from the vlist structure.
	\var vlist The list operate on
	\return 0 if no such item exists else the item
*/
void * vlist_remove_cursor_next(
	vlist_t* vlist);

/*!
    \brief This function moves the cursor to the front of the vlist structure.
	\var vlist The list operate on
*/
void vlist_reset_cursor(
	vlist_t* vlist);

/*!
    \brief This function returns item at the cursor but doesn't remove it.
	\var vlist The list operate on
	\return 0 if no such item exists else the item
*/
void * vlist_peek_cursor(
	vlist_t* vlist);

/*!
    \brief This function returns next item after the cursor but doesn't remove it.
	\var vlist The list operate on
	\return 0 if no such item exists else the item
*/
void * vlist_peek_cursor_next(
	vlist_t* vlist);

/*!
    \brief This function returns next item's next after the cursor but doesn't remove it.
	\var vlist The list operate on
	\return 0 if no such item exists else the item
*/
void * vlist_peek_cursor_next_next(
	vlist_t* vlist);

/*!
    \brief This function moves the cursor to the next postition in the vlist structure.
	\var vlist The list operate on
*/
void vlist_advance_cursor(
	vlist_t* vlist);

/*!
    \brief This function returns first item on the vlist but doesn't remove it.
	\var vlist The list operate on
	\return 0 if no such item exists else the item
*/
void * vlist_peek_first(
	vlist_t* vlist);

/*!
    \brief This function returns last item on the vlist but doesn't remove it.
	\var vlist The list operate on
	\return 0 if no such item exists else the item
*/
void * vlist_peek_last(
	vlist_t* vlist);

/*!
    \brief This function returns number of items in the vlist.
	\var vlist The list operate on
	\return Number of items in the list.
*/
unsigned long vlist_count(
	vlist_t* vlist);

/*!
    \brief This function checks the existence of a item in the vlist.
	\var vlist The list operate on
	\var item A void pointer look for
	\return 0 if item not found ~0 if found
*/
int vlist_item_exists(
	vlist_t* vlist, 
	void* item);

/*!
    \brief This function writes the items in the vlist into a array doesn't change the list .
	\var vlist The list operate on
	\var items An array of void pointers the size of number of items in the list
*/
void vlist_items_to_array(
	vlist_t* vlist, 
	void** items);

/*!
    \brief This function calls a function once for each item in the vlist. 
	\var vlist The list operate on
	\var function A pointer to a function taking an item and user data.
	\var user_data A pointer passed with each function call. 
*/
void vlist_map_function(
	vlist_t* vlist, 
	void(*function)(void*, void*),
	void * user_data);

/*!
    \brief This function peeks at the nth item in the vlist. 
	\var vlist The list operate on
	\var index A 0 based index into the vlist
	\return 0 if no such item exists else the item
*/
void* vlist_nth_item(
	vlist_t* vlist,
	unsigned long index);

/*!
    \brief This function reverses the items in the vlist.
	\var vlist The list operate on
*/
void vlist_reverse(
	vlist_t* vlist);

/*!
    \brief This function wipes out all items in the list
	\var vlist The list operate on
*/
void vlist_flush(
	vlist_t* vlist);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#ifndef V_POINTER_SIZED_INT
#ifdef _M_IA64
#define  V_POINTER_SIZED_INT    unsigned __int64
#else
#define  V_POINTER_SIZED_INT    unsigned long
#endif
#endif

/* these are used for the internal functions */
#define VLIST_RESET_CURSOR(XL) \
	(XL)->cursor = (XL)->head; \
	(XL)->cursor_index=0;

#define VLIST_PEEK_CURSOR(XL) \
    ((!(XL)->cursor)?0:(XL)->cursor->item)

#define	VLIST_ADVANCE_CURSOR(XL) \
    if((XL)->cursor) {\
	(XL)->cursor = (XL)->cursor->next; \
	(XL)->cursor_index++; \
    }


#define START_LIST_ITERATION(TYPE, LIST) VLIST_RESET_CURSOR(LIST);\
				while (1) {\
				TYPE *temp;\
				if (!(temp = (TYPE *)VLIST_PEEK_CURSOR(LIST)))\
					break;

#define END_LIST_ITERATION(LIST)  VLIST_ADVANCE_CURSOR(LIST); }


/* template wrapper */
#ifdef __cplusplus
extern "C++" {

/*the following macro generates a compile-time errors
  if anyone tries to instantiate a VList with incompatible types */
#define VLIST_COMPILE_TIME_ASSERT(expr) { char unnamed[(expr)?1:0]; (void)unnamed; }

template <class T> class VList {
public:

	VList() { m_vlist = new_vlist(malloc,free); size_assert(); };
	
	VList(void * (VLIST_CDECL *  vlist_malloc) (size_t), void (VLIST_CDECL *vlist_free) (void *)) {m_vlist = new_vlist( vlist_malloc,vlist_free); size_assert(); };

	~VList() {delete_vlist(m_vlist);};

	void AddFirst(T item) { vlist_add_first(m_vlist, (void *)item); };

	void AddLast(T item) { vlist_add_last(m_vlist, (void *)item); };

	void AddAfterCursor(T item) { vlist_add_after_cursor(m_vlist, (void *)item); };

	int Remove(T item) { return (vlist_remove(m_vlist, (void *)item)); };

	T RemoveFirst() { return ((T)(V_POINTER_SIZED_INT)vlist_remove_first(m_vlist)); };

 	void ResetCursor() { vlist_reset_cursor(m_vlist); };

 	T PeekCursor() { return ((T)(V_POINTER_SIZED_INT)vlist_peek_cursor(m_vlist)); };

 	T PeekCursorNext() { return ((T)(V_POINTER_SIZED_INT)vlist_peek_cursor_next(m_vlist)); };

	T PeekCursorNextNext() { return ((T)(V_POINTER_SIZED_INT)vlist_peek_cursor_next_next(m_vlist)); };

	void AdvanceCursor() { vlist_advance_cursor(m_vlist); };

	T PeekFirst() { return ((T)(V_POINTER_SIZED_INT)vlist_peek_first(m_vlist)); };

 	T PeekLast() { return ((T)(V_POINTER_SIZED_INT)vlist_peek_last(m_vlist)); };

 	unsigned long Count() { return (vlist_count(m_vlist)); };

	int ItemExists(T item) { return (vlist_item_exists(m_vlist, (void *)item)); };

	void ItemsToArray(T* items) { vlist_items_to_array(m_vlist, (void **)items); };

	void MapFunction(void(*function)(T, void*), void * user_data) {
		unsigned long i = Count();
		ResetCursor();
		while(i){
			(*function)(PeekCursor(),user_data);
			AdvanceCursor();
			i--;
		}
	};

	T NthItem(unsigned long index) { return ((T)(V_POINTER_SIZED_INT)vlist_nth_item(m_vlist, index)); };
	
	void Reverse() { vlist_reverse(m_vlist); };
	
	void Flush() { vlist_flush(m_vlist); };

private:
	struct vlist_s *m_vlist;

	/* VList cannot hold any type that cannot be converted to and from a (void *) */
	void size_assert() { VLIST_COMPILE_TIME_ASSERT(sizeof(T) <= sizeof(void *)); };

};
} /* extern "C++" */
#endif

#endif /*VLIST_H*/



