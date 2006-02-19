
/*! 
    \file vhash.h
    \brief A documented header file containing the vhash 
*/


#ifndef VHASH_H
#define VHASH_H

#include <stdlib.h>

#include "vlist.h"

#ifdef _WIN32_WCE
#define VHASH_CDECL __cdecl
#else
#define VHASH_CDECL 
#endif


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
    The vhash is a linear probing hash.

	This vhash assumes all keys are unique.
	If not remove the old key/item first the add the new one.

	The vhash can operate on void pointers or string keys.
	Do not mix these uses.

	This auto resizes at 50% capacity for
	best perfomance choose a size 2*max items expected.
*/

typedef struct vhash_s {
	struct hash_node_s * table;
	unsigned long count;
	unsigned long table_size;
	long key_length;
	char * key_string;
	void *(VHASH_CDECL *malloc) (size_t);
	void (VHASH_CDECL *free) (void*);
} vhash_t;

/*! \def VHASH_STATUS_FAILED
    \brief A failure status
*/
#define VHASH_STATUS_FAILED 0
/*! \def VHASH_STATUS_SUCCESS
    \brief A success status
*/
#define VHASH_STATUS_SUCCESS 1
/*! \def VHASH_STATUS_INSERTED
    \brief A item was inserted 
*/
#define VHASH_STATUS_INSERTED 2

/*! \def vhash_status_t
    \brief A type for function status
*/
typedef int vhash_status_t;

/*!
    \brief This function creates and initalises a vhash structure.
	\var table_size How big to create the hash >0 please
    \var vhas_malloc A malloc function for vhash to use    
    \var vhas_free A free function for vhash to use    
    \return The vhash structure 
*/
vhash_t * new_vhash(
    unsigned long table_size,
    void *(VHASH_CDECL *vhash_malloc) (size_t),
    void (VHASH_CDECL *vhash_free) (void *));

/*!
	\brief This function destroys and cleans up a vhash structure.
	\var vhash The vhash to delete
*/
void delete_vhash(vhash_t * vhash);

/*!
	\brief This function rebuilds the hash a specifed size never rebuild less than 2*count
	\var vhash The vhash to operate on
	\var table_size The new size
	\return a vhash_status_t the result of the rebuild
*/
vhash_status_t vhash_rebuild_table(
    vhash_t * vhash,
    unsigned long table_size);

/*!
	\brief This function returns the number of items in the vhash.
	\var vhash The vhash to operate on
	\return The number of items in the vhash
*/
unsigned long vhash_count(vhash_t* vhash);

/*!
	\brief This function calls a function once per item in the vhash.
	\brief Your function returns VHASH_STATUS_SUCCESS to keep mapping.
	\brief Your function returns VHASH_STATUS_FAILED to stop mapping.
	\var vhash The vhash to operate on
	\var function The function to call (item,key,user_data) 
	\var user_data A void pointer to pass with each item
*/
void vhash_map_function(
    vhash_t * v,
    void(*function)(void*, void*, void*),
    void * user_data);

/*! \def vhash_pair_t
    \brief A type for key item pairs
*/
typedef struct vhash_pair_s {
    void * key;
    void * item;
} vhash_pair_t;

/*! \def vhash_string_key_pair_t
    \brief A type for key item pairs when the vhash is storing strings
*/
typedef struct vhash_string_key_pair_s {
    const char * key;
    void * item;
} vhash_string_key_pair_t;

/*!
	\brief This function builds a vlist of item key pairs.
	\var vhash The vhash to operate on
	\var vlist A vlist to populate with key item pairs
	\var vhash_pair_malloc A malloc function to user to allocate the key item pairs
	\return a vhash_status_t the result of the action
*/
vhash_status_t vhash_to_vlist(
    vhash_t * vhash,
    vlist_t * vlist,
    void *(VHASH_CDECL *vhash_pair_malloc) (size_t));


/*!
	\brief This function replaces or adds a item to the vhash.
	\var vhash The vhash to operate on
	\var in_key A item key
	\var new_item The item to add to the vhash
	\var replaced_item A pointer to a void pointer that a replaced item can be write to
	\return a vhash_status_t the result of the action (returns VHASH_STATUS_SUCCESS always)
*/
vhash_status_t vhash_replace_item(
    vhash_t * v,
    void * in_key, 
    void * new_item,
    void ** replaced_item);

/*!
	\brief This function adds a item to the vhash.
	\var vhash The vhash to operate on
	\var in_key A item key
	\var item The item to add to the vhash
	\return a vhash_status_t the result of the action (returns VHASH_STATUS_SUCCESS always)
*/
vhash_status_t vhash_insert_item(
    vhash_t * v,
    void * in_key,
    void * item);

/*!
	\brief This function removes an item from the vhash.
	\var vhash The vhash to operate on
	\var in_key A item key
	\var removed_item A pointer to a void pointer that a removed item can be write to
	\return a vhash_status_t the result of the action 
*/
vhash_status_t vhash_remove_item(
    vhash_t * v,
    void * in_key,
    void ** removed_item);

/*!
	\brief This function looks up an item from the vhash.
	\var vhash The vhash to operate on
	\var in_key A item key
	\var out_item A pointer to a void pointer that a looked up item can be write to
	\return a vhash_status_t the result of the action
*/
vhash_status_t vhash_lookup_item(
    const vhash_t * v,
    void * in_key,
    void ** out_item);

/*!
	\brief This function behaves exactly like vhash_lookup_item, except it skips a number of 
	    matches equal to the given 'n'.  When hash keys are not guaranteed to be unique, this function
	    allows access to all of the items that match a given key.
	\var vhash The vhash to operate on
	\var in_key A item key
	\var n The number of matches to skip
	\var out_item A pointer to a void pointer that a looked up item can be write to
	\return a vhash_status_t the result of the action
*/
vhash_status_t vhash_lookup_nth_item(
    const vhash_t * v,
    void * in_key,
    int n,
    void ** out_item);

/*!
	\brief This function calls a function once per item in the vhash.
	\brief The keys are strings.
	\brief Your function returns VHASH_STATUS_SUCCESS to keep mapping.
	\brief Your function returns VHASH_STATUS_FAILED to stop mapping.
	\var vhash The vhash to operate on
	\var function The function to call (item,key,user_data) 
	\var user_data A void pointer to pass with each item
*/
void vhash_string_key_map_function(
    vhash_t * v,
    void(*function)(void*, const char *, void*),
    void * user_data);

/*!
	\brief This function replaces or adds a item to the vhash.
	\brief The keys are strings.
	\brief replaced_item is valid only if return status is VHASH_STATUS_SUCCESS 
	\brief returns VHASH_STATUS_INSERTED is no item with same key existed
	\var vhash The vhash to operate on
	\var string_key A null terminated item key
	\var new_item The item to add to the vhash
	\var replaced_item A pointer to a void pointer that a replaced item can be write to
	\return a vhash_status_t the result of the action
*/
vhash_status_t vhash_replace_string_key_item(
    vhash_t * v,
    const char * string_key, 
    void * new_item,
    void ** replaced_item);

/*!
	\brief This function adds a item to the vhash.
	\brief The keys are strings.
	\var vhash The vhash to operate on
	\var string_key A null terminated item key
	\var item The item to add to the vhash
	\return a vhash_status_t the result of the action
*/
vhash_status_t vhash_insert_string_key_item(
    vhash_t * v,
    const char * string_key, 
    void * item);

/*!
	\brief This function removes an item from the vhash.
	\brief The keys are strings.
	\var vhash The vhash to operate on
	\var string_key A null terminated item key
	\var removed_item A pointer to a void pointer that a removed item can be write to
	\return a vhash_status_t the result of the action
*/
vhash_status_t vhash_remove_string_key_item(
    vhash_t * v,
    const char * string_key,
    void ** removed_item);

/*!
	\brief This function looks up an item from the vhash.
	\brief The keys are strings.
	\var vhash The vhash to operate on
	\var string_key A null terminated item key
	\var out_item A pointer to a void pointer that a looked up item can be write to
	\return a vhash_status_t the result of the action
*/
vhash_status_t vhash_lookup_string_key_item(
    vhash_t * v,
    const char * string_key,
    void ** out_item);

/*!
	\brief This function behaves exactly like vhash_lookup_string_key_item, except it skips a number of 
	    matches equal to the given 'n'.  When hash keys are not guaranteed to be unique, this function
	    allows access to all of the items that match a given key.
	\brief The keys are strings.
	\var vhash The vhash to operate on
	\var string_key A null terminated item key
	\var n The number of matches to skip
	\var out_item A pointer to a void pointer that a looked up item can be write to
	\return a vhash_status_t the result of the action
*/
vhash_status_t vhash_lookup_nth_string_key_item(
    vhash_t * v,
    const char * string_key,
    int n,
    void ** out_item);


/*!
	\brief This function builds a string key item pair from the vhash.
	\var vhash The vhash to operate on
	\var vlist A vlist to populate with string key item pairs
	\var vhash_pair_malloc A malloc function to user to allocate the key item pairs
	\return a vhash_status_t the result of the action
*/
vhash_status_t vhash_string_keys_to_vlist(
    vhash_t * vhash,
    vlist_t * vlist,
    void *(VHASH_CDECL *vhash_pair_malloc) (size_t));


/*THIS IS OBSOLETE*/
void vhash_insert(
    vhash_t * vhash,
    void * key, void * item);

/*THIS IS OBSOLETE*/
void * vhash_remove(
    vhash_t * vhash,
    void * key);

/*THIS IS OBSOLETE*/
void * vhash_lookup(
    vhash_t * vhash,
    void * key);


/*THIS IS OBSOLETE*/
void vhash_insert_string_key(
	vhash_t * v,
	const char * string, 
	void * item);

/*THIS IS OBSOLETE*/
void * vhash_remove_string_key(
	vhash_t * v,
	const char * string);

/*THIS IS OBSOLETE*/
void * vhash_lookup_string_key(
	vhash_t * v,
	const char * string);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /*VHASH_H*/


