

#ifndef VCONF_H
#define VCONF_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "vhash.h"
#include "vlist.h"

#define VCONF_FILE_STYLE_FLAT		0
#define VCONF_FILE_STYLE_CHUNKED 	1

typedef struct vconf_s {
	const char * filename;
	vhash_t * options_hash;
	vlist_t * options_list;
	char delimiter;
	int style;
	int chunk;
	void *(VHASH_CDECL *malloc) (size_t);
	void (VHASH_CDECL *free) (void*);
} vconf_t;


vconf_t* new_vconf(
	void *(VHASH_CDECL *vconf_malloc) (size_t),
	void (VHASH_CDECL *vconf_free) (void *));

void delete_vconf(
	vconf_t* vconf); 

void vconf_clear_options(
	vconf_t* vconf);

void vconf_set_delimiter(
	vconf_t* vconf, 
	char delimiter);

int vconf_set_file_style(
	vconf_t* vconf, 
	int style);

unsigned int vconf_chunk_count(
	vconf_t* vconf);

int vconf_set_chunk(
	vconf_t* vconf, 
	unsigned int chunk);

int vconf_read_file(
	vconf_t* vconf, 
	const char * filename);

int vconf_write_file(
	vconf_t* vconf, 
	const char * filename);

const char * vconf_get_filename(
	vconf_t* vconf);

vhash_t * vconf_get_option_hash(
	vconf_t* vconf);

const char * vconf_get_option(
	vconf_t* vconf, 
	const char * option);

void vconf_set_option(
	vconf_t* vconf, 
	const char * option,
	const char * value);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /*VCONF_H*/


