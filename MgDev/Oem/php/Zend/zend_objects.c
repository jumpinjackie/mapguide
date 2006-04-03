/* 
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2004 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id: zend_objects.c,v 1.47.2.4 2005/08/04 08:46:53 dmitry Exp $ */

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
#include "zend_API.h"
#include "zend_interfaces.h"


ZEND_API void zend_objects_destroy_object(zend_object *object, zend_object_handle handle TSRMLS_DC)
{
	zend_function *destructor = object->ce->destructor;

	if (destructor) {
		zval zobj, *obj = &zobj;
		zval *old_exception;
		
		if (destructor->op_array.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED)) {
			if (destructor->op_array.fn_flags & ZEND_ACC_PRIVATE) {
				/* Ensure that if we're calling a private function, we're allowed to do so.
				 */
				if (object->ce != EG(scope)) {
					zend_class_entry *ce = object->ce;

					zend_error(EG(in_execution) ? E_ERROR : E_WARNING, 
						"Call to private %s::__destruct() from context '%s'%s", 
						ce->name, 
						EG(scope) ? EG(scope)->name : "", 
						EG(in_execution) ? "" : " during shutdown ignored");
					return;
				}
			} else {
				/* Ensure that if we're calling a protected function, we're allowed to do so.
				 */
				if (!zend_check_protected(destructor->common.scope, EG(scope))) {
					zend_class_entry *ce = object->ce;

					zend_error(EG(in_execution) ? E_ERROR : E_WARNING, 
						"Call to protected %s::__destruct() from context '%s'%s", 
						ce->name, 
						EG(scope) ? EG(scope)->name : "", 
						EG(in_execution) ? "" : " during shutdown ignored");
					return;
				}
			}
		}

		zobj.type = IS_OBJECT;
		zobj.value.obj.handle = handle;
		zobj.value.obj.handlers = &std_object_handlers;
		INIT_PZVAL(obj);

		/* Make sure that destructors are protected from previously thrown exceptions.
		 * For example, if an exception was thrown in a function and when the function's
		 * local variable destruction results in a destructor being called.
		 */
		old_exception = EG(exception);
		EG(exception) = NULL;
		zend_call_method_with_0_params(&obj, object->ce, NULL, "__destruct", NULL);
		if (old_exception) {
			if (EG(exception)) {
				zend_error(E_ERROR, "Ignoring exception from %s::__destruct() while an exception is already active", object->ce->name);
				zval_ptr_dtor(&EG(exception));
			}
			EG(exception) = old_exception;
		}
	}
}

ZEND_API void zend_objects_free_object_storage(zend_object *object TSRMLS_DC)
{
	zend_hash_destroy(object->properties);
	FREE_HASHTABLE(object->properties);
	efree(object);
}

ZEND_API zend_object_value zend_objects_new(zend_object **object, zend_class_entry *class_type TSRMLS_DC)
{	
	zend_object_value retval;

	*object = emalloc(sizeof(zend_object));
	(*object)->ce = class_type;
	retval.handle = zend_objects_store_put(*object, (zend_objects_store_dtor_t) zend_objects_destroy_object, (zend_objects_free_object_storage_t) zend_objects_free_object_storage, NULL TSRMLS_CC);
	retval.handlers = &std_object_handlers;
	(*object)->in_get = 0;
	(*object)->in_set = 0;
	return retval;
}

ZEND_API zend_object *zend_objects_get_address(zval *zobject TSRMLS_DC)
{
	return (zend_object *)zend_object_store_get_object(zobject TSRMLS_CC);
}

static void zval_add_ref_or_clone(zval **p)
{
	if (Z_TYPE_PP(p) == IS_OBJECT && !PZVAL_IS_REF(*p)) {
		TSRMLS_FETCH();

		if (Z_OBJ_HANDLER_PP(p, clone_obj) == NULL) {
			zend_error(E_ERROR, "Trying to clone an uncloneable object of class %s",  Z_OBJCE_PP(p)->name);
		} else {
			zval *orig = *p;

			ALLOC_ZVAL(*p);
			**p = *orig;
			INIT_PZVAL(*p);
			(*p)->value.obj = Z_OBJ_HT_PP(p)->clone_obj(orig TSRMLS_CC);
		}
	} else {
		(*p)->refcount++;
	}
}

ZEND_API void zend_objects_clone_members(zend_object *new_object, zend_object_value new_obj_val, zend_object *old_object, zend_object_handle handle TSRMLS_DC)
{
	if (EG(ze1_compatibility_mode)) {
		zend_hash_copy(new_object->properties, old_object->properties, (copy_ctor_func_t) zval_add_ref_or_clone, (void *) NULL /* Not used anymore */, sizeof(zval *));
	} else {
		zend_hash_copy(new_object->properties, old_object->properties, (copy_ctor_func_t) zval_add_ref, (void *) NULL /* Not used anymore */, sizeof(zval *));
	}
	if (old_object->ce->clone) {
		zval *new_obj;
		zval *clone_func_name;
		zval *retval_ptr;
		HashTable symbol_table;

		MAKE_STD_ZVAL(new_obj);
		new_obj->type = IS_OBJECT;
		new_obj->value.obj = new_obj_val;
		zval_copy_ctor(new_obj);

		/* FIXME: Optimize this so that we use the old_object->ce->clone function pointer instead of the name */
		MAKE_STD_ZVAL(clone_func_name);
		clone_func_name->type = IS_STRING;
		clone_func_name->value.str.val = estrndup("__clone", sizeof("__clone")-1);
		clone_func_name->value.str.len = sizeof("__clone")-1;

		ZEND_INIT_SYMTABLE(&symbol_table);
		
		call_user_function_ex(NULL, &new_obj, clone_func_name, &retval_ptr, 0, NULL, 0, &symbol_table TSRMLS_CC);

		zend_hash_destroy(&symbol_table);
		zval_ptr_dtor(&new_obj);
		zval_ptr_dtor(&clone_func_name);
		if(retval_ptr) {
			zval_ptr_dtor(&retval_ptr);
		}
	}
}

ZEND_API zend_object_value zend_objects_clone_obj(zval *zobject TSRMLS_DC)
{
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);

	/* assume that create isn't overwritten, so when clone depends on the 
	 * overwritten one then it must itself be overwritten */
	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = zend_objects_new(&new_object, old_object->ce TSRMLS_CC);

	ALLOC_HASHTABLE(new_object->properties);
	zend_hash_init(new_object->properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
