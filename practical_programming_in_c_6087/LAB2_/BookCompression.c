/*
 * BookCompression.c
 *
 *  Created on: Jan 16, 2021
 *      Author: Dell
 */
#include "HuffmanEncoder.h"

//GLOBALS
int char_counter[ASCII_EXT_CHARS];
int TOTAL_CHARS = 0;
int num_unique_chars = 0;

//Head of Priority Queue
struct tnode* qhead = NULL;
struct tnode * root = NULL;

struct tnode* talloc(int symbol,float freq)
{
    struct tnode* p=(struct tnode*)malloc(sizeof(struct tnode));
    if(p!=NULL)
    {
        p->left=p->right=p->parent=p->next=NULL;
        p->symbol=symbol;
        p->freq=freq;
		p->isleaf=1;
    }
    return p;
}

//Priority Queue Insert Function
void pq_insert(struct tnode* p)
{
	struct tnode* curr=NULL;
	struct tnode* prev=NULL;
	printf("inserting:%c,%f\n",p->symbol,p->freq);
	if(qhead==NULL) /*qhead is null*/
	{
	   qhead = p;
	   return;
	}
	curr = qhead;
	while(curr && (p->freq > curr->freq))
	{
	   prev = curr;
	   curr = curr->next;
	}
	if(curr==qhead)
	{
	   p->next = qhead;
	   qhead = p;
	}
	else /*insert between prev and next*/
	{
	   prev->next = p;
	   p->next = curr;
	}
}

struct tnode* pq_pop()
{
    struct tnode* p=NULL;
    p = qhead;
    if (!qhead)
    	return p;
    qhead = qhead->next;
    p->next = NULL;
    return p;
}

void generate_code(struct tnode* r,int depth)
{
	int symbol;
	int len; /*length of code*/
	if(r->isleaf)
	{
//		printf("gen code found: %c at depth %d\n",r->symbol, depth);
//		return;
		symbol=r->symbol;
		len = depth;
		/*start backwards*/
		code[symbol][len]=0;
		for (int x = 1; x<=len; x++)
		{
			if((r->parent)->right == r)
			{
				code[symbol][len-x] = '1';
				r = r->parent;
			}
			else if((r->parent)->left == r)
			{
				code[symbol][len-x] = '0';
				r = r->parent;
			}
			else
			{
				printf("generate_code encountered error with pointer comparison.\n");
				break;
			}
		}
		printf("built code:%c,%s\n",symbol,code[symbol]);
	}
	else
	{
		generate_code(r->left,depth+1);
		generate_code(r->right,depth+1);
	}

}

void generate_tree()
{
    struct tnode* p=NULL;
    struct tnode* lc,*rc;
	int i;
	/*build tree*/
	for(i=0;i<num_unique_chars-1;i++)
	{
		lc=pq_pop();
		rc=pq_pop();
		/*create parent*/
		p=talloc(0,lc->freq+rc->freq);
		/*set parent link*/
		lc->parent=rc->parent=p;
		/*set child link*/
		p->right=rc; p->left=lc;
		/*make it non-leaf*/
		p->isleaf=0;
		/*add the new node to the queue*/
		pq_insert(p);
	}
	/*get root*/
	root=pq_pop();
}

void CountCharacters(FILE * fp)
{
	unsigned int c;
	for (int i = 0; i < ASCII_EXT_CHARS; i++)
	char_counter[i] = 0;

	if (!fp)
	{
		printf("could not open book file!\n");
		return;
	}
	while((c = fgetc(fp)) != EOF)
	{
		char_counter[c]++;
		TOTAL_CHARS++;
	}
}

void CalcFreq(float * freq)
{
	for (int i = 0; i < ASCII_EXT_CHARS; i++)
	{
		freq[i] = ((float)char_counter[i]/TOTAL_CHARS);
	}
}

void CreatePriorityQueue(float * freq)
{
	for(int i=0;i<ASCII_EXT_CHARS;i++)
    {
    	if (freq[i])
    	{
    		pq_insert(talloc(i,freq[i]));
    		num_unique_chars++;
    	}
    	else
    		continue;
    }
}

void encode(FILE * fin,char * encode_file)
{
	FILE * fout = fopen(encode_file,"w");
	unsigned int c;
	rewind(fin);
	while((c = fgetc(fin)) != EOF)
	{
		fprintf(fout,"%s",code[c]);
	}
	fclose(fout);
}

void dump_code(char * key_file)
{
	FILE * fp = fopen(key_file, "w");
	int i=0;
	for(i=0;i<MAX_SYMBOLS;i++)
	{
		if(code[i][0]) /*non empty*/
			fprintf(fp,"%c %s\n",i,code[i]);
	}
	fclose(fp);
}
void printCode()
{
	for (int i = 0; i<ASCII_EXT_CHARS-1;i++)
	{
		printf("%c : %s\n",i,code[i]);
	}
}
void initCodeZero()
{
	for(int i = 0; i<ASCII_EXT_CHARS-1; i++)
	memset(code[i],0,sizeof(MAX_LEN));//set code array to zeroes
}

int main(){
	float freq[ASCII_EXT_CHARS];//stores freq of occur of each char
	initCodeZero();


 	FILE * fp;
	char book[] = "C:\\Users\\Dell\\Desktop\\MIT_OCW\\practical_programming_in_c_6087\\LAB2_\\book.txt";
	char encoded_book[] = "C:\\Users\\Dell\\Desktop\\MIT_OCW\\practical_programming_in_c_6087\\LAB2_\\encoded_book2.txt";
	char key_file[] = "C:\\Users\\Dell\\Desktop\\MIT_OCW\\practical_programming_in_c_6087\\LAB2_\\keyfile2.txt";
	char decoded_book[] = "C:\\Users\\Dell\\Desktop\\MIT_OCW\\practical_programming_in_c_6087\\LAB2_\\decoded_book2.txt";
	fp = fopen(book,"r");

	CountCharacters(fp);//counts characters in txt file and stores results in globals char_counter[] and TOTAL_CHARS
	CalcFreq(freq); //calcs frequency of each char in txt file
	CreatePriorityQueue(freq); //creates priority queue from Struct TNodes. Highest freq chars in back, removed last.
	generate_tree(); //generates Huffman encoding binary tree from TNodes in priority queue
	generate_code(root,0); //generates code by walking down binary tree
	encode(fp, encoded_book); //creates new file and encodes "book" in Huffman Code
	dump_code(key_file); //writes character-key pairs into a keyfile to allow book to be decoded

	/*Demonstration of decode section below*/
	freetree(root); //clears existing binary tree
	build_tree(key_file); //rebuilds tree from key file
	decode(encoded_book, decoded_book); //reads encoded book and decodes it
	Verify(fp, decoded_book); //verifies that the decoded version matches the original
	fclose(fp);
	return 0;
}

