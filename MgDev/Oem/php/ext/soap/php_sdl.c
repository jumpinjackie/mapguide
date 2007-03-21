/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@zend.com>                             |
  +----------------------------------------------------------------------+
*/
/* $Id: php_sdl.c,v 1.88.2.12.2.6 2007/01/01 09:36:06 sebastian Exp $ */

#include "php_soap.h"
#include "ext/libxml/php_libxml.h"
#include "libxml/uri.h"

#include "ext/standard/md5.h"
#include "tsrm_virtual_cwd.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef O_BINARY
# define O_BINARY 0
#endif

static void delete_fault(void *fault);
static void delete_fault_persistent(void *fault);
static void delete_binding(void *binding);
static void delete_binding_persistent(void *binding);
static void delete_function(void *function);
static void delete_function_persistent(void *function);
static void delete_parameter(void *paramater);
static void delete_parameter_persistent(void *paramater);
static void delete_header(void *header);
static void delete_header_persistent(void *header);
static void delete_document(void *doc_ptr);

encodePtr get_encoder_from_prefix(sdlPtr sdl, xmlNodePtr node, const xmlChar *type)
{
	encodePtr enc = NULL;
	xmlNsPtr nsptr;
	char *ns, *cptype;

	parse_namespace(type, &cptype, &ns);
	nsptr = xmlSearchNs(node->doc, node, BAD_CAST(ns));
	if (nsptr != NULL) {
		enc = get_encoder(sdl, (char*)nsptr->href, cptype);
		if (enc == NULL) {
			enc = get_encoder_ex(sdl, cptype, strlen(cptype));
		}
	} else {
		enc = get_encoder_ex(sdl, (char*)type, xmlStrlen(type));
	}
	efree(cptype);
	if (ns) {efree(ns);}
	return enc;
}

static sdlTypePtr get_element(sdlPtr sdl, xmlNodePtr node, const xmlChar *type)
{
	sdlTypePtr ret = NULL;

	if (sdl->elements) {
		xmlNsPtr nsptr;
		char *ns, *cptype;
		sdlTypePtr *sdl_type;

		parse_namespace(type, &cptype, &ns);
		nsptr = xmlSearchNs(node->doc, node, BAD_CAST(ns));
		if (nsptr != NULL) {
			int ns_len = xmlStrlen(nsptr->href);
			int type_len = strlen(cptype);
			int len = ns_len + type_len + 1;
			char *nscat = emalloc(len + 1);

			memcpy(nscat, nsptr->href, ns_len);
			nscat[ns_len] = ':';
			memcpy(nscat+ns_len+1, cptype, type_len);
			nscat[len] = '\0';

			if (zend_hash_find(sdl->elements, nscat, len + 1, (void **)&sdl_type) == SUCCESS) {
				ret = *sdl_type;
			} else if (zend_hash_find(sdl->elements, (char*)type, type_len + 1, (void **)&sdl_type) == SUCCESS) {
				ret = *sdl_type;
			}
			efree(nscat);
		} else {
			if (zend_hash_find(sdl->elements, (char*)type, xmlStrlen(type) + 1, (void **)&sdl_type) == SUCCESS) {
				ret = *sdl_type;
			}
		}

		efree(cptype);
		if (ns) {efree(ns);}
	}
	return ret;
}

encodePtr get_encoder(sdlPtr sdl, const char *ns, const char *type)
{
	encodePtr enc = NULL;
	char *nscat;
	int ns_len = strlen(ns);
	int type_len = strlen(type);
	int len = ns_len + type_len + 1;

	nscat = emalloc(len + 1);
	memcpy(nscat, ns, ns_len);
	nscat[ns_len] = ':';
	memcpy(nscat+ns_len+1, type, type_len);
	nscat[len] = '\0';

	enc = get_encoder_ex(sdl, nscat, len);

	if (enc == NULL &&
	    ((ns_len == sizeof(SOAP_1_1_ENC_NAMESPACE)-1 &&
	      memcmp(ns, SOAP_1_1_ENC_NAMESPACE, sizeof(SOAP_1_1_ENC_NAMESPACE)-1) == 0) ||
	     (ns_len == sizeof(SOAP_1_2_ENC_NAMESPACE)-1 &&
	      memcmp(ns, SOAP_1_2_ENC_NAMESPACE, sizeof(SOAP_1_2_ENC_NAMESPACE)-1) == 0))) {
		char *enc_nscat;
		int enc_ns_len;
		int enc_len;

		enc_ns_len = sizeof(XSD_NAMESPACE)-1;
		enc_len = enc_ns_len + type_len + 1;
		enc_nscat = emalloc(enc_len + 1);
		memcpy(enc_nscat, XSD_NAMESPACE, sizeof(XSD_NAMESPACE)-1);
		enc_nscat[enc_ns_len] = ':';
		memcpy(enc_nscat+enc_ns_len+1, type, type_len);
		enc_nscat[enc_len] = '\0';

		enc = get_encoder_ex(NULL, enc_nscat, enc_len);
		efree(enc_nscat);
		if (enc && sdl) {
			encodePtr new_enc	= pemalloc(sizeof(encode), sdl->is_persistent);
			memcpy(new_enc, enc, sizeof(encode));
			if (sdl->is_persistent) {
				new_enc->details.ns = zend_strndup(ns, ns_len);
				new_enc->details.type_str = strdup(new_enc->details.type_str);
			} else {
				new_enc->details.ns = estrndup(ns, ns_len);
				new_enc->details.type_str = estrdup(new_enc->details.type_str);
			}
			if (sdl->encoders == NULL) {
				sdl->encoders = pemalloc(sizeof(HashTable), sdl->is_persistent);
				zend_hash_init(sdl->encoders, 0, NULL, delete_encoder, sdl->is_persistent);
			}
			zend_hash_update(sdl->encoders, nscat, len + 1, &new_enc, sizeof(encodePtr), NULL);
			enc = new_enc;
		}
	}
	efree(nscat);
	return enc;
}

encodePtr get_encoder_ex(sdlPtr sdl, const char *nscat, int len)
{
	encodePtr *enc;
	TSRMLS_FETCH();

	if (zend_hash_find(&SOAP_GLOBAL(defEnc), (char*)nscat, len + 1, (void **)&enc) == SUCCESS) {
		return (*enc);
	} else if (sdl && sdl->encoders && zend_hash_find(sdl->encoders, (char*)nscat, len + 1, (void **)&enc) == SUCCESS) {
		return (*enc);
	}
	return NULL;
}

sdlBindingPtr get_binding_from_type(sdlPtr sdl, int type)
{
	sdlBindingPtr *binding;

	if (sdl == NULL) {
		return NULL;
	}

	for (zend_hash_internal_pointer_reset(sdl->bindings);
		zend_hash_get_current_data(sdl->bindings, (void **) &binding) == SUCCESS;
		zend_hash_move_forward(sdl->bindings)) {
		if ((*binding)->bindingType == type) {
			return *binding;
		}
	}
	return NULL;
}

sdlBindingPtr get_binding_from_name(sdlPtr sdl, char *name, char *ns)
{
	sdlBindingPtr binding = NULL;
	smart_str key = {0};

	smart_str_appends(&key, ns);
	smart_str_appendc(&key, ':');
	smart_str_appends(&key, name);
	smart_str_0(&key);

	zend_hash_find(sdl->bindings, key.c, key.len, (void **)&binding);

	smart_str_free(&key);
	return binding;
}

static int is_wsdl_element(xmlNodePtr node)
{
	if (node->ns && strcmp((char*)node->ns->href, WSDL_NAMESPACE) != 0) {
		xmlAttrPtr attr;
		if ((attr = get_attribute_ex(node->properties, "required", WSDL_NAMESPACE)) != NULL &&
		     attr->children && attr->children->content &&
		     (strcmp((char*)attr->children->content, "1") == 0 ||
		      strcmp((char*)attr->children->content, "true") == 0)) {
			soap_error1(E_ERROR, "Parsing WSDL: Unknown required WSDL extension '%s'", node->ns->href);
		}
		return 0;
	}
	return 1;
}

static void load_wsdl_ex(zval *this_ptr, char *struri, sdlCtx *ctx, int include TSRMLS_DC)
{
	sdlPtr tmpsdl = ctx->sdl;
	xmlDocPtr wsdl;
	xmlNodePtr root, definitions, trav;
	xmlAttrPtr targetNamespace;

	if (zend_hash_exists(&ctx->docs, struri, strlen(struri)+1)) {
		return;
	}
	
	wsdl = soap_xmlParseFile(struri TSRMLS_CC);
	
	if (!wsdl) {
		soap_error1(E_ERROR, "Parsing WSDL: Couldn't load from '%s'", struri);
	}

	zend_hash_add(&ctx->docs, struri, strlen(struri)+1, (void**)&wsdl, sizeof(xmlDocPtr), NULL);

	root = wsdl->children;
	definitions = get_node_ex(root, "definitions", WSDL_NAMESPACE);
	if (!definitions) {
		if (include) {
			xmlNodePtr schema = get_node_ex(root, "schema", XSD_NAMESPACE);
			if (schema) {
				load_schema(ctx, schema TSRMLS_CC);
				return;
			}
		}
		soap_error1(E_ERROR, "Parsing WSDL: Couldn't find <definitions> in '%s'", struri);
	}

	if (!include) {
		targetNamespace = get_attribute(definitions->properties, "targetNamespace");
		if (targetNamespace) {
			tmpsdl->target_ns = estrdup((char*)targetNamespace->children->content);
		}
	}

	trav = definitions->children;
	while (trav != NULL) {
		if (!is_wsdl_element(trav)) {
			trav = trav->next;
			continue;
		}
		if (node_is_equal(trav,"types")) {
			/* TODO: Only one "types" is allowed */
			xmlNodePtr trav2 = trav->children;

			while (trav2 != NULL) {
				if (node_is_equal_ex(trav2, "schema", XSD_NAMESPACE)) {
					load_schema(ctx, trav2 TSRMLS_CC);
				} else if (is_wsdl_element(trav2) && !node_is_equal(trav2,"documentation")) {
					soap_error1(E_ERROR, "Parsing WSDL: Unexpected WSDL element <%s>", trav2->name);
				}
				trav2 = trav2->next;
			}
		} else if (node_is_equal(trav,"import")) {
			/* TODO: namespace ??? */
			xmlAttrPtr tmp = get_attribute(trav->properties, "location");
			if (tmp) {
				xmlChar *uri;
				xmlChar *base = xmlNodeGetBase(trav->doc, trav);

				if (base == NULL) {
					uri = xmlBuildURI(tmp->children->content, trav->doc->URL);
				} else {
					uri = xmlBuildURI(tmp->children->content, base);
					xmlFree(base);
				}
				load_wsdl_ex(this_ptr, (char*)uri, ctx, 1 TSRMLS_CC);
				xmlFree(uri);
			}

		} else if (node_is_equal(trav,"message")) {
			xmlAttrPtr name = get_attribute(trav->properties, "name");
			if (name && name->children && name->children->content) {
				if (zend_hash_add(&ctx->messages, (char*)name->children->content, xmlStrlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL) != SUCCESS) {
					soap_error1(E_ERROR, "Parsing WSDL: <message> '%s' already defined", name->children->content);
				}
			} else {
				soap_error0(E_ERROR, "Parsing WSDL: <message> hasn't name attribute");
			}

		} else if (node_is_equal(trav,"portType")) {
			xmlAttrPtr name = get_attribute(trav->properties, "name");
			if (name && name->children && name->children->content) {
				if (zend_hash_add(&ctx->portTypes, (char*)name->children->content, xmlStrlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL) != SUCCESS) {
					soap_error1(E_ERROR, "Parsing WSDL: <portType> '%s' already defined", name->children->content);
				}
			} else {
				soap_error0(E_ERROR, "Parsing WSDL: <portType> hasn't name attribute");
			}

		} else if (node_is_equal(trav,"binding")) {
			xmlAttrPtr name = get_attribute(trav->properties, "name");
			if (name && name->children && name->children->content) {
				if (zend_hash_add(&ctx->bindings, (char*)name->children->content, xmlStrlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL) != SUCCESS) {
					soap_error1(E_ERROR, "Parsing WSDL: <binding> '%s' already defined", name->children->content);
				}
			} else {
				soap_error0(E_ERROR, "Parsing WSDL: <binding> hasn't name attribute");
			}

		} else if (node_is_equal(trav,"service")) {
			xmlAttrPtr name = get_attribute(trav->properties, "name");
			if (name && name->children && name->children->content) {
				if (zend_hash_add(&ctx->services, (char*)name->children->content, xmlStrlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL) != SUCCESS) {
					soap_error1(E_ERROR, "Parsing WSDL: <service> '%s' already defined", name->children->content);
				}
			} else {
				soap_error0(E_ERROR, "Parsing WSDL: <service> hasn't name attribute");
			}
		} else if (!node_is_equal(trav,"documentation")) {
			soap_error1(E_ERROR, "Parsing WSDL: Unexpected WSDL element <%s>", trav->name);
		}
		trav = trav->next;
	}
}

static sdlSoapBindingFunctionHeaderPtr wsdl_soap_binding_header(sdlCtx* ctx, xmlNodePtr header, char* wsdl_soap_namespace, int fault)
{
	xmlAttrPtr tmp;
	xmlNodePtr *message, part;
	char *ctype;
	sdlSoapBindingFunctionHeaderPtr h;

	tmp = get_attribute(header->properties, "message");
	if (!tmp) {
		soap_error0(E_ERROR, "Parsing WSDL: Missing message attribute for <header>");
	}

	ctype = strrchr((char*)tmp->children->content,':');
	if (ctype == NULL) {
		ctype = (char*)tmp->children->content;
	} else {
		++ctype;
	}
	if (zend_hash_find(&ctx->messages, ctype, strlen(ctype)+1, (void**)&message) != SUCCESS) {
		soap_error1(E_ERROR, "Parsing WSDL: Missing <message> with name '%s'", tmp->children->content);
	}

	tmp = get_attribute(header->properties, "part");
	if (!tmp) {
		soap_error0(E_ERROR, "Parsing WSDL: Missing part attribute for <header>");
	}
	part = get_node_with_attribute_ex((*message)->children, "part", WSDL_NAMESPACE, "name", (char*)tmp->children->content, NULL);
	if (!part) {
		soap_error1(E_ERROR, "Parsing WSDL: Missing part '%s' in <message>", tmp->children->content);
	}

	h = emalloc(sizeof(sdlSoapBindingFunctionHeader));
	memset(h, 0, sizeof(sdlSoapBindingFunctionHeader));
	h->name = estrdup((char*)tmp->children->content);

	tmp = get_attribute(header->properties, "use");
	if (tmp && !strncmp((char*)tmp->children->content, "encoded", sizeof("encoded"))) {
		h->use = SOAP_ENCODED;
	} else {
		h->use = SOAP_LITERAL;
	}

	tmp = get_attribute(header->properties, "namespace");
	if (tmp) {
		h->ns = estrdup((char*)tmp->children->content);
	}

	if (h->use == SOAP_ENCODED) {
		tmp = get_attribute(header->properties, "encodingStyle");
		if (tmp) {
			if (strncmp((char*)tmp->children->content, SOAP_1_1_ENC_NAMESPACE, sizeof(SOAP_1_1_ENC_NAMESPACE)) == 0) {
				h->encodingStyle = SOAP_ENCODING_1_1;
			} else if (strncmp((char*)tmp->children->content, SOAP_1_2_ENC_NAMESPACE, sizeof(SOAP_1_2_ENC_NAMESPACE)) == 0) {
				h->encodingStyle = SOAP_ENCODING_1_2;
			} else {
				soap_error1(E_ERROR, "Parsing WSDL: Unknown encodingStyle '%s'", tmp->children->content);
			}
		} else {
			soap_error0(E_ERROR, "Parsing WSDL: Unspecified encodingStyle");
		}
	}

	tmp = get_attribute(part->properties, "type");
	if (tmp != NULL) {
		h->encode = get_encoder_from_prefix(ctx->sdl, part, tmp->children->content);
	} else {
		tmp = get_attribute(part->properties, "element");
		if (tmp != NULL) {
			h->element = get_element(ctx->sdl, part, tmp->children->content);
			if (h->element) {
				h->encode = h->element->encode;
				if (!h->ns && h->element->namens) {
					h->ns = estrdup(h->element->namens);
				}
				if (h->element->name) {
					efree(h->name);
					h->name = estrdup(h->element->name);
				}
			}
		}
	}
	if (!fault) {
		xmlNodePtr trav = header->children;
		while (trav != NULL) {
			if (node_is_equal_ex(trav, "headerfault", wsdl_soap_namespace)) {
				sdlSoapBindingFunctionHeaderPtr hf = wsdl_soap_binding_header(ctx, trav, wsdl_soap_namespace, 1);
				smart_str key = {0};

				if (h->headerfaults == NULL) {
					h->headerfaults = emalloc(sizeof(HashTable));
					zend_hash_init(h->headerfaults, 0, NULL, delete_header, 0);
				}

				if (hf->ns) {
					smart_str_appends(&key,hf->ns);
					smart_str_appendc(&key,':');
				}
				smart_str_appends(&key,hf->name);
				smart_str_0(&key);
				if (zend_hash_add(h->headerfaults, key.c, key.len+1, (void**)&hf, sizeof(sdlSoapBindingFunctionHeaderPtr), NULL) != SUCCESS) {
					delete_header((void**)&hf);
				}
				smart_str_free(&key);
			} else if (is_wsdl_element(trav) && !node_is_equal(trav,"documentation")) {
				soap_error1(E_ERROR, "Parsing WSDL: Unexpected WSDL element <%s>", trav->name);
			}
			trav = trav->next;
		}
	}
	return h;
}

static void wsdl_soap_binding_body(sdlCtx* ctx, xmlNodePtr node, char* wsdl_soap_namespace, sdlSoapBindingFunctionBody *binding, HashTable* params)
{
	xmlNodePtr body, trav;
	xmlAttrPtr tmp;

	trav = node->children;
	while (trav != NULL) {
		if (node_is_equal_ex(trav, "body", wsdl_soap_namespace)) {
			body = trav;

			tmp = get_attribute(body->properties, "use");
			if (tmp && !strncmp((char*)tmp->children->content, "literal", sizeof("literal"))) {
				binding->use = SOAP_LITERAL;
			} else {
				binding->use = SOAP_ENCODED;
			}

			tmp = get_attribute(body->properties, "namespace");
			if (tmp) {
				binding->ns = estrdup((char*)tmp->children->content);
			}

			tmp = get_attribute(body->properties, "parts");
			if (tmp) {
				HashTable    ht;
				char *parts = (char*)tmp->children->content;

				/* Delete all parts those are not in the "parts" attribute */
				zend_hash_init(&ht, 0, NULL, delete_parameter, 0);
				while (*parts) {
					HashPosition pos;
					sdlParamPtr *param;
					int found = 0;
					char *end;

					while (*parts == ' ') ++parts;
					if (*parts == '\0') break;
					end = strchr(parts, ' ');
					if (end) *end = '\0';
					zend_hash_internal_pointer_reset_ex(params, &pos);
					while (zend_hash_get_current_data_ex(params, (void **)&param, &pos) != FAILURE) {
						if ((*param)->paramName &&
						    strcmp(parts, (*param)->paramName) == 0) {
					  	sdlParamPtr x_param;
					  	x_param = emalloc(sizeof(sdlParam));
					  	*x_param = **param;
					  	(*param)->paramName = NULL;
					  	zend_hash_next_index_insert(&ht, &x_param, sizeof(sdlParamPtr), NULL);
					  	found = 1;
					  	break;
						}
						zend_hash_move_forward_ex(params, &pos);
					}
					if (!found) {
						soap_error1(E_ERROR, "Parsing WSDL: Missing part '%s' in <message>", parts);
					}
					parts += strlen(parts);
					if (end) *end = ' ';
				}
				zend_hash_destroy(params);
				*params = ht;
			}

			if (binding->use == SOAP_ENCODED) {
				tmp = get_attribute(body->properties, "encodingStyle");
				if (tmp) {
					if (strncmp((char*)tmp->children->content, SOAP_1_1_ENC_NAMESPACE, sizeof(SOAP_1_1_ENC_NAMESPACE)) == 0) {
						binding->encodingStyle = SOAP_ENCODING_1_1;
					} else if (strncmp((char*)tmp->children->content, SOAP_1_2_ENC_NAMESPACE, sizeof(SOAP_1_2_ENC_NAMESPACE)) == 0) {
						binding->encodingStyle = SOAP_ENCODING_1_2;
					} else {
						soap_error1(E_ERROR, "Parsing WSDL: Unknown encodingStyle '%s'", tmp->children->content);
					}
				} else {
					soap_error0(E_ERROR, "Parsing WSDL: Unspecified encodingStyle");
				}
			}
		} else if (node_is_equal_ex(trav, "header", wsdl_soap_namespace)) {
			sdlSoapBindingFunctionHeaderPtr h = wsdl_soap_binding_header(ctx, trav, wsdl_soap_namespace, 0);
			smart_str key = {0};

			if (binding->headers == NULL) {
				binding->headers = emalloc(sizeof(HashTable));
				zend_hash_init(binding->headers, 0, NULL, delete_header, 0);
			}

			if (h->ns) {
				smart_str_appends(&key,h->ns);
				smart_str_appendc(&key,':');
			}
			smart_str_appends(&key,h->name);
			smart_str_0(&key);
			if (zend_hash_add(binding->headers, key.c, key.len+1, (void**)&h, sizeof(sdlSoapBindingFunctionHeaderPtr), NULL) != SUCCESS) {
				delete_header((void**)&h);
			}
			smart_str_free(&key);
		} else if (is_wsdl_element(trav) && !node_is_equal(trav,"documentation")) {
			soap_error1(E_ERROR, "Parsing WSDL: Unexpected WSDL element <%s>", trav->name);
		}
		trav = trav->next;
	}
}

static HashTable* wsdl_message(sdlCtx *ctx, xmlChar* message_name)
{
	xmlNodePtr trav, part, message = NULL, *tmp;
	HashTable* parameters = NULL;
	char *ctype;

	ctype = strrchr((char*)message_name,':');
	if (ctype == NULL) {
		ctype = (char*)message_name;
	} else {
		++ctype;
	}
	if (zend_hash_find(&ctx->messages, ctype, strlen(ctype)+1, (void**)&tmp) != SUCCESS) {
		soap_error1(E_ERROR, "Parsing WSDL: Missing <message> with name '%s'", message_name);
	}
	message = *tmp;

	parameters = emalloc(sizeof(HashTable));
	zend_hash_init(parameters, 0, NULL, delete_parameter, 0);

	trav = message->children;
	while (trav != NULL) {
		xmlAttrPtr element, type, name;
		sdlParamPtr param;

		if (trav->ns != NULL && strcmp((char*)trav->ns->href, WSDL_NAMESPACE) != 0) {
			soap_error1(E_ERROR, "Parsing WSDL: Unexpected extensibility element <%s>", trav->name);
		}
		if (node_is_equal(trav,"documentation")) {
			trav = trav->next;
			continue;
		}
		if (!node_is_equal(trav,"part")) {
			soap_error1(E_ERROR, "Parsing WSDL: Unexpected WSDL element <%s>", trav->name);
		}
		part = trav;
		param = emalloc(sizeof(sdlParam));
		memset(param,0,sizeof(sdlParam));
		param->order = 0;

		name = get_attribute(part->properties, "name");
		if (name == NULL) {
			soap_error1(E_ERROR, "Parsing WSDL: No name associated with <part> '%s'", message->name);
		}

		param->paramName = estrdup((char*)name->children->content);

		type = get_attribute(part->properties, "type");
		if (type != NULL) {
			param->encode = get_encoder_from_prefix(ctx->sdl, part, type->children->content);
		} else {
			element = get_attribute(part->properties, "element");
			if (element != NULL) {
				param->element = get_element(ctx->sdl, part, element->children->content);
				if (param->element) {
					param->encode = param->element->encode;
				}
			}
		}

		zend_hash_next_index_insert(parameters, &param, sizeof(sdlParamPtr), NULL);

		trav = trav->next;
	}
	return parameters;
}

static sdlPtr load_wsdl(zval *this_ptr, char *struri TSRMLS_DC)
{
	sdlCtx ctx;
	int i,n;

	memset(&ctx,0,sizeof(ctx));
	ctx.sdl = emalloc(sizeof(sdl));
	memset(ctx.sdl, 0, sizeof(sdl));
	ctx.sdl->source = estrdup(struri);
	zend_hash_init(&ctx.sdl->functions, 0, NULL, delete_function, 0);

	zend_hash_init(&ctx.docs, 0, NULL, delete_document, 0);
	zend_hash_init(&ctx.messages, 0, NULL, NULL, 0);
	zend_hash_init(&ctx.bindings, 0, NULL, NULL, 0);
	zend_hash_init(&ctx.portTypes, 0, NULL, NULL, 0);
	zend_hash_init(&ctx.services,  0, NULL, NULL, 0);

	load_wsdl_ex(this_ptr, struri,&ctx, 0 TSRMLS_CC);
	schema_pass2(&ctx);

	n = zend_hash_num_elements(&ctx.services);
	if (n > 0) {
		zend_hash_internal_pointer_reset(&ctx.services);
		for (i = 0; i < n; i++) {
			xmlNodePtr *tmp, service;
			xmlNodePtr trav, port;

			zend_hash_get_current_data(&ctx.services, (void **)&tmp);
			service = *tmp;

			trav = service->children;
			while (trav != NULL) {
				xmlAttrPtr type, name, bindingAttr, location;
				xmlNodePtr portType, operation;
				xmlNodePtr address, binding, trav2;
				char *ctype;
				sdlBindingPtr tmpbinding;
				char *wsdl_soap_namespace = NULL;

				if (!is_wsdl_element(trav) || node_is_equal(trav,"documentation")) {
					trav = trav->next;
					continue;
				}
				if (!node_is_equal(trav,"port")) {
					soap_error1(E_ERROR, "Parsing WSDL: Unexpected WSDL element <%s>", trav->name);
				}

				port = trav;

				tmpbinding = emalloc(sizeof(sdlBinding));
				memset(tmpbinding, 0, sizeof(sdlBinding));

				bindingAttr = get_attribute(port->properties, "binding");
				if (bindingAttr == NULL) {
					soap_error0(E_ERROR, "Parsing WSDL: No binding associated with <port>");
				}

				/* find address and figure out binding type */
				address = NULL;
				trav2 = port->children;
				while (trav2 != NULL) {
					if (node_is_equal(trav2,"address") && trav2->ns) {
						if (!strncmp((char*)trav2->ns->href, WSDL_SOAP11_NAMESPACE, sizeof(WSDL_SOAP11_NAMESPACE))) {
							address = trav2;
							wsdl_soap_namespace = WSDL_SOAP11_NAMESPACE;
							tmpbinding->bindingType = BINDING_SOAP;
						} else if (!strncmp((char*)trav2->ns->href, WSDL_SOAP12_NAMESPACE, sizeof(WSDL_SOAP12_NAMESPACE))) {
							address = trav2;
							wsdl_soap_namespace = WSDL_SOAP12_NAMESPACE;
							tmpbinding->bindingType = BINDING_SOAP;
						} else if (!strncmp((char*)trav2->ns->href, RPC_SOAP12_NAMESPACE, sizeof(RPC_SOAP12_NAMESPACE))) {
							address = trav2;
							wsdl_soap_namespace = RPC_SOAP12_NAMESPACE;
							tmpbinding->bindingType = BINDING_SOAP;
						} else if (!strncmp((char*)trav2->ns->href, WSDL_HTTP11_NAMESPACE, sizeof(WSDL_HTTP11_NAMESPACE))) {
							address = trav2;
							tmpbinding->bindingType = BINDING_HTTP;
						} else if (!strncmp((char*)trav2->ns->href, WSDL_HTTP12_NAMESPACE, sizeof(WSDL_HTTP12_NAMESPACE))) {
							address = trav2;
							tmpbinding->bindingType = BINDING_HTTP;
						}
					}
					if (trav2 != address && is_wsdl_element(trav2) && !node_is_equal(trav2,"documentation")) {
						soap_error1(E_ERROR, "Parsing WSDL: Unexpected WSDL element <%s>", trav2->name);
					}
				  trav2 = trav2->next;
				}
				if (!address) {
					soap_error0(E_ERROR, "Parsing WSDL: No address associated with <port>");
				}

				location = get_attribute(address->properties, "location");
				if (!location) {
					soap_error0(E_ERROR, "Parsing WSDL: No location associated with <port>");
				}

				tmpbinding->location = estrdup((char*)location->children->content);

				ctype = strrchr((char*)bindingAttr->children->content,':');
				if (ctype == NULL) {
					ctype = (char*)bindingAttr->children->content;
				} else {
					++ctype;
				}
				if (zend_hash_find(&ctx.bindings, ctype, strlen(ctype)+1, (void*)&tmp) != SUCCESS) {
					soap_error1(E_ERROR, "Parsing WSDL: No <binding> element with name '%s'", ctype);
				}
				binding = *tmp;

				if (tmpbinding->bindingType == BINDING_SOAP) {
					sdlSoapBindingPtr soapBinding;
					xmlNodePtr soapBindingNode;
					xmlAttrPtr tmp;

					soapBinding = emalloc(sizeof(sdlSoapBinding));
					memset(soapBinding, 0, sizeof(sdlSoapBinding));
					soapBinding->style = SOAP_DOCUMENT;

					soapBindingNode = get_node_ex(binding->children, "binding", wsdl_soap_namespace);
					if (soapBindingNode) {
						tmp = get_attribute(soapBindingNode->properties, "style");
						if (tmp && !strncmp((char*)tmp->children->content, "rpc", sizeof("rpc"))) {
							soapBinding->style = SOAP_RPC;
						}

						tmp = get_attribute(soapBindingNode->properties, "transport");
						if (tmp) {
							if (strncmp((char*)tmp->children->content, WSDL_HTTP_TRANSPORT, sizeof(WSDL_HTTP_TRANSPORT)) == 0) {
								soapBinding->transport = SOAP_TRANSPORT_HTTP;
							} else {
								soap_error1(E_ERROR, "Parsing WSDL: PHP-SOAP doesn't support transport '%s'", tmp->children->content);
							}
						}
					}
					tmpbinding->bindingAttributes = (void *)soapBinding;
				}

				name = get_attribute(binding->properties, "name");
				if (name == NULL) {
					soap_error0(E_ERROR, "Parsing WSDL: Missing 'name' attribute for <binding>");
				}
				tmpbinding->name = estrdup((char*)name->children->content);

				type = get_attribute(binding->properties, "type");
				if (type == NULL) {
					soap_error0(E_ERROR, "Parsing WSDL: Missing 'type' attribute for <binding>");
				}

				ctype = strrchr((char*)type->children->content,':');
				if (ctype == NULL) {
					ctype = (char*)type->children->content;
				} else {
					++ctype;
				}
				if (zend_hash_find(&ctx.portTypes, ctype, strlen(ctype)+1, (void**)&tmp) != SUCCESS) {
					soap_error1(E_ERROR, "Parsing WSDL: Missing <portType> with name '%s'", name->children->content);
				}
				portType = *tmp;

				trav2 = binding->children;
				while (trav2 != NULL) {
					sdlFunctionPtr function;
					xmlNodePtr input, output, fault, portTypeOperation, trav3;
					xmlAttrPtr op_name, paramOrder;

					if ((tmpbinding->bindingType == BINDING_SOAP &&
					    node_is_equal_ex(trav2, "binding", wsdl_soap_namespace)) ||
					    !is_wsdl_element(trav2) ||
					    node_is_equal(trav2,"documentation")) {
						trav2 = trav2->next;
						continue;
					}
					if (!node_is_equal(trav2,"operation")) {
						soap_error1(E_ERROR, "Parsing WSDL: Unexpected WSDL element <%s>", trav2->name);
					}

					operation = trav2;

					op_name = get_attribute(operation->properties, "name");
					if (op_name == NULL) {
						soap_error0(E_ERROR, "Parsing WSDL: Missing 'name' attribute for <operation>");
					}

					trav3 = operation->children;
					while  (trav3 != NULL) {
						if (tmpbinding->bindingType == BINDING_SOAP &&
						    node_is_equal_ex(trav3, "operation", wsdl_soap_namespace)) {
						} else if (is_wsdl_element(trav3) &&
						           !node_is_equal(trav3,"input") &&
						           !node_is_equal(trav3,"output") &&
						           !node_is_equal(trav3,"fault") &&
						           !node_is_equal(trav3,"documentation")) {
							soap_error1(E_ERROR, "Parsing WSDL: Unexpected WSDL element <%s>", trav3->name);
						}
						trav3 = trav3->next;
					}

					portTypeOperation = get_node_with_attribute_ex(portType->children, "operation", WSDL_NAMESPACE, "name", (char*)op_name->children->content, NULL);
					if (portTypeOperation == NULL) {
						soap_error1(E_ERROR, "Parsing WSDL: Missing <portType>/<operation> with name '%s'", op_name->children->content);
					}

					function = emalloc(sizeof(sdlFunction));
					memset(function, 0, sizeof(sdlFunction));
					function->functionName = estrdup((char*)op_name->children->content);

					if (tmpbinding->bindingType == BINDING_SOAP) {
						sdlSoapBindingFunctionPtr soapFunctionBinding;
						sdlSoapBindingPtr soapBinding;
						xmlNodePtr soapOperation;
						xmlAttrPtr tmp;

						soapFunctionBinding = emalloc(sizeof(sdlSoapBindingFunction));
						memset(soapFunctionBinding, 0, sizeof(sdlSoapBindingFunction));
						soapBinding = (sdlSoapBindingPtr)tmpbinding->bindingAttributes;
						soapFunctionBinding->style = soapBinding->style;

						soapOperation = get_node_ex(operation->children, "operation", wsdl_soap_namespace);
						if (soapOperation) {
							tmp = get_attribute(soapOperation->properties, "soapAction");
							if (tmp) {
								soapFunctionBinding->soapAction = estrdup((char*)tmp->children->content);
							}

							tmp = get_attribute(soapOperation->properties, "style");
							if (tmp) {
								if (!strncmp((char*)tmp->children->content, "rpc", sizeof("rpc"))) {
									soapFunctionBinding->style = SOAP_RPC;
								} else {
									soapFunctionBinding->style = SOAP_DOCUMENT;
								}
							} else {
								soapFunctionBinding->style = soapBinding->style;
							}
						}

						function->bindingAttributes = (void *)soapFunctionBinding;
					}

					input = get_node_ex(portTypeOperation->children, "input", WSDL_NAMESPACE);
					if (input != NULL) {
						xmlAttrPtr message, name;

						message = get_attribute(input->properties, "message");
						if (message == NULL) {
							soap_error1(E_ERROR, "Parsing WSDL: Missing name for <input> of '%s'", op_name->children->content);
						}
						function->requestParameters = wsdl_message(&ctx, message->children->content);

						name = get_attribute(input->properties, "name");
/* FIXME
						if (name != NULL) {
							function->requestName = estrdup(name->children->content);
						} else {
*/
						{
							function->requestName = estrdup(function->functionName);
						}

						if (tmpbinding->bindingType == BINDING_SOAP) {
							input = get_node_ex(operation->children, "input", WSDL_NAMESPACE);
							if (input != NULL) {
								sdlSoapBindingFunctionPtr soapFunctionBinding = function->bindingAttributes;
								wsdl_soap_binding_body(&ctx, input, wsdl_soap_namespace, &soapFunctionBinding->input, function->requestParameters);
							}
						}
					}

					output = get_node_ex(portTypeOperation->children, "output", WSDL_NAMESPACE);
					if (output != NULL) {
						xmlAttrPtr message, name;

						message = get_attribute(output->properties, "message");
						if (message == NULL) {
							soap_error1(E_ERROR, "Parsing WSDL: Missing name for <output> of '%s'", op_name->children->content);
						}
						function->responseParameters = wsdl_message(&ctx, message->children->content);

						name = get_attribute(output->properties, "name");
/* FIXME
						if (name != NULL) {
							function->responseName = estrdup(name->children->content);
						} else if (input == NULL) {
							function->responseName = estrdup(function->functionName);
						} else {
*/
						{
							int len = strlen(function->functionName);
							function->responseName = emalloc(len + sizeof("Response"));
							memcpy(function->responseName, function->functionName, len);
							memcpy(function->responseName+len, "Response", sizeof("Response"));
						}

						if (tmpbinding->bindingType == BINDING_SOAP) {
							output = get_node_ex(operation->children, "output", WSDL_NAMESPACE);
							if (output != NULL) {
								sdlSoapBindingFunctionPtr soapFunctionBinding = function->bindingAttributes;
								wsdl_soap_binding_body(&ctx, output, wsdl_soap_namespace, &soapFunctionBinding->output, function->responseParameters);
							}
						}
					}

					paramOrder = get_attribute(portTypeOperation->properties, "parameterOrder");
					if (paramOrder) {
						/* FIXME: */
					}

					fault = portTypeOperation->children;
					while (fault != NULL) {
						if (node_is_equal_ex(fault, "fault", WSDL_NAMESPACE)) {
							xmlAttrPtr message, name;
							sdlFaultPtr f;

							name = get_attribute(fault->properties, "name");
							if (name == NULL) {
								soap_error1(E_ERROR, "Parsing WSDL: Missing name for <fault> of '%s'", op_name->children->content);
							}
							message = get_attribute(fault->properties, "message");
							if (message == NULL) {
								soap_error1(E_ERROR, "Parsing WSDL: Missing name for <output> of '%s'", op_name->children->content);
							}

							f = emalloc(sizeof(sdlFault));
							memset(f, 0, sizeof(sdlFault));

							f->name = estrdup((char*)name->children->content);
							f->details = wsdl_message(&ctx, message->children->content);
							if (f->details == NULL || zend_hash_num_elements(f->details) > 1) {
								soap_error1(E_ERROR, "Parsing WSDL: The fault message '%s' must have a single part", message->children->content);
							}

							if (tmpbinding->bindingType == BINDING_SOAP) {
								xmlNodePtr soap_fault = get_node_with_attribute_ex(operation->children, "fault", WSDL_NAMESPACE, "name", f->name, NULL);
								if (soap_fault != NULL) {
									xmlNodePtr trav = soap_fault->children;
									while (trav != NULL) {
										if (node_is_equal_ex(trav, "fault", wsdl_soap_namespace)) {
											xmlAttrPtr tmp;
										  sdlSoapBindingFunctionFaultPtr binding;

											binding = f->bindingAttributes = emalloc(sizeof(sdlSoapBindingFunctionFault));
											memset(f->bindingAttributes, 0, sizeof(sdlSoapBindingFunctionFault));

											tmp = get_attribute(trav->properties, "use");
											if (tmp && !strncmp((char*)tmp->children->content, "encoded", sizeof("encoded"))) {
												binding->use = SOAP_ENCODED;
											} else {
												binding->use = SOAP_LITERAL;
											}

											tmp = get_attribute(trav->properties, "namespace");
											if (tmp) {
												binding->ns = estrdup((char*)tmp->children->content);
											}

											if (binding->use == SOAP_ENCODED) {
												tmp = get_attribute(trav->properties, "encodingStyle");
												if (tmp) {
													if (strncmp((char*)tmp->children->content, SOAP_1_1_ENC_NAMESPACE, sizeof(SOAP_1_1_ENC_NAMESPACE)) == 0) {
														binding->encodingStyle = SOAP_ENCODING_1_1;
													} else if (strncmp((char*)tmp->children->content, SOAP_1_2_ENC_NAMESPACE, sizeof(SOAP_1_2_ENC_NAMESPACE)) == 0) {
														binding->encodingStyle = SOAP_ENCODING_1_2;
													} else {
														soap_error1(E_ERROR, "Parsing WSDL: Unknown encodingStyle '%s'", tmp->children->content);
													}
												} else {
													soap_error0(E_ERROR, "Parsing WSDL: Unspecified encodingStyle");
												}
											}
										} else if (is_wsdl_element(trav) && !node_is_equal(trav,"documentation")) {
											soap_error1(E_ERROR, "Parsing WSDL: Unexpected WSDL element <%s>", trav->name);
										}
										trav = trav->next;
									}
								}
							}
							if (function->faults == NULL) {
								function->faults = emalloc(sizeof(HashTable));
								zend_hash_init(function->faults, 0, NULL, delete_fault, 0);
							}
							if (zend_hash_add(function->faults, f->name, strlen(f->name)+1, (void**)&f, sizeof(sdlFaultPtr), NULL) != SUCCESS) {
								soap_error2(E_ERROR, "Parsing WSDL: <fault> with name '%s' already defined in '%s'", f->name, op_name->children->content);
							}
						}
						fault = fault->next;
					}

					function->binding = tmpbinding;

					{
						char *tmp = estrdup(function->functionName);
						int  len = strlen(tmp);

						if (zend_hash_add(&ctx.sdl->functions, php_strtolower(tmp, len), len+1, &function, sizeof(sdlFunctionPtr), NULL) != SUCCESS) {
							zend_hash_next_index_insert(&ctx.sdl->functions, &function, sizeof(sdlFunctionPtr), NULL);
						}
						efree(tmp);
						if (function->requestName != NULL && strcmp(function->requestName,function->functionName) != 0) {
							if (ctx.sdl->requests == NULL) {
								ctx.sdl->requests = emalloc(sizeof(HashTable));
								zend_hash_init(ctx.sdl->requests, 0, NULL, NULL, 0);
							}
							tmp = estrdup(function->requestName);
							len = strlen(tmp);
							zend_hash_add(ctx.sdl->requests, php_strtolower(tmp, len), len+1, &function, sizeof(sdlFunctionPtr), NULL);
							efree(tmp);
						}
					}
					trav2 = trav2->next;
				}

				if (!ctx.sdl->bindings) {
					ctx.sdl->bindings = emalloc(sizeof(HashTable));
					zend_hash_init(ctx.sdl->bindings, 0, NULL, delete_binding, 0);
				}

				zend_hash_add(ctx.sdl->bindings, tmpbinding->name, strlen(tmpbinding->name), &tmpbinding, sizeof(sdlBindingPtr), NULL);
				trav= trav->next;
			}

			zend_hash_move_forward(&ctx.services);
		}
	} else {
		soap_error0(E_ERROR, "Parsing WSDL: Couldn't bind to service");
	}

	zend_hash_destroy(&ctx.messages);
	zend_hash_destroy(&ctx.bindings);
	zend_hash_destroy(&ctx.portTypes);
	zend_hash_destroy(&ctx.services);
	zend_hash_destroy(&ctx.docs);

	return ctx.sdl;
}

#define WSDL_CACHE_VERSION 0x0e

#define WSDL_CACHE_GET(ret,type,buf)   memcpy(&ret,*buf,sizeof(type)); *buf += sizeof(type);
#define WSDL_CACHE_GET_INT(ret,buf)    ret = ((unsigned char)(*buf)[0])|((unsigned char)(*buf)[1]<<8)|((unsigned char)(*buf)[2]<<16)|((int)(*buf)[3]<<24); *buf += 4;
#define WSDL_CACHE_GET_1(ret,type,buf) ret = (type)(**buf); (*buf)++;
#define WSDL_CACHE_GET_N(ret,n,buf)    memcpy(ret,*buf,n); *buf += n;
#define WSDL_CACHE_SKIP(n,buf)         *buf += n;

#define WSDL_CACHE_PUT_INT(val,buf)    smart_str_appendc(buf,val & 0xff); \
                                       smart_str_appendc(buf,(val >> 8) & 0xff); \
                                       smart_str_appendc(buf,(val >> 16) & 0xff); \
                                       smart_str_appendc(buf,(val >> 24) & 0xff);
#define WSDL_CACHE_PUT_1(val,buf)      smart_str_appendc(buf,val);
#define WSDL_CACHE_PUT_N(val,n,buf)    smart_str_appendl(buf,(char*)val,n);

static char* sdl_deserialize_string(char **in)
{
	char *s;
	int len;

	WSDL_CACHE_GET_INT(len, in);
	if (len == 0x7fffffff) {
		return NULL;
	} else {
		s = emalloc(len+1);
		WSDL_CACHE_GET_N(s, len, in);
		s[len] = '\0';
		return s;
	}
}

static void sdl_deserialize_key(HashTable* ht, void* data, char **in)
{
	int len;

	WSDL_CACHE_GET_INT(len, in);
	if (len == 0) {
		zend_hash_next_index_insert(ht, &data, sizeof(void*), NULL);
	} else {
		zend_hash_add(ht, *in, len, &data, sizeof(void*), NULL);
		WSDL_CACHE_SKIP(len, in);
	}
}

static void sdl_deserialize_attribute(sdlAttributePtr attr, encodePtr *encoders, char **in)
{
	int i;

	attr->name = sdl_deserialize_string(in);
	attr->namens = sdl_deserialize_string(in);
	attr->ref = sdl_deserialize_string(in);
	attr->def = sdl_deserialize_string(in);
	attr->fixed = sdl_deserialize_string(in);
	WSDL_CACHE_GET_1(attr->form, sdlForm, in);
	WSDL_CACHE_GET_1(attr->use, sdlUse, in);
	WSDL_CACHE_GET_INT(i, in);
	attr->encode = encoders[i];
	WSDL_CACHE_GET_INT(i, in);
	if (i > 0) {
		attr->extraAttributes = emalloc(sizeof(HashTable));
		zend_hash_init(attr->extraAttributes, i, NULL, delete_extra_attribute, 0);
		while (i > 0) {
			sdlExtraAttributePtr x = emalloc(sizeof(sdlExtraAttribute));
			sdl_deserialize_key(attr->extraAttributes, x, in);
			x->ns = sdl_deserialize_string(in);
			x->val = sdl_deserialize_string(in);
			--i;
		}
	}
}

static sdlRestrictionIntPtr sdl_deserialize_resriction_int(char **in)
{
	if (**in == 1) {
		sdlRestrictionIntPtr x = emalloc(sizeof(sdlRestrictionInt));
		WSDL_CACHE_SKIP(1, in);
		WSDL_CACHE_GET_INT(x->value, in);
		WSDL_CACHE_GET_1(x->fixed, char, in);
		return x;
	} else {
		WSDL_CACHE_SKIP(1, in);
		return NULL;
	}
}

static sdlRestrictionCharPtr sdl_deserialize_resriction_char(char **in)
{
	if (**in == 1) {
		sdlRestrictionCharPtr x = emalloc(sizeof(sdlRestrictionChar));
		WSDL_CACHE_SKIP(1, in);
		x->value = sdl_deserialize_string(in);
		WSDL_CACHE_GET_1(x->fixed, char, in);
		return x;
	} else {
		WSDL_CACHE_SKIP(1, in);
		return NULL;
	}
}

static sdlContentModelPtr sdl_deserialize_model(sdlTypePtr *types, sdlTypePtr *elements, char **in)
{
	int i;
	sdlContentModelPtr model = emalloc(sizeof(sdlContentModel));

	WSDL_CACHE_GET_1(model->kind, sdlContentKind, in);
	WSDL_CACHE_GET_INT(model->min_occurs, in);
	WSDL_CACHE_GET_INT(model->max_occurs, in);
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT:
			WSDL_CACHE_GET_INT(i, in);
			model->u.element = elements[i];
			break;
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_ALL:
		case XSD_CONTENT_CHOICE:
			WSDL_CACHE_GET_INT(i, in);
			model->u.content = emalloc(sizeof(HashTable));
			zend_hash_init(model->u.content, i, NULL, delete_model, 0);
			while (i > 0) {
				sdlContentModelPtr x = sdl_deserialize_model(types, elements, in);
				zend_hash_next_index_insert(model->u.content,&x,sizeof(sdlContentModelPtr),NULL);
				i--;
			}
			break;
		case XSD_CONTENT_GROUP_REF:
			model->u.group_ref = sdl_deserialize_string(in);
			break;
		case XSD_CONTENT_GROUP:
			WSDL_CACHE_GET_INT(i, in);
			model->u.group = types[i];
			break;
		default:
			break;
	}
	return model;
}

static void sdl_deserialize_type(sdlTypePtr type, sdlTypePtr *types, encodePtr *encoders, char **in)
{
	int i;
	sdlTypePtr *elements = NULL;

	WSDL_CACHE_GET_1(type->kind, sdlTypeKind, in);
	type->name = sdl_deserialize_string(in);
	type->namens = sdl_deserialize_string(in);
	type->def = sdl_deserialize_string(in);
	type->fixed = sdl_deserialize_string(in);
	type->ref = sdl_deserialize_string(in);
	WSDL_CACHE_GET_1(type->nillable, char, in);
	WSDL_CACHE_GET_1(type->form, sdlForm, in);

	WSDL_CACHE_GET_INT(i, in);
	type->encode = encoders[i];

	if (**in == 1) {
		WSDL_CACHE_SKIP(1, in);
		type->restrictions = emalloc(sizeof(sdlRestrictions));
		/*memset(type->restrictions, 0, sizeof(sdlRestrictions));*/
		type->restrictions->minExclusive = sdl_deserialize_resriction_int(in);
		type->restrictions->minInclusive = sdl_deserialize_resriction_int(in);
		type->restrictions->maxExclusive = sdl_deserialize_resriction_int(in);
		type->restrictions->maxInclusive = sdl_deserialize_resriction_int(in);
		type->restrictions->totalDigits = sdl_deserialize_resriction_int(in);
		type->restrictions->fractionDigits = sdl_deserialize_resriction_int(in);
		type->restrictions->length = sdl_deserialize_resriction_int(in);
		type->restrictions->minLength = sdl_deserialize_resriction_int(in);
		type->restrictions->maxLength = sdl_deserialize_resriction_int(in);
		type->restrictions->whiteSpace = sdl_deserialize_resriction_char(in);
		type->restrictions->pattern = sdl_deserialize_resriction_char(in);
		WSDL_CACHE_GET_INT(i, in);
		if (i > 0) {
			type->restrictions->enumeration = emalloc(sizeof(HashTable));
			zend_hash_init(type->restrictions->enumeration, i, NULL, delete_restriction_var_char, 0);
			while (i > 0) {
				sdlRestrictionCharPtr x = sdl_deserialize_resriction_char(in);
				sdl_deserialize_key(type->restrictions->enumeration, x, in);
				--i;
			}
		} else {
			type->restrictions->enumeration = NULL;
		}
	} else {
		WSDL_CACHE_SKIP(1, in);
	}

	WSDL_CACHE_GET_INT(i, in);
	if (i > 0) {
		elements = emalloc((i+1) * sizeof(sdlTypePtr));
		elements[0] = NULL;
		type->elements = emalloc(sizeof(HashTable));
		zend_hash_init(type->elements, i, NULL, delete_type, 0);
		while (i > 0) {
			sdlTypePtr t = emalloc(sizeof(sdlType));
			memset(t, 0, sizeof(sdlType));
			sdl_deserialize_key(type->elements, t, in);
			sdl_deserialize_type(t, types, encoders, in);
			elements[i] = t;
			--i;
		}
	}

	WSDL_CACHE_GET_INT(i, in);
	if (i > 0) {
		type->attributes = emalloc(sizeof(HashTable));
		zend_hash_init(type->attributes, i, NULL, delete_attribute, 0);
		while (i > 0) {
			sdlAttributePtr attr = emalloc(sizeof(sdlAttribute));
			memset(attr, 0, sizeof(sdlAttribute));
			sdl_deserialize_key(type->attributes, attr, in);
			sdl_deserialize_attribute(attr, encoders, in);
			--i;
		}
	}

	if (**in != 0) {
		WSDL_CACHE_SKIP(1, in);
		type->model = sdl_deserialize_model(types, elements, in);
	} else {
		WSDL_CACHE_SKIP(1, in);
	}
	if (elements != NULL) {
		efree(elements);
	}
}

static void sdl_deserialize_encoder(encodePtr enc, sdlTypePtr *types, char **in)
{
	int i;

	WSDL_CACHE_GET_INT(enc->details.type, in);
	enc->details.type_str = sdl_deserialize_string(in);
	enc->details.ns = sdl_deserialize_string(in);
	WSDL_CACHE_GET_INT(i, in);
	enc->details.sdl_type = types[i];
	enc->to_xml = sdl_guess_convert_xml;
	enc->to_zval = sdl_guess_convert_zval;

	if (enc->details.sdl_type == NULL) {
		int ns_len = strlen(enc->details.ns);
		int type_len = strlen(enc->details.type_str);

		if (((ns_len == sizeof(SOAP_1_1_ENC_NAMESPACE)-1 &&
		      memcmp(enc->details.ns, SOAP_1_1_ENC_NAMESPACE, sizeof(SOAP_1_1_ENC_NAMESPACE)-1) == 0) ||
		     (ns_len == sizeof(SOAP_1_2_ENC_NAMESPACE)-1 &&
		      memcmp(enc->details.ns, SOAP_1_2_ENC_NAMESPACE, sizeof(SOAP_1_2_ENC_NAMESPACE)-1) == 0))) {
			char *enc_nscat;
			int enc_ns_len;
			int enc_len;
			encodePtr real_enc;

			enc_ns_len = sizeof(XSD_NAMESPACE)-1;
			enc_len = enc_ns_len + type_len + 1;
			enc_nscat = emalloc(enc_len + 1);
			memcpy(enc_nscat, XSD_NAMESPACE, sizeof(XSD_NAMESPACE)-1);
			enc_nscat[enc_ns_len] = ':';
			memcpy(enc_nscat+enc_ns_len+1, enc->details.type_str, type_len);
			enc_nscat[enc_len] = '\0';

			real_enc = get_encoder_ex(NULL, enc_nscat, enc_len);
			efree(enc_nscat);
			if (real_enc) {
				enc->to_zval = real_enc->to_zval;
				enc->to_xml = real_enc->to_xml;
			}
		}
	}	
}

static void sdl_deserialize_soap_body(sdlSoapBindingFunctionBodyPtr body, encodePtr *encoders, sdlTypePtr *types, char **in)
{
	int i, j, n;

	WSDL_CACHE_GET_1(body->use, sdlEncodingUse, in);
	if (body->use == SOAP_ENCODED) {
		WSDL_CACHE_GET_1(body->encodingStyle, sdlRpcEncodingStyle, in);
	} else {
		body->encodingStyle = SOAP_ENCODING_DEFAULT;
	}
	body->ns = sdl_deserialize_string(in);
	WSDL_CACHE_GET_INT(i, in);
	if (i > 0) {
		body->headers = emalloc(sizeof(HashTable));
		zend_hash_init(body->headers, i, NULL, delete_header, 0);
		while (i > 0) {
			sdlSoapBindingFunctionHeaderPtr tmp = emalloc(sizeof(sdlSoapBindingFunctionHeader));
			memset(tmp, 0, sizeof(sdlSoapBindingFunctionHeader));
			sdl_deserialize_key(body->headers, tmp, in);
			WSDL_CACHE_GET_1(tmp->use, sdlEncodingUse, in);
			if (tmp->use == SOAP_ENCODED) {
				WSDL_CACHE_GET_1(tmp->encodingStyle, sdlRpcEncodingStyle, in);
			} else {
				tmp->encodingStyle = SOAP_ENCODING_DEFAULT;
			}
			tmp->name = sdl_deserialize_string(in);
			tmp->ns = sdl_deserialize_string(in);
			WSDL_CACHE_GET_INT(n, in);
			tmp->encode = encoders[n];
			WSDL_CACHE_GET_INT(n, in);
			tmp->element = types[n];
			--i;
			WSDL_CACHE_GET_INT(j, in);
			if (j > 0) {
				tmp->headerfaults = emalloc(sizeof(HashTable));
				zend_hash_init(tmp->headerfaults, i, NULL, delete_header, 0);
				while (j > 0) {
					sdlSoapBindingFunctionHeaderPtr tmp2 = emalloc(sizeof(sdlSoapBindingFunctionHeader));
					memset(tmp2, 0, sizeof(sdlSoapBindingFunctionHeader));
					sdl_deserialize_key(tmp->headerfaults, tmp2, in);
					WSDL_CACHE_GET_1(tmp2->use, sdlEncodingUse, in);
					if (tmp2->use == SOAP_ENCODED) {
						WSDL_CACHE_GET_1(tmp2->encodingStyle, sdlRpcEncodingStyle, in);
					} else {
						tmp2->encodingStyle = SOAP_ENCODING_DEFAULT;
					}
					tmp2->name = sdl_deserialize_string(in);
					tmp2->ns = sdl_deserialize_string(in);
					WSDL_CACHE_GET_INT(n, in);
					tmp2->encode = encoders[n];
					WSDL_CACHE_GET_INT(n, in);
					tmp2->element = types[n];
					--j;
				}
			}
		}
	}
}

static HashTable* sdl_deserialize_parameters(encodePtr *encoders, sdlTypePtr *types, char **in)
{
	int i, n;
	HashTable *ht;

	WSDL_CACHE_GET_INT(i, in);
	if (i == 0) {return NULL;}
	ht = emalloc(sizeof(HashTable));
	zend_hash_init(ht, i, NULL, delete_parameter, 0);
	while (i > 0) {
		sdlParamPtr param = emalloc(sizeof(sdlParam));
		sdl_deserialize_key(ht, param, in);
		param->paramName = sdl_deserialize_string(in);
		WSDL_CACHE_GET_INT(param->order, in);
		WSDL_CACHE_GET_INT(n, in);
		param->encode = encoders[n];
		WSDL_CACHE_GET_INT(n, in);
		param->element = types[n];
		--i;
	}
	return ht;
}

static sdlPtr get_sdl_from_cache(const char *fn, const char *uri, time_t t, time_t *cached TSRMLS_DC)
{
	sdlPtr sdl;
	time_t old_t;
	int  i, num_groups, num_types, num_elements, num_encoders, num_bindings, num_func;
	sdlFunctionPtr *functions = NULL;
	sdlBindingPtr *bindings;
	sdlTypePtr *types;
	encodePtr *encoders;
	encodePtr enc;

	int f;
	struct stat st;
	char *in, *buf;

	f = open(fn, O_RDONLY|O_BINARY);
	if (f < 0) {
		return NULL;
	}
	if (fstat(f, &st) != 0) {
		close(f);
		return NULL;
	}
	buf = in = emalloc(st.st_size);
	if (read(f, in, st.st_size) != st.st_size) {
		close(f);
		efree(in);
		return NULL;
	}
	close(f);

	if (strncmp(in,"wsdl",4) != 0 || in[4] != WSDL_CACHE_VERSION || in[5] != '\0') {
		unlink(fn);
		efree(buf);
		return NULL;
	}
	in += 6;

	WSDL_CACHE_GET(old_t, time_t, &in);
	if (old_t < t) {
		unlink(fn);
		efree(buf);
		return NULL;
	}
	*cached = old_t;

	WSDL_CACHE_GET_INT(i, &in);
	if (i == 0 && strncmp(in, uri, i) != 0) {
		unlink(fn);
		efree(buf);
		return NULL;
	}
	WSDL_CACHE_SKIP(i, &in);

	sdl = emalloc(sizeof(*sdl));
	memset(sdl, 0, sizeof(*sdl));

	sdl->source = sdl_deserialize_string(&in);
	sdl->target_ns = sdl_deserialize_string(&in);

	WSDL_CACHE_GET_INT(num_groups, &in);
	WSDL_CACHE_GET_INT(num_types, &in);
	WSDL_CACHE_GET_INT(num_elements, &in);
	WSDL_CACHE_GET_INT(num_encoders, &in);

	i = num_groups+num_types+num_elements;
	types = emalloc((i+1)*sizeof(sdlTypePtr));
	types[0] = NULL;
	while (i > 0) {
		types[i] = emalloc(sizeof(sdlType));
		memset(types[i], 0, sizeof(sdlType));
		i--;
	}

	i = num_encoders;
	enc = defaultEncoding;
	while (enc->details.type != END_KNOWN_TYPES) {
		i++; enc++;
	}
	encoders = emalloc((i+1)*sizeof(encodePtr));
	i = num_encoders;
	encoders[0] = NULL;
	while (i > 0) {
		encoders[i] = emalloc(sizeof(encode));
		memset(encoders[i], 0, sizeof(encode));
		i--;
	}
	i = num_encoders;
	enc = defaultEncoding;
	while (enc->details.type != END_KNOWN_TYPES) {
		encoders[++i] = enc++;
	}

	i = 1;
	if (num_groups > 0) {
		sdl->groups = emalloc(sizeof(HashTable));
		zend_hash_init(sdl->groups, num_groups, NULL, delete_type, 0);
		while (i < num_groups+1) {
			sdl_deserialize_key(sdl->groups, types[i], &in);
			sdl_deserialize_type(types[i], types, encoders, &in);
			i++;
		}
	}

	if (num_types > 0) {
		sdl->types = emalloc(sizeof(HashTable));
		zend_hash_init(sdl->types, num_types, NULL, delete_type, 0);
		while (i < num_groups+num_types+1) {
			sdl_deserialize_key(sdl->types, types[i], &in);
			sdl_deserialize_type(types[i], types, encoders, &in);
			i++;
		}
	}

	if (num_elements > 0) {
		sdl->elements = emalloc(sizeof(HashTable));
		zend_hash_init(sdl->elements, num_elements, NULL, delete_type, 0);
		while (i < num_groups+num_types+num_elements+1) {
			sdl_deserialize_key(sdl->elements, types[i], &in);
			sdl_deserialize_type(types[i], types, encoders, &in);
			i++;
		}
	}

	i = 1;
	if (num_encoders > 0) {
		sdl->encoders = emalloc(sizeof(HashTable));
		zend_hash_init(sdl->encoders, num_encoders, NULL, delete_encoder, 0);
		while (i < num_encoders+1) {
			sdl_deserialize_key(sdl->encoders, encoders[i], &in);
			sdl_deserialize_encoder(encoders[i], types, &in);
			i++;
		}
	}

	/* deserialize bindings */
	WSDL_CACHE_GET_INT(num_bindings, &in);
	bindings = emalloc(num_bindings*sizeof(sdlBindingPtr));
	if (num_bindings > 0) {
		sdl->bindings = emalloc(sizeof(HashTable));
		zend_hash_init(sdl->bindings, num_bindings, NULL, delete_binding, 0);
		for (i = 0; i < num_bindings; i++) {
			sdlBindingPtr binding = emalloc(sizeof(sdlBinding));
			memset(binding, 0, sizeof(sdlBinding));
			sdl_deserialize_key(sdl->bindings, binding, &in);
			binding->name = sdl_deserialize_string(&in);
			binding->location = sdl_deserialize_string(&in);
			WSDL_CACHE_GET_1(binding->bindingType,sdlBindingType,&in);
			if (binding->bindingType == BINDING_SOAP && *in != 0) {
			  sdlSoapBindingPtr soap_binding = binding->bindingAttributes = emalloc(sizeof(sdlSoapBinding));
				WSDL_CACHE_GET_1(soap_binding->style,sdlEncodingStyle,&in);
				WSDL_CACHE_GET_1(soap_binding->transport,sdlTransport,&in);
			} else {
				WSDL_CACHE_SKIP(1,&in);
			}
			bindings[i] = binding;
		}
	}

	/* deserialize functions */
	WSDL_CACHE_GET_INT(num_func, &in);
	zend_hash_init(&sdl->functions, num_func, NULL, delete_function, 0);
	if (num_func > 0) {
		functions = emalloc(num_func*sizeof(sdlFunctionPtr));
		for (i = 0; i < num_func; i++) {
			int binding_num, num_faults;
			sdlFunctionPtr func = emalloc(sizeof(sdlFunction));
			sdl_deserialize_key(&sdl->functions, func, &in);
			func->functionName = sdl_deserialize_string(&in);
			func->requestName = sdl_deserialize_string(&in);
			func->responseName = sdl_deserialize_string(&in);

			WSDL_CACHE_GET_INT(binding_num, &in);
			if (binding_num == 0) {
				func->binding = NULL;
			} else {
				func->binding = bindings[binding_num-1];
			}
			if (func->binding && func->binding->bindingType == BINDING_SOAP && *in != 0) {
				sdlSoapBindingFunctionPtr binding = func->bindingAttributes = emalloc(sizeof(sdlSoapBindingFunction));
				memset(binding, 0, sizeof(sdlSoapBindingFunction));
				WSDL_CACHE_GET_1(binding->style,sdlEncodingStyle,&in);
				binding->soapAction = sdl_deserialize_string(&in);
				sdl_deserialize_soap_body(&binding->input, encoders, types, &in);
				sdl_deserialize_soap_body(&binding->output, encoders, types, &in);
			} else {
				WSDL_CACHE_SKIP(1, &in);
				func->bindingAttributes = NULL;
			}

			func->requestParameters = sdl_deserialize_parameters(encoders, types, &in);
			func->responseParameters = sdl_deserialize_parameters(encoders, types, &in);

			WSDL_CACHE_GET_INT(num_faults, &in);
			if (num_faults > 0) {
			  int j;

				func->faults = emalloc(sizeof(HashTable));
				zend_hash_init(func->faults, num_faults, NULL, delete_fault, 0);

				for (j = 0; j < num_faults; j++) {
					sdlFaultPtr fault = emalloc(sizeof(sdlFault));

					sdl_deserialize_key(func->faults, fault, &in);
					fault->name =sdl_deserialize_string(&in);
					fault->details =sdl_deserialize_parameters(encoders, types, &in);
					if (*in != 0) {
						sdlSoapBindingFunctionFaultPtr binding = fault->bindingAttributes = emalloc(sizeof(sdlSoapBindingFunctionFault));
						memset(binding, 0, sizeof(sdlSoapBindingFunctionFault));
						WSDL_CACHE_GET_1(binding->use,sdlEncodingUse,&in);
						if (binding->use == SOAP_ENCODED) {
							WSDL_CACHE_GET_1(binding->encodingStyle, sdlRpcEncodingStyle, &in);
						} else {
							binding->encodingStyle = SOAP_ENCODING_DEFAULT;
						}
						binding->ns = sdl_deserialize_string(&in);
					} else {
						WSDL_CACHE_SKIP(1, &in);
						fault->bindingAttributes = NULL;
					}
				}
			} else {
				func->faults = NULL;
			}
			functions[i] = func;
		}
	}

	/* deserialize requests */
	WSDL_CACHE_GET_INT(i, &in);
	if (i > 0) {
		sdl->requests = emalloc(sizeof(HashTable));
		zend_hash_init(sdl->requests, i, NULL, NULL, 0);
		while (i > 0) {
			int function_num;

			WSDL_CACHE_GET_INT(function_num, &in);
			sdl_deserialize_key(sdl->requests, functions[function_num-1], &in);
			i--;
		}
	}

	if (functions) {
		efree(functions);
	}
	efree(bindings);
	efree(encoders);
	efree(types);
	efree(buf);
	return sdl;
}

static void sdl_serialize_string(const char *str, smart_str *out)
{
	int i;

	if (str) {
		i = strlen(str);
		WSDL_CACHE_PUT_INT(i, out);
		if (i > 0) {
			WSDL_CACHE_PUT_N(str, i, out);
		}
	} else {
		WSDL_CACHE_PUT_INT(0x7fffffff, out);
	}
}

static void sdl_serialize_key(HashTable *ht, smart_str *out)
{
	char *key;
	uint  key_len;
	ulong index;

	if (zend_hash_get_current_key_ex(ht, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
		WSDL_CACHE_PUT_INT(key_len, out);
		WSDL_CACHE_PUT_N(key, key_len, out);
	} else {
		WSDL_CACHE_PUT_INT(0, out);
	}
}

static void sdl_serialize_encoder_ref(encodePtr enc, HashTable *tmp_encoders, smart_str *out) {
	if (enc) {
		int *encoder_num;
		if (zend_hash_find(tmp_encoders, (char*)&enc, sizeof(enc), (void**)&encoder_num) == SUCCESS) {
			WSDL_CACHE_PUT_INT(*encoder_num, out);
		} else {
			WSDL_CACHE_PUT_INT(0, out);
		}
	} else {
		WSDL_CACHE_PUT_INT(0, out);
	}
}

static void sdl_serialize_type_ref(sdlTypePtr type, HashTable *tmp_types, smart_str *out) {
	if (type) {
		int *type_num;
		if (zend_hash_find(tmp_types, (char*)&type, sizeof(type), (void**)&type_num) == SUCCESS) {
			WSDL_CACHE_PUT_INT(*type_num, out);
		} else {
			WSDL_CACHE_PUT_INT(0, out);
		}
	} else {
		WSDL_CACHE_PUT_INT(0,out);
	}
}

static void sdl_serialize_attribute(sdlAttributePtr attr, HashTable *tmp_encoders, smart_str *out)
{
	int i;

	sdl_serialize_string(attr->name, out);
	sdl_serialize_string(attr->namens, out);
	sdl_serialize_string(attr->ref, out);
	sdl_serialize_string(attr->def, out);
	sdl_serialize_string(attr->fixed, out);
	WSDL_CACHE_PUT_1(attr->form, out);
	WSDL_CACHE_PUT_1(attr->use, out);
	sdl_serialize_encoder_ref(attr->encode, tmp_encoders, out);
	if (attr->extraAttributes) {
		i = zend_hash_num_elements(attr->extraAttributes);
	} else {
		i = 0;
	}
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		sdlExtraAttributePtr *tmp;
		zend_hash_internal_pointer_reset(attr->extraAttributes);
		while (zend_hash_get_current_data(attr->extraAttributes, (void**)&tmp) == SUCCESS) {
			sdl_serialize_key(attr->extraAttributes, out);
			sdl_serialize_string((*tmp)->ns, out);
			sdl_serialize_string((*tmp)->val, out);
			zend_hash_move_forward(attr->extraAttributes);
		}
	}
}

static void sdl_serialize_model(sdlContentModelPtr model, HashTable *tmp_types, HashTable *tmp_elements, smart_str *out)
{
	WSDL_CACHE_PUT_1(model->kind, out);
	WSDL_CACHE_PUT_INT(model->min_occurs, out);
	WSDL_CACHE_PUT_INT(model->max_occurs, out);
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT:
			sdl_serialize_type_ref(model->u.element, tmp_elements, out);
			break;
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_ALL:
		case XSD_CONTENT_CHOICE: {
				sdlContentModelPtr *tmp;
				int i = zend_hash_num_elements(model->u.content);

				WSDL_CACHE_PUT_INT(i, out);
				zend_hash_internal_pointer_reset(model->u.content);
				while (zend_hash_get_current_data(model->u.content, (void**)&tmp) == SUCCESS) {
					sdl_serialize_model(*tmp, tmp_types, tmp_elements, out);
					zend_hash_move_forward(model->u.content);
				}
			}
			break;
		case XSD_CONTENT_GROUP_REF:
			sdl_serialize_string(model->u.group_ref,out);
			break;
		case XSD_CONTENT_GROUP:
			sdl_serialize_type_ref(model->u.group, tmp_types, out);
			break;
		default:
			break;
	}
}

static void sdl_serialize_resriction_int(sdlRestrictionIntPtr x, smart_str *out)
{
	if (x) {
		WSDL_CACHE_PUT_1(1, out);
		WSDL_CACHE_PUT_INT(x->value, out);
		WSDL_CACHE_PUT_1(x->fixed, out);
	} else {
		WSDL_CACHE_PUT_1(0, out);
	}
}

static void sdl_serialize_resriction_char(sdlRestrictionCharPtr x, smart_str *out)
{
	if (x) {
		WSDL_CACHE_PUT_1(1, out);
		sdl_serialize_string(x->value, out);
		WSDL_CACHE_PUT_1(x->fixed, out);
	} else {
		WSDL_CACHE_PUT_1(0, out);
	}
}

static void sdl_serialize_type(sdlTypePtr type, HashTable *tmp_encoders, HashTable *tmp_types, smart_str *out)
{
	int i;
	HashTable *tmp_elements = NULL;

	WSDL_CACHE_PUT_1(type->kind, out);
	sdl_serialize_string(type->name, out);
	sdl_serialize_string(type->namens, out);
	sdl_serialize_string(type->def, out);
	sdl_serialize_string(type->fixed, out);
	sdl_serialize_string(type->ref, out);
	WSDL_CACHE_PUT_1(type->nillable, out);
	WSDL_CACHE_PUT_1(type->form, out);
	sdl_serialize_encoder_ref(type->encode, tmp_encoders, out);

	if (type->restrictions) {
		WSDL_CACHE_PUT_1(1, out);
		sdl_serialize_resriction_int(type->restrictions->minExclusive,out);
		sdl_serialize_resriction_int(type->restrictions->minInclusive,out);
		sdl_serialize_resriction_int(type->restrictions->maxExclusive,out);
		sdl_serialize_resriction_int(type->restrictions->maxInclusive,out);
		sdl_serialize_resriction_int(type->restrictions->totalDigits,out);
		sdl_serialize_resriction_int(type->restrictions->fractionDigits,out);
		sdl_serialize_resriction_int(type->restrictions->length,out);
		sdl_serialize_resriction_int(type->restrictions->minLength,out);
		sdl_serialize_resriction_int(type->restrictions->maxLength,out);
		sdl_serialize_resriction_char(type->restrictions->whiteSpace,out);
		sdl_serialize_resriction_char(type->restrictions->pattern,out);
		if (type->restrictions->enumeration) {
			i = zend_hash_num_elements(type->restrictions->enumeration);
		} else {
			i = 0;
		}
		WSDL_CACHE_PUT_INT(i, out);
		if (i > 0) {
			sdlRestrictionCharPtr *tmp;

			zend_hash_internal_pointer_reset(type->restrictions->enumeration);
			while (zend_hash_get_current_data(type->restrictions->enumeration, (void**)&tmp) == SUCCESS) {
				sdl_serialize_resriction_char(*tmp, out);
				sdl_serialize_key(type->restrictions->enumeration, out);
				zend_hash_move_forward(type->restrictions->enumeration);
			}
		}
	} else {
		WSDL_CACHE_PUT_1(0, out);
	}
	if (type->elements) {
		i = zend_hash_num_elements(type->elements);
	} else {
		i = 0;
	}
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		sdlTypePtr *tmp;

		tmp_elements = emalloc(sizeof(HashTable));
		zend_hash_init(tmp_elements, i, NULL, NULL, 0);

		zend_hash_internal_pointer_reset(type->elements);
		while (zend_hash_get_current_data(type->elements, (void**)&tmp) == SUCCESS) {
			sdl_serialize_key(type->elements, out);
			sdl_serialize_type(*tmp, tmp_encoders, tmp_types, out);
			zend_hash_add(tmp_elements, (char*)tmp, sizeof(*tmp), &i, sizeof(int), NULL);
			i--;
			zend_hash_move_forward(type->elements);
		}
	}

	if (type->attributes) {
		i = zend_hash_num_elements(type->attributes);
	} else {
		i = 0;
	}
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		sdlAttributePtr *tmp;
		zend_hash_internal_pointer_reset(type->attributes);
		while (zend_hash_get_current_data(type->attributes, (void**)&tmp) == SUCCESS) {
			sdl_serialize_key(type->attributes, out);
			sdl_serialize_attribute(*tmp, tmp_encoders, out);
			zend_hash_move_forward(type->attributes);
		}
	}
	if (type->model) {
		WSDL_CACHE_PUT_1(1, out);
		sdl_serialize_model(type->model, tmp_types, tmp_elements, out);
	} else {
		WSDL_CACHE_PUT_1(0, out);
	}
	if (tmp_elements != NULL) {
		zend_hash_destroy(tmp_elements);
		efree(tmp_elements);
	}
}

static void sdl_serialize_encoder(encodePtr enc, HashTable *tmp_types, smart_str *out)
{
	WSDL_CACHE_PUT_INT(enc->details.type, out);
	sdl_serialize_string(enc->details.type_str, out);
	sdl_serialize_string(enc->details.ns, out);
	sdl_serialize_type_ref(enc->details.sdl_type, tmp_types, out);
}

static void sdl_serialize_parameters(HashTable *ht, HashTable *tmp_encoders, HashTable *tmp_types, smart_str *out)
{
	int i;

	if (ht) {
		i = zend_hash_num_elements(ht);
	} else {
		i = 0;
	}
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		sdlParamPtr *tmp;

		zend_hash_internal_pointer_reset(ht);
		while (zend_hash_get_current_data(ht, (void**)&tmp) == SUCCESS) {
			sdl_serialize_key(ht, out);
			sdl_serialize_string((*tmp)->paramName, out);
			WSDL_CACHE_PUT_INT((*tmp)->order, out);
			sdl_serialize_encoder_ref((*tmp)->encode, tmp_encoders, out);
			sdl_serialize_type_ref((*tmp)->element, tmp_types, out);
			zend_hash_move_forward(ht);
		}
	}
}

static void sdl_serialize_soap_body(sdlSoapBindingFunctionBodyPtr body, HashTable *tmp_encoders, HashTable *tmp_types, smart_str *out)
{
	int i, j;

	WSDL_CACHE_PUT_1(body->use, out);
	if (body->use == SOAP_ENCODED) {
		WSDL_CACHE_PUT_1(body->encodingStyle, out);
	}
	sdl_serialize_string(body->ns, out);
	if (body->headers) {
		i = zend_hash_num_elements(body->headers);
	} else {
		i = 0;
	}
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		sdlSoapBindingFunctionHeaderPtr *tmp;
		zend_hash_internal_pointer_reset(body->headers);
		while (zend_hash_get_current_data(body->headers, (void**)&tmp) == SUCCESS) {
			sdl_serialize_key(body->headers, out);
			WSDL_CACHE_PUT_1((*tmp)->use, out);
			if ((*tmp)->use == SOAP_ENCODED) {
				WSDL_CACHE_PUT_1((*tmp)->encodingStyle, out);
			}
			sdl_serialize_string((*tmp)->name, out);
			sdl_serialize_string((*tmp)->ns, out);
			sdl_serialize_encoder_ref((*tmp)->encode, tmp_encoders, out);
			sdl_serialize_type_ref((*tmp)->element, tmp_types, out);
			if ((*tmp)->headerfaults) {
				j = zend_hash_num_elements((*tmp)->headerfaults);
			} else {
				j = 0;
			}
			WSDL_CACHE_PUT_INT(j, out);
			if (j > 0) {
				sdlSoapBindingFunctionHeaderPtr *tmp2;
				zend_hash_internal_pointer_reset((*tmp)->headerfaults);
				while (zend_hash_get_current_data((*tmp)->headerfaults, (void**)&tmp2) == SUCCESS) {
					sdl_serialize_key((*tmp)->headerfaults, out);
					WSDL_CACHE_PUT_1((*tmp2)->use, out);
					if ((*tmp2)->use == SOAP_ENCODED) {
						WSDL_CACHE_PUT_1((*tmp2)->encodingStyle, out);
					}
					sdl_serialize_string((*tmp2)->name, out);
					sdl_serialize_string((*tmp2)->ns, out);
					sdl_serialize_encoder_ref((*tmp2)->encode, tmp_encoders, out);
					sdl_serialize_type_ref((*tmp2)->element, tmp_types, out);
					zend_hash_move_forward((*tmp)->headerfaults);
				}
			}
			zend_hash_move_forward(body->headers);
		}
	}
}

static void add_sdl_to_cache(const char *fn, const char *uri, time_t t, sdlPtr sdl TSRMLS_DC)
{
	smart_str buf = {0};
	smart_str *out = &buf;
	int i;
	int type_num = 1;
	int encoder_num = 1;
	int f;
	encodePtr enc;
	HashTable tmp_types;
	HashTable tmp_encoders;
	HashTable tmp_bindings;
	HashTable tmp_functions;

#ifdef ZEND_WIN32
	f = open(fn,O_CREAT|O_WRONLY|O_EXCL|O_BINARY,S_IREAD|S_IWRITE);
#else
	f = open(fn,O_CREAT|O_WRONLY|O_EXCL|O_BINARY,S_IREAD|S_IWRITE|S_IROTH|S_IWOTH|S_IRGRP|S_IWGRP);
#endif
	if (f < 0) {return;}

	zend_hash_init(&tmp_types, 0, NULL, NULL, 0);
	zend_hash_init(&tmp_encoders, 0, NULL, NULL, 0);
	zend_hash_init(&tmp_bindings, 0, NULL, NULL, 0);
	zend_hash_init(&tmp_functions, 0, NULL, NULL, 0);

	WSDL_CACHE_PUT_N("wsdl", 4, out);
	WSDL_CACHE_PUT_1(WSDL_CACHE_VERSION,out);
	WSDL_CACHE_PUT_1(0,out);
	WSDL_CACHE_PUT_N(&t, sizeof(t), out);

	sdl_serialize_string(uri, out);
	sdl_serialize_string(sdl->source, out);
	sdl_serialize_string(sdl->target_ns, out);

	if (sdl->groups) {
		i = zend_hash_num_elements(sdl->groups);
	} else {
		i = 0;
	}
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		sdlTypePtr *tmp;

		zend_hash_internal_pointer_reset(sdl->groups);
		while (zend_hash_get_current_data(sdl->groups, (void**)&tmp) == SUCCESS) {
			zend_hash_add(&tmp_types, (char*)tmp, sizeof(*tmp), (void**)&type_num, sizeof(type_num), NULL);
			++type_num;
			zend_hash_move_forward(sdl->groups);
		}
	}

	if (sdl->types) {
		i = zend_hash_num_elements(sdl->types);
	} else {
		i = 0;
	}
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		sdlTypePtr *tmp;

		zend_hash_internal_pointer_reset(sdl->types);
		while (zend_hash_get_current_data(sdl->types, (void**)&tmp) == SUCCESS) {
			zend_hash_add(&tmp_types, (char*)tmp, sizeof(*tmp), (void**)&type_num, sizeof(type_num), NULL);
			++type_num;
			zend_hash_move_forward(sdl->types);
		}
	}

	if (sdl->elements) {
		i = zend_hash_num_elements(sdl->elements);
	} else {
		i = 0;
	}
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		sdlTypePtr *tmp;

		zend_hash_internal_pointer_reset(sdl->elements);
		while (zend_hash_get_current_data(sdl->elements, (void**)&tmp) == SUCCESS) {
			zend_hash_add(&tmp_types, (char*)tmp, sizeof(*tmp), (void**)&type_num, sizeof(type_num), NULL);
			++type_num;
			zend_hash_move_forward(sdl->elements);
		}
	}

	if (sdl->encoders) {
		i = zend_hash_num_elements(sdl->encoders);
	} else {
		i = 0;
	}
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		encodePtr *tmp;

		zend_hash_internal_pointer_reset(sdl->encoders);
		while (zend_hash_get_current_data(sdl->encoders, (void**)&tmp) == SUCCESS) {
			zend_hash_add(&tmp_encoders, (char*)tmp, sizeof(*tmp), (void**)&encoder_num, sizeof(encoder_num), NULL);
			++encoder_num;
			zend_hash_move_forward(sdl->encoders);
		}
	}
	enc = defaultEncoding;
	while (enc->details.type != END_KNOWN_TYPES) {
		zend_hash_add(&tmp_encoders, (char*)&enc, sizeof(encodePtr), (void**)&encoder_num, sizeof(encoder_num), NULL);
		enc++;
		++encoder_num;
	}

	if (sdl->groups) {
		sdlTypePtr *tmp;
		zend_hash_internal_pointer_reset(sdl->groups);
		while (zend_hash_get_current_data(sdl->groups, (void**)&tmp) == SUCCESS) {
			sdl_serialize_key(sdl->groups, out);
			sdl_serialize_type(*tmp, &tmp_encoders, &tmp_types, out);
			zend_hash_move_forward(sdl->groups);
		}
	}

	if (sdl->types) {
		sdlTypePtr *tmp;
		zend_hash_internal_pointer_reset(sdl->types);
		while (zend_hash_get_current_data(sdl->types, (void**)&tmp) == SUCCESS) {
			sdl_serialize_key(sdl->types, out);
			sdl_serialize_type(*tmp, &tmp_encoders, &tmp_types, out);
			zend_hash_move_forward(sdl->types);
		}
	}

	if (sdl->elements) {
		sdlTypePtr *tmp;
		zend_hash_internal_pointer_reset(sdl->elements);
		while (zend_hash_get_current_data(sdl->elements, (void**)&tmp) == SUCCESS) {
			sdl_serialize_key(sdl->elements, out);
			sdl_serialize_type(*tmp, &tmp_encoders, &tmp_types, out);
			zend_hash_move_forward(sdl->elements);
		}
	}

	if (sdl->encoders) {
		encodePtr *tmp;
		zend_hash_internal_pointer_reset(sdl->encoders);
		while (zend_hash_get_current_data(sdl->encoders, (void**)&tmp) == SUCCESS) {
			sdl_serialize_key(sdl->encoders, out);
			sdl_serialize_encoder(*tmp, &tmp_types, out);
			zend_hash_move_forward(sdl->encoders);
		}
	}

	/* serialize bindings */
	if (sdl->bindings) {
		i = zend_hash_num_elements(sdl->bindings);
	} else {
		i = 0;
	}
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		sdlBindingPtr *tmp;
		int binding_num = 1;

		zend_hash_internal_pointer_reset(sdl->bindings);
		while (zend_hash_get_current_data(sdl->bindings, (void**)&tmp) == SUCCESS) {
			sdl_serialize_key(sdl->bindings, out);
			sdl_serialize_string((*tmp)->name, out);
			sdl_serialize_string((*tmp)->location, out);
			WSDL_CACHE_PUT_1((*tmp)->bindingType,out);
			if ((*tmp)->bindingType == BINDING_SOAP && (*tmp)->bindingAttributes != NULL) {
				sdlSoapBindingPtr binding = (sdlSoapBindingPtr)(*tmp)->bindingAttributes;
				WSDL_CACHE_PUT_1(binding->style, out);
				WSDL_CACHE_PUT_1(binding->transport, out);
			} else {
				WSDL_CACHE_PUT_1(0,out);
			}

			zend_hash_add(&tmp_bindings, (char*)tmp, sizeof(*tmp), (void**)&binding_num, sizeof(binding_num), NULL);
			binding_num++;
			zend_hash_move_forward(sdl->bindings);
		}
	}

	/* serialize functions */
	i = zend_hash_num_elements(&sdl->functions);
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		sdlFunctionPtr *tmp;
		int *binding_num;
		int function_num = 1;

		zend_hash_internal_pointer_reset(&sdl->functions);
		while (zend_hash_get_current_data(&sdl->functions, (void**)&tmp) == SUCCESS) {
			sdl_serialize_key(&sdl->functions, out);
			sdl_serialize_string((*tmp)->functionName, out);
			sdl_serialize_string((*tmp)->requestName, out);
			sdl_serialize_string((*tmp)->responseName, out);

			if ((*tmp)->binding == NULL ||
			    zend_hash_find(&tmp_bindings,(char*)&(*tmp)->binding,sizeof((*tmp)->binding), (void**)&binding_num) != SUCCESS) {
			}
			WSDL_CACHE_PUT_INT(*binding_num, out);
			if (*binding_num >= 0) {
				if ((*tmp)->binding->bindingType == BINDING_SOAP && (*tmp)->bindingAttributes != NULL) {
					sdlSoapBindingFunctionPtr binding = (sdlSoapBindingFunctionPtr)(*tmp)->bindingAttributes;
					WSDL_CACHE_PUT_1(binding->style, out);
					sdl_serialize_string(binding->soapAction, out);
					sdl_serialize_soap_body(&binding->input, &tmp_encoders, &tmp_types, out);
					sdl_serialize_soap_body(&binding->output, &tmp_encoders, &tmp_types, out);
				} else {
					WSDL_CACHE_PUT_1(0,out);
				}
			}
			sdl_serialize_parameters((*tmp)->requestParameters, &tmp_encoders, &tmp_types, out);
			sdl_serialize_parameters((*tmp)->responseParameters, &tmp_encoders, &tmp_types, out);

			if ((*tmp)->faults) {
				sdlFaultPtr *fault;

				WSDL_CACHE_PUT_INT(zend_hash_num_elements((*tmp)->faults), out);

				zend_hash_internal_pointer_reset((*tmp)->faults);
				while (zend_hash_get_current_data((*tmp)->faults, (void**)&fault) == SUCCESS) {
					sdl_serialize_key((*tmp)->faults, out);
					sdl_serialize_string((*fault)->name, out);
					sdl_serialize_parameters((*fault)->details, &tmp_encoders, &tmp_types, out);
					if ((*tmp)->binding->bindingType == BINDING_SOAP && (*fault)->bindingAttributes != NULL) {
						sdlSoapBindingFunctionFaultPtr binding = (sdlSoapBindingFunctionFaultPtr)(*fault)->bindingAttributes;
						WSDL_CACHE_PUT_1(binding->use, out);
						if (binding->use == SOAP_ENCODED) {
							WSDL_CACHE_PUT_1(binding->encodingStyle, out);
						}
						sdl_serialize_string(binding->ns, out);
					} else {
						WSDL_CACHE_PUT_1(0, out);
					}
					zend_hash_move_forward((*tmp)->faults);
				}
			} else {
				WSDL_CACHE_PUT_INT(0, out);
			}

			zend_hash_add(&tmp_functions, (char*)tmp, sizeof(*tmp), (void**)&function_num, sizeof(function_num), NULL);
			function_num++;
			zend_hash_move_forward(&sdl->functions);
		}
	}

	/* serialize requests */
	if (sdl->requests) {
		i = zend_hash_num_elements(sdl->requests);
	} else {
		i = 0;
	}
	WSDL_CACHE_PUT_INT(i, out);
	if (i > 0) {
		sdlFunctionPtr *tmp;
		int *function_num;

		zend_hash_internal_pointer_reset(sdl->requests);
		while (zend_hash_get_current_data(sdl->requests, (void**)&tmp) == SUCCESS) {
			if (zend_hash_find(&tmp_functions, (char*)tmp, sizeof(*tmp), (void**)&function_num) != SUCCESS) {
			}
			WSDL_CACHE_PUT_INT(*function_num, out);
			sdl_serialize_key(sdl->requests, out);
			zend_hash_move_forward(sdl->requests);
		}
	}

	write(f, buf.c, buf.len);
	close(f);
	smart_str_free(&buf);
	zend_hash_destroy(&tmp_functions);
	zend_hash_destroy(&tmp_bindings);
	zend_hash_destroy(&tmp_encoders);
	zend_hash_destroy(&tmp_types);
}


static void make_persistent_restriction_int(void *data)
{
	sdlRestrictionIntPtr *rest = (sdlRestrictionIntPtr *)data;
	sdlRestrictionIntPtr prest = NULL;

	prest = malloc(sizeof(sdlRestrictionInt));
	*prest = **rest;
	*rest = prest;
}


static void make_persistent_restriction_char(void *data)
{
	sdlRestrictionCharPtr *rest = (sdlRestrictionCharPtr *)data;
	sdlRestrictionCharPtr prest = NULL;

	prest = malloc(sizeof(sdlRestrictionChar));
	memset(prest, 0, sizeof(sdlRestrictionChar));
	prest->value = strdup((*rest)->value);
	prest->fixed = (*rest)->fixed;
	*rest = prest;
}


static void make_persistent_sdl_type_ref(sdlTypePtr *type, HashTable *ptr_map, HashTable *bp_types)
{
	sdlTypePtr *tmp;

	if (zend_hash_find(ptr_map, (char *)type, sizeof(sdlTypePtr), (void**)&tmp) == SUCCESS) {
		*type = *tmp;
	} else {
		zend_hash_next_index_insert(bp_types, (void*)&type, sizeof(sdlTypePtr*), NULL);
	}
}


static void make_persistent_sdl_encoder_ref(encodePtr *enc, HashTable *ptr_map, HashTable *bp_encoders)
{
	encodePtr *tmp;

	/* do not process defaultEncoding's here */
	if ((*enc) >= defaultEncoding && (*enc) < defaultEncoding + numDefaultEncodings) {
		return;
	}

	if (zend_hash_find(ptr_map, (char *)enc, sizeof(encodePtr), (void**)&tmp) == SUCCESS) {
		*enc = *tmp;
	} else {
		zend_hash_next_index_insert(bp_encoders, (void*)&enc, sizeof(encodePtr*), NULL);
	}
}


static HashTable* make_persistent_sdl_function_headers(HashTable *headers, HashTable *ptr_map)
{
	HashTable *pheaders;
	sdlSoapBindingFunctionHeaderPtr *tmp, pheader;
	encodePtr *penc;
	sdlTypePtr *ptype;
	ulong index;
	char *key;
	uint key_len;

	pheaders = malloc(sizeof(HashTable));
	zend_hash_init(pheaders, zend_hash_num_elements(headers), NULL, delete_header_persistent, 1);

	zend_hash_internal_pointer_reset(headers);
	while (zend_hash_get_current_data(headers, (void**)&tmp) == SUCCESS) {
		pheader = malloc(sizeof(sdlSoapBindingFunctionHeader));
		memset(pheader, 0, sizeof(sdlSoapBindingFunctionHeader));
		*pheader = **tmp;

		if (pheader->name) {
			pheader->name = strdup(pheader->name);
		}
		if (pheader->ns) {
			pheader->ns = strdup(pheader->ns);
		}

		if (pheader->encode->details.sdl_type) {
			if (zend_hash_find(ptr_map, (char*)&pheader->encode, sizeof(encodePtr), (void**)&penc) == FAILURE) {
				assert(0);
			}
			pheader->encode = *penc;
		}
		if (pheader->element) {
			if (zend_hash_find(ptr_map, (char*)&pheader->element, sizeof(sdlTypePtr), (void**)&ptype) == FAILURE) {
				assert(0);
			}
			pheader->element = *ptype;
		}

		if (pheader->headerfaults) {
			pheader->headerfaults = make_persistent_sdl_function_headers(pheader->headerfaults, ptr_map);
		}

		if (zend_hash_get_current_key_ex(headers, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
			zend_hash_add(pheaders, key, key_len, (void*)&pheader, sizeof(sdlSoapBindingFunctionHeaderPtr), NULL);
		} else {
			zend_hash_next_index_insert(pheaders, (void*)&pheader, sizeof(sdlSoapBindingFunctionHeaderPtr), NULL);
		}

		zend_hash_move_forward(headers);
	}

	return pheaders;
}


static void make_persistent_sdl_soap_body(sdlSoapBindingFunctionBodyPtr body, HashTable *ptr_map)
{
	if (body->ns) {
		body->ns = strdup(body->ns);
	}

	if (body->headers) {
		body->headers = make_persistent_sdl_function_headers(body->headers, ptr_map);
	}
}


static HashTable* make_persistent_sdl_parameters(HashTable *params, HashTable *ptr_map)
{
	HashTable *pparams;
	sdlParamPtr *tmp, pparam;
	sdlTypePtr *ptype;
	encodePtr *penc;
	ulong index;
	char *key;
	uint key_len;

	pparams = malloc(sizeof(HashTable));
	zend_hash_init(pparams, zend_hash_num_elements(params), NULL, delete_parameter_persistent, 1);

	zend_hash_internal_pointer_reset(params);
	while (zend_hash_get_current_data(params, (void**)&tmp) == SUCCESS) {
		pparam = malloc(sizeof(sdlParam));
		memset(pparam, 0, sizeof(sdlParam));
		*pparam = **tmp;

		if (pparam->paramName) {
			pparam->paramName = strdup(pparam->paramName);
		}

		if (pparam->encode && pparam->encode->details.sdl_type) {
			if (zend_hash_find(ptr_map, (char*)&pparam->encode, sizeof(encodePtr), (void**)&penc) == FAILURE) {
				assert(0);
			}
			pparam->encode = *penc;
		}
		if (pparam->element) {
			if (zend_hash_find(ptr_map, (char*)&pparam->element, sizeof(sdlTypePtr), (void**)&ptype) == FAILURE) {
				assert(0);
			}
			pparam->element = *ptype;
		}

		if (zend_hash_get_current_key_ex(params, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
			zend_hash_add(pparams, key, key_len, (void*)&pparam, sizeof(sdlParamPtr), NULL);
		} else {
			zend_hash_next_index_insert(pparams, (void*)&pparam, sizeof(sdlParamPtr), NULL);
		}

		zend_hash_move_forward(params);
	}


	return pparams;
}

static HashTable* make_persistent_sdl_function_faults(sdlFunctionPtr func, HashTable *faults, HashTable *ptr_map)
{
	HashTable *pfaults;
	sdlFaultPtr  *tmp, pfault;
	ulong index;
	char *key;
	uint key_len;

	pfaults = malloc(sizeof(HashTable));
	zend_hash_init(pfaults, zend_hash_num_elements(faults), NULL, delete_fault_persistent, 1);

	zend_hash_internal_pointer_reset(faults);
	while (zend_hash_get_current_data(faults, (void**)&tmp) == SUCCESS) {
		pfault = malloc(sizeof(sdlFault));
		memset(pfault, 0, sizeof(sdlFault));
		*pfault = **tmp;

		if (pfault->name) {
			pfault->name = strdup(pfault->name);
		}
		if (pfault->details) {
			pfault->details = make_persistent_sdl_parameters(pfault->details, ptr_map);
		}

		if (func->binding->bindingType == BINDING_SOAP && pfault->bindingAttributes) {
			sdlSoapBindingFunctionFaultPtr soap_binding;

		   	soap_binding = malloc(sizeof(sdlSoapBindingFunctionFault));
			memset(soap_binding, 0, sizeof(sdlSoapBindingFunctionFault));
			*soap_binding = *(sdlSoapBindingFunctionFaultPtr)pfault->bindingAttributes;
			if (soap_binding->ns) {
				soap_binding->ns = strdup(soap_binding->ns);
			}
			pfault->bindingAttributes = soap_binding;
		}

		if (zend_hash_get_current_key_ex(faults, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
			zend_hash_add(pfaults, key, key_len, (void*)&pfault, sizeof(sdlParamPtr), NULL);
		} else {
			zend_hash_next_index_insert(pfaults, (void*)&pfault, sizeof(sdlParamPtr), NULL);
		}

		zend_hash_move_forward(faults);
	}


	return pfaults;
}


static sdlAttributePtr make_persistent_sdl_attribute(sdlAttributePtr attr, HashTable *ptr_map, HashTable *bp_types, HashTable *bp_encoders)
{
	sdlAttributePtr pattr;
	ulong index;
	char *key;
	uint key_len;

	pattr = malloc(sizeof(sdlAttribute));
	memset(pattr, 0, sizeof(sdlAttribute));

	*pattr = *attr;

	if (pattr->name) {
		pattr->name = strdup(pattr->name);
	}
	if (pattr->namens) {
		pattr->namens = strdup(pattr->namens);
	}
	if (pattr->ref) {
		pattr->ref = strdup(pattr->ref);
	}
	if (pattr->def) {
		pattr->def = strdup(pattr->def);
	}
	if (pattr->fixed) {
		pattr->fixed = strdup(pattr->fixed);
	}

	/* we do not want to process defaultEncoding's here */
	if (pattr->encode) {
		make_persistent_sdl_encoder_ref(&pattr->encode, ptr_map, bp_encoders);
	}

	if (pattr->extraAttributes) {
		sdlExtraAttributePtr *tmp, pextra;

		pattr->extraAttributes = malloc(sizeof(HashTable));
		zend_hash_init(pattr->extraAttributes, zend_hash_num_elements(attr->extraAttributes), NULL, delete_extra_attribute_persistent, 1);

		zend_hash_internal_pointer_reset(pattr->extraAttributes);
		while (zend_hash_get_current_data(attr->extraAttributes, (void**)&tmp) == SUCCESS) {
			pextra = malloc(sizeof(sdlExtraAttribute));
			memset(pextra, 0, sizeof(sdlExtraAttribute));
			if ((*tmp)->ns) {
				pextra->ns = strdup((*tmp)->ns);
			}
			if ((*tmp)->val) {
				pextra->val = strdup((*tmp)->val);
			}

			if (zend_hash_get_current_key_ex(attr->extraAttributes, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
				zend_hash_add(pattr->extraAttributes, key, key_len, (void*)&pextra, sizeof(sdlExtraAttributePtr), NULL);
			}

			zend_hash_move_forward(attr->extraAttributes);
		}
	}

	return pattr;
}


static sdlContentModelPtr make_persistent_sdl_model(sdlContentModelPtr model, HashTable *ptr_map, HashTable *bp_types, HashTable *bp_encoders)
{
	sdlContentModelPtr pmodel;
	sdlContentModelPtr *tmp, pcontent;

	pmodel = malloc(sizeof(sdlContentModel));
	memset(pmodel, 0, sizeof(sdlContentModel));
	*pmodel = *model;

	switch (pmodel->kind) {
		case XSD_CONTENT_ELEMENT:
			if (pmodel->u.element) {
				make_persistent_sdl_type_ref(&pmodel->u.element, ptr_map, bp_types);
			}
			break;

		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_ALL:
		case XSD_CONTENT_CHOICE:
			pmodel->u.content = malloc(sizeof(HashTable));
			zend_hash_init(pmodel->u.content, zend_hash_num_elements(model->u.content), NULL, delete_model_persistent, 1);

			zend_hash_internal_pointer_reset(model->u.content);
			while (zend_hash_get_current_data(model->u.content, (void**)&tmp) == SUCCESS) {
				pcontent = make_persistent_sdl_model(*tmp, ptr_map, bp_types, bp_encoders);
				zend_hash_next_index_insert(pmodel->u.content, (void*)&pcontent, sizeof(sdlContentModelPtr), NULL);
				zend_hash_move_forward(model->u.content);
			}
			break;

		case XSD_CONTENT_GROUP_REF:
			if (pmodel->u.group_ref) {
				pmodel->u.group_ref = strdup(pmodel->u.group_ref);
			}
			break;

		case XSD_CONTENT_GROUP:
			if (pmodel->u.group) {
				make_persistent_sdl_type_ref(&pmodel->u.group, ptr_map, bp_types);
			}
			break;

		default:
			break;
	}

	return pmodel;
}


static sdlTypePtr make_persistent_sdl_type(sdlTypePtr type, HashTable *ptr_map, HashTable *bp_types, HashTable *bp_encoders)
{
	ulong index;
	char *key;
	uint key_len;
	sdlTypePtr ptype = NULL;

	ptype = malloc(sizeof(sdlType));
	memset(ptype, 0, sizeof(sdlType));

	*ptype = *type;

	if (ptype->name) {
		ptype->name = strdup(ptype->name);
	}
	if (ptype->namens) {
		ptype->namens = strdup(ptype->namens);
	}
	if (ptype->def) {
		ptype->def = strdup(ptype->def);
	}
	if (ptype->fixed) {
		ptype->fixed = strdup(ptype->fixed);
	}
	if (ptype->ref) {
		ptype->ref = strdup(ptype->ref);
	}

	/* we do not want to process defaultEncoding's here */
	if (ptype->encode) {
		make_persistent_sdl_encoder_ref(&ptype->encode, ptr_map, bp_encoders);
	}

	if (ptype->restrictions) {
		ptype->restrictions = malloc(sizeof(sdlRestrictions));
		memset(ptype->restrictions, 0, sizeof(sdlRestrictions));
		*ptype->restrictions = *type->restrictions;

		if (ptype->restrictions->minExclusive) {
			make_persistent_restriction_int(&ptype->restrictions->minExclusive);
		}
		if (ptype->restrictions->maxExclusive) {
			make_persistent_restriction_int(&ptype->restrictions->maxExclusive);
		}
		if (ptype->restrictions->minInclusive) {
			make_persistent_restriction_int(&ptype->restrictions->minInclusive);
		}
		if (ptype->restrictions->maxInclusive) {
			make_persistent_restriction_int(&ptype->restrictions->maxInclusive);
		}
		if (ptype->restrictions->totalDigits) {
			make_persistent_restriction_int(&ptype->restrictions->totalDigits);
		}
		if (ptype->restrictions->fractionDigits) {
			make_persistent_restriction_int(&ptype->restrictions->fractionDigits);
		}
		if (ptype->restrictions->length) {
			make_persistent_restriction_int(&ptype->restrictions->length);
		}
		if (ptype->restrictions->minLength) {
			make_persistent_restriction_int(&ptype->restrictions->minLength);
		}
		if (ptype->restrictions->maxLength) {
			make_persistent_restriction_int(&ptype->restrictions->maxLength);
		}
		if (ptype->restrictions->whiteSpace) {
			make_persistent_restriction_char(&ptype->restrictions->whiteSpace);
		}
		if (ptype->restrictions->pattern) {
			make_persistent_restriction_char(&ptype->restrictions->pattern);
		}

		if (type->restrictions->enumeration) {
			sdlRestrictionCharPtr tmp;

			ptype->restrictions->enumeration = malloc(sizeof(HashTable));
			zend_hash_init(ptype->restrictions->enumeration, zend_hash_num_elements(type->restrictions->enumeration), NULL, delete_restriction_var_char_persistent, 1);
			zend_hash_copy(ptype->restrictions->enumeration, type->restrictions->enumeration, make_persistent_restriction_char, (void*)&tmp, sizeof(sdlRestrictionCharPtr));
		}
	}

	if (ptype->elements) {
		sdlTypePtr *tmp, pelem;

		ptype->elements = malloc(sizeof(HashTable));
		zend_hash_init(ptype->elements, zend_hash_num_elements(type->elements), NULL, delete_type_persistent, 1);

		zend_hash_internal_pointer_reset(type->elements);
		while (zend_hash_get_current_data(type->elements, (void **)&tmp) == SUCCESS) {
			pelem = make_persistent_sdl_type(*tmp, ptr_map, bp_types, bp_encoders);
			if (zend_hash_get_current_key_ex(type->elements, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
				zend_hash_add(ptype->elements, key, key_len, (void*)&pelem, sizeof(sdlTypePtr), NULL);
			} else {
				zend_hash_next_index_insert(ptype->elements, (void*)&pelem, sizeof(sdlTypePtr), NULL);
			}
			zend_hash_add(ptr_map, (char*)tmp, sizeof(*tmp), (void*)&pelem, sizeof(sdlTypePtr), NULL);
			zend_hash_move_forward(type->elements);
		}
	}

	if (ptype->attributes) {
		sdlAttributePtr *tmp, pattr;

		ptype->attributes = malloc(sizeof(HashTable));
		zend_hash_init(ptype->attributes, zend_hash_num_elements(type->attributes), NULL, delete_attribute_persistent, 1);

		zend_hash_internal_pointer_reset(type->attributes);
		while (zend_hash_get_current_data(type->attributes, (void **)&tmp) == SUCCESS) {
			pattr = make_persistent_sdl_attribute(*tmp, ptr_map, bp_types, bp_encoders);
			if (zend_hash_get_current_key_ex(type->attributes, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
				zend_hash_add(ptype->attributes, key, key_len, (void*)&pattr, sizeof(sdlAttributePtr), NULL);
			} else {
				zend_hash_next_index_insert(ptype->attributes, (void*)&pattr, sizeof(sdlAttributePtr), NULL);
			}
			zend_hash_move_forward(type->attributes);
		}
	}

	if (type->model) {
		ptype->model = make_persistent_sdl_model(ptype->model, ptr_map, bp_types, bp_encoders);
	}

	return ptype;
}

static encodePtr make_persistent_sdl_encoder(encodePtr enc, HashTable *ptr_map, HashTable *bp_types, HashTable *bp_encoders)
{
	encodePtr penc = NULL;

	penc = malloc(sizeof(encode));
	memset(penc, 0, sizeof(encode));

	*penc = *enc;

	if (penc->details.type_str) {
		penc->details.type_str = strdup(penc->details.type_str);
	}
	if (penc->details.ns) {
		penc->details.ns = strdup(penc->details.ns);
	}

	if (penc->details.sdl_type) {
		make_persistent_sdl_type_ref(&penc->details.sdl_type, ptr_map, bp_types);
	}

	return penc;
}

static sdlBindingPtr make_persistent_sdl_binding(sdlBindingPtr bind, HashTable *ptr_map)
{
	sdlBindingPtr pbind = NULL;

	pbind = malloc(sizeof(sdlBinding));
	memset(pbind, 0, sizeof(sdlBinding));

	*pbind = *bind;

	if (pbind->name) {
		pbind->name = strdup(pbind->name);
	}
	if (pbind->location) {
		pbind->location = strdup(pbind->location);
	}

	if (pbind->bindingType == BINDING_SOAP && pbind->bindingAttributes) {
		sdlSoapBindingPtr soap_binding;
	   
		soap_binding = malloc(sizeof(sdlSoapBinding));
		memset(soap_binding, 0, sizeof(sdlSoapBinding));
		*soap_binding = *(sdlSoapBindingPtr)pbind->bindingAttributes;
		pbind->bindingAttributes = soap_binding;
	}

	return pbind;
}

static sdlFunctionPtr make_persistent_sdl_function(sdlFunctionPtr func, HashTable *ptr_map)
{
	sdlFunctionPtr pfunc = NULL;

	pfunc = malloc(sizeof(sdlFunction));
	memset(pfunc, 0, sizeof(sdlFunction));

	*pfunc = *func;

	if (pfunc->functionName) {
		pfunc->functionName = strdup(pfunc->functionName);
	}
	if (pfunc->requestName) {
		pfunc->requestName = strdup(pfunc->requestName);
	}
	if (pfunc->responseName) {
		pfunc->responseName = strdup(pfunc->responseName);
	}

	if (pfunc->binding) {
		sdlBindingPtr *tmp;

		if (zend_hash_find(ptr_map, (char*)&pfunc->binding, sizeof(pfunc->binding), (void**)&tmp) == FAILURE) {
			assert(0);
		}
		pfunc->binding = *tmp;
		
		if (pfunc->binding->bindingType == BINDING_SOAP && pfunc->bindingAttributes) {
			sdlSoapBindingFunctionPtr soap_binding;

		   	soap_binding = malloc(sizeof(sdlSoapBindingFunction));
			memset(soap_binding, 0, sizeof(sdlSoapBindingFunction));
			*soap_binding = *(sdlSoapBindingFunctionPtr)pfunc->bindingAttributes;
			if (soap_binding->soapAction) {
				soap_binding->soapAction = strdup(soap_binding->soapAction);
			}
			make_persistent_sdl_soap_body(&soap_binding->input, ptr_map);
			make_persistent_sdl_soap_body(&soap_binding->output, ptr_map);
			pfunc->bindingAttributes = soap_binding;
		}

		if (pfunc->requestParameters) {
			pfunc->requestParameters = make_persistent_sdl_parameters(pfunc->requestParameters, ptr_map);
		}
		if (pfunc->responseParameters) {
			pfunc->responseParameters = make_persistent_sdl_parameters(pfunc->responseParameters, ptr_map);
		}
		if (pfunc->faults) {
			pfunc->faults = make_persistent_sdl_function_faults(pfunc, pfunc->faults, ptr_map);
		}
	}

	return pfunc;
}

static sdlPtr make_persistent_sdl(sdlPtr sdl TSRMLS_DC)
{
	sdlPtr psdl = NULL;
	HashTable ptr_map;
	HashTable bp_types, bp_encoders;
	ulong index;
	char *key;
	uint key_len;

	zend_hash_init(&bp_types, 0, NULL, NULL, 0);
	zend_hash_init(&bp_encoders, 0, NULL, NULL, 0);
	zend_hash_init(&ptr_map, 0, NULL, NULL, 0);

	psdl = malloc(sizeof(*sdl));
	memset(psdl, 0, sizeof(*sdl));

	if (sdl->source) {
		psdl->source = strdup(sdl->source);
	}
	if (sdl->target_ns) {
		psdl->target_ns = strdup(sdl->target_ns);
	}

	if (sdl->groups) {
		sdlTypePtr *tmp;
		sdlTypePtr ptype;

		psdl->groups = malloc(sizeof(HashTable));
		zend_hash_init(psdl->groups, zend_hash_num_elements(sdl->groups), NULL, delete_type_persistent, 1);

		zend_hash_internal_pointer_reset(sdl->groups);
		while (zend_hash_get_current_data(sdl->groups, (void **)&tmp) == SUCCESS) {
			ptype = make_persistent_sdl_type(*tmp, &ptr_map, &bp_types, &bp_encoders);
			if (zend_hash_get_current_key_ex(sdl->groups, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
				zend_hash_add(psdl->groups, key, key_len, (void*)&ptype, sizeof(sdlTypePtr), NULL);
			} else {
				zend_hash_next_index_insert(psdl->groups, (void*)&ptype, sizeof(sdlTypePtr), NULL);
			}
			zend_hash_add(&ptr_map, (char*)tmp, sizeof(*tmp), (void*)&ptype, sizeof(sdlTypePtr), NULL);
			zend_hash_move_forward(sdl->groups);
		}
	}

	if (sdl->types) {
		sdlTypePtr *tmp;
		sdlTypePtr ptype;

		psdl->types = malloc(sizeof(HashTable));
		zend_hash_init(psdl->types, zend_hash_num_elements(sdl->types), NULL, delete_type_persistent, 1);

		zend_hash_internal_pointer_reset(sdl->types);
		while (zend_hash_get_current_data(sdl->types, (void **)&tmp) == SUCCESS) {
			ptype = make_persistent_sdl_type(*tmp, &ptr_map, &bp_types, &bp_encoders);
			if (zend_hash_get_current_key_ex(sdl->types, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
				zend_hash_add(psdl->types, key, key_len, (void*)&ptype, sizeof(sdlTypePtr), NULL);
			} else {
				zend_hash_next_index_insert(psdl->types, (void*)&ptype, sizeof(sdlTypePtr), NULL);
			}
			zend_hash_add(&ptr_map, (char*)tmp, sizeof(*tmp), (void*)&ptype, sizeof(sdlTypePtr), NULL);
			zend_hash_move_forward(sdl->types);
		}
	}

	if (sdl->elements) {
		sdlTypePtr *tmp;
		sdlTypePtr ptype;

		psdl->elements = malloc(sizeof(HashTable));
		zend_hash_init(psdl->elements, zend_hash_num_elements(sdl->elements), NULL, delete_type_persistent, 1);

		zend_hash_internal_pointer_reset(sdl->elements);
		while (zend_hash_get_current_data(sdl->elements, (void **)&tmp) == SUCCESS) {
			ptype = make_persistent_sdl_type(*tmp, &ptr_map, &bp_types, &bp_encoders);
			if (zend_hash_get_current_key_ex(sdl->elements, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
				zend_hash_add(psdl->elements, key, key_len, (void*)&ptype, sizeof(sdlTypePtr), NULL);
			} else {
				zend_hash_next_index_insert(psdl->elements, (void*)&ptype, sizeof(sdlTypePtr), NULL);
			}
			zend_hash_add(&ptr_map, (char*)tmp, sizeof(*tmp), (void*)&ptype, sizeof(sdlTypePtr), NULL);
			zend_hash_move_forward(sdl->elements);
		}
	}

	if (sdl->encoders) {
		encodePtr *tmp;
		encodePtr penc;

		psdl->encoders = malloc(sizeof(HashTable));
		zend_hash_init(psdl->encoders, zend_hash_num_elements(sdl->encoders), NULL, delete_encoder_persistent, 1);

		zend_hash_internal_pointer_reset(sdl->encoders);
		while (zend_hash_get_current_data(sdl->encoders, (void **)&tmp) == SUCCESS) {
			penc = make_persistent_sdl_encoder(*tmp, &ptr_map, &bp_types, &bp_encoders);
			if (zend_hash_get_current_key_ex(sdl->encoders, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
				zend_hash_add(psdl->encoders, key, key_len, (void*)&penc, sizeof(encodePtr), NULL);
			} else {
				zend_hash_next_index_insert(psdl->encoders, (void*)&penc, sizeof(encodePtr), NULL);
			}
			zend_hash_add(&ptr_map, (char*)tmp, sizeof(*tmp), (void*)&penc, sizeof(encodePtr), NULL);
			zend_hash_move_forward(sdl->encoders);
		}
	}

	/* do backpatching here */
	if (zend_hash_num_elements(&bp_types)) {
		sdlTypePtr **tmp, *ptype = NULL;

		zend_hash_internal_pointer_reset(&bp_types);
		while (zend_hash_get_current_data(&bp_types, (void**)&tmp) == SUCCESS) {
			if (zend_hash_find(&ptr_map, (char*)(*tmp), sizeof(**tmp), (void**)&ptype) == FAILURE) {
				assert(0);
			}
			**tmp = *ptype;
			zend_hash_move_forward(&bp_types);
		}
	}
	if (zend_hash_num_elements(&bp_encoders)) {
		encodePtr **tmp, *penc = NULL;

		zend_hash_internal_pointer_reset(&bp_encoders);
		while (zend_hash_get_current_data(&bp_encoders, (void**)&tmp) == SUCCESS) {
			if (zend_hash_find(&ptr_map, (char*)(*tmp), sizeof(**tmp), (void**)&penc) == FAILURE) {
				assert(0);
			}
			**tmp = *penc;
			zend_hash_move_forward(&bp_encoders);
		}
	}


	if (sdl->bindings) {
		sdlBindingPtr *tmp;
		sdlBindingPtr pbind;

		psdl->bindings = malloc(sizeof(HashTable));
		zend_hash_init(psdl->bindings, zend_hash_num_elements(sdl->bindings), NULL, delete_binding_persistent, 1);

		zend_hash_internal_pointer_reset(sdl->bindings);
		while (zend_hash_get_current_data(sdl->bindings, (void **)&tmp) == SUCCESS) {
			pbind = make_persistent_sdl_binding(*tmp, &ptr_map);
			if (zend_hash_get_current_key_ex(sdl->bindings, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
				zend_hash_add(psdl->bindings, key, key_len, (void*)&pbind, sizeof(sdlBindingPtr), NULL);
			} else {
				zend_hash_next_index_insert(psdl->bindings, (void*)&pbind, sizeof(sdlBindingPtr), NULL);
			}
			zend_hash_add(&ptr_map, (char*)tmp, sizeof(*tmp), (void*)&pbind, sizeof(sdlBindingPtr), NULL);
			zend_hash_move_forward(sdl->bindings);
		}
	}

	zend_hash_init(&psdl->functions, zend_hash_num_elements(&sdl->functions), NULL, delete_function_persistent, 1);
	if (zend_hash_num_elements(&sdl->functions)) {
		sdlFunctionPtr *tmp;
		sdlFunctionPtr pfunc;

		zend_hash_internal_pointer_reset(&sdl->functions);
		while (zend_hash_get_current_data(&sdl->functions, (void **)&tmp) == SUCCESS) {
			pfunc = make_persistent_sdl_function(*tmp, &ptr_map);
			if (zend_hash_get_current_key_ex(&sdl->functions, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
				zend_hash_add(&psdl->functions, key, key_len, (void*)&pfunc, sizeof(sdlFunctionPtr), NULL);
			} else {
				zend_hash_next_index_insert(&psdl->functions, (void*)&pfunc, sizeof(sdlFunctionPtr), NULL);
			}
			zend_hash_add(&ptr_map, (char*)tmp, sizeof(*tmp), (void*)&pfunc, sizeof(sdlFunctionPtr), NULL);
			zend_hash_move_forward(&sdl->functions);
		}
	}

	if (sdl->requests) {
		sdlFunctionPtr *tmp;
		sdlFunctionPtr *preq;

		psdl->requests = malloc(sizeof(HashTable));
		zend_hash_init(psdl->requests, zend_hash_num_elements(sdl->requests), NULL, NULL, 1);

		zend_hash_internal_pointer_reset(sdl->requests);
		while (zend_hash_get_current_data(sdl->requests, (void **)&tmp) == SUCCESS) {
			if (zend_hash_find(&ptr_map, (char*)tmp, sizeof(*tmp), (void**)&preq) == FAILURE) {
				assert(0);
			}
			*tmp = *preq;
			if (zend_hash_get_current_key_ex(sdl->requests, &key, &key_len, &index, 0, NULL) == HASH_KEY_IS_STRING) {
				zend_hash_add(psdl->requests, key, key_len, (void*)&preq, sizeof(sdlFunctionPtr), NULL);
			}
			zend_hash_move_forward(sdl->requests);
		}
	}

	zend_hash_destroy(&ptr_map);
	zend_hash_destroy(&bp_encoders);
	zend_hash_destroy(&bp_types);

	return psdl;
}

typedef struct _sdl_cache_bucket {
	sdlPtr sdl;
	time_t time;
} sdl_cache_bucket;

static void delete_psdl(void *data)
{
	sdl_cache_bucket *p = (sdl_cache_bucket*)data;
	sdlPtr tmp = p->sdl;

	zend_hash_destroy(&tmp->functions);
	if (tmp->source) {
		free(tmp->source);
	}
	if (tmp->target_ns) {
		free(tmp->target_ns);
	}
	if (tmp->elements) {
		zend_hash_destroy(tmp->elements);
		free(tmp->elements);
	}
	if (tmp->encoders) {
		zend_hash_destroy(tmp->encoders);
		free(tmp->encoders);
	}
	if (tmp->types) {
		zend_hash_destroy(tmp->types);
		free(tmp->types);
	}
	if (tmp->groups) {
		zend_hash_destroy(tmp->groups);
		free(tmp->groups);
	}
	if (tmp->bindings) {
		zend_hash_destroy(tmp->bindings);
		free(tmp->bindings);
	}
	if (tmp->requests) {
		zend_hash_destroy(tmp->requests);
		free(tmp->requests);
	}
	free(tmp);
}

sdlPtr get_sdl(zval *this_ptr, char *uri, long cache_wsdl TSRMLS_DC)
{
	char  fn[MAXPATHLEN];
	sdlPtr sdl = NULL;
	char* old_error_code = SOAP_GLOBAL(error_code);
	int uri_len = 0;
	php_stream_context *context=NULL;
	zval **tmp, **proxy_host, **proxy_port, *orig_context = NULL, *new_context = NULL;
	smart_str headers = {0};
	char* key = NULL;
	time_t t = time(0);

	if (strchr(uri,':') != NULL || IS_ABSOLUTE_PATH(uri, uri_len)) {
		uri_len = strlen(uri);
	} else if (VCWD_REALPATH(uri, fn) == NULL) {
		cache_wsdl = WSDL_CACHE_NONE;
	} else {
		uri = fn;
		uri_len = strlen(uri);
	}

	if ((cache_wsdl & WSDL_CACHE_MEMORY) && SOAP_GLOBAL(mem_cache)) {
		sdl_cache_bucket *p;

		if (SUCCESS == zend_hash_find(SOAP_GLOBAL(mem_cache), uri, uri_len+1, (void*)&p)) {
			if (p->time < t - SOAP_GLOBAL(cache_ttl)) {
				/* in-memory cache entry is expired */
				zend_hash_del(&EG(persistent_list), uri, uri_len+1);
			} else {
				return p->sdl;
			}
		}
	}

	if ((cache_wsdl & WSDL_CACHE_DISK) && (uri_len < MAXPATHLEN)) {
		time_t t = time(0);
		char md5str[33];
		PHP_MD5_CTX context;
		unsigned char digest[16];
		int len = strlen(SOAP_GLOBAL(cache_dir));
		time_t cached;

		md5str[0] = '\0';
		PHP_MD5Init(&context);
		PHP_MD5Update(&context, (unsigned char*)uri, uri_len);
		PHP_MD5Final(digest, &context);
		make_digest(md5str, digest);
		key = emalloc(len+sizeof("/wsdl-")-1+sizeof(md5str));
		memcpy(key,SOAP_GLOBAL(cache_dir),len);
		memcpy(key+len,"/wsdl-",sizeof("/wsdl-")-1);
		memcpy(key+len+sizeof("/wsdl-")-1,md5str,sizeof(md5str));

		if ((sdl = get_sdl_from_cache(key, uri, t-SOAP_GLOBAL(cache_ttl), &cached TSRMLS_CC)) != NULL) {
			t = cached;
			efree(key);
			goto cache_in_memory;
		}
	}

	if (SUCCESS == zend_hash_find(Z_OBJPROP_P(this_ptr),
			"_stream_context", sizeof("_stream_context"), (void**)&tmp)) {
		context = php_stream_context_from_zval(*tmp, 0);
	}

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_proxy_host", sizeof("_proxy_host"), (void **) &proxy_host) == SUCCESS &&
	    Z_TYPE_PP(proxy_host) == IS_STRING &&
	    zend_hash_find(Z_OBJPROP_P(this_ptr), "_proxy_port", sizeof("_proxy_port"), (void **) &proxy_port) == SUCCESS &&
	    Z_TYPE_PP(proxy_port) == IS_LONG) {
	    	zval str_port, *str_proxy;
	    	smart_str proxy = {0};
		str_port = **proxy_port;
		zval_copy_ctor(&str_port);
		convert_to_string(&str_port);
		smart_str_appends(&proxy,"tcp://");
		smart_str_appends(&proxy,Z_STRVAL_PP(proxy_host));
		smart_str_appends(&proxy,":");
		smart_str_appends(&proxy,Z_STRVAL(str_port));
		smart_str_0(&proxy);
		zval_dtor(&str_port);
		MAKE_STD_ZVAL(str_proxy);
		ZVAL_STRING(str_proxy, proxy.c, 1);
		smart_str_free(&proxy);
		
		if (!context) {
			context = php_stream_context_alloc();
		}
		php_stream_context_set_option(context, "http", "proxy", str_proxy);
		zval_ptr_dtor(&str_proxy);

		MAKE_STD_ZVAL(str_proxy);
		ZVAL_BOOL(str_proxy, 1);
		php_stream_context_set_option(context, "http", "request_fulluri", str_proxy);
		zval_ptr_dtor(&str_proxy);

		proxy_authentication(this_ptr, &headers TSRMLS_CC);
	}

	basic_authentication(this_ptr, &headers TSRMLS_CC);

	if (headers.len > 0) {
		zval *str_headers;

		if (!context) {
			context = php_stream_context_alloc();
		}

		smart_str_0(&headers);
		MAKE_STD_ZVAL(str_headers);
		ZVAL_STRING(str_headers, headers.c, 1);
		php_stream_context_set_option(context, "http", "header", str_headers);
		smart_str_free(&headers);
		zval_ptr_dtor(&str_headers);
	}

	if (context) {
		MAKE_STD_ZVAL(new_context);
		php_stream_context_to_zval(context, new_context);
		orig_context = php_libxml_switch_context(new_context TSRMLS_CC);
	}

	SOAP_GLOBAL(error_code) = "WSDL";

	sdl = load_wsdl(this_ptr, uri TSRMLS_CC);
	if (sdl) {
		sdl->is_persistent = 0;
	}

	SOAP_GLOBAL(error_code) = old_error_code;

	if (context) {
		php_libxml_switch_context(orig_context TSRMLS_CC);
		zval_ptr_dtor(&new_context);
	}

	if ((cache_wsdl & WSDL_CACHE_DISK) && key) {
		if (sdl) {
			add_sdl_to_cache(key, uri, t, sdl TSRMLS_CC);
		}
		efree(key);
	}

cache_in_memory:
	if (cache_wsdl & WSDL_CACHE_MEMORY) {
		if (sdl) {
			sdlPtr psdl;
			sdl_cache_bucket p;

			if (SOAP_GLOBAL(mem_cache) == NULL) {
				SOAP_GLOBAL(mem_cache) = malloc(sizeof(HashTable));
				zend_hash_init(SOAP_GLOBAL(mem_cache), 0, NULL, delete_psdl, 1);
			} else if (SOAP_GLOBAL(cache_limit) > 0 &&
			           SOAP_GLOBAL(cache_limit) <= zend_hash_num_elements(SOAP_GLOBAL(mem_cache))) {
				/* in-memory cache overflow */
				sdl_cache_bucket *q;
				HashPosition pos;
				time_t latest = t;
				char *key = NULL;
				uint key_len;
				ulong idx;

				for (zend_hash_internal_pointer_reset_ex(SOAP_GLOBAL(mem_cache), &pos);
					 zend_hash_get_current_data_ex(SOAP_GLOBAL(mem_cache), (void **) &q, &pos) == SUCCESS;
					 zend_hash_move_forward_ex(SOAP_GLOBAL(mem_cache), &pos)) {
					if (q->time < latest) {
						latest = q->time;
						zend_hash_get_current_key_ex(SOAP_GLOBAL(mem_cache), &key, &key_len, &idx, 0, &pos);
					}
				}
				if (key) {
					zend_hash_del(SOAP_GLOBAL(mem_cache), key, key_len);
				} else {
					return sdl;
				}
			}

			psdl = make_persistent_sdl(sdl TSRMLS_CC);
			psdl->is_persistent = 1;
			p.time = t;
			p.sdl = psdl;

			if (SUCCESS == zend_hash_update(SOAP_GLOBAL(mem_cache), uri,
											uri_len+1, (void*)&p, sizeof(sdl_cache_bucket), NULL)) {
				/* remove non-persitent sdl structure */
				delete_sdl_impl(sdl);
				/* and replace it with persistent one */
				sdl = psdl;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to register persistent entry");
				/* clean up persistent sdl */
				delete_psdl(&p);
				/* keep non-persistent sdl and return it */
			}
		}
	}

	return sdl;
}

/* Deletes */
void delete_sdl_impl(void *handle)
{
	sdlPtr tmp = (sdlPtr)handle;

	zend_hash_destroy(&tmp->functions);
	if (tmp->source) {
		efree(tmp->source);
	}
	if (tmp->target_ns) {
		efree(tmp->target_ns);
	}
	if (tmp->elements) {
		zend_hash_destroy(tmp->elements);
		efree(tmp->elements);
	}
	if (tmp->encoders) {
		zend_hash_destroy(tmp->encoders);
		efree(tmp->encoders);
	}
	if (tmp->types) {
		zend_hash_destroy(tmp->types);
		efree(tmp->types);
	}
	if (tmp->groups) {
		zend_hash_destroy(tmp->groups);
		efree(tmp->groups);
	}
	if (tmp->bindings) {
		zend_hash_destroy(tmp->bindings);
		efree(tmp->bindings);
	}
	if (tmp->requests) {
		zend_hash_destroy(tmp->requests);
		efree(tmp->requests);
	}
	efree(tmp);
}

void delete_sdl(void *handle)
{
	sdlPtr tmp = (sdlPtr)handle;

	if (!tmp->is_persistent) {
		delete_sdl_impl(tmp);
	}
}

static void delete_binding(void *data)
{
	sdlBindingPtr binding = *((sdlBindingPtr*)data);

	if (binding->location) {
		efree(binding->location);
	}
	if (binding->name) {
		efree(binding->name);
	}

	if (binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingPtr soapBind = binding->bindingAttributes;
		if (soapBind) {
			efree(soapBind);
		}
	}
	efree(binding);
}

static void delete_binding_persistent(void *data)
{
	sdlBindingPtr binding = *((sdlBindingPtr*)data);

	if (binding->location) {
		free(binding->location);
	}
	if (binding->name) {
		free(binding->name);
	}

	if (binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingPtr soapBind = binding->bindingAttributes;
		if (soapBind) {
			free(soapBind);
		}
	}
	free(binding);
}

static void delete_sdl_soap_binding_function_body(sdlSoapBindingFunctionBody body)
{
	if (body.ns) {
		efree(body.ns);
	}
	if (body.headers) {
		zend_hash_destroy(body.headers);
		efree(body.headers);
	}
}

static void delete_sdl_soap_binding_function_body_persistent(sdlSoapBindingFunctionBody body)
{
	if (body.ns) {
		free(body.ns);
	}
	if (body.headers) {
		zend_hash_destroy(body.headers);
		free(body.headers);
	}
}

static void delete_function(void *data)
{
	sdlFunctionPtr function = *((sdlFunctionPtr*)data);

	if (function->functionName) {
		efree(function->functionName);
	}
	if (function->requestName) {
		efree(function->requestName);
	}
	if (function->responseName) {
		efree(function->responseName);
	}
	if (function->requestParameters) {
		zend_hash_destroy(function->requestParameters);
		efree(function->requestParameters);
	}
	if (function->responseParameters) {
		zend_hash_destroy(function->responseParameters);
		efree(function->responseParameters);
	}
	if (function->faults) {
		zend_hash_destroy(function->faults);
		efree(function->faults);
	}

	if (function->bindingAttributes &&
	    function->binding && function->binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingFunctionPtr soapFunction = function->bindingAttributes;
		if (soapFunction->soapAction) {
			efree(soapFunction->soapAction);
		}
		delete_sdl_soap_binding_function_body(soapFunction->input);
		delete_sdl_soap_binding_function_body(soapFunction->output);
		efree(soapFunction);
	}
	efree(function);
}

static void delete_function_persistent(void *data)
{
	sdlFunctionPtr function = *((sdlFunctionPtr*)data);

	if (function->functionName) {
		free(function->functionName);
	}
	if (function->requestName) {
		free(function->requestName);
	}
	if (function->responseName) {
		free(function->responseName);
	}
	if (function->requestParameters) {
		zend_hash_destroy(function->requestParameters);
		free(function->requestParameters);
	}
	if (function->responseParameters) {
		zend_hash_destroy(function->responseParameters);
		free(function->responseParameters);
	}
	if (function->faults) {
		zend_hash_destroy(function->faults);
		free(function->faults);
	}

	if (function->bindingAttributes &&
	    function->binding && function->binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingFunctionPtr soapFunction = function->bindingAttributes;
		if (soapFunction->soapAction) {
			free(soapFunction->soapAction);
		}
		delete_sdl_soap_binding_function_body_persistent(soapFunction->input);
		delete_sdl_soap_binding_function_body_persistent(soapFunction->output);
		free(soapFunction);
	}
	free(function);
}

static void delete_parameter(void *data)
{
	sdlParamPtr param = *((sdlParamPtr*)data);
	if (param->paramName) {
		efree(param->paramName);
	}
	efree(param);
}

static void delete_parameter_persistent(void *data)
{
	sdlParamPtr param = *((sdlParamPtr*)data);
	if (param->paramName) {
		free(param->paramName);
	}
	free(param);
}

static void delete_header(void *data)
{
	sdlSoapBindingFunctionHeaderPtr hdr = *((sdlSoapBindingFunctionHeaderPtr*)data);
	if (hdr->name) {
		efree(hdr->name);
	}
	if (hdr->ns) {
		efree(hdr->ns);
	}
	if (hdr->headerfaults) {
		zend_hash_destroy(hdr->headerfaults);
		efree(hdr->headerfaults);
	}
	efree(hdr);
}

static void delete_header_persistent(void *data)
{
	sdlSoapBindingFunctionHeaderPtr hdr = *((sdlSoapBindingFunctionHeaderPtr*)data);
	if (hdr->name) {
		free(hdr->name);
	}
	if (hdr->ns) {
		free(hdr->ns);
	}
	if (hdr->headerfaults) {
		zend_hash_destroy(hdr->headerfaults);
		free(hdr->headerfaults);
	}
	free(hdr);
}

static void delete_fault(void *data)
{
	sdlFaultPtr fault = *((sdlFaultPtr*)data);
	if (fault->name) {
		efree(fault->name);
	}
	if (fault->details) {
		zend_hash_destroy(fault->details);
		efree(fault->details);
	}
	if (fault->bindingAttributes) {
		sdlSoapBindingFunctionFaultPtr binding = (sdlSoapBindingFunctionFaultPtr)fault->bindingAttributes;

		if (binding->ns) {
			efree(binding->ns);
		}
		efree(fault->bindingAttributes);
	}
	efree(fault);
}

static void delete_fault_persistent(void *data)
{
	sdlFaultPtr fault = *((sdlFaultPtr*)data);
	if (fault->name) {
		free(fault->name);
	}
	if (fault->details) {
		zend_hash_destroy(fault->details);
		free(fault->details);
	}
	if (fault->bindingAttributes) {
		sdlSoapBindingFunctionFaultPtr binding = (sdlSoapBindingFunctionFaultPtr)fault->bindingAttributes;

		if (binding->ns) {
			free(binding->ns);
		}
		free(fault->bindingAttributes);
	}
	free(fault);
}

static void delete_document(void *doc_ptr)
{
	xmlDocPtr doc = *((xmlDocPtr*)doc_ptr);
	xmlFreeDoc(doc);
}

