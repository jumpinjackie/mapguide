/*
 * Copyright 2007, Lloyd Hilaiel.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 * 
 *  3. Neither the name of Lloyd Hilaiel nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */ 

#include "yajl_common.h"

#ifndef __YAJL_PARSE_H__
#define __YAJL_PARSE_H__

#ifdef __cplusplus
extern "C" {
#endif    
    /** error codes returned from this interface */
    typedef enum {
        yajl_status_ok,
        yajl_status_client_canceled,
        yajl_status_insufficient_data,
        yajl_status_error
    } yajl_status;

    /** attain a human readable, english, string for an error */
    const char * YAJL_API yajl_status_to_string(yajl_status code);

    /** an opaque handle to a parser */
    typedef struct yajl_handle_t * yajl_handle;

    /** yajl is an event driven parser.  this means as json elements are
     *  parsed, you are called back to do something with the data.  The
     *  functions in this table indicate the various events for which
     *  you will be called back.  Each callback accepts a "context"
     *  pointer, this is a void * that is passed into the yajl_parse
     *  function which the client code may use to pass around context.
     *
     *  All callbacks return an integer.  If non-zero, the parse will
     *  continue.  If zero, the parse will be canceled and
     *  yajl_status_client_canceled will be returned from the parse.
     */
    typedef struct {
        int (* yajl_null)(void * ctx);
        int (* yajl_boolean)(void * ctx, int boolVal);
        int (* yajl_integer)(void * ctx, long integerVal);
        int (* yajl_double)(void * ctx, double doubleVal);
        /* strings are returned as pointers into the JSON text when,
         * possible, they are not null padded */
        int (* yajl_string)(void * ctx, const unsigned char * stringVal,
                            unsigned int stringLen);

        int (* yajl_start_map)(void * ctx);
        int (* yajl_map_key)(void * ctx, const unsigned char * key,
                             unsigned int stringLen);
        int (* yajl_end_map)(void * ctx);

        int (* yajl_start_array)(void * ctx);
        int (* yajl_end_array)(void * ctx);
    } yajl_callbacks;
    
    /** configuration structure for the generator */
    typedef struct {
        /** if nonzero, javascript style comments will be allowed in
         *  the json input, both slash star and slash slash */
        unsigned int allowComments;
    } yajl_parser_config;

    /** allocate a parser handle
     *  \param callbacks  a yajl callbacks structure specifying the
     *                    functions to call when different JSON entities
     *                    are encountered in the input text.  May be NULL,
     *                    which is only useful for validation.
     *  \param config     configuration parameters for the parse.
     *  \param ctx        a context pointer that will be passed to callbacks.
     */
    yajl_handle YAJL_API yajl_alloc(const yajl_callbacks * callbacks,
                                    const yajl_parser_config * config,
                                    void * ctx);

    /** free a parser handle */    
    void YAJL_API yajl_free(yajl_handle handle);

    /** parse some json! */
    yajl_status YAJL_API yajl_parse(yajl_handle hand,
                                    const unsigned char * jsonText,
                                    unsigned int jsonTextLength);

    /** get an error string describing the state of the
     *  parse.
     *
     *  If verbose is non-zero, the message will include the JSON
     *  text where the error occured, along with an arrow pointing to
     *  the specific char.
     *
     *  A dynamically allocated string will be returned which should
     *  be freed with yajl_free_error 
     */
    unsigned char * YAJL_API yajl_get_error(yajl_handle hand, int verbose,
                                            const unsigned char * jsonText,
                                            unsigned int jsonTextLength);

    void YAJL_API yajl_free_error(unsigned char * str);

#ifdef __cplusplus
};
#endif    

#endif
