/*
 * hash.h
 *
 * This module provides a hash function based on the function described
 * in:
 *      Fast Hashing of Variable-Length Text Strings
 *      Peter K. Pearson
 *      CACM 33(6), June 1990.
 *
 * Initial Creation, 6/25/92 TJHARVEY
 * modified for use in the ILOC subset parser, 10/31/00 Todd Waterman
 */

#ifndef _HASH_H_
#define _HASH_H_
#define HASH_SIZE 256

unsigned int hash(char *key_string);

#endif /* ifndef _HASH_H_ */
