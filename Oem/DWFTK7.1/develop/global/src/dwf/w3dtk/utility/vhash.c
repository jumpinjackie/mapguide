

#include "vhash.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _M_IA64
#define  V_POINTER_SIZED_INT    unsigned __int64
#else
#define  V_POINTER_SIZED_INT    unsigned long
#endif

#define NODE_STATE_NEW 0
#define NODE_STATE_OLD 1
#define NODE_STATE_IN_USE 2

typedef	struct hash_node_s{
    V_POINTER_SIZED_INT key;
    void * item;
    unsigned char state;
} vhash_node_t;

typedef struct hash_string_node_s{
    char * string;
    void * item;
} vhash_string_node_t;

static unsigned long get_power_of_2_size(unsigned long in_size)
{
    register unsigned long out_size;
    for (out_size=1;  out_size < in_size;  out_size = out_size << 1) ;
    return out_size;
}

vhash_t * new_vhash(
    unsigned long table_size,
    void *(VHASH_CDECL *vhash_malloc) (size_t),
    void (VHASH_CDECL *vhash_free) (void *)) 
{ 
    vhash_t *v = 0;

    v = (vhash_t *) vhash_malloc(sizeof(vhash_t));

    v->table_size = get_power_of_2_size(table_size);

    v->table = (vhash_node_t *) vhash_malloc(sizeof(vhash_node_t)*v->table_size);

    memset(v->table, 0, sizeof(vhash_node_t)*v->table_size);

    v->count=0;

    v->key_length = 0;
    v->key_string = 0;

    v->malloc = vhash_malloc;
    v->free = vhash_free;

    return v;
}

void delete_vhash(vhash_t * v)
{

    if(v->key_string){

	    unsigned long i;

	    v->free(v->key_string);

	    for(i=0; i<v->table_size; i++) {

		    if(v->table[i].state == NODE_STATE_IN_USE){
			    v->free(((vhash_string_node_t*)v->table[i].item)->string);
			    v->free(v->table[i].item);
		    }
	    }
    }

    v->free(v->table);
    v->free(v);
}


vhash_status_t vhash_rebuild_table(
	vhash_t * v,
	unsigned long table_size)
{

    unsigned long i;
    unsigned long old_size = v->table_size;
    vhash_node_t * old_table = v->table;

    if(table_size < v->count) 
	    table_size = v->count;

    v->table_size = get_power_of_2_size(table_size);
    v->table = (vhash_node_t *) v->malloc(sizeof(vhash_node_t) * table_size);

    memset(v->table, 0, sizeof(vhash_node_t)*v->table_size);

    v->count=0;

    for(i=0; i<old_size; i++) 
	    if(old_table[i].state == NODE_STATE_IN_USE)
		    vhash_insert_item(v, (void*) old_table[i].key, old_table[i].item);

    v->free(old_table);

    return VHASH_STATUS_SUCCESS;
}

unsigned long vhash_count(vhash_t* vhash)
{
    return vhash->count;
}


vhash_status_t vhash_replace_item(
	vhash_t * v,
	void * in_key, 
	void * new_item,
	void ** replaced_item)
{
    void * item = 0;
    vhash_status_t status = VHASH_STATUS_INSERTED;

    if(vhash_remove_item(v, in_key, &item)){
	if(replaced_item)
	    *replaced_item=item;
    	status = VHASH_STATUS_SUCCESS;
    }
	
    vhash_insert_item(v, in_key, new_item);

    return status;
}


vhash_status_t vhash_insert_item(
    vhash_t * v,
    void * in_key, 
    void * item)
{ 
    V_POINTER_SIZED_INT key = (V_POINTER_SIZED_INT) in_key;

    unsigned long hash;
    
    if(v->count*2 > v->table_size) {

	    vhash_rebuild_table(v, v->table_size*2);
    }

    hash = (unsigned long) (key&(v->table_size-1));

    while(v->table[hash].state == NODE_STATE_IN_USE){

	    hash++;

	    if(hash == v->table_size) 
		    hash=0;
    }

    v->table[hash].key=key;
    v->table[hash].item=item;
    v->table[hash].state=NODE_STATE_IN_USE;
    v->count++;

    return VHASH_STATUS_SUCCESS;
}

void vhash_insert(
	vhash_t * v,
	void * in_key, 
	void * item) 
{
    vhash_insert_item(v, in_key, item);
}

vhash_status_t vhash_remove_item(
	vhash_t * v,
	void * in_key,
	void ** removed_item)
{
    V_POINTER_SIZED_INT key = (V_POINTER_SIZED_INT) in_key;

    unsigned long hash;
    unsigned long first;
    
    hash = (unsigned long) (key&(v->table_size-1));

    first = hash;

    while(1) {

	    if(v->table[hash].state == NODE_STATE_NEW)
		    return VHASH_STATUS_FAILED;    	
	
	    if(v->table[hash].state == NODE_STATE_IN_USE && 
		v->table[hash].key == key) break;

	    hash++;

	    if(hash == v->table_size) hash=0;

	    /*full circle*/
	    if(hash == first) 
		return VHASH_STATUS_FAILED;
    }

    v->count--;

    v->table[hash].state = NODE_STATE_OLD;

    if(removed_item)
    	*removed_item = v->table[hash].item;
    
    return VHASH_STATUS_SUCCESS;
}

void * vhash_remove(
	vhash_t * v,
	void * in_key) 
{ 
    void * item = 0;
    vhash_remove_item(v, in_key, &item);
    return item;
}

vhash_status_t vhash_lookup_nth_item(
	const vhash_t * v,
	void * in_key,
	int n,
	void ** out_item) 
{

    V_POINTER_SIZED_INT key = (V_POINTER_SIZED_INT) in_key;

    unsigned long hash;
    unsigned long first;
    
    hash = (unsigned long) (key&(v->table_size-1));

    first = hash;

    while(1) {
	
	    if(v->table[hash].state == NODE_STATE_NEW) 
		return VHASH_STATUS_FAILED;

	    if(v->table[hash].state == NODE_STATE_IN_USE && 
	       v->table[hash].key == key) {
		if( n == 0 )
		    break;
		else
		    n--;
	    }
		    
	    hash++;

	    if(hash == v->table_size) hash=0;

	    /*full circle*/
	    if(hash == first) 
		return VHASH_STATUS_FAILED;
    }

    if(out_item)
	*out_item = v->table[hash].item;

    return VHASH_STATUS_SUCCESS;
}

vhash_status_t vhash_lookup_item(
	const vhash_t * v,
	void * in_key,
	void ** out_item) 
{
    return vhash_lookup_nth_item( v, in_key, 0, out_item );
}

void * vhash_lookup(
	vhash_t * v,
	void * in_key) 
{ 
    void * item = 0;
    vhash_lookup_item(v, in_key, &item);
    return item;
}



static V_POINTER_SIZED_INT vhash_char_key(
	vhash_t * v,
	const char * string)
{
    V_POINTER_SIZED_INT key = 0;

    int len = (int) strlen(string);
    int i;

    if(!v->key_string){

	    /*make a key*/
	    v->key_length = 64;
	    v->key_string = (char*) v->malloc(v->key_length);

	    for(i=0; i<v->key_length; i++){

		    v->key_string[i] = 1+(unsigned char) ((float)INT_MAX*rand()/(RAND_MAX+1.0));
	    }
    }

    for(i=0; i<len; i++){

	    key += string[i]*v->key_string[i];
    }

    return key;
}


vhash_status_t vhash_insert_string_key_item(
	vhash_t * v,
	const char * string, 
	void * item) 
{ 
    V_POINTER_SIZED_INT key = vhash_char_key(v, string);

    /*build up string node*/
    vhash_string_node_t * sn = (vhash_string_node_t *) v->malloc(sizeof(vhash_string_node_t));
    sn->item = item;
    sn->string = (char *) v->malloc(strlen(string)+1);
    strcpy(sn->string, string);
    
    return vhash_insert_item(v, (void*) key, (void*) sn);
}

void vhash_insert_string_key(
	vhash_t * v,
	const char * string, 
	void * item) 
{ 
    vhash_insert_string_key_item(v,string,item);
}


vhash_status_t vhash_remove_string_key_item(
    vhash_t * v,
    const char * string_key,
    void ** removed_item)
{ 
    V_POINTER_SIZED_INT key = vhash_char_key(v, string_key);

    vhash_string_node_t * sn = 0;
    
    unsigned long hash;
    unsigned long first;

    if(removed_item)
	*removed_item = 0;
    
    hash = (unsigned long) (key&(v->table_size-1));

    first = hash;

SEARCH_MORE:
    while(1) {
	    
	    if(v->table[hash].state == NODE_STATE_NEW) 
		return VHASH_STATUS_FAILED;

	    if(v->table[hash].state == NODE_STATE_IN_USE && 
		    v->table[hash].key == key) break;

	    hash++;
	    if(hash == v->table_size) hash=0;

	    /*full circle*/
	    if(hash == first) 
		return VHASH_STATUS_FAILED;
    }

    /*if here we have a posable match check it now*/
    sn = (vhash_string_node_t *) v->table[hash].item;

    if(strcmp(sn->string, string_key)) {
	    
	    hash++;
	    if(hash == v->table_size) hash=0;
	    /*full circle*/
	    if(hash == first) 
		return VHASH_STATUS_FAILED;

	    goto SEARCH_MORE;
    }

    v->count--;
    v->table[hash].state=NODE_STATE_OLD;

    if(removed_item)
	*removed_item = sn->item;

    /*throw out node*/
    v->free(sn->string);
    v->free(sn);
	
    return VHASH_STATUS_SUCCESS;
}

void * vhash_remove_string_key(
    vhash_t * v,
    const char * string)
{
    void * item = 0;
    vhash_remove_string_key_item(v, string, &item); 
    return item;
}



vhash_status_t vhash_lookup_nth_string_key_item(
    vhash_t * v,
    const char * string_key,
    int n,
    void ** out_item)
{ 
    V_POINTER_SIZED_INT key = vhash_char_key(v, string_key);

    vhash_string_node_t * sn = 0;
    
    unsigned long hash;
    unsigned long first;

    if(out_item)
	    *out_item = 0;
    
    hash = (unsigned long) (key&(v->table_size-1));

    first = hash;

SEARCH_MORE:
    while(1) {
	    
	    if(v->table[hash].state == NODE_STATE_NEW)
		return VHASH_STATUS_FAILED;
	    
	    if(v->table[hash].state == NODE_STATE_IN_USE &&
		v->table[hash].key == key) {
		if (n == 0)
		    break;
		else
		    n--;
	    }

	    hash++;
	    if(hash == v->table_size) hash=0;

	    /*full circle*/
	    if(hash == first) 
		return VHASH_STATUS_FAILED;
    }

    /*if here we have a posable match check it now*/
    sn = (vhash_string_node_t *) v->table[hash].item;

    if(strcmp(sn->string, string_key)) {
	    
	    hash++;
	    if(hash == v->table_size) hash=0;
	    /*full circle*/
	    if(hash == first) 
		return VHASH_STATUS_FAILED;
	    goto SEARCH_MORE;
    }

    if(out_item)
	    *out_item = sn->item;

    return VHASH_STATUS_SUCCESS;
}

vhash_status_t vhash_lookup_string_key_item(
    vhash_t * v,
    const char * string_key,
    void ** out_item)
{ 
    return vhash_lookup_nth_string_key_item( v, string_key, 0, out_item );
}

vhash_status_t vhash_replace_string_key_item(
	vhash_t * v,
	const char * string_key, 
	void * new_item,
	void ** replaced_item)
{
    void * item = 0;
    vhash_status_t status = VHASH_STATUS_INSERTED;

    if(vhash_remove_string_key_item(v, string_key, &item)){
		if(replaced_item)
			*replaced_item=item;
			status = VHASH_STATUS_SUCCESS;
    }
	
    vhash_insert_string_key_item(v, string_key, new_item);

    return status;
}



void * vhash_lookup_string_key(
	vhash_t * v,
	const char * string)
{
    void * item = 0;
    vhash_lookup_string_key_item(v, string, &item); 
    return item; 
}

void vhash_map_function(
    vhash_t * v,
    void(*function)(void*, void*, void*),
    void * user_data)
{
    unsigned long i;
    
    for(i=0; i<v->table_size; i++) {

	    if(v->table[i].state == NODE_STATE_IN_USE){

		    (*function)(v->table[i].item, (void*) v->table[i].key, user_data);

	    }
    }
}
    

void vhash_string_key_map_function(
    vhash_t * v,
    void (*function)(void*, const char *, void*),
    void * user_data)
{
    unsigned long i;
    vhash_string_node_t * sn;
    
    for(i=0; i<v->table_size; i++) {

	    if(v->table[i].state == NODE_STATE_IN_USE){

		    sn = (vhash_string_node_t *) v->table[i].item;

		    (*function)(sn->item, sn->string, user_data);
	    }
    }
}

struct vhash_to_vlist_data{
    void *(VHASH_CDECL *malloc) (size_t);
    vlist_t * vlist;
};

static void vhash_to_vlist_function(
	void* item, void* key, void* user_data)
{
    struct vhash_to_vlist_data* ud = (struct vhash_to_vlist_data*) user_data;	 
    vhash_pair_t * pair;

    pair = (vhash_pair_t *) ud->malloc(sizeof(vhash_pair_t)); 

    pair->item = item;
    pair->key = key;

    vlist_add_first(ud->vlist, (void*) pair);

}

vhash_status_t vhash_to_vlist(
    vhash_t * vhash,
    vlist_t * vlist,
    void *(VHASH_CDECL *vhash_pair_malloc) (size_t))
{
    struct vhash_to_vlist_data ud;

    ud.malloc = vhash_pair_malloc;
    ud.vlist = vlist;

    vhash_map_function(vhash, vhash_to_vlist_function, (void*)&ud);
    
    return VHASH_STATUS_SUCCESS;
}

static void vhash_string_keys_to_vlist_function(
	void* item, const char* key, void* user_data)
{
    struct vhash_to_vlist_data* ud = (struct vhash_to_vlist_data*) user_data;	 
    vhash_string_key_pair_t * pair;

    pair = (vhash_string_key_pair_t *) ud->malloc(sizeof(vhash_string_key_pair_t)); 

    pair->item = item;
    pair->key = key;

    vlist_add_first(ud->vlist, (void*) pair);

}

vhash_status_t vhash_string_keys_to_vlist(
    vhash_t * vhash,
    vlist_t * vlist,
    void *(VHASH_CDECL *vhash_pair_malloc) (size_t))
{
    struct vhash_to_vlist_data ud;

    ud.malloc = vhash_pair_malloc;
    ud.vlist = vlist;

    vhash_string_key_map_function(vhash, vhash_string_keys_to_vlist_function, (void*)&ud);
    
    return VHASH_STATUS_SUCCESS;
}


 
