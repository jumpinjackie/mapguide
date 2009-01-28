//  Copyright (C) 2003-2009 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE CORRECTNESS
//  OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE IT. AUTODESK
//  PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY DISCLAIMS ANY
//  LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//

///////////////////////////////////////////////////////////////////////////////////////////////
// password.h
//
// Encryption/password support implemented by Jeffrey Klug, as per .ZIP File Format
// Specification Version: 4.5 at http://www.pkware.com/products/enterprise/white_papers/appnote.html
//
// The encryption used in PKZIP was generously supplied by Roger Schlafly. PKWARE
// is grateful to Mr. Schlafly for his expert help and advice in the field of data encryption.
//
// PKZIP encrypts the compressed data stream. Encrypted files must be decrypted before
// they can be extracted.
//
// Each encrypted file has an extra 12 bytes stored at the start of the data area defining
// the encryption header for that file. The encryption header is originally set to random
// values, and then itself encrypted, using three, 32-bit keys. The key values are initialized
// using the supplied encryption password. After each byte is encrypted, the keys are then
// updated using pseudo-random number generation techniques in combination with the same
// CRC-32 algorithm used in PKZIP and described elsewhere in this document.
//
// The following is the basic steps required to decrypt a file:
//
//   Initialize the three 32-bit keys with the password.
//   Read and decrypt the 12-byte encryption header, further initializing the encryption keys.
//   Read and decrypt the compressed data stream using the encryption keys.

#ifndef _password_H
#define _password_H

#ifdef __cplusplus
extern "C" {
#endif

ZEXTERN void ZEXPORT init_keys OF((const char* password, unsigned long *key));
ZEXTERN void ZEXPORT decrypt_buffer OF((char *buffer, unsigned long size, unsigned long *key));
void update_keys(char ch, unsigned long *key);
char decrypt_byte(unsigned long *key);
unsigned long decrypt_crc32(unsigned long l, char c);
void encrypt_header(long iCrc, char *buf, unsigned long *key);
char encrypt_byte(char c, unsigned long *key);
void encrypt_buffer(char *buf, unsigned long size, unsigned long *key);

#ifdef __cplusplus
}
#endif

#endif /* _zip_H */
