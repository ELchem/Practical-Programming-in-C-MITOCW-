/*
 * Decoder.c
 *
 *  Created on: Jan 17, 2021
 *      Author: Dell
 */

#include "HuffmanEncoder.h"


void build_tree(char * file)
{
	char	symbol;
	char	strcode[MAX_LEN], line[MAX_LEN+3];
	int		items_read;
	int		len;
	struct	tnode* curr=NULL;
	FILE * fp = fopen(file,"r");
	root = talloc(0,0);
	while(fgets(line,MAX_LEN+3,fp))
	{
		symbol = line[0];
		if(symbol == '\n')
		{
			fgets(line,MAX_LEN+3,fp);
			strcpy(strcode,&line[1]);
		}
		else
			strcpy(strcode,&line[2]);
		printf("build tree %c: %s\n",symbol,strcode);
		curr=root;
		len=strlen(strcode);
		for(int i=0; i<len; i++)
		{
			if (strcode[i] == '1')
			{
				if (!(curr->right))
				{
					curr->isleaf = 0;
					curr->right = talloc(0,0);
					(curr->right)->parent = curr;
					curr = curr->right;
				}
				else
					curr = curr->right;
			}
			else if (strcode[i] == '0')
			{
				if(!(curr->left))
				{
					curr->isleaf = 0;
					curr->left = talloc(0,0);
					(curr->left)->parent = curr;
					curr = curr->left;
				}
				else
					curr = curr->left;
			}
		}
		/*assign code*/
		curr->isleaf=1;
		curr->symbol=symbol;
//		printf("inserted symbol:%c\n",symbol);

	}
	fclose(fp);
}

/*
	function decode
*/
void decode(char * encoded_file, char * decoded_file)
{
	char c;
	struct tnode* curr=root;
	FILE * fin = fopen(encoded_file, "r");
	FILE * fout = fopen(decoded_file,"w");
	while((c=getc(fin))!=EOF)
	{
		/*TODO:
			traverse the tree
			print the symbols only if you encounter a leaf node
		*/
		if (c == '1')
		{
			if (((curr->right)->isleaf) == 1)
			{
				fputc((curr->right)->symbol,fout);
				curr = root;
			}
			else
				curr = curr->right;
		}
		else if (c == '0')
		{
			if (((curr->left)->isleaf) == 1)
			{
				fputc((curr->left)->symbol,fout);
				curr = root;
			}
			else
				curr = curr->left;
		}
	}
	fclose(fin);
	fclose(fout);
}


void freetree(struct tnode* r)
{
	if(r==NULL)
		return;
	freetree(r->right);
	freetree(r->left);
	free(r);
	r = NULL;
}

void Verify (FILE * forig, char * decoded)
{
	rewind(forig);
	char c, d;
	FILE * fdecode = fopen(decoded,"r");
	while((c = fgetc(fdecode)) != EOF)
	{
		d = fgetc(forig);
		if(c != d)
		{
				printf("Unexpected result: Decoded book does not match original in while.\n");
				return;
		}
	}
	d = fgetc(forig);
	if (d == EOF)
	{
		printf("Decoded book matches original.\nEncoded file, keyfile and decoded file stored in program directory. Program success.\n");
		return;
	}
	else
	{
		printf("Unexpected result: Decoded book does not match original.\n");
		return;
	}
	fclose(fdecode);
}
