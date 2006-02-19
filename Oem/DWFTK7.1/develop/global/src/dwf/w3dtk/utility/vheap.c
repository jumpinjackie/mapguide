/*
 * Copyright (c) 1998 by Tech Soft America, LLC.
 * The information contained herein is confidential and proprietary to
 * Tech Soft America, LLC., and considered a trade secret as defined under
 * civil and criminal statutes.  Tech Soft America shall pursue its civil
 * and criminal remedies in the event of unauthorized use or misappropriation
 * of its trade secrets.  Use of this information by anyone other than
 * authorized employees of Tech Soft America, LLC. is granted only under a
 * written non-disclosure agreement, expressly prescribing the scope and
 * manner of such use.
 *
 * $Id: vheap.c,v 1.2 2004/09/20 18:37:55 heppe Exp $
 */

#include "vheap.h"
#include <stdlib.h>
#include <string.h>

/* just emulate HOOPS' internals for now. */
#define ALLOC_ARRAY(p, count, type) \
 	p = (type *)malloc((count) * sizeof(type))
#define TRY_ALLOC_ARRAY(p, count, type) ALLOC_ARRAY(p, count, type)
#define FREE_ARRAY(p, count, type)  free((void *)(p))
#define ALLOC(p, type)   ALLOC_ARRAY(p, 1, type)
#define FREE(p, type)    FREE_ARRAY(p, 1, type)
#define	COPY_MEMORY(_src_, _siz_, _dst_) memcpy(_dst_, _src_, _siz_)
#define	SET_MEMORY(_dst_, _siz_, _val_)	 memset(_dst_, _val_, _siz_)

typedef int HT_Boolean;
#define false 0
#define true 1
typedef long POINTER_SIZED_INT;

#define PARENT(i) (((i)-1)/2)
#define LEFT(i) (2*(i)+1)
#define RIGHT(i) (2*(i)+2)
#define SWAP(a,b,t)  ((t)=(a),(a)=(b),(b)=(t))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define BIG_FLOAT (1e25f)

static void heap_swap( iheap_t *heap, int index1, int index2 ) {
    int itemp;
    float ftemp;

    SWAP(heap->items[index1], heap->items[index2], itemp );
    SWAP(heap->values[index1], heap->values[index2], ftemp );
    SWAP(heap->lookup[heap->items[index1]], heap->lookup[heap->items[index2]], itemp );
}

static void heap_expand( iheap_t *heap, int size ) {
    POINTER_SIZED_INT *iarray;
    int *iarray2;
    float *farray;
    int old_allocated;

    old_allocated = heap->allocated;
    if( size > 10 )
	heap->allocated = size;
    else
	heap->allocated = 10;
    ALLOC_ARRAY( iarray, heap->allocated, POINTER_SIZED_INT );
    ALLOC_ARRAY( farray, heap->allocated, float );
    ALLOC_ARRAY( iarray2, heap->allocated, int );
    SET_MEMORY( iarray2, heap->allocated * sizeof( int ), -1 );
    if( old_allocated ) {
	COPY_MEMORY( heap->items, heap->used * sizeof( POINTER_SIZED_INT ), iarray );
	COPY_MEMORY( heap->values, heap->used * sizeof( int ), farray );
	COPY_MEMORY( heap->lookup, old_allocated * sizeof( int ), iarray2 );
	FREE_ARRAY( heap->items, old_allocated, POINTER_SIZED_INT );
	FREE_ARRAY( heap->values, old_allocated, float );
	FREE_ARRAY( heap->lookup, old_allocated, int );
    }
    heap->items = iarray;
    heap->values = farray;
    heap->lookup = iarray2;
}


static void heap_down( iheap_t *heap, int item ) {
    int l, r, i, largest;

    i = heap->lookup[item];
    l = LEFT(i);
    r = RIGHT(i);
    while( l < heap->used ) {
	if( r >= heap->used ) /* it could happen that l is in range, but r is not */
	    largest = l;
	else if( heap->values[l] > heap->values[r] )
	    largest = l;
	else
	    largest = r;
	/* now that we know which child is bigger, compare that to i */
	if( heap->values[largest] > heap->values[i] ) {
	    heap_swap( heap, i, largest );
	    i = largest;
	    l = LEFT(i);
	    r = RIGHT(i);
	}
	else
	    break;
    }
}


static void heap_up( iheap_t *heap, int item ) {
    int i, p;
    
    i = heap->lookup[item];
    p = PARENT(i);
    while( i > 0 && 
	   heap->values[i] > heap->values[p] ) {
	heap_swap( heap, i, p );
	i = p;
	p = PARENT(p);
    }
}


iheap_t *new_iheap() {
    iheap_t *heap;
    ALLOC( heap, iheap_t );
    heap->used = 0;
    heap->allocated = 0;
    heap->items = NULL;
    heap->values = NULL;
    heap->lookup = NULL;
    return heap;
}


vheap_t *new_vheap() {
    vheap_t *heap;
    ALLOC( heap, vheap_t );
    heap->iheap = new_iheap();
    heap->vhash = new_vhash(100, malloc, free);
    heap->reverse_vhash = new_vhash(100, malloc, free);
    heap->next_index = 0;
    return heap;
}


void delete_iheap( iheap_t *heap ) {
    if( heap->allocated ) {
	FREE_ARRAY( heap->items, heap->allocated, int );
	FREE_ARRAY( heap->values, heap->allocated, float );
	FREE_ARRAY( heap->lookup, heap->allocated, int );
    }
    FREE( heap, iheap_t );
}


void delete_vheap( vheap_t *heap ) {
    delete_iheap( heap->iheap );
    delete_vhash( heap->vhash );
    delete_vhash( heap->reverse_vhash );
    FREE( heap, vheap_t );
}


void iheap_insert( iheap_t *heap, int item, float value ) {
    if( item >= heap->allocated || heap->used >= heap->allocated ) {
	heap_expand( heap, MAX(item,heap->used)*2 );
    }
    heap->items[heap->used] = item;
    heap->values[heap->used] = value;
    heap->lookup[item] = heap->used;
    heap->used++;
    heap_up( heap, item );
}


void vheap_insert( vheap_t *heap, void const *item, float value ) {
    POINTER_SIZED_INT idx = heap->next_index++;

    vhash_insert_item( heap->vhash, (void *)item, (void *)idx );
    vhash_insert_item( heap->reverse_vhash, (void *)idx, (void *)item );
    iheap_insert( heap->iheap, idx, value );
}


void iheap_update( iheap_t *heap, int item, float value ) {
    float old_value;

    old_value = heap->values[ heap->lookup[item] ];
    heap->values[ heap->lookup[item] ] = value;
    if( value > old_value )
	heap_up( heap, item );
    else
	heap_down( heap, item );
}


void vheap_update( vheap_t *heap, void const *item, float value ) {
    POINTER_SIZED_INT idx;

    if( !vhash_lookup_item( heap->vhash, (void *)item, (void **)&idx ) )
	return;
    iheap_update( heap->iheap, idx, value );
}


HT_Boolean iheap_kill( iheap_t *heap, int item ) {
    if( heap->lookup[item] < 0 )
	return false;
    heap->values[ heap->lookup[item] ] = -BIG_FLOAT;
    heap_down( heap, item );
    heap->lookup[item] = -1;
    heap->used--;
    return true;
}


HT_Boolean vheap_kill( vheap_t *heap, void const *item ) {
    POINTER_SIZED_INT idx;

    if( !vhash_lookup_item( heap->vhash, (void *)item, (void **)&idx ) )
	return false;
    if( !iheap_kill( heap->iheap, idx ) )
	return false;
    if( !vhash_remove_item( heap->vhash, (void *)item, NULL ) )
	return false;
    if( !vhash_remove_item( heap->reverse_vhash, (void *)idx, NULL ) )
	return false;
    return true;
}


HT_Boolean iheap_pop( iheap_t *heap, POINTER_SIZED_INT *item_out, float *value_out ) 
{
    POINTER_SIZED_INT temp;

    if( heap->used <= 0 )
	return false;
    heap->used--;
    heap_swap( heap, 0, heap->used );
    temp = heap->items[ heap->used ];
    heap->lookup[ temp ] = -1;
    if( item_out )
        *item_out = temp;
    if( value_out )
        *value_out = heap->values[ heap->used ];
    heap_down( heap, heap->items[0] );
    return true;
}


HT_Boolean vheap_pop( vheap_t *heap, void **item, float *value ) {
    POINTER_SIZED_INT idx;
    void *ptr;

    if( !iheap_pop( heap->iheap, &idx, value ) )
	return false;
    if( !vhash_lookup_item( heap->reverse_vhash, (void *)idx, &ptr) )
	return false;
    if( item != NULL )
	*item = ptr;
    if( !vhash_remove_item( heap->vhash, ptr, NULL ) )
	return false;
    if( !vhash_remove_item( heap->reverse_vhash, (void *)idx, NULL ) )
	return false;
    return true;
}


HT_Boolean iheap_peek( iheap_t *heap, POINTER_SIZED_INT *item, float *value ) {
    if( heap->used <= 0 )
	return false;
    *item = heap->items[ 0 ];
    *value = heap->values[ 0 ];
    return true;
}

HT_Boolean vheap_peek( vheap_t *heap, void **item, float *value ) {
    POINTER_SIZED_INT idx;
    void *ptr;

    if( !iheap_peek( heap->iheap, &idx, value ) )
	return false;
    if( !vhash_lookup_item( heap->reverse_vhash, (void *)idx, &ptr) )
	return false;
    if( item != NULL )
	*item = ptr;
    return true;
}



