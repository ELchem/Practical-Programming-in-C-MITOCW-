/*
 * HuffmanEncoder.h
 *
 *  Created on: Jan 17, 2021
 *      Author: Dell
 */

#ifndef HUFFMANENCODER_H_
#define HUFFMANENCODER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASCII_EXT_CHARS 256
#define MAX_SYMBOLS 256
#define MAX_LEN     50

struct tnode
{
    struct  tnode* left; /*used when in tree*/
    struct  tnode*right; /*used when in tree*/
    struct  tnode* next; /*used when in list*/
    struct  tnode*parent;
    float   freq;
    int     isleaf;
    unsigned char    symbol;

};


//code[x][y] where x is character and y is string of binary code for x.
char code[MAX_SYMBOLS][MAX_LEN];
//Root of Huffman Tree
extern struct tnode * root;

void freetree(struct tnode* root);
void decode(char * efile, char * dfile);
void build_tree(char * file);
struct tnode* talloc(int symbol,float freq);
#endif /* HUFFMANENCODER_H_ */
