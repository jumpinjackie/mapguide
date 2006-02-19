
#include "vlist.h"

#include <stdio.h>
#include <stdlib.h>



vlist_t* new_vlist(
	void *(VLIST_CDECL *vlist_malloc) (size_t),
	void (VLIST_CDECL *vlist_free) (void *))
{
	vlist_t *vlist = 0;

	vlist = (vlist_t *) vlist_malloc(sizeof(vlist_t));

	vlist->head = 0;
	vlist->tail = 0;
	vlist->cursor = 0;
	vlist->cursor_index = 0;
	vlist->count = 0;
	vlist->malloc = vlist_malloc;
	vlist->free = vlist_free;

	return vlist;
}


void delete_vlist(
	vlist_t* vlist)
{
	vlist_node_t * head = vlist->head;
	vlist_node_t * tmp = 0;

	while(head) {
		tmp=head;
		head = head->next;
		vlist->free(tmp);
	}

	vlist->free(vlist);
}


void vlist_add_first(
	vlist_t* vlist,
	void* item) 
{
	vlist_node_t* node;

	node = (vlist_node_t *) vlist->malloc(sizeof(vlist_node_t));
	node->item = item;
	node->next = vlist->head;

	if(!vlist->head) vlist->tail=node;

	vlist->head = node;
	vlist->count++;
	vlist->cursor=0;
	vlist->cursor_index=0;
}


void vlist_add_last(
	vlist_t* vlist,
	void* item) 
{
	vlist_node_t* node;

	if(!vlist->head){
		vlist_add_first(vlist,item);
		return;
	}

	node = (vlist_node_t *) vlist->malloc(sizeof(vlist_node_t));
	node->item = item;
	node->next = 0;

	vlist->tail->next = node;
	vlist->tail = node;

	vlist->count++;
	vlist->cursor=0;
	vlist->cursor_index=0;
}


void vlist_add_after_cursor(
	vlist_t* vlist,
	void* item) 
{
	vlist_node_t* node;

	if(!vlist->cursor){
		vlist_add_first(vlist,item);
		return;
	}

	node = (vlist_node_t *) vlist->malloc(sizeof(vlist_node_t));
	node->item = item;
	node->next = vlist->cursor->next;

	vlist->cursor->next = node;
	if( vlist->cursor == vlist->tail ) {
		vlist->tail = node;
	}

	vlist->count++;
}


int vlist_remove(
	vlist_t* vlist, 
	void* item)
{
	vlist_node_t* node = vlist->head;
	vlist_node_t** node_last = &vlist->head;
	vlist_node_t* saved_tail=0;

	vlist->cursor=0;
	vlist->cursor_index=0;
	while(node){

		if(node->item == item) {

			*node_last = node->next;	
			if(vlist->tail==node) vlist->tail=saved_tail;
			vlist->free(node); 
			vlist->count--; 
			return 1;
		}

		node_last=&node->next;
		saved_tail=node;
		node=node->next; 
	}
	return 0;
}


void * vlist_remove_first(
	vlist_t* vlist)
{
	vlist_node_t* node = vlist->head;
	void * item;
	
	if (!vlist->head) return 0;
	vlist->cursor=0;
	vlist->cursor_index=0;

	vlist->head = node->next;

	item = node->item;

	vlist->free(node); 
	vlist->count--; 

	if(!vlist->head) vlist->tail=0;
	return item;
}


void * vlist_remove_cursor_next(
	vlist_t* vlist)
{
    vlist_node_t* cursor = vlist->cursor;
	vlist_node_t* node;
	void * item;
	
	if (!cursor) return 0;
	if (!cursor->next) return 0;
    node = cursor->next;
	item = node->item;
	if(vlist->tail == node) 
        vlist->tail = cursor;
    cursor->next = node->next;
	vlist->free(node); 
	vlist->count--; 
	return item;
}


void vlist_reset_cursor(
	vlist_t* vlist)
{
    VLIST_RESET_CURSOR(vlist)
}


void * vlist_peek_cursor(
	vlist_t* vlist)
{
    return VLIST_PEEK_CURSOR(vlist);
}


void * vlist_peek_cursor_next(
	vlist_t* vlist)
{
	if(!vlist->cursor) return 0;
	if(!vlist->cursor->next) return 0;
	return vlist->cursor->next->item;
}


void * vlist_peek_cursor_next_next(
	vlist_t* vlist)
{
	if(!vlist->cursor) return 0;
	if(!vlist->cursor->next) return 0;
	if(!vlist->cursor->next->next) return 0;
	return vlist->cursor->next->next->item;
}


void vlist_advance_cursor(
	vlist_t* vlist)
{
    VLIST_ADVANCE_CURSOR(vlist);
}


void * vlist_peek_first(
	vlist_t* vlist)
{
	if(!vlist->head) return 0;
	return vlist->head->item;
}


void * vlist_peek_last(
	vlist_t* vlist)
{
	if(!vlist->tail) return 0;
	return vlist->tail->item;
}


unsigned long vlist_count(
	vlist_t* vlist)
{
	return vlist->count;
}


int vlist_item_exists(
	vlist_t* vlist, 
	void* item)
{
	vlist_node_t* node = vlist->head;

	while(node) {

		if (node->item == item) return 1;

		node = node->next;
	}

	return 0;
}

void vlist_items_to_array(
	vlist_t* vlist, 
	void ** items)
{
	vlist_node_t* node = vlist->head;
	unsigned long n=0;

	while(node) {
		items[n] = node->item;	
		n++;
		node = node->next;
    }
}

void vlist_map_function(
	vlist_t* vlist, 
	void(*function)(void*, void*),
	void * user_data)
{
	vlist_node_t* node = vlist->head;

	while(node) {

		(*function)(node->item, user_data);
		node = node->next;
    }
}

void* vlist_nth_item(
	vlist_t* vlist,
	unsigned long index)
{
	if( (index+1) > vlist->count ) 
		return 0;
	else
	{
		if(!vlist->cursor || (vlist->cursor_index > index) ) {
			vlist->cursor = vlist->head;
			vlist->cursor_index=0;
		}
		index -= vlist->cursor_index;
		while(index > 0)
		{
			vlist->cursor = vlist->cursor->next;
			vlist->cursor_index++;
			index--;
		}

		return vlist->cursor->item;
	}

}


void vlist_reverse(
	vlist_t* vlist)
{
	void *item;

	if( (item = vlist_remove_first( vlist )) ) {
		vlist_reverse( vlist );
		vlist_add_last( vlist, item );
	}
}


void vlist_flush(
	vlist_t* vlist)
{

	while(vlist->head) {

		vlist_node_t* node = vlist->head;
		vlist->head = node->next;
		vlist->free(node); 
	}

	vlist->head = 0;
	vlist->tail = 0;
	vlist->cursor = 0;
	vlist->cursor_index = 0;
	vlist->count = 0;
	
}


