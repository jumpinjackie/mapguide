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
   | Authors: Timm Friebe <thekid@thekid.de>                              |
   |          George Schlossnagle <george@omniti.com>                     |
   |          Andrei Zmievski <andrei@gravitonic.com>                     |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id: zend_reflection_api.c,v 1.111.2.25 2005/08/30 06:46:35 helly Exp $ */
#include "zend.h"
#include "zend_API.h"
#include "zend_exceptions.h"
#include "zend_operators.h"
#include "zend_constants.h"
#include "zend_ini.h"
#include "zend_interfaces.h"

/* Class entry pointers */
zend_class_entry *reflector_ptr;
zend_class_entry *reflection_exception_ptr;
zend_class_entry *reflection_ptr;
zend_class_entry *reflection_function_ptr;
zend_class_entry *reflection_parameter_ptr;
zend_class_entry *reflection_class_ptr;
zend_class_entry *reflection_object_ptr;
zend_class_entry *reflection_method_ptr;
zend_class_entry *reflection_property_ptr;
zend_class_entry *reflection_extension_ptr;

/* Method macros */
#define METHOD_NOTSTATIC                                                                                    \
	if (!this_ptr) {                                                                                        \
		zend_error(E_ERROR, "%s() cannot be called statically", get_active_function_name(TSRMLS_C));        \
		return;                                                                                             \
	}                                                                                                       \

#define METHOD_NOTSTATIC_NUMPARAMS(c) METHOD_NOTSTATIC                                                      \
	if (ZEND_NUM_ARGS() > c) {                                                                              \
		ZEND_WRONG_PARAM_COUNT();                                                                           \
	}                                                                                                       \

/* Exception throwing macro */
#define _DO_THROW(msg)                                                                                      \
	zend_throw_exception(reflection_exception_ptr, msg, 0 TSRMLS_CC);                                       \
	return;                                                                                                 \

#define RETURN_ON_EXCEPTION                                                                                 \
	if (EG(exception) && Z_OBJCE_P(EG(exception)) == reflection_exception_ptr) {                            \
		return;                                                                                             \
	}

#define GET_REFLECTION_OBJECT_PTR(target)                                                                   \
	intern = (reflection_object *) zend_object_store_get_object(getThis() TSRMLS_CC);                       \
	if (intern == NULL || intern->ptr == NULL) {                                                            \
		RETURN_ON_EXCEPTION                                                                                 \
		zend_error(E_ERROR, "Internal error: Failed to retrieve the reflection object");                    \
	}                                                                                                       \
	target = intern->ptr;                                                                                   \

/* {{{ Smart string functions */
typedef struct _string {
	char *string;
	int len;
	int alloced;
} string;

static void string_init(string *str)
{
	str->string = (char *) emalloc(1024);
	str->len = 1;
	str->alloced = 1024;
	*str->string = '\0';
}
	
static string *string_printf(string *str, const char *format, ...)
{
	int len;
	va_list arg;
	char *s_tmp;

	va_start(arg, format);
	len = zend_vspprintf(&s_tmp, 0, format, arg);
	if (len) {
		register int nlen = (str->len + len + (1024 - 1)) & ~(1024 - 1);
		if (str->alloced < nlen) {
			str->alloced = nlen;
			str->string = erealloc(str->string, str->alloced);
		}
		memcpy(str->string + str->len - 1, s_tmp, len + 1);
		str->len += len;
	}
	efree(s_tmp);
	va_end(arg);
	return str;
}

static string *string_write(string *str, char *buf, int len)
{
	register int nlen = (str->len + len + (1024 - 1)) & ~(1024 - 1);
	if (str->alloced < nlen) {
		str->alloced = nlen;
		str->string = erealloc(str->string, str->alloced);
	}
	memcpy(str->string + str->len - 1, buf, len);
	str->len += len;
	str->string[str->len - 1] = '\0';
	return str;
}

static string *string_append(string *str, string *append)
{
	if (append->len > 1) {
		string_write(str, append->string, append->len - 1);
	}
	return str;
}

static void string_free(string *str)
{
	efree(str->string);
	str->len = 0;
	str->alloced = 0;
	str->string = NULL;
}
/* }}} */

/* Struct for properties */
typedef struct _property_reference {
	zend_class_entry *ce;
	zend_property_info *prop;
} property_reference;

/* Struct for parameters */
typedef struct _parameter_reference {
	zend_uint offset;
	zend_uint required;
	struct _zend_arg_info *arg_info;
	zend_function *fptr;
} parameter_reference;

/* Struct for reflection objects */
typedef struct {
	zend_object zo;
	void *ptr;
	unsigned int free_ptr:1;
	zval *obj;
	zend_class_entry *ce;
} reflection_object;

static zend_object_handlers reflection_object_handlers;

static void _default_get_entry(zval *object, char *name, int name_len, zval *return_value TSRMLS_DC)
{
	zval **value;

	if (zend_hash_find(Z_OBJPROP_P(object), name, name_len, (void **) &value) == FAILURE) {
		RETURN_FALSE;
	}

	*return_value = **value;
	zval_copy_ctor(return_value);
}

static void reflection_register_implement(zend_class_entry *class_entry, zend_class_entry *interface_entry TSRMLS_DC)
{
	zend_uint num_interfaces = ++class_entry->num_interfaces;

	class_entry->interfaces = (zend_class_entry **) realloc(class_entry->interfaces, sizeof(zend_class_entry *) * num_interfaces);
	class_entry->interfaces[num_interfaces - 1] = interface_entry;
}

static void reflection_free_objects_storage(zend_object *object TSRMLS_DC)
{
	reflection_object *intern = (reflection_object *) object;

	if (intern->free_ptr && intern->ptr) {
		efree(intern->ptr);
		intern->ptr = NULL;
	}
	if (intern->obj) {
		zval_ptr_dtor(&intern->obj);
	}
	zend_objects_free_object_storage(object TSRMLS_CC);
}

static void reflection_objects_clone(void *object, void **object_clone TSRMLS_DC)
{
	reflection_object *intern = (reflection_object *) object;
	reflection_object **intern_clone = (reflection_object **) object_clone;

	*intern_clone = emalloc(sizeof(reflection_object));
	(*intern_clone)->zo.ce = intern->zo.ce;
	(*intern_clone)->zo.in_get = 0;
	(*intern_clone)->zo.in_set = 0;
	ALLOC_HASHTABLE((*intern_clone)->zo.properties);
	(*intern_clone)->ptr = intern->ptr;
	(*intern_clone)->free_ptr = intern->free_ptr;
	(*intern_clone)->obj = intern->obj;
	if (intern->obj) {
		zval_add_ref(&intern->obj);
	}
}

static zend_object_value reflection_objects_new(zend_class_entry *class_type TSRMLS_DC)
{
	zval tmp;
	zend_object_value retval;
	reflection_object *intern;

	intern = emalloc(sizeof(reflection_object));
	intern->zo.ce = class_type;
	intern->zo.in_get = 0;
	intern->zo.in_set = 0;
	intern->ptr = NULL;
	intern->obj = NULL;
	intern->free_ptr = 0;

	ALLOC_HASHTABLE(intern->zo.properties);
	zend_hash_init(intern->zo.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	retval.handle = zend_objects_store_put(intern, NULL, reflection_free_objects_storage, reflection_objects_clone TSRMLS_CC);
	retval.handlers = &reflection_object_handlers;
	return retval;
}

static zval * reflection_instanciate(zend_class_entry *pce, zval *object TSRMLS_DC)
{
	if (!object) {
		ALLOC_ZVAL(object);
	}
	Z_TYPE_P(object) = IS_OBJECT;
	object_init_ex(object, pce);
	object->refcount = 1;
	object->is_ref = 1;
	return object;
}

static void _const_string(string *str, char *name, zval *value, char *indent TSRMLS_DC);
static void _function_string(string *str, zend_function *fptr, char *indent TSRMLS_DC);
static void _property_string(string *str, zend_property_info *prop, char *prop_name, char* indent TSRMLS_DC);
static void _class_string(string *str, zend_class_entry *ce, zval *obj, char *indent TSRMLS_DC);
static void _extension_string(string *str, zend_module_entry *module, char *indent TSRMLS_DC);

/* {{{ _class_string */
static void _class_string(string *str, zend_class_entry *ce, zval *obj, char *indent TSRMLS_DC)
{
	int count, count_static_props = 0, count_static_funcs = 0, count_shadow_props = 0;
	string sub_indent;
	
	string_init(&sub_indent);
	string_printf(&sub_indent, "%s    ", indent);

	/* TBD: Repair indenting of doc comment (or is this to be done in the parser?) */
	if (ce->type == ZEND_USER_CLASS && ce->doc_comment) {
		string_printf(str, "%s%s", indent, ce->doc_comment);
		string_write(str, "\n", 1);
	}

	if (obj) {
		string_printf(str, "%sObject of class [ ", indent);
	} else {
		string_printf(str, "%s%s [ ", indent, (ce->ce_flags & ZEND_ACC_INTERFACE) ? "Interface" : "Class");
	}
	string_printf(str, (ce->type == ZEND_USER_CLASS) ? "<user" : "<internal");
	if (ce->module) {
		string_printf(str, ":%s", ce->module->name);
	}
	string_printf(str, "> ");
	if (ce->get_iterator != NULL) {
		string_printf(str, "<iterateable> ");
	}
	if (ce->ce_flags & ZEND_ACC_INTERFACE) {
		string_printf(str, "interface ");
	} else {
		if (ce->ce_flags & (ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
			string_printf(str, "abstract ");
		}
		if (ce->ce_flags & ZEND_ACC_FINAL_CLASS) {
			string_printf(str, "final ");
		} 
		string_printf(str, "class ");
	}
	string_write(str, ce->name, ce->name_length);
	if (ce->parent) {
		string_printf(str, " extends %s", ce->parent->name);
	}

	if (ce->num_interfaces) {
		zend_uint i;

		string_printf(str, " implements %s", ce->interfaces[0]->name);
		for (i = 1; i < ce->num_interfaces; ++i) {
			string_printf(str, ", %s", ce->interfaces[i]->name);
		}
	}
	string_printf(str, " ] {\n");

	/* The information where a class is declared is only available for user classes */
	if (ce->type == ZEND_USER_CLASS) {
		string_printf(str, "%s  @@ %s %d-%d\n", indent, ce->filename,
					  ce->line_start, ce->line_end);
	}

	/* Constants */
	if (&ce->constants_table) {
		string_printf(str, "\n");
		count = zend_hash_num_elements(&ce->constants_table);
		string_printf(str, "%s  - Constants [%d] {\n", indent, count);
		if (count > 0) {
			HashPosition pos;
			zval **value;
			char *key;
			uint key_len;
			ulong num_index;

			zend_hash_internal_pointer_reset_ex(&ce->constants_table, &pos);

			while (zend_hash_get_current_data_ex(&ce->constants_table, (void **) &value, &pos) == SUCCESS) {
				zend_hash_get_current_key_ex(&ce->constants_table, &key, &key_len, &num_index, 0, &pos);

				_const_string(str, key, *value, indent TSRMLS_CC);
				zend_hash_move_forward_ex(&ce->constants_table, &pos);
			}
		}
		string_printf(str, "%s  }\n", indent);
	}

	/* Static properties */
	if (&ce->properties_info) {
		/* counting static properties */		
		count = zend_hash_num_elements(&ce->properties_info);
		if (count > 0) {
			HashPosition pos;
			zend_property_info *prop;

			zend_hash_internal_pointer_reset_ex(&ce->properties_info, &pos);

			while (zend_hash_get_current_data_ex(&ce->properties_info, (void **) &prop, &pos) == SUCCESS) {
				if(prop->flags & ZEND_ACC_SHADOW) {
					count_shadow_props++;
				} else if (prop->flags & ZEND_ACC_STATIC) {
					count_static_props++;
				}
				zend_hash_move_forward_ex(&ce->properties_info, &pos);
			}
		}

		/* static properties */		
		string_printf(str, "\n%s  - Static properties [%d] {\n", indent, count_static_props);
		if (count_static_props > 0) {
			HashPosition pos;
			zend_property_info *prop;

			zend_hash_internal_pointer_reset_ex(&ce->properties_info, &pos);

			while (zend_hash_get_current_data_ex(&ce->properties_info, (void **) &prop, &pos) == SUCCESS) {
				if ((prop->flags & ZEND_ACC_STATIC) && !(prop->flags & ZEND_ACC_SHADOW)) {
					_property_string(str, prop, NULL, sub_indent.string TSRMLS_CC);
				}

				zend_hash_move_forward_ex(&ce->properties_info, &pos);
			}
		}
		string_printf(str, "%s  }\n", indent);
	}
	
	/* Static methods */
	if (&ce->function_table) {
		/* counting static properties */		
		count = zend_hash_num_elements(&ce->function_table);
		if (count > 0) {
			HashPosition pos;
			zend_function *mptr;

			zend_hash_internal_pointer_reset_ex(&ce->function_table, &pos);

			while (zend_hash_get_current_data_ex(&ce->function_table, (void **) &mptr, &pos) == SUCCESS) {
				if (mptr->common.fn_flags & ZEND_ACC_STATIC) {
					count_static_funcs++;
				}
				zend_hash_move_forward_ex(&ce->function_table, &pos);
			}
		}

		/* static properties */		
		string_printf(str, "\n%s  - Static methods [%d] {", indent, count_static_funcs);
		if (count_static_funcs > 0) {
			HashPosition pos;
			zend_function *mptr;

			zend_hash_internal_pointer_reset_ex(&ce->function_table, &pos);

			while (zend_hash_get_current_data_ex(&ce->function_table, (void **) &mptr, &pos) == SUCCESS) {
				if (mptr->common.fn_flags & ZEND_ACC_STATIC) {
					string_printf(str, "\n");
					_function_string(str, mptr, sub_indent.string TSRMLS_CC);
				}
				zend_hash_move_forward_ex(&ce->function_table, &pos);
			}
		} else {
			string_printf(str, "\n");
		}
		string_printf(str, "%s  }\n", indent);
	}

	/* Default/Implicit properties */
	if (&ce->properties_info) {
		count = zend_hash_num_elements(&ce->properties_info) - count_static_props - count_shadow_props;
		string_printf(str, "\n%s  - Properties [%d] {\n", indent, count);
		if (count > 0) {
			HashPosition pos;
			zend_property_info *prop;

			zend_hash_internal_pointer_reset_ex(&ce->properties_info, &pos);

			while (zend_hash_get_current_data_ex(&ce->properties_info, (void **) &prop, &pos) == SUCCESS) {
				if (!(prop->flags & (ZEND_ACC_STATIC|ZEND_ACC_SHADOW))) {
					_property_string(str, prop, NULL, sub_indent.string TSRMLS_CC);
				}
				zend_hash_move_forward_ex(&ce->properties_info, &pos);
			}
		}
		string_printf(str, "%s  }\n", indent);
	}

	if (obj && Z_OBJ_HT_P(obj)->get_properties) {
		string       dyn;
		HashTable    *properties = Z_OBJ_HT_P(obj)->get_properties(obj TSRMLS_CC);
		HashPosition pos;
		zval         **prop;

		string_init(&dyn);
		count = 0;

		zend_hash_internal_pointer_reset_ex(properties, &pos);

		while (zend_hash_get_current_data_ex(properties, (void **) &prop, &pos) == SUCCESS) {
			char  *prop_name;
			uint  prop_name_size;
			ulong index;

			if (zend_hash_get_current_key_ex(properties, &prop_name, &prop_name_size, &index, 1, &pos) == HASH_KEY_IS_STRING) {
				if (prop_name_size && prop_name[0]) { /* skip all private and protected properties */
					if (!zend_hash_quick_exists(&ce->properties_info, prop_name, prop_name_size, zend_get_hash_value(prop_name, prop_name_size))) {
						count++;
						_property_string(&dyn, NULL, prop_name, sub_indent.string TSRMLS_CC);	
					}
				}
				efree(prop_name);
			}
			zend_hash_move_forward_ex(properties, &pos);
		}

		string_printf(str, "\n%s  - Dynamic properties [%d] {\n", indent, count);
		string_append(str, &dyn);
		string_printf(str, "%s  }\n", indent);
		string_free(&dyn);
	}

	/* Non static methods */
	if (&ce->function_table) {
		count = zend_hash_num_elements(&ce->function_table) - count_static_funcs;
		string_printf(str, "\n%s  - Methods [%d] {", indent, count);
		if (count > 0) {
			HashPosition pos;
			zend_function *mptr;

			zend_hash_internal_pointer_reset_ex(&ce->function_table, &pos);

			while (zend_hash_get_current_data_ex(&ce->function_table, (void **) &mptr, &pos) == SUCCESS) {
				if (!(mptr->common.fn_flags & ZEND_ACC_STATIC)) {
					string_printf(str, "\n");
					_function_string(str, mptr, sub_indent.string TSRMLS_CC);
				}
				zend_hash_move_forward_ex(&ce->function_table, &pos);
			}
		} else {
			string_printf(str, "\n");
		}
		string_printf(str, "%s  }\n", indent);
	}
	
	string_printf(str, "%s}\n", indent);
	string_free(&sub_indent);
}
/* }}} */

/* {{{ _const_string */
static void _const_string(string *str, char *name, zval *value, char *indent TSRMLS_DC)
{
	string_printf(str, "%s    Constant [ %s %s ] { }\n",
			   indent,
			   zend_zval_type_name(value),
			   name);
}
/* }}} */

/* {{{ _parameter_string */
static void _parameter_string(string *str, zend_function *fptr, struct _zend_arg_info *arg_info, zend_uint offset, zend_uint required, char* indent TSRMLS_DC)
{
	string_printf(str, "Parameter #%d [ ", offset);
	if (offset >= required) {
		string_printf(str, "<optional> ");
	} else {
		string_printf(str, "<required> ");
	}
	if (arg_info->class_name) {
		string_printf(str, "%s ", arg_info->class_name);
		if (arg_info->allow_null) {
			string_printf(str, "or NULL ");
		}
	}
	if (arg_info->pass_by_reference) {
		string_write(str, "&", sizeof("&")-1);
	}
	if (arg_info->name) {
		string_printf(str, "$%s", arg_info->name);
	} else {
		string_printf(str, "$param%d", offset);
	}
	if (fptr->type == ZEND_USER_FUNCTION && offset >= required) {
		zend_op *precv = &((zend_op_array*)fptr)->opcodes[offset*2 + 1];
		if (precv->opcode == ZEND_RECV_INIT && precv->op2.op_type != IS_UNUSED) {
			zval *zv, zv_copy;
			int use_copy;
			string_write(str, " = ", sizeof(" = ")-1);
			zv_copy = precv->op2.u.constant;
			zv = &zv_copy;
			zval_update_constant(&zv, (void*)1 TSRMLS_CC);
			if (Z_TYPE_P(zv) == IS_NULL) {
				string_write(str, "NULL", sizeof("NULL")-1);
			} else if (Z_TYPE_P(zv) == IS_STRING) {
				string_write(str, "'", sizeof("'")-1);
				string_write(str, Z_STRVAL_P(zv), MIN(Z_STRLEN_P(zv), 15));
				if (Z_STRLEN_P(zv) > 15) {
					string_write(str, "...", sizeof("...")-1);
				}
				string_write(str, "'", sizeof("'")-1);
			} else {
				zend_make_printable_zval(zv, &zv_copy, &use_copy);
				string_write(str, Z_STRVAL(zv_copy), Z_STRLEN(zv_copy));
				zval_dtor(&zv_copy);
			}
			zval_ptr_dtor(&zv);
		}
	}
	string_write(str, " ]", sizeof(" ]")-1);
}
/* }}} */

/* {{{ _function_parameter_string */
static void _function_parameter_string(string *str, zend_function *fptr, char* indent TSRMLS_DC)
{
	struct _zend_arg_info *arg_info = fptr->common.arg_info;
	zend_uint i, required = fptr->common.required_num_args;

	if (!arg_info) {
		return;
	}

	string_printf(str, "\n");
	string_printf(str, "%s- Parameters [%d] {\n", indent, fptr->common.num_args);
	for (i = 0; i < fptr->common.num_args; i++) {
		string_printf(str, "%s  ", indent);
		_parameter_string(str, fptr, arg_info, i, required, indent TSRMLS_CC);
		string_write(str, "\n", sizeof("\n")-1);
		arg_info++;
	}
	string_printf(str, "%s}\n", indent);
}
/* }}} */

/* {{{ _function_string */
static void _function_string(string *str, zend_function *fptr, char* indent TSRMLS_DC)
{
	string param_indent;

	/* TBD: Repair indenting of doc comment (or is this to be done in the parser?)
	 * What's "wrong" is that any whitespace before the doc comment start is 
	 * swallowed, leading to an unaligned comment.
	 */
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.doc_comment) {
		string_printf(str, "%s%s\n", indent, fptr->op_array.doc_comment);
	}

	string_printf(str, fptr->common.scope ? "%sMethod [ " : "%sFunction [ ", indent);
	string_printf(str, (fptr->type == ZEND_USER_FUNCTION) ? "<user> " : "<internal> ");
	if (fptr->common.fn_flags & ZEND_ACC_CTOR) {
		string_printf(str, "<ctor> ");
	}
	if (fptr->common.fn_flags & ZEND_ACC_DTOR) {
		string_printf(str, "<dtor> ");
	}
	if (fptr->common.fn_flags & ZEND_ACC_ABSTRACT) {
		string_printf(str, "abstract ");
	}
	if (fptr->common.fn_flags & ZEND_ACC_FINAL) {
		string_printf(str, "final ");
	}
	if (fptr->common.fn_flags & ZEND_ACC_STATIC) {
		string_printf(str, "static ");
	}

	/* These are mutually exclusive */
	switch (fptr->common.fn_flags & ZEND_ACC_PPP_MASK) {
		case ZEND_ACC_PUBLIC:
			string_printf(str, "public ");
			break;
		case ZEND_ACC_PRIVATE:
			string_printf(str, "private ");
			break;
		case ZEND_ACC_PROTECTED:
			string_printf(str, "protected ");
			break;
		default:
		    string_printf(str, "<visibility error> ");
		    break;
	}

	string_printf(str, fptr->common.scope ? "method " : "function ");
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.return_reference) {
		string_printf(str, "&");
	}
	string_printf(str, "%s ] {\n", fptr->common.function_name);
	/* The information where a function is declared is only available for user classes */
	if (fptr->type == ZEND_USER_FUNCTION) {
		string_printf(str, "%s  @@ %s %d - %d\n", indent, 
												  fptr->op_array.filename,
												  fptr->op_array.line_start,
												  fptr->op_array.line_end);
	}
	string_init(&param_indent);
	string_printf(&param_indent, "%s  ", indent);
	_function_parameter_string(str, fptr, param_indent.string TSRMLS_CC);
	string_free(&param_indent);
	string_printf(str, "%s}\n", indent);
}
/* }}} */

/* {{{ _property_string */
static void _property_string(string *str, zend_property_info *prop, char *prop_name, char* indent TSRMLS_DC)
{
	char *class_name;

	string_printf(str, "%sProperty [ ", indent);
	if (!prop) {
		string_printf(str, "<dynamic> public $%s", prop_name);
	} else {
		if (!(prop->flags & ZEND_ACC_STATIC)) {
			if (prop->flags & ZEND_ACC_IMPLICIT_PUBLIC) {
				string_write(str, "<implicit> ", sizeof("<implicit> ") - 1);
			} else {
				string_write(str, "<default> ", sizeof("<default> ") - 1);
			}
		}
	
		/* These are mutually exclusive */
		switch (prop->flags & ZEND_ACC_PPP_MASK) {
			case ZEND_ACC_PUBLIC:
				string_printf(str, "public ");
				break;
			case ZEND_ACC_PRIVATE:
				string_printf(str, "private ");
				break;
			case ZEND_ACC_PROTECTED:
				string_printf(str, "protected ");
				break;
		}
		if(prop->flags & ZEND_ACC_STATIC) {
			string_printf(str, "static ");
		}

		zend_unmangle_property_name(prop->name, &class_name, &prop_name);
		string_printf(str, "$%s", prop_name);
	}

	string_printf(str, " ]\n");
}
/* }}} */

static int _extension_ini_string(zend_ini_entry *ini_entry, int num_args, va_list args, zend_hash_key *hash_key)
{
	string *str = va_arg(args, string *);
	char *indent = va_arg(args, char *);
	int number = va_arg(args, int);
	char *comma = "";

	if (number == ini_entry->module_number) {
		string_printf(str, "    %sEntry [ %s <", indent, ini_entry->name);
		if (ini_entry->modifiable & ZEND_INI_ALL) {
			string_printf(str, "ALL");
		} else {
			if (ini_entry->modifiable & ZEND_INI_USER) {
				string_printf(str, "USER");
				comma = ",";
			}
			if (ini_entry->modifiable & ZEND_INI_PERDIR) {
				string_printf(str, "%sPERDIR", comma);
				comma = ",";
			}
			if (ini_entry->modifiable & ZEND_INI_SYSTEM) {
				string_printf(str, "%sSYSTEM", comma);
			}
		}
 
		string_printf(str, "> ]\n", indent, ini_entry->name);
		string_printf(str, "    %s  Current = '%s'\n", indent, ini_entry->value ? ini_entry->value : "");
		if (ini_entry->modified) {
			string_printf(str, "    %s  Default = '%s'\n", indent, ini_entry->orig_value ? ini_entry->orig_value : "");
		}
		string_printf(str, "    %s}\n", indent);
	}
	return ZEND_HASH_APPLY_KEEP;
}

static int _extension_class_string(zend_class_entry **pce, int num_args, va_list args, zend_hash_key *hash_key)
{
	string *str = va_arg(args, string *);
	char *indent = va_arg(args, char *);
	struct _zend_module_entry *module = va_arg(args, struct _zend_module_entry*);
	int *num_classes = va_arg(args, int*);

	if ((*pce)->module && !strcasecmp((*pce)->module->name, module->name)) {
		TSRMLS_FETCH();
		string_printf(str, "\n");
		_class_string(str, *pce, NULL, indent TSRMLS_CC);
		(*num_classes)++;
	}
	return ZEND_HASH_APPLY_KEEP;
}

/* {{{ _extension_string */
static void _extension_string(string *str, zend_module_entry *module, char *indent TSRMLS_DC)
{
	string_printf(str, "%sExtension [ ", indent);
	if (module->type == MODULE_PERSISTENT) {
		string_printf(str, "<persistent>");
	}
	if (module->type == MODULE_TEMPORARY) {
		string_printf(str, "<temporary>" );
	}
	string_printf(str, " extension #%d %s version %s ] {\n",
				  module->module_number, module->name,
				  (module->version == NO_VERSION_YET) ? "<no_version>" : module->version);

	{
		string str_ini;
		string_init(&str_ini);
		zend_hash_apply_with_arguments(EG(ini_directives), (apply_func_args_t) _extension_ini_string, 3, &str_ini, indent, module->module_number);
		if (str_ini.len > 1) {
			string_printf(str, "\n - INI {\n");
			string_append(str, &str_ini);
			string_printf(str, "%s  }\n", indent);
		}
		string_free(&str_ini);
	}

	if (module->functions && module->functions->fname) {
		zend_function *fptr;
		zend_function_entry *func = module->functions;

		string_printf(str, "\n - Functions {\n");

		/* Is there a better way of doing this? */
		while (func->fname) {
			if (zend_hash_find(EG(function_table), func->fname, strlen(func->fname) + 1, (void**) &fptr) == FAILURE) {
				zend_error(E_WARNING, "Internal error: Cannot find extension function %s in global function table", func->fname);
				continue;
			}
			
			_function_string(str, fptr, "    " TSRMLS_CC);
			func++;
		}
		string_printf(str, "%s  }\n", indent);
	}
	
	{
		string str_classes;
		string sub_indent;
		int num_classes = 0;
		
		string_init(&sub_indent);
		string_printf(&sub_indent, "%s    ", indent);
		string_init(&str_classes);
		zend_hash_apply_with_arguments(EG(class_table), (apply_func_args_t) _extension_class_string, 4, &str_classes, sub_indent.string, module, &num_classes TSRMLS_CC);
		if (num_classes) {
			string_printf(str, "\n - Classes [%d] {\n", num_classes);
			string_append(str, &str_classes);
			string_printf(str, "%s  }\n", indent);
		}
		string_free(&str_classes);
		string_free(&sub_indent);
	}

	string_printf(str, "%s}\n", indent);
}
/* }}} */

/* {{{ _function_check_flag */
static void _function_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask)
{
	reflection_object *intern;
	zend_function *mptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(mptr);
	RETURN_BOOL(mptr->common.fn_flags & mask);
}
/* }}} */

/* {{{ zend_reflection_class_factory */
ZEND_API void zend_reflection_class_factory(zend_class_entry *ce, zval *object TSRMLS_DC)
{
	reflection_object *intern;
	zval *name;

	MAKE_STD_ZVAL(name);
	ZVAL_STRINGL(name, ce->name, ce->name_length, 1);
	reflection_instanciate(reflection_class_ptr, object TSRMLS_CC);
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	intern->ptr = ce;
	intern->free_ptr = 0;
	intern->ce = ce;
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ reflection_extension_factory */
static void reflection_extension_factory(zval *object, char *name_str TSRMLS_DC)
{
	reflection_object *intern;
	zval *name;
	int name_len = strlen(name_str);
	char *lcname;
	struct _zend_module_entry *module;

	lcname = do_alloca(name_len + 1);
	zend_str_tolower_copy(lcname, name_str, name_len);
	if (zend_hash_find(&module_registry, lcname, name_len + 1, (void **)&module) == FAILURE) {
		free_alloca(lcname);
		return;
	}
	free_alloca(lcname);

	reflection_instanciate(reflection_extension_ptr, object TSRMLS_CC);
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	MAKE_STD_ZVAL(name);
	ZVAL_STRINGL(name, module->name, name_len, 1);
	intern->ptr = module;
	intern->free_ptr = 0;
	intern->ce = NULL;
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ reflection_parameter_factory */
static void reflection_parameter_factory(zend_function *fptr, struct _zend_arg_info *arg_info, zend_uint offset, zend_uint required, zval *object TSRMLS_DC)
{
	reflection_object *intern;
	parameter_reference *reference;
	zval *name;

	MAKE_STD_ZVAL(name);
	if (arg_info->name) {
		ZVAL_STRINGL(name, arg_info->name, arg_info->name_len, 1);
	} else {
		ZVAL_NULL(name);
	}
	reflection_instanciate(reflection_parameter_ptr, object TSRMLS_CC);
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	reference = (parameter_reference*) emalloc(sizeof(parameter_reference));
	reference->arg_info = arg_info;
	reference->offset = offset;
	reference->required = required;
	reference->fptr = fptr;
	intern->ptr = reference;
	intern->free_ptr = 1;
	intern->ce = fptr->common.scope;
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ reflection_function_factory */
static void reflection_function_factory(zend_function *function, zval *object TSRMLS_DC)
{
	reflection_object *intern;
	zval *name;

	MAKE_STD_ZVAL(name);
	ZVAL_STRING(name, function->common.function_name, 1);

	reflection_instanciate(reflection_function_ptr, object TSRMLS_CC);
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	intern->ptr = function;
	intern->free_ptr = 0;
	intern->ce = NULL;
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ reflection_method_factory */
static void reflection_method_factory(zend_class_entry *ce, zend_function *method, zval *object TSRMLS_DC)
{
	reflection_object *intern;
	zval *name;
	zval *classname;

	MAKE_STD_ZVAL(name);
	ZVAL_STRING(name, method->common.function_name, 1);
	MAKE_STD_ZVAL(classname);
	ZVAL_STRINGL(classname, ce->name, ce->name_length, 1);
	reflection_instanciate(reflection_method_ptr, object TSRMLS_CC);
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	intern->ptr = method;
	intern->free_ptr = 0;
	intern->ce = ce;
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
	zend_hash_update(Z_OBJPROP_P(object), "class", sizeof("class"), (void **) &classname, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ reflection_property_factory */
static void reflection_property_factory(zend_class_entry *ce, zend_property_info *prop, zval *object TSRMLS_DC)
{
	reflection_object *intern;
	zval *name;
	zval *classname;
	property_reference *reference;
	char *class_name, *prop_name;

	zend_unmangle_property_name(prop->name, &class_name, &prop_name);

	if (!(prop->flags & ZEND_ACC_PRIVATE)) {
		/* we have to seach the class hierarchy for this (implicit) public or protected property */
		zend_class_entry *tmp_ce = ce->parent;
		zend_property_info *tmp_info;
		
		while (tmp_ce && zend_hash_find(&tmp_ce->properties_info, prop_name, strlen(prop_name) + 1, (void **) &tmp_info) == SUCCESS) {
			if (tmp_info->flags & ZEND_ACC_PRIVATE) {
				/* private in super class => NOT the same property */
				break;
			}
			ce = tmp_ce;
			prop = tmp_info;
			tmp_ce = tmp_ce->parent;
		}
	}

	MAKE_STD_ZVAL(name);
	ZVAL_STRING(name, prop_name, 1);

	MAKE_STD_ZVAL(classname);
	ZVAL_STRINGL(classname, ce->name, ce->name_length, 1);

	reflection_instanciate(reflection_property_ptr, object TSRMLS_CC);
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	reference = (property_reference*) emalloc(sizeof(property_reference));
	reference->ce = ce;
	reference->prop = prop;
	intern->ptr = reference;
	intern->free_ptr = 1;
	intern->ce = ce;
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
	zend_hash_update(Z_OBJPROP_P(object), "class", sizeof("class"), (void **) &classname, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ _reflection_export */
static void _reflection_export(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce_ptr, int ctor_argc)
{
	zval reflector, *reflector_ptr = &reflector;
	zval output, *output_ptr = &output;
	zval *argument_ptr, *argument2_ptr;
	zval *retval_ptr, **params[2];
	int result;
	int return_output = 0;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval fname;

	if (ctor_argc == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &argument_ptr, &return_output) == FAILURE) {
			return;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|b", &argument_ptr, &argument2_ptr, &return_output) == FAILURE) {
			return;
		}
	}

	INIT_PZVAL(&output);

	/* Create object */
	INIT_PZVAL(&reflector);
	if (object_and_properties_init(&reflector, ce_ptr, NULL) == FAILURE) {
		_DO_THROW("Could not create reflector");
	}

	/* Call __construct() */
	params[0] = &argument_ptr;
	params[1] = &argument2_ptr;

	fci.size = sizeof(fci);
	fci.function_table = NULL;
	fci.function_name = NULL;
	fci.symbol_table = NULL;
	fci.object_pp = &reflector_ptr;
	fci.retval_ptr_ptr = &retval_ptr;
	fci.param_count = ctor_argc;
	fci.params = params;
	fci.no_separation = 1;

	fcc.initialized = 1;
	fcc.function_handler = ce_ptr->constructor;
	fcc.calling_scope = ce_ptr;
	fcc.object_pp = &reflector_ptr;

	result = zend_call_function(&fci, &fcc TSRMLS_CC);
	
	RETURN_ON_EXCEPTION

	if (result == FAILURE) {
		zval_dtor(&reflector);
		_DO_THROW("Could not create reflector");
	}

	if (retval_ptr) {
		zval_ptr_dtor(&retval_ptr);
	}

	/* Call static reflection::export */
	ZVAL_BOOL(&output, return_output);
	params[0] = &reflector_ptr;
	params[1] = &output_ptr;

	ZVAL_STRINGL(&fname, "export", sizeof("export") - 1, 0);
	fci.function_table = &reflection_ptr->function_table;
	fci.function_name = &fname;
	fci.object_pp = NULL;
	fci.retval_ptr_ptr = &retval_ptr;
	fci.param_count = 2;
	fci.params = params;
	fci.no_separation = 1;

	result = zend_call_function(&fci, NULL TSRMLS_CC);

	if (result == FAILURE && EG(exception) == NULL) {
		zval_dtor(&reflector);
		zval_ptr_dtor(&retval_ptr);
		_DO_THROW("Could not execute reflection::export()");
	}

	if (return_output) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	} else {
		zval_ptr_dtor(&retval_ptr);
	}

	/* Destruct reflector which is no longer needed */
	zval_dtor(&reflector);
}
/* }}} */

/* {{{ Preventing __clone from being called */
ZEND_METHOD(reflection, __clone)
{
	/* Should never be executable */
	_DO_THROW("Cannot clone object using __clone()");
}
/* }}} */

/* {{{ proto public static mixed Reflection::export(Reflector r [, bool return])
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection, export)
{
	zval *object, *fname, *retval_ptr;
	int result;
	zend_bool return_output = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|b", &object, reflector_ptr, &return_output) == FAILURE) {
		return;
	}

	/* Invoke the __toString() method */
	MAKE_STD_ZVAL(fname);
	ZVAL_STRINGL(fname, "__tostring", sizeof("__tostring") - 1, 1);
	result= call_user_function_ex(NULL, &object, fname, &retval_ptr, 0, NULL, 0, NULL TSRMLS_CC);
	zval_ptr_dtor(&fname);

	if (result == FAILURE) {
		_DO_THROW("Invocation of method __toString() failed");
		/* Returns from this function */
	}

	if (!retval_ptr) {
		zend_error(E_WARNING, "%s::__toString() did not return anything", Z_OBJCE_P(object)->name);
		RETURN_FALSE;
	}

	if (return_output) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	} else {
		/* No need for _r variant, return of __toString should always be a string */
		zend_print_zval(retval_ptr, 0);
		zend_printf("\n");
		zval_ptr_dtor(&retval_ptr);
	}
}
/* }}} */

/* {{{ proto public static array Reflection::getModifierNames(int modifiers)
   Returns an array of modifier names */
ZEND_METHOD(reflection, getModifierNames)
{
	long modifiers;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &modifiers) == FAILURE) {
		return;
	}

	array_init(return_value);

	if (modifiers & (ZEND_ACC_ABSTRACT | ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		add_next_index_stringl(return_value, "abstract", sizeof("abstract")-1, 1);
	}
	if (modifiers & (ZEND_ACC_FINAL | ZEND_ACC_FINAL_CLASS)) {
		add_next_index_stringl(return_value, "final", sizeof("final")-1, 1);
	}

	/* These are mutually exclusive */
	switch (modifiers & ZEND_ACC_PPP_MASK) {
		case ZEND_ACC_PUBLIC:
			add_next_index_stringl(return_value, "public", sizeof("public")-1, 1);
			break;
		case ZEND_ACC_PRIVATE:
			add_next_index_stringl(return_value, "private", sizeof("private")-1, 1);
			break;
		case ZEND_ACC_PROTECTED:
			add_next_index_stringl(return_value, "protected", sizeof("protected")-1, 1);
			break;
	}

	if (modifiers & ZEND_ACC_STATIC) {
		add_next_index_stringl(return_value, "static", sizeof("static")-1, 1);
	}
}
/* }}} */

/* {{{ proto public static mixed ReflectionFunction::export(string name [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_function, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_function_ptr, 1);
}
/* }}} */

/* {{{ proto public ReflectionFunction::__construct(string name)
   Constructor. Throws an Exception in case the given function does not exist */
ZEND_METHOD(reflection_function, __construct)
{
	zval *name;
	zval *object;
	char *lcname;
	reflection_object *intern;
	zend_function *fptr;
	char *name_str;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name_str, &name_len) == FAILURE) {
		return;
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL) {
		return;
	}
	lcname = do_alloca(name_len + 1);
	zend_str_tolower_copy(lcname, name_str, name_len);
	if (zend_hash_find(EG(function_table), lcname, name_len + 1, (void **)&fptr) == FAILURE) {
		free_alloca(lcname);
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
			"Function %s() does not exist", name_str);
		return;
	}
	free_alloca(lcname);
	MAKE_STD_ZVAL(name);
	ZVAL_STRING(name, fptr->common.function_name, 1);
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
	intern->ptr = fptr;
	intern->free_ptr = 0;
	intern->ce = NULL;
}
/* }}} */

/* {{{ proto public string ReflectionFunction::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_function, __toString)
{
	reflection_object *intern;
	zend_function *fptr;
	string str;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	string_init(&str);
	_function_string(&str, fptr, "" TSRMLS_CC);
	RETURN_STRINGL(str.string, str.len - 1, 0);
}
/* }}} */

/* {{{ proto public string ReflectionFunction::getName()
   Returns this function's name */
ZEND_METHOD(reflection, function_getName)
{
	METHOD_NOTSTATIC_NUMPARAMS(0);
	_default_get_entry(getThis(), "name", sizeof("name"), return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::isInternal()
   Returns whether this is an internal function */
ZEND_METHOD(reflection, function_isInternal)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_BOOL(fptr->type == ZEND_INTERNAL_FUNCTION);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::isUserDefined()
   Returns whether this is an user-defined function */
ZEND_METHOD(reflection_function, isUserDefined)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_BOOL(fptr->type == ZEND_USER_FUNCTION);
}
/* }}} */

/* {{{ proto public string ReflectionFunction::getFileName()
   Returns the filename of the file this function was declared in */
ZEND_METHOD(reflection_function, getFileName)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_STRING(fptr->op_array.filename, 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public int ReflectionFunction::getStartLine()
   Returns the line this function's declaration starts at */
ZEND_METHOD(reflection_function, getStartLine)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(fptr->op_array.line_start);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public int ReflectionFunction::getEndLine()
   Returns the line this function's declaration ends at */
ZEND_METHOD(reflection_function, getEndLine)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(fptr->op_array.line_end);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public string ReflectionFunction::getDocComment()
   Returns the doc comment for this function */
ZEND_METHOD(reflection_function, getDocComment)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.doc_comment) {
		RETURN_STRINGL(fptr->op_array.doc_comment, fptr->op_array.doc_comment_len, 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public array ReflectionFunction::getStaticVariables()
   Returns an associative array containing this function's static variables and their values */
ZEND_METHOD(reflection_function, getStaticVariables)
{
	zval *tmp_copy;
	reflection_object *intern;
	zend_function *fptr;
	
	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(fptr);

	/* Return an empty array in case no static variables exist */
	array_init(return_value);
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.static_variables != NULL) {
		zend_hash_apply_with_argument(fptr->op_array.static_variables, (apply_func_arg_t) zval_update_constant, (void*)1 TSRMLS_CC);
		zend_hash_copy(Z_ARRVAL_P(return_value), fptr->op_array.static_variables, (copy_ctor_func_t) zval_add_ref, (void *) &tmp_copy, sizeof(zval *));
	}
}
/* }}} */

/* {{{ proto public mixed ReflectionFunction::invoke(mixed* args)
   Invokes the function */
ZEND_METHOD(reflection_function, invoke)
{
	zval *retval_ptr;
	zval ***params;
	int result;
	int argc = ZEND_NUM_ARGS();
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	reflection_object *intern;
	zend_function *fptr;
	
	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(fptr);

	params = safe_emalloc(sizeof(zval **), argc, 0);
	if (zend_get_parameters_array_ex(argc, params) == FAILURE) {
		efree(params);
		RETURN_FALSE;
	}

	fci.size = sizeof(fci);
	fci.function_table = NULL;
	fci.function_name = NULL;
	fci.symbol_table = NULL;
	fci.object_pp = NULL;
	fci.retval_ptr_ptr = &retval_ptr;
	fci.param_count = argc;
	fci.params = params;
	fci.no_separation = 1;

	fcc.initialized = 1;
	fcc.function_handler = fptr;
	fcc.calling_scope = EG(scope);
	fcc.object_pp = NULL;

	result = zend_call_function(&fci, &fcc TSRMLS_CC);

	efree(params);

	if (result == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
			"Invocation of method %s() failed", fptr->common.function_name);
		return;
	}

	if (retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	}
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::returnsReference()
   Gets whether this function returns a reference */
ZEND_METHOD(reflection_function, returnsReference)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(fptr);

	RETURN_BOOL(fptr->op_array.return_reference);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::getNumberOfParameters()
   Gets the number of required parameters */
ZEND_METHOD(reflection_function, getNumberOfParameters)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(fptr);

	RETURN_LONG(fptr->common.num_args);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::getNumberOfRequiredParameters()
   Gets the number of required parameters */
ZEND_METHOD(reflection_function, getNumberOfRequiredParameters)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(fptr);

	RETURN_LONG(fptr->common.required_num_args);
}
/* }}} */

/* {{{ proto public ReflectionParameter[] Reflection_Function::getParameters()
   Returns an array of parameter objects for this function */
ZEND_METHOD(reflection_function, getParameters)
{
	reflection_object *intern;
	zend_function *fptr;
	zend_uint i;
	struct _zend_arg_info *arg_info;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(fptr);

	arg_info= fptr->common.arg_info;

	array_init(return_value);
	for (i = 0; i < fptr->common.num_args; i++) {
		 zval *parameter;   

		 ALLOC_ZVAL(parameter);
		 reflection_parameter_factory(fptr, arg_info, i, fptr->common.required_num_args, parameter TSRMLS_CC);
		 add_next_index_zval(return_value, parameter);
		 
		 arg_info++;
	}
}
/* }}} */

/* {{{ proto public static mixed ReflectionParameter::export(mixed function, mixed parameter [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_parameter, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_parameter_ptr, 2);
}
/* }}} */

/* {{{ proto public ReflectionParameter::__construct(mixed function, mixed parameter)
   Constructor. Throws an Exception in case the given method does not exist */
ZEND_METHOD(reflection_parameter, __construct)
{
	parameter_reference *ref;
	zval *reference, *parameter;
	zval *object;
	zval *name;
	reflection_object *intern;
	zend_function *fptr;
	struct _zend_arg_info *arg_info;
	int position;
	zend_class_entry *ce = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &reference, &parameter) == FAILURE) {
		return;
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL) {
		return;
	}
	
	/* First, find the function */
	switch (Z_TYPE_P(reference)) {
		case IS_STRING: {
		  		char *lcname;

				convert_to_string_ex(&reference);
				lcname = do_alloca(Z_STRLEN_P(reference) + 1);
				zend_str_tolower_copy(lcname, Z_STRVAL_P(reference), Z_STRLEN_P(reference));
				if (zend_hash_find(EG(function_table), lcname, (int) Z_STRLEN_P(reference) + 1, (void**) &fptr) == FAILURE) {
					free_alloca(lcname);
					zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
						"Function %s() does not exist", Z_STRVAL_P(reference));
					return;
				}
				free_alloca(lcname);
			}
			break;

		case IS_ARRAY: {
				zval **classref;
				zval **method;
				zend_class_entry **pce;
	  			char *lcname;
				
				if ((zend_hash_index_find(Z_ARRVAL_P(reference), 0, (void **) &classref) == FAILURE)
					|| (zend_hash_index_find(Z_ARRVAL_P(reference), 1, (void **) &method) == FAILURE)) {
					_DO_THROW("Expected array($object, $method) or array($classname, $method)");
					/* returns out of this function */
				}

				if (Z_TYPE_PP(classref) == IS_OBJECT) {
					ce = Z_OBJCE_PP(classref);
				} else {
					convert_to_string_ex(classref);
					if (zend_lookup_class(Z_STRVAL_PP(classref), Z_STRLEN_PP(classref), &pce TSRMLS_CC) == FAILURE) {
						zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC,
								"Class %s does not exist", Z_STRVAL_PP(classref));
						return;
					}
					ce = *pce;
				}
				
				convert_to_string_ex(method);
				lcname = do_alloca(Z_STRLEN_PP(method) + 1);
				zend_str_tolower_copy(lcname, Z_STRVAL_PP(method), Z_STRLEN_PP(method));
				if (zend_hash_find(&ce->function_table, lcname, (int)(Z_STRLEN_PP(method) + 1), (void **) &fptr) == FAILURE) {
					free_alloca(lcname);
					zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
						"Method %s::%s() does not exist", Z_STRVAL_PP(classref), Z_STRVAL_PP(method));
					return;
				}
				free_alloca(lcname);
			}
			break;
			
		default:
			_DO_THROW("The parameter class is expected to be either a string or an array(class, method)");
			/* returns out of this function */
	}
	
	/* Now, search for the parameter */
	arg_info = fptr->common.arg_info;
	if (Z_TYPE_P(parameter) == IS_LONG) {
		position= Z_LVAL_P(parameter);
		if (position < 0 || (zend_uint)position >= fptr->common.num_args) {
			_DO_THROW("The parameter specified by its offset could not be found");
			/* returns out of this function */
		}
	} else {
		zend_uint i;

		position= -1;
		convert_to_string_ex(&parameter);
		for (i = 0; i < fptr->common.num_args; i++) {
			if (arg_info[i].name && strcmp(arg_info[i].name, Z_STRVAL_P(parameter)) == 0) {
				position= i;
				break;
			}
		}
		if (position == -1) {
			_DO_THROW("The parameter specified by its name could not be found");
			/* returns out of this function */
		}
	}
	
	MAKE_STD_ZVAL(name);
	if (arg_info[position].name) {
		ZVAL_STRINGL(name, arg_info[position].name, arg_info[position].name_len, 1);
	} else {
		ZVAL_NULL(name);
	}
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);

	ref = (parameter_reference*) emalloc(sizeof(parameter_reference));
	ref->arg_info = &arg_info[position];
	ref->offset = (zend_uint)position;
	ref->required = fptr->common.required_num_args;
	ref->fptr = fptr;
	intern->ptr = ref;
	intern->free_ptr = 1;
	intern->ce = ce;
}
/* }}} */

/* {{{ proto public string ReflectionParameter::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_parameter, __toString)
{
	reflection_object *intern;
	parameter_reference *param;
	string str;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(param);
	string_init(&str);
	_parameter_string(&str, param->fptr, param->arg_info, param->offset, param->required, "" TSRMLS_CC);
	RETURN_STRINGL(str.string, str.len - 1, 0);
}
/* }}} */

/* {{{ proto public string ReflectionParameter::getName()
   Returns this parameters's name */
ZEND_METHOD(reflection_parameter, getName)
{
	METHOD_NOTSTATIC_NUMPARAMS(0);
	_default_get_entry(getThis(), "name", sizeof("name"), return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto public ReflectionClass ReflectionParameter::getClass()
   Returns this parameters's class hint or NULL if there is none */
ZEND_METHOD(reflection_parameter, getClass)
{
	reflection_object *intern;
	parameter_reference *param;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(param);

	if (!param->arg_info->class_name) {
		RETURN_NULL();
	} else {
		zend_class_entry **pce;
		char *lcname = do_alloca(param->arg_info->class_name_len + 1);
		zend_str_tolower_copy(lcname, param->arg_info->class_name, param->arg_info->class_name_len);
		if (zend_hash_find(EG(class_table), lcname, param->arg_info->class_name_len + 1, (void **) &pce) == FAILURE) {
			free_alloca(lcname);
			zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
				"Class %s does not exist", param->arg_info->class_name);
			return;
		}
		free_alloca(lcname);
		zend_reflection_class_factory(*pce, return_value TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::allowsNull()
   Returns whether NULL is allowed as this parameters's value */
ZEND_METHOD(reflection_parameter, allowsNull)
{
	reflection_object *intern;
	parameter_reference *param;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->arg_info->allow_null);
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::isPassedByReference()
   Returns whether this parameters is passed to by reference */
ZEND_METHOD(reflection_parameter, isPassedByReference)
{
	reflection_object *intern;
	parameter_reference *param;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->arg_info->pass_by_reference);
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::isOptional()
   Returns whether this parameter is an optional parameter */
ZEND_METHOD(reflection_parameter, isOptional)
{
	reflection_object *intern;
	parameter_reference *param;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->offset >= param->required);
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::isDefaultValueAvailable()
   Returns whether the default value of this parameter is available */
ZEND_METHOD(reflection_parameter, isDefaultValueAvailable)
{
	reflection_object *intern;
	parameter_reference *param;
	zend_op *precv;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(param);

	if (param->fptr->type != ZEND_USER_FUNCTION)
	{
		RETURN_FALSE;
	}
	if (param->offset < param->required) {
		RETURN_FALSE;
	}
	precv = &((zend_op_array*)param->fptr)->opcodes[param->offset*2 + 1];
	if (precv->opcode != ZEND_RECV_INIT || precv->op2.op_type == IS_UNUSED) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::getDefaultValue()
   Returns the default value of this parameter or throws an exception */
ZEND_METHOD(reflection_parameter, getDefaultValue)
{
	reflection_object *intern;
	parameter_reference *param;
	zend_op *precv;
	zval *zv, zv_copy;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(param);

	if (param->fptr->type != ZEND_USER_FUNCTION)
	{
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, "Cannot determine default value for internal functions");
		return;
	}
	if (param->offset < param->required) {
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, "Parameter is not optional"); 
		return;
	}
	precv = &((zend_op_array*)param->fptr)->opcodes[param->offset*2 + 1];
	if (precv->opcode != ZEND_RECV_INIT || precv->op2.op_type == IS_UNUSED) {
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, "Internal error"); 
		return;
	}

	zv_copy = precv->op2.u.constant;
	zv = &zv_copy;
	zval_update_constant(&zv, (void*)0 TSRMLS_CC);
	RETURN_ZVAL(zv, 1, 1);
}
/* }}} */

/* {{{ proto public static mixed ReflectionMethod::export(mixed class, string name [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_method, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_method_ptr, 2);
}
/* }}} */

/* {{{ proto public ReflectionMethod::__construct(mixed class, string name)
   Constructor. Throws an Exception in case the given method does not exist */
ZEND_METHOD(reflection_method, __construct)
{
	zval *name, *classname;
	zval *object;
	reflection_object *intern;
	char *lcname;
	zend_class_entry **pce;
	zend_class_entry *ce;
	zend_function *mptr;
	char *name_str;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs", &classname, &name_str, &name_len) == FAILURE) {
		return;
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL) {
		return;
	}
	
	/* Find the class entry */
	switch (Z_TYPE_P(classname)) {
		case IS_STRING:
			if (zend_lookup_class(Z_STRVAL_P(classname), Z_STRLEN_P(classname), &pce TSRMLS_CC) == FAILURE) {
				zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC,
						"Class %s does not exist", Z_STRVAL_P(classname)); 
				return;
			}
			ce = *pce;
			break;

		case IS_OBJECT:
			ce = Z_OBJCE_P(classname);
			break;

		default:
			_DO_THROW("The parameter class is expected to be either a string or an object");
			/* returns out of this function */
	}

	MAKE_STD_ZVAL(classname);
	ZVAL_STRINGL(classname, ce->name, ce->name_length, 1);
	zend_hash_update(Z_OBJPROP_P(object), "class", sizeof("class"), (void **) &classname, sizeof(zval *), NULL);
	
	lcname = do_alloca(name_len + 1);
	zend_str_tolower_copy(lcname, name_str, name_len);

	if (zend_hash_find(&ce->function_table, lcname, name_len + 1, (void **) &mptr) == FAILURE) {
		free_alloca(lcname);
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
			"Method %s::%s() does not exist", ce->name, name_str);
		return;
	}
	free_alloca(lcname);

	MAKE_STD_ZVAL(name);
	ZVAL_STRING(name, mptr->common.function_name, 1);
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
	intern->ptr = mptr;
	intern->free_ptr = 0;
	intern->ce = ce;
}
/* }}} */

/* {{{ proto public string ReflectionMethod::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_method, __toString)
{
	reflection_object *intern;
	zend_function *mptr;
	string str;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(mptr);
	string_init(&str);
	_function_string(&str, mptr, "" TSRMLS_CC);
	RETURN_STRINGL(str.string, str.len - 1, 0);
}
/* }}} */

/* {{{ proto public mixed ReflectionMethod::invoke(mixed object, mixed* args)
   Invokes the method. */
ZEND_METHOD(reflection_method, invoke)
{
	zval *retval_ptr;
	zval ***params;
	zval **object_pp;
	reflection_object *intern;
	zend_function *mptr;
	int argc = ZEND_NUM_ARGS();
	int result;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_class_entry *obj_ce;
	
	METHOD_NOTSTATIC;

	if (argc < 1) {
		zend_error(E_WARNING, "Invoke() expects at least one parameter, none given");
		RETURN_FALSE;
	}
	
	GET_REFLECTION_OBJECT_PTR(mptr);

	if (!(mptr->common.fn_flags & ZEND_ACC_PUBLIC) ||
		(mptr->common.fn_flags & ZEND_ACC_ABSTRACT)) {
		if (mptr->common.fn_flags & ZEND_ACC_ABSTRACT) {
			zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
				"Trying to invoke abstract method %s::%s()", 
				mptr->common.scope->name, mptr->common.function_name);
		} else {
			zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC,
				"Trying to invoke %s method %s::%s() from scope %s", 
				mptr->common.fn_flags & ZEND_ACC_PROTECTED ? "protected" : "private",
				mptr->common.scope->name, mptr->common.function_name,
				Z_OBJCE_P(getThis())->name);
		}
		return;
	}

	params = safe_emalloc(sizeof(zval **), argc, 0);
	if (zend_get_parameters_array_ex(argc, params) == FAILURE) {
		efree(params);
		RETURN_FALSE;
	}
	
	/* In case this is a static method, we should'nt pass an object_pp
	 * (which is used as calling context aka $this). We can thus ignore the
	 * first parameter.
	 *
	 * Else, we verify that the given object is an instance of the class.
	 */
	if (mptr->common.fn_flags & ZEND_ACC_STATIC) {
		object_pp = NULL;
		obj_ce = NULL;
	} else {
		if ((Z_TYPE_PP(params[0]) != IS_OBJECT)) {
			efree(params);
			_DO_THROW("Non-object passed to Invoke()");
			/* Returns from this function */
		}
		obj_ce = Z_OBJCE_PP(params[0]);

		if (!instanceof_function(obj_ce, mptr->common.scope TSRMLS_CC)) {
			efree(params);
			_DO_THROW("Given object is not an instance of the class this method was declared in");
			/* Returns from this function */
		}
	
		object_pp = params[0];
	}
	
	fci.size = sizeof(fci);
	fci.function_table = NULL;
	fci.function_name = NULL;
	fci.symbol_table = NULL;
	fci.object_pp = object_pp;
	fci.retval_ptr_ptr = &retval_ptr;
	fci.param_count = argc-1;
	fci.params = params+1;
	fci.no_separation = 1;

	fcc.initialized = 1;
	fcc.function_handler = mptr;
	fcc.calling_scope = obj_ce;
	fcc.object_pp = object_pp;

	result = zend_call_function(&fci, &fcc TSRMLS_CC);
	
	efree(params);

	if (result == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
			"Invocation of method %s::%s() failed", mptr->common.scope->name, mptr->common.function_name);
		return;
	}

	if (retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	}
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isFinal()
   Returns whether this method is final */
ZEND_METHOD(reflection_method, isFinal)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_FINAL);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isAbstract()
   Returns whether this method is abstract */
ZEND_METHOD(reflection_method, isAbstract)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_ABSTRACT);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isPublic()
   Returns whether this method is public */
ZEND_METHOD(reflection_method, isPublic)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isPrivate()
   Returns whether this method is private */
ZEND_METHOD(reflection_method, isPrivate)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PRIVATE);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isProtected()
   Returns whether this method is protected */
ZEND_METHOD(reflection_method, isProtected)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PROTECTED);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isStatic()
   Returns whether this method is static */
ZEND_METHOD(reflection_method, isStatic)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_STATIC);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isConstructor()
   Returns whether this method is the constructor */
ZEND_METHOD(reflection_method, isConstructor)
{
	reflection_object *intern;
	zend_function *mptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(mptr);
	/* we need to check if the ctor is the ctor of the class level we we 
	 * looking at since we might be looking at an inherited old style ctor
	 * defined in base class. */
	RETURN_BOOL(mptr->common.fn_flags & ZEND_ACC_CTOR && intern->ce->constructor && intern->ce->constructor->common.scope == mptr->common.scope);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isDestructor()
   Returns whether this method is static */
ZEND_METHOD(reflection_method, isDestructor)
{
	reflection_object *intern;
	zend_function *mptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(mptr);
	RETURN_BOOL(mptr->common.fn_flags & ZEND_ACC_DTOR);
}
/* }}} */

/* {{{ proto public int ReflectionMethod::getModifiers()
   Returns a bitfield of the access modifiers for this method */
ZEND_METHOD(reflection_method, getModifiers)
{
	reflection_object *intern;
	zend_function *mptr;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(mptr);

	RETURN_LONG(mptr->common.fn_flags);
}
/* }}} */

/* {{{ proto public ReflectionClass ReflectionMethod::getDeclaringClass()
   Get the declaring class */
ZEND_METHOD(reflection_method, getDeclaringClass)
{
	reflection_object *intern;
	zend_function *mptr;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(mptr);

	zend_reflection_class_factory(mptr->common.scope, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto public static mixed ReflectionClass::export(mixed argument [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_class, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_class_ptr, 1);
}
/* }}} */

/* {{{ reflection_class_object_ctor */
static void reflection_class_object_ctor(INTERNAL_FUNCTION_PARAMETERS, int is_object)
{
	zval *argument;
	zval *object;
	zval *classname;
	reflection_object *intern;
	zend_class_entry **ce;

	if (is_object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &argument) == FAILURE) {
			return;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &argument) == FAILURE) {
			return;
		}
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL) {
		return;
	}
	
	if (Z_TYPE_P(argument) == IS_OBJECT) {
		MAKE_STD_ZVAL(classname);
		ZVAL_STRINGL(classname, Z_OBJCE_P(argument)->name, Z_OBJCE_P(argument)->name_length, 1);
		zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &classname, sizeof(zval *), NULL);
		intern->ptr = Z_OBJCE_P(argument);
		if (is_object) {
			intern->obj = argument;
			zval_add_ref(&argument);
		}
	} else { 
		convert_to_string_ex(&argument);
		if (zend_lookup_class(Z_STRVAL_P(argument), Z_STRLEN_P(argument), &ce TSRMLS_CC) == FAILURE) {
			zend_throw_exception_ex(reflection_exception_ptr, -1 TSRMLS_CC, "Class %s does not exist", Z_STRVAL_P(argument));
			return;
		}

		MAKE_STD_ZVAL(classname);
		ZVAL_STRINGL(classname, (*ce)->name, (*ce)->name_length, 1);
		zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &classname, sizeof(zval *), NULL);

		intern->ptr = *ce;
	}
	intern->free_ptr = 0;
}
/* }}} */

/* {{{ proto public ReflectionClass::__construct(mixed argument) throws ReflectionException
   Constructor. Takes a string or an instance as an argument */
ZEND_METHOD(reflection_class, __construct)
{
	reflection_class_object_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto public array ReflectionClass::getStaticProperties()
   Returns an associative array containing all static property values of the class */
ZEND_METHOD(reflection_class, getStaticProperties)
{
	zval *tmp_copy;
	reflection_object *intern;
	zend_class_entry *ce;
	
	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(ce);

	zend_update_class_constants(ce TSRMLS_CC);

	array_init(return_value);
	zend_hash_copy(Z_ARRVAL_P(return_value), ce->static_members, (copy_ctor_func_t) zval_add_ref, (void *) &tmp_copy, sizeof(zval *));
}
/* }}} */

/* {{{ proto public array ReflectionClass::getDefaultProperties()
   Returns an associative array containing copies of all default property values of the class */
ZEND_METHOD(reflection_class, getDefaultProperties)
{
	reflection_object *intern;
	zend_class_entry *ce;
	int count;
	
	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(ce);
	array_init(return_value);

	zend_update_class_constants(ce TSRMLS_CC);

	count = zend_hash_num_elements(&ce->default_properties);
	if (count > 0) {
		HashPosition pos;
		zval **prop;

		zend_hash_internal_pointer_reset_ex(&ce->default_properties, &pos);
		while (zend_hash_get_current_data_ex(&ce->default_properties, (void **) &prop, &pos) == SUCCESS) {
			char *key, *class_name, *prop_name;
			uint key_len;
			ulong num_index;
			zval *prop_copy;

			zend_hash_get_current_key_ex(&ce->default_properties, &key, &key_len, &num_index, 0, &pos);
			zend_hash_move_forward_ex(&ce->default_properties, &pos);
			zend_unmangle_property_name(key, &class_name, &prop_name);
			if (class_name && class_name[0] != '*' && strcmp(class_name, ce->name)) {
				/* filter privates from base classes */
				continue;
			}

			/* copy: enforce read only access */
			ALLOC_ZVAL(prop_copy);
			*prop_copy = **prop;
			zval_copy_ctor(prop_copy);
			INIT_PZVAL(prop_copy);

			add_assoc_zval(return_value, prop_name, prop_copy);
		}
	}
}
/* }}} */

/* {{{ proto public string ReflectionClass::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_class, __toString)
{
	reflection_object *intern;
	zend_class_entry *ce;
	string str;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	string_init(&str);
	_class_string(&str, ce, intern->obj, "" TSRMLS_CC);
	RETURN_STRINGL(str.string, str.len - 1, 0);
}
/* }}} */

/* {{{ proto public string ReflectionClass::getName()
   Returns the class' name */
ZEND_METHOD(reflection_class, getName)
{
	METHOD_NOTSTATIC_NUMPARAMS(0);
	_default_get_entry(getThis(), "name", sizeof("name"), return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isInternal()
   Returns whether this class is an internal class */
ZEND_METHOD(reflection_class, isInternal)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(ce->type == ZEND_INTERNAL_CLASS);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isUserDefined()
   Returns whether this class is user-defined */
ZEND_METHOD(reflection_class, isUserDefined)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(ce->type == ZEND_USER_CLASS);
}
/* }}} */

/* {{{ proto public string ReflectionClass::getFileName()
   Returns the filename of the file this class was declared in */
ZEND_METHOD(reflection_class, getFileName)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS) {
		RETURN_STRING(ce->filename, 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public int ReflectionClass::getStartLine()
   Returns the line this class' declaration starts at */
ZEND_METHOD(reflection_class, getStartLine)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(ce->line_start);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public int ReflectionClass::getEndLine()
   Returns the line this class' declaration ends at */
ZEND_METHOD(reflection_class, getEndLine)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS) {
		RETURN_LONG(ce->line_end);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public string ReflectionClass::getDocComment()
   Returns the doc comment for this class */
ZEND_METHOD(reflection_class, getDocComment)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS && ce->doc_comment) {
		RETURN_STRINGL(ce->doc_comment, ce->doc_comment_len, 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public ReflectionMethod ReflectionClass::getConstructor()
   Returns the class' constructor if there is one, NULL otherwise */
ZEND_METHOD(reflection_class, getConstructor)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->constructor) {
		reflection_method_factory(ce, ce->constructor, return_value TSRMLS_CC);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto public ReflectionMethod ReflectionClass::getMethod(string name) throws ReflectionException
   Returns the class' method specified by its name */
ZEND_METHOD(reflection_class, getMethod)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_function *mptr;
	char *name, *lc_name; 
	int name_len;

	METHOD_NOTSTATIC;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	lc_name = zend_str_tolower_dup(name, name_len);
	if (zend_hash_find(&ce->function_table, lc_name, name_len + 1, (void**) &mptr) == SUCCESS) {
		reflection_method_factory(ce, mptr, return_value TSRMLS_CC);
		efree(lc_name);
	} else {
		efree(lc_name);
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
				"Method %s does not exist", name);
		return;
	}
}
/* }}} */

/* {{{ _addmethod */
static int _addmethod(zend_function *mptr, int num_args, va_list args, zend_hash_key *hash_key TSRMLS_DC)
{
	zval *method;
	zend_class_entry *ce = *va_arg(args, zend_class_entry**);
	zval *retval = va_arg(args, zval*);
	long filter = va_arg(args, long);

	if (mptr->common.fn_flags & filter) {
		TSRMLS_FETCH();
		ALLOC_ZVAL(method);
		reflection_method_factory(ce, mptr, method TSRMLS_CC);
		add_next_index_zval(retval, method);
	}
	return 0;
}
/* }}} */

/* {{{ proto public ReflectionMethod[] ReflectionClass::getMethods()
   Returns an array of this class' methods */
ZEND_METHOD(reflection_class, getMethods)
{
	reflection_object *intern;
	zend_class_entry *ce;
	long filter = 0;
	int argc = ZEND_NUM_ARGS();

	METHOD_NOTSTATIC;
	if (argc) {
		if (zend_parse_parameters(argc TSRMLS_CC, "|l", &filter) == FAILURE) {
			return;
		}
	} else {
		/* No parameters given, default to "return all" */
		filter = ZEND_ACC_PPP_MASK | ZEND_ACC_ABSTRACT | ZEND_ACC_FINAL | ZEND_ACC_STATIC;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);
	zend_hash_apply_with_arguments(&ce->function_table, (apply_func_args_t) _addmethod, 3, &ce, return_value, filter);
}
/* }}} */

/* {{{ proto public ReflectionProperty ReflectionClass::getProperty(string name) throws ReflectionException
   Returns the class' property specified by its name */
ZEND_METHOD(reflection_class, getProperty)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_property_info *property_info;
	char *name; 
	int name_len;

	METHOD_NOTSTATIC;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	if (zend_hash_find(&ce->properties_info, name, name_len + 1, (void**) &property_info) == SUCCESS && (property_info->flags & ZEND_ACC_SHADOW) == 0) {
		reflection_property_factory(ce, property_info, return_value TSRMLS_CC);
	} else {
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
				"Property %s does not exist", name);
		return;
	}
}
/* }}} */

/* {{{ _addproperty */
static int _addproperty(zend_property_info *pptr, int num_args, va_list args, zend_hash_key *hash_key TSRMLS_DC)
{
	zval *property;
	zend_class_entry *ce = *va_arg(args, zend_class_entry**);
	zval *retval = va_arg(args, zval*);
	long filter = va_arg(args, long);

	if (pptr->flags	& ZEND_ACC_SHADOW) {
		return 0;
	}
	
	if (pptr->flags	& filter) {
		TSRMLS_FETCH();
		ALLOC_ZVAL(property);
		reflection_property_factory(ce, pptr, property TSRMLS_CC);
		add_next_index_zval(retval, property);
	}
	return 0;
}
/* }}} */

/* {{{ proto public ReflectionProperty[] ReflectionClass::getProperties()
   Returns an array of this class' properties */
ZEND_METHOD(reflection_class, getProperties)
{
	reflection_object *intern;
	zend_class_entry *ce;
	long filter = 0;
	int argc = ZEND_NUM_ARGS();

	METHOD_NOTSTATIC;
	if (argc) {
		if (zend_parse_parameters(argc TSRMLS_CC, "|l", &filter) == FAILURE) {
			return;
		}
	} else {
		/* No parameters given, default to "return all" */
		filter = ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);
	zend_hash_apply_with_arguments(&ce->properties_info, (apply_func_args_t) _addproperty, 3, &ce, return_value, filter);
}
/* }}} */

/* {{{ proto public array ReflectionClass::getConstants()
   Returns an associative array containing this class' constants and their values */
ZEND_METHOD(reflection_class, getConstants)
{
	zval *tmp_copy;
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(ce);
	array_init(return_value);
	zend_hash_apply_with_argument(&ce->constants_table, (apply_func_arg_t) zval_update_constant, (void*)1 TSRMLS_CC);
	zend_hash_copy(Z_ARRVAL_P(return_value), &ce->constants_table, (copy_ctor_func_t) zval_add_ref, (void *) &tmp_copy, sizeof(zval *));
}
/* }}} */

/* {{{ proto public mixed ReflectionClass::getConstant(string name)
   Returns the class' constant specified by its name */
ZEND_METHOD(reflection_class, getConstant)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zval **value;
	char *name; 
	int name_len;

	METHOD_NOTSTATIC;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	zend_hash_apply_with_argument(&ce->constants_table, (apply_func_arg_t) zval_update_constant, (void*)1 TSRMLS_CC);
	if (zend_hash_find(&ce->constants_table, name, name_len + 1, (void **) &value) == FAILURE) {
		RETURN_FALSE;
	}
	*return_value = **value;
	zval_copy_ctor(return_value);
}
/* }}} */

/* {{{ _class_check_flag */
static void _class_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	RETVAL_BOOL(ce->ce_flags & mask);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isInstantiable()
   Returns whether this class is instantiable */
ZEND_METHOD(reflection_class, isInstantiable)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->ce_flags & (ZEND_ACC_INTERFACE | ZEND_ACC_IMPLICIT_ABSTRACT_CLASS | ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		RETURN_FALSE;
	}

	/* Basically, the class is instantiable. Though, if there is a constructor
	 * and it is not publicly accessible, it isn't! */
	if (!ce->constructor) {
		RETURN_TRUE;
	}

	RETURN_BOOL(ce->constructor->common.fn_flags & ZEND_ACC_PUBLIC);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isInterface()
   Returns whether this is an interface or a class */
ZEND_METHOD(reflection_class, isInterface)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_INTERFACE);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isFinal()
   Returns whether this class is final */
ZEND_METHOD(reflection_class, isFinal)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_FINAL_CLASS);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isAbstract()
   Returns whether this class is abstract */
ZEND_METHOD(reflection_class, isAbstract)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);
}
/* }}} */

/* {{{ proto public int ReflectionClass::getModifiers()
   Returns a bitfield of the access modifiers for this class */
ZEND_METHOD(reflection_class, getModifiers)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);

	RETURN_LONG(ce->ce_flags);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isInstance(stdclass object)
   Returns whether the given object is an instance of this class */
ZEND_METHOD(reflection_class, isInstance)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zval *object;

	METHOD_NOTSTATIC;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &object) == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);	
	RETURN_BOOL(ce == Z_OBJCE_P(object));
}
/* }}} */

/* {{{ proto public stdclass ReflectionClass::newInstance(mixed* args, ...)
   Returns an instance of this class */
ZEND_METHOD(reflection_class, newInstance)
{
	zval *retval_ptr;
	reflection_object *intern;
	zend_class_entry *ce;
	int argc = ZEND_NUM_ARGS();
	
	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(ce);

	object_init_ex(return_value, ce);

	/* Run the constructor if there is one */
	if (ce->constructor) {
		zval ***params;
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;

		if (!(ce->constructor->common.fn_flags & ZEND_ACC_PUBLIC)) {
			zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, "Access to non-public constructor of class %s", ce->name);
			return;
		}

		params = safe_emalloc(sizeof(zval **), argc, 0);
		if (zend_get_parameters_array_ex(argc, params) == FAILURE) {
			efree(params);
			RETURN_FALSE;
		}

		fci.size = sizeof(fci);
		fci.function_table = EG(function_table);
		fci.function_name = NULL;
		fci.symbol_table = NULL;
		fci.object_pp = &return_value;
		fci.retval_ptr_ptr = &retval_ptr;
		fci.param_count = argc;
		fci.params = params;
		fci.no_separation = 1;

		fcc.initialized = 1;
		fcc.function_handler = ce->constructor;
		fcc.calling_scope = EG(scope);
		fcc.object_pp = &return_value;

		if (zend_call_function(&fci, &fcc TSRMLS_CC) == FAILURE) {
			efree(params);
			zval_ptr_dtor(&retval_ptr);
			zend_error(E_WARNING, "Invocation of %s's constructor failed", ce->name);
			RETURN_NULL();
		}
		if (retval_ptr) {
			zval_ptr_dtor(&retval_ptr);
		}
		efree(params);
	}
}
/* }}} */

/* {{{ proto public ReflectionClass[] ReflectionClass::getInterfaces()
   Returns an array of interfaces this class implements */
ZEND_METHOD(reflection_class, getInterfaces)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);

	/* Return an empty array if this class implements no interfaces */
	array_init(return_value);

	if (ce->num_interfaces) {
		zend_uint i;

	   	for (i=0; i < ce->num_interfaces; i++) {
			zval *interface;
			ALLOC_ZVAL(interface);
			zend_reflection_class_factory(ce->interfaces[i], interface TSRMLS_CC);
			add_next_index_zval(return_value, interface);
		}
	}
}
/* }}} */

/* {{{ proto public ReflectionClass ReflectionClass::getParentClass()
   Returns the class' parent class, or, if none exists, FALSE */
ZEND_METHOD(reflection_class, getParentClass)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ce);
	
	if (ce->parent) {
		zend_reflection_class_factory(ce->parent, return_value TSRMLS_CC);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isSubclassOf(string|ReflectionClass class)
   Returns whether this class is a subclass of another class */
ZEND_METHOD(reflection_class, isSubclassOf)
{
	reflection_object *intern, *argument;
	zend_class_entry *ce, **pce, *class_ce;
	zval *class_name;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(ce);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &class_name) == FAILURE) {
		return;
	}
	
	switch(class_name->type) {
		case IS_STRING:
			if (zend_lookup_class(Z_STRVAL_P(class_name), Z_STRLEN_P(class_name), &pce TSRMLS_CC) == FAILURE) {
				zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
						"Interface %s does not exist", Z_STRVAL_P(class_name));
				return;
			}
			class_ce = *pce;
			break;			
		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(class_name), reflection_class_ptr TSRMLS_CC)) {
				argument = (reflection_object *) zend_object_store_get_object(class_name TSRMLS_CC);
				if (argument == NULL || argument->ptr == NULL) {
					zend_error(E_ERROR, "Internal error: Failed to retrieve the argument's reflection object");
					/* Bails out */
				}
				class_ce = argument->ptr;
				break;
			}
			/* no break */
		default:
			zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
					"Parameter one must either be a string or a ReflectionClass object");
			return;
	}


	RETURN_BOOL(instanceof_function(ce, class_ce TSRMLS_CC));
}
/* }}} */

/* {{{ proto public bool ReflectionClass::implementsInterface(string|ReflectionClass interface_name)
   Returns whether this class is a subclass of another class */
ZEND_METHOD(reflection_class, implementsInterface)
{
	reflection_object *intern, *argument;
	zend_class_entry *ce, *interface_ce, **pce;
	zval *interface;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(ce);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &interface) == FAILURE) {
		return;
	}
	
	switch(interface->type) {
		case IS_STRING:
			if (zend_lookup_class(Z_STRVAL_P(interface), Z_STRLEN_P(interface), &pce TSRMLS_CC) == FAILURE) {
				zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
						"Interface %s does not exist", Z_STRVAL_P(interface));
				return;
			}
			interface_ce = *pce;
			break;			
		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(interface), reflection_class_ptr TSRMLS_CC)) {
				argument = (reflection_object *) zend_object_store_get_object(interface TSRMLS_CC);
				if (argument == NULL || argument->ptr == NULL) {
					zend_error(E_ERROR, "Internal error: Failed to retrieve the argument's reflection object");
					/* Bails out */
				}
				interface_ce = argument->ptr;
				break;
			}
			/* no break */
		default:
			zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
					"Parameter one must either be a string or a ReflectionClass object");
			return;
	}

	if (!(interface_ce->ce_flags & ZEND_ACC_INTERFACE)) {
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
				"Interface %s is a Class", interface_ce->name);
		return;
	}
	RETURN_BOOL(instanceof_function(ce, interface_ce TSRMLS_CC));
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isIterateable()
   Returns whether this class is iterateable (can be used inside foreach) */
ZEND_METHOD(reflection_class, isIterateable)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(ce);

	RETURN_BOOL(ce->get_iterator != NULL);
}
/* }}} */

/* {{{ proto public ReflectionExtension|NULL ReflectionClass::getExtension()
   Returns NULL or the extension the class belongs to */
ZEND_METHOD(reflection_class, getExtension)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->module) {
		reflection_extension_factory(return_value, ce->module->name TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto public string|false ReflectionClass::getExtensionName()
   Returns false or the name of the extension the class belongs to */
ZEND_METHOD(reflection_class, getExtensionName)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->module) {
		RETURN_STRING(ce->module->name, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto public static mixed ReflectionObject::export(mixed argument [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_object, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_object_ptr, 1);
}
/* }}} */

/* {{{ proto public ReflectionObject::__construct(mixed argument) throws ReflectionException
   Constructor. Takes an instance as an argument */
ZEND_METHOD(reflection_object, __construct)
{
	reflection_class_object_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto public static mixed ReflectionProperty::export(mixed class, string name [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_property, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_property_ptr, 2);
}
/* }}} */

/* {{{ proto public ReflectionProperty::__construct(mixed class, string name)
   Constructor. Throws an Exception in case the given property does not exist */
ZEND_METHOD(reflection_property, __construct)
{
	zval *propname, *classname;
	char *name_str, *class_name, *prop_name;
	int name_len;
	zval *object;
	reflection_object *intern;
	zend_class_entry **pce;
	zend_class_entry *ce;
	zend_property_info *property_info;
	property_reference *reference;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs", &classname, &name_str, &name_len) == FAILURE) {
		return;
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL) {
		return;
	}
	
	/* Find the class entry */
	switch (Z_TYPE_P(classname)) {
		case IS_STRING:
			if (zend_lookup_class(Z_STRVAL_P(classname), Z_STRLEN_P(classname), &pce TSRMLS_CC) == FAILURE) {
				zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC,
						"Class %s does not exist", Z_STRVAL_P(classname)); 
				return;
			}
			ce = *pce;
			break;

		case IS_OBJECT:
			ce = Z_OBJCE_P(classname);
			break;
			
		default:
			_DO_THROW("The parameter class is expected to be either a string or an object");
			/* returns out of this function */
	}

	if (zend_hash_find(&ce->properties_info, name_str, name_len + 1, (void **) &property_info) == FAILURE || (property_info->flags & ZEND_ACC_SHADOW)) {
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
			"Property %s::$%s does not exist", ce->name, name_str);
		return;
	}
	
	if (!(property_info->flags & ZEND_ACC_PRIVATE)) {
		/* we have to seach the class hierarchy for this (implicit) public or protected property */
		zend_class_entry *tmp_ce = ce->parent;
		zend_property_info *tmp_info;
		
		while (tmp_ce && zend_hash_find(&tmp_ce->properties_info, name_str, name_len + 1, (void **) &tmp_info) == SUCCESS) {
			if (tmp_info->flags & ZEND_ACC_PRIVATE) {
				/* private in super class => NOT the same property */
				break;
			}
			ce = tmp_ce;
			property_info = tmp_info;
			tmp_ce = tmp_ce->parent;
		}
	}

	MAKE_STD_ZVAL(classname);
	ZVAL_STRINGL(classname, ce->name, ce->name_length, 1);
	zend_hash_update(Z_OBJPROP_P(object), "class", sizeof("class"), (void **) &classname, sizeof(zval *), NULL);
	
	zend_unmangle_property_name(property_info->name, &class_name, &prop_name);
	MAKE_STD_ZVAL(propname);
	ZVAL_STRING(propname, prop_name, 1);
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &propname, sizeof(zval *), NULL);

	reference = (property_reference*) emalloc(sizeof(property_reference));
	reference->ce = ce;
	reference->prop = property_info;
	intern->ptr = reference;
	intern->free_ptr = 1;
	intern->ce = ce;
}
/* }}} */

/* {{{ proto public string ReflectionProperty::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_property, __toString)
{
	reflection_object *intern;
	property_reference *ref;
	string str;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ref);
	string_init(&str);
	_property_string(&str, ref->prop, NULL, "" TSRMLS_CC);
	RETURN_STRINGL(str.string, str.len - 1, 0);
}
/* }}} */

/* {{{ proto public string ReflectionProperty::getName()
   Returns the class' name */
ZEND_METHOD(reflection_property, getName)
{
	METHOD_NOTSTATIC_NUMPARAMS(0);
	_default_get_entry(getThis(), "name", sizeof("name"), return_value TSRMLS_CC);
}
/* }}} */

static void _property_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask)
{
	reflection_object *intern;
	property_reference *ref;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ref);
	RETURN_BOOL(ref->prop->flags & mask);
}

/* {{{ proto public bool ReflectionProperty::isPublic()
   Returns whether this property is public */
ZEND_METHOD(reflection_property, isPublic)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC);
}
/* }}} */

/* {{{ proto public bool ReflectionProperty::isPrivate()
   Returns whether this property is private */
ZEND_METHOD(reflection_property, isPrivate)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PRIVATE);
}
/* }}} */

/* {{{ proto public bool ReflectionProperty::isProtected()
   Returns whether this property is protected */
ZEND_METHOD(reflection_property, isProtected)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PROTECTED);
}
/* }}} */

/* {{{ proto public bool ReflectionProperty::isStatic()
   Returns whether this property is static */
ZEND_METHOD(reflection_property, isStatic)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_STATIC);
}
/* }}} */

/* {{{ proto public bool ReflectionProperty::isDefault()
   Returns whether this property is default (declared at compilation time). */
ZEND_METHOD(reflection_property, isDefault)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ~ZEND_ACC_IMPLICIT_PUBLIC);
}
/* }}} */

/* {{{ proto public int ReflectionProperty::getModifiers()
   Returns a bitfield of the access modifiers for this property */
ZEND_METHOD(reflection_property, getModifiers)
{
	reflection_object *intern;
	property_reference *ref;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ref);

	RETURN_LONG(ref->prop->flags);
}
/* }}} */

/* {{{ proto public mixed ReflectionProperty::getValue([stdclass object])
   Returns this property's value */
ZEND_METHOD(reflection_property, getValue)
{
	reflection_object *intern;
	property_reference *ref;
	zval *object;
	zval **member= NULL;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(ref);

	if (!(ref->prop->flags & ZEND_ACC_PUBLIC)) {
		_DO_THROW("Cannot access non-public member");
		/* Returns from this function */
	}

	if ((ref->prop->flags & ZEND_ACC_STATIC)) {
		zend_update_class_constants(intern->ce TSRMLS_CC);
		if (zend_hash_quick_find(intern->ce->static_members, ref->prop->name, ref->prop->name_length + 1, ref->prop->h, (void **) &member) == FAILURE) {
			zend_error(E_ERROR, "Internal error: Could not find the property %s", ref->prop->name);
			/* Bails out */
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &object) == FAILURE) {
			return;
		}
		if (zend_hash_quick_find(Z_OBJPROP_P(object), ref->prop->name, ref->prop->name_length + 1, ref->prop->h, (void **) &member) == FAILURE) {
			zend_error(E_ERROR, "Internal error: Could not find the property %s", ref->prop->name);
			/* Bails out */
		}
	}

	*return_value= **member;
	zval_copy_ctor(return_value);
}
/* }}} */

/* {{{ proto public void ReflectionProperty::setValue([stdclass object,] mixed value)
   Sets this property's value */
ZEND_METHOD(reflection_property, setValue)
{
	reflection_object *intern;
	property_reference *ref;
	zval **variable_ptr;
	zval *object;
	zval *value;
	int setter_done = 0;
	zval *tmp;
	HashTable *prop_table;

	METHOD_NOTSTATIC;
	GET_REFLECTION_OBJECT_PTR(ref);

	if (ref->prop->flags & ~(ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)) {
		_DO_THROW("Cannot access non-public member");
		/* Returns from this function */
	}

	if ((ref->prop->flags & ZEND_ACC_STATIC)) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &tmp, &value) == FAILURE) {
				return;
			}
		}
		prop_table = intern->ce->static_members;
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oz", &object, &value) == FAILURE) {
			return;
		}
		prop_table = Z_OBJPROP_P(object);
	}

	if (zend_hash_quick_find(prop_table, ref->prop->name, ref->prop->name_length + 1, ref->prop->h, (void **) &variable_ptr) == FAILURE) {
		zend_error(E_ERROR, "Internal error: Could not find the property %s", ref->prop->name);
		/* Bails out */
	}
	if (*variable_ptr == value) {
		setter_done = 1;
	} else {
		if (PZVAL_IS_REF(*variable_ptr)) {
			zval_dtor(*variable_ptr);
			(*variable_ptr)->type = value->type;
			(*variable_ptr)->value = value->value;
			if (value->refcount > 0) {
				zval_copy_ctor(*variable_ptr);
			}
			setter_done = 1;
		}
	}
	if (!setter_done) {
		zval **foo;

		value->refcount++;
		if (PZVAL_IS_REF(value)) {
			SEPARATE_ZVAL(&value);
		}
		zend_hash_quick_update(prop_table, ref->prop->name, ref->prop->name_length+1, ref->prop->h, &value, sizeof(zval *), (void **) &foo);
	}
}
/* }}} */

/* {{{ proto public ReflectionClass ReflectionProperty::getDeclaringClass()
   Get the declaring class */
ZEND_METHOD(reflection_property, getDeclaringClass)
{
	reflection_object *intern;
	property_reference *ref;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(ref);

	zend_reflection_class_factory(ref->ce, return_value TSRMLS_CC);
}

/* {{{ proto public static mixed ReflectionExtension::export(string name [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_extension, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_extension_ptr, 1);
}
/* }}} */

/* {{{ proto public ReflectionExtension::__construct(string name)
   Constructor. Throws an Exception in case the given extension does not exist */
ZEND_METHOD(reflection_extension, __construct)
{
	zval *name;
	zval *object;
	char *lcname;
	reflection_object *intern;
	zend_module_entry *module;
	char *name_str;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name_str, &name_len) == FAILURE) {
		return;
	}

	object = getThis();
	intern = (reflection_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (intern == NULL) {
		return;
	}
	lcname = do_alloca(name_len + 1);
	zend_str_tolower_copy(lcname, name_str, name_len);
	if (zend_hash_find(&module_registry, lcname, name_len + 1, (void **)&module) == FAILURE) {
		free_alloca(lcname);
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
			"Extension %s does not exist", name_str);
		return;
	}
	free_alloca(lcname);
	MAKE_STD_ZVAL(name);
	ZVAL_STRING(name, module->name, 1);
	zend_hash_update(Z_OBJPROP_P(object), "name", sizeof("name"), (void **) &name, sizeof(zval *), NULL);
	intern->ptr = module;
	intern->free_ptr = 0;
	intern->ce = NULL;
}
/* }}} */

/* {{{ proto public string ReflectionExtension::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_extension, __toString)
{
	reflection_object *intern;
	zend_module_entry *module;
	string str;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(module);
	string_init(&str);
	_extension_string(&str, module, "" TSRMLS_CC);
	RETURN_STRINGL(str.string, str.len - 1, 0);
}
/* }}} */

/* {{{ proto public string ReflectionExtension::getName()
   Returns this extension's name */
ZEND_METHOD(reflection_extension, getName)
{
	METHOD_NOTSTATIC_NUMPARAMS(0);
	_default_get_entry(getThis(), "name", sizeof("name"), return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto public string ReflectionExtension::getVersion()
   Returns this extension's version */
ZEND_METHOD(reflection_extension, getVersion)
{
	reflection_object *intern;
	zend_module_entry *module;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(module);

	/* An extension does not necessarily have a version number */
	if (module->version == NO_VERSION_YET) {
		RETURN_NULL();
	} else {
		RETURN_STRING(module->version, 1);
	}
}
/* }}} */

/* {{{ proto public ReflectionFunction[] ReflectionExtension::getFunctions()
   Returns an array of this extension's fuctions */
ZEND_METHOD(reflection_extension, getFunctions)
{
	reflection_object *intern;
	zend_module_entry *module;

	METHOD_NOTSTATIC_NUMPARAMS(0);
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	if (module->functions) {
		zval *function;
		zend_function *fptr;
		zend_function_entry *func = module->functions;

		/* Is there a better way of doing this? */
		while (func->fname) {
			if (zend_hash_find(EG(function_table), func->fname, strlen(func->fname) + 1, (void**) &fptr) == FAILURE) {
				zend_error(E_WARNING, "Internal error: Cannot find extension function %s in global function table", func->fname);
				continue;
			}
			
			ALLOC_ZVAL(function);
			reflection_function_factory(fptr, function TSRMLS_CC);
			add_assoc_zval_ex(return_value, func->fname, strlen(func->fname)+1, function);
			func++;
		}
	}
}
/* }}} */

static int _addconstant(zend_constant *constant, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *const_val;
	zval *retval = va_arg(args, zval*);
	int number = va_arg(args, int);

	if (number == constant->module_number) {
		MAKE_STD_ZVAL(const_val);
		*const_val = constant->value;
		zval_copy_ctor(const_val);
		INIT_PZVAL(const_val);
		add_assoc_zval_ex(retval, constant->name, constant->name_len, const_val);
	}
	return 0;
}

/* {{{ proto public array ReflectionExtension::getConstants()
   Returns an associative array containing this extension's constants and their values */
ZEND_METHOD(reflection_extension, getConstants)
{
	reflection_object *intern;
	zend_module_entry *module;

	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(zend_constants), (apply_func_args_t) _addconstant, 2, return_value, module->module_number);
}
/* }}} */

/* {{{ _addinientry */
static int _addinientry(zend_ini_entry *ini_entry, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *retval = va_arg(args, zval*);
	int number = va_arg(args, int);

	if (number == ini_entry->module_number) {
		if (ini_entry->value) {
			add_assoc_stringl(retval, ini_entry->name, ini_entry->value, ini_entry->value_length, 1);
		} else {
			add_assoc_null(retval, ini_entry->name);
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ proto public array ReflectionExtension::getINIEntries()
   Returns an associative array containing this extension's INI entries and their values */
ZEND_METHOD(reflection_extension, getINIEntries)
{
	reflection_object *intern;
	zend_module_entry *module;

	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(ini_directives), (apply_func_args_t) _addinientry, 2, return_value, module->module_number);
}
/* }}} */

/* {{{ add_extension_class */
static int add_extension_class(zend_class_entry **pce, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *class_array = va_arg(args, zval*), *zclass;
	struct _zend_module_entry *module = va_arg(args, struct _zend_module_entry*);
	int add_reflection_class = va_arg(args, int);

	if ((*pce)->module && !strcasecmp((*pce)->module->name, module->name)) {
		TSRMLS_FETCH();
		if (add_reflection_class) {
			ALLOC_ZVAL(zclass);
			zend_reflection_class_factory(*pce, zclass TSRMLS_CC);
			add_assoc_zval_ex(class_array, (*pce)->name, (*pce)->name_length + 1, zclass);
		} else {
			add_next_index_stringl(class_array, (*pce)->name, (*pce)->name_length, 1);
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ proto public ReflectionClass[] ReflectionExtension::getClasses()
   Returns an array containing ReflectionClass objects for all classes of this extension */
ZEND_METHOD(reflection_extension, getClasses)
{
	reflection_object *intern;
	zend_module_entry *module;

	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table), (apply_func_args_t) add_extension_class, 3, return_value, module, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ proto public array ReflectionExtension::getClassNames()
   Returns an array containing all names of all classes of this extension */
ZEND_METHOD(reflection_extension, getClassNames)
{
	reflection_object *intern;
	zend_module_entry *module;

	METHOD_NOTSTATIC_NUMPARAMS(0);	
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table), (apply_func_args_t) add_extension_class, 3, return_value, module, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ method tables */
static zend_function_entry reflection_exception_functions[] = {
	{NULL, NULL, NULL}
};

static zend_function_entry reflection_functions[] = {
	ZEND_ME(reflection, getModifierNames, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(reflection, export, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};

static zend_function_entry reflector_functions[] = {
	ZEND_FENTRY(export, NULL, NULL, ZEND_ACC_STATIC|ZEND_ACC_ABSTRACT|ZEND_ACC_PUBLIC)
	ZEND_ABSTRACT_ME(reflector, __toString, NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry reflection_function_functions[] = {
	ZEND_ME(reflection, __clone, NULL, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_function, export, NULL, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_function, __construct, NULL, 0)
	ZEND_ME(reflection_function, __toString, NULL, 0)
	ZEND_ME(reflection_function, isInternal, NULL, 0)
	ZEND_ME(reflection_function, isUserDefined, NULL, 0)
	ZEND_ME(reflection_function, getName, NULL, 0)
	ZEND_ME(reflection_function, getFileName, NULL, 0)
	ZEND_ME(reflection_function, getStartLine, NULL, 0)
	ZEND_ME(reflection_function, getEndLine, NULL, 0)
	ZEND_ME(reflection_function, getDocComment, NULL, 0)
	ZEND_ME(reflection_function, getStaticVariables, NULL, 0)
	ZEND_ME(reflection_function, invoke, NULL, 0)
	ZEND_ME(reflection_function, returnsReference, NULL, 0)
	ZEND_ME(reflection_function, getParameters, NULL, 0)
	ZEND_ME(reflection_function, getNumberOfParameters, NULL, 0)
	ZEND_ME(reflection_function, getNumberOfRequiredParameters, NULL, 0)
	{NULL, NULL, NULL}
};

static zend_function_entry reflection_method_functions[] = {
	ZEND_ME(reflection_method, export, NULL, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_method, __construct, NULL, 0)
	ZEND_ME(reflection_method, __toString, NULL, 0)
	ZEND_ME(reflection_method, isPublic, NULL, 0)
	ZEND_ME(reflection_method, isPrivate, NULL, 0)
	ZEND_ME(reflection_method, isProtected, NULL, 0)
	ZEND_ME(reflection_method, isAbstract, NULL, 0)
	ZEND_ME(reflection_method, isFinal, NULL, 0)
	ZEND_ME(reflection_method, isStatic, NULL, 0)
	ZEND_ME(reflection_method, isConstructor, NULL, 0)
	ZEND_ME(reflection_method, isDestructor, NULL, 0)
	ZEND_ME(reflection_method, getModifiers, NULL, 0)
	ZEND_ME(reflection_method, invoke, NULL, 0)
	ZEND_ME(reflection_method, getDeclaringClass, NULL, 0)
	{NULL, NULL, NULL}
};

static zend_function_entry reflection_class_functions[] = {
	ZEND_ME(reflection, __clone, NULL, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_class, export, NULL, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_class, __construct, NULL, 0)
	ZEND_ME(reflection_class, __toString, NULL, 0)
	ZEND_ME(reflection_class, getName, NULL, 0)
	ZEND_ME(reflection_class, isInternal, NULL, 0)
	ZEND_ME(reflection_class, isUserDefined, NULL, 0)
	ZEND_ME(reflection_class, isInstantiable, NULL, 0)
	ZEND_ME(reflection_class, getFileName, NULL, 0)
	ZEND_ME(reflection_class, getStartLine, NULL, 0)
	ZEND_ME(reflection_class, getEndLine, NULL, 0)
	ZEND_ME(reflection_class, getDocComment, NULL, 0)
	ZEND_ME(reflection_class, getConstructor, NULL, 0)
	ZEND_ME(reflection_class, getMethod, NULL, 0)
	ZEND_ME(reflection_class, getMethods, NULL, 0)
	ZEND_ME(reflection_class, getProperty, NULL, 0)
	ZEND_ME(reflection_class, getProperties, NULL, 0)
	ZEND_ME(reflection_class, getConstants, NULL, 0)
	ZEND_ME(reflection_class, getConstant, NULL, 0)
	ZEND_ME(reflection_class, getInterfaces, NULL, 0)
	ZEND_ME(reflection_class, isInterface, NULL, 0)
	ZEND_ME(reflection_class, isAbstract, NULL, 0)
	ZEND_ME(reflection_class, isFinal, NULL, 0)
	ZEND_ME(reflection_class, getModifiers, NULL, 0)
	ZEND_ME(reflection_class, isInstance, NULL, 0)
	ZEND_ME(reflection_class, newInstance, NULL, 0)
	ZEND_ME(reflection_class, getParentClass, NULL, 0)
	ZEND_ME(reflection_class, isSubclassOf, NULL, 0)
	ZEND_ME(reflection_class, getStaticProperties, NULL, 0)
	ZEND_ME(reflection_class, getDefaultProperties, NULL, 0)
	ZEND_ME(reflection_class, isIterateable, NULL, 0)
	ZEND_ME(reflection_class, implementsInterface, NULL, 0)
	ZEND_ME(reflection_class, getExtension, NULL, 0)
	ZEND_ME(reflection_class, getExtensionName, NULL, 0)
	{NULL, NULL, NULL}
};

static zend_function_entry reflection_object_functions[] = {
	ZEND_ME(reflection_object, export, NULL, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_object, __construct, NULL, 0)
	{NULL, NULL, NULL}
};

static zend_function_entry reflection_property_functions[] = {
	ZEND_ME(reflection, __clone, NULL, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_property, export, NULL, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_property, __construct, NULL, 0)
	ZEND_ME(reflection_property, __toString, NULL, 0)
	ZEND_ME(reflection_property, getName, NULL, 0)
	ZEND_ME(reflection_property, getValue, NULL, 0)
	ZEND_ME(reflection_property, setValue, NULL, 0)
	ZEND_ME(reflection_property, isPublic, NULL, 0)
	ZEND_ME(reflection_property, isPrivate, NULL, 0)
	ZEND_ME(reflection_property, isProtected, NULL, 0)
	ZEND_ME(reflection_property, isStatic, NULL, 0)
	ZEND_ME(reflection_property, isDefault, NULL, 0)
	ZEND_ME(reflection_property, getModifiers, NULL, 0)
	ZEND_ME(reflection_property, getDeclaringClass, NULL, 0)
	{NULL, NULL, NULL}
};

static zend_function_entry reflection_parameter_functions[] = {
	ZEND_ME(reflection, __clone, NULL, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_parameter, export, NULL, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_parameter, __construct, NULL, 0)
	ZEND_ME(reflection_parameter, __toString, NULL, 0)
	ZEND_ME(reflection_parameter, getName, NULL, 0)
	ZEND_ME(reflection_parameter, isPassedByReference, NULL, 0)
	ZEND_ME(reflection_parameter, getClass, NULL, 0)
	ZEND_ME(reflection_parameter, allowsNull, NULL, 0)
	ZEND_ME(reflection_parameter, isOptional, NULL, 0)
	ZEND_ME(reflection_parameter, isDefaultValueAvailable, NULL, 0)
	ZEND_ME(reflection_parameter, getDefaultValue, NULL, 0)
	{NULL, NULL, NULL}
};

static zend_function_entry reflection_extension_functions[] = {
	ZEND_ME(reflection, __clone, NULL, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_extension, export, NULL, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_extension, __construct, NULL, 0)
	ZEND_ME(reflection_extension, __toString, NULL, 0)
	ZEND_ME(reflection_extension, getName, NULL, 0)
	ZEND_ME(reflection_extension, getVersion, NULL, 0)
	ZEND_ME(reflection_extension, getFunctions, NULL, 0)
	ZEND_ME(reflection_extension, getConstants, NULL, 0)
	ZEND_ME(reflection_extension, getINIEntries, NULL, 0)
	ZEND_ME(reflection_extension, getClasses, NULL, 0)
	ZEND_ME(reflection_extension, getClassNames, NULL, 0)
	{NULL, NULL, NULL}
};
/* }}} */

static zend_object_handlers *zend_std_obj_handlers;

/* {{{ _reflection_write_property */
static void _reflection_write_property(zval *object, zval *member, zval *value TSRMLS_DC)
{
	if (Z_TYPE_P(member) == IS_STRING 
	&& zend_hash_exists(&Z_OBJCE_P(object)->default_properties, Z_STRVAL_P(member), Z_STRLEN_P(member)+1)
	&& (!strcmp(Z_STRVAL_P(member), "name") || !strcmp(Z_STRVAL_P(member), "class")))
	{
		zend_throw_exception_ex(reflection_exception_ptr, 0 TSRMLS_CC, 
			"Cannot set read-only property %s::$%s", Z_OBJCE_P(object)->name, Z_STRVAL_P(member));
	}
	else
	{
		zend_std_obj_handlers->write_property(object, member, value TSRMLS_CC);		
	}
}
/* }}} */

/* {{{ zend_register_reflection_api */
ZEND_API void zend_register_reflection_api(TSRMLS_D) {
	zend_class_entry _reflection_entry;

	zend_std_obj_handlers = zend_get_std_object_handlers();
	memcpy(&reflection_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	reflection_object_handlers.clone_obj = NULL;
	reflection_object_handlers.write_property = _reflection_write_property;

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionException", reflection_exception_functions);
	reflection_exception_ptr = zend_register_internal_class_ex(&_reflection_entry, zend_exception_get_default(), NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "Reflection", reflection_functions);
	reflection_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "Reflector", reflector_functions);
	reflector_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);
	reflector_ptr->ce_flags = ZEND_ACC_ABSTRACT | ZEND_ACC_INTERFACE;

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionFunction", reflection_function_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_function_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);
	reflection_register_implement(reflection_function_ptr, reflector_ptr TSRMLS_CC);
	zend_declare_property_string(reflection_function_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionParameter", reflection_parameter_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_parameter_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);
	reflection_register_implement(reflection_parameter_ptr, reflector_ptr TSRMLS_CC);
	zend_declare_property_string(reflection_parameter_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionMethod", reflection_method_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_method_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_function_ptr, NULL TSRMLS_CC);
	zend_declare_property_string(reflection_method_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(reflection_method_ptr, "class", sizeof("class")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionClass", reflection_class_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_class_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);
	reflection_register_implement(reflection_class_ptr, reflector_ptr TSRMLS_CC);
	zend_declare_property_string(reflection_class_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionObject", reflection_object_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_object_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_class_ptr, NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionProperty", reflection_property_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_property_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);
	reflection_register_implement(reflection_property_ptr, reflector_ptr TSRMLS_CC);
	zend_declare_property_string(reflection_property_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(reflection_property_ptr, "class", sizeof("class")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionExtension", reflection_extension_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_extension_ptr = zend_register_internal_class(&_reflection_entry TSRMLS_CC);
	reflection_register_implement(reflection_extension_ptr, reflector_ptr TSRMLS_CC);
	zend_declare_property_string(reflection_extension_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);

	/* Property modifiers */
	REGISTER_MAIN_LONG_CONSTANT("P_STATIC", ZEND_ACC_STATIC, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("P_PUBLIC", ZEND_ACC_PUBLIC, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("P_PROTECTED", ZEND_ACC_PROTECTED, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("P_PRIVATE", ZEND_ACC_PRIVATE, CONST_PERSISTENT|CONST_CS);

	/* Method modifiers */
	REGISTER_MAIN_LONG_CONSTANT("M_STATIC", ZEND_ACC_STATIC, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("M_PUBLIC", ZEND_ACC_PUBLIC, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("M_PROTECTED", ZEND_ACC_PROTECTED, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("M_PRIVATE", ZEND_ACC_PRIVATE, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("M_ABSTRACT", ZEND_ACC_ABSTRACT, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("M_FINAL", ZEND_ACC_FINAL, CONST_PERSISTENT|CONST_CS);

	/* Class modifiers */
	REGISTER_MAIN_LONG_CONSTANT("C_IMPLICIT_ABSTRACT", ZEND_ACC_IMPLICIT_ABSTRACT_CLASS, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("C_EXPLICIT_ABSTRACT", ZEND_ACC_EXPLICIT_ABSTRACT_CLASS, CONST_PERSISTENT|CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("C_FINAL", ZEND_ACC_FINAL_CLASS, CONST_PERSISTENT|CONST_CS);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
