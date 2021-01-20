/*
 * prob2.c
 *
 *  Created on:
 *      Author:
 */

/* header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define UCHAR_MAX 256
/* the trie node data structure */
struct s_trie_node
{
	char * translation; /* NULL if node not a word */

	/* pointer array to child nodes */
	struct s_trie_node * children[UCHAR_MAX+1];
};

struct s_trie_node_test
{
	char *translation;

	struct s_trie_node * children[150];
};

/* pointer to the root node of the trie structure */
static struct s_trie_node * proot = NULL;
//static struct s_trie_node * null_list[UCHAR_MAX+1];

/* helper functions for trie structure */

/* allocate new node on the heap
   output: pointer to new node (must be freed) */
struct s_trie_node * new_node(void);

/* delete node and all its children
   input: pointer to node to delete
   postcondition: node and children are freed */
void delete_node(struct s_trie_node * pnode);

/* add word to trie, with translation
   input: word and translation
   output: non-zero if new node added, zero otherwise
   postcondition: word exists in trie */
int add_word(const char * word, char * translation);

/* read dictionary file into trie structure */
unsigned int load_dictionary(const char * filename);

/* search trie structure for word and return translations
   input: word to search
   output: translation, or NULL if not found */
char * lookup_word(const char * word);

/* maximum number of characters for word to search */
#define WORD_MAX 256
/* maximum number of characters in line */
#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

/* main function */
//int main(int argc, char * argv[]) {
int main(int argc, char * argv[]){
	char word[WORD_MAX], * translation;
	int len;
	if (argc <= 1)
	{
		printf("no dictionary specified.");
		return 0; /* no dictionary specified */
	}

	/* load dictionary */

	proot = new_node();
	load_dictionary(argv[1]);
	do {
		printf("Enter word to translate: ");
		fflush(stdout);
		if (!fgets(word, WORD_MAX, stdin))
			abort();
		if(!strcmp(word,"exit dictionary\n"))
			break;
		/* strip trailing newline */
		len = strlen(word);
		if (len > 0 && word[len-1] == '\n') {
			word[len-1] = '\0';
			--len;
		}

		/* exit on empty string */
		if (len == 0)
			break;

		/* lookup word */
		translation = lookup_word(word);
		//printf("%lu\n",sizeof(struct s_trie_node_test));
		if (translation)
			printf("%s -> %s\n", word, translation);
		else
			printf("\"%s\" not found\n", word);
		printf("To exit, type 'exit dictionary' into search.\n");
	} while (1);

	/* clean up trie structure */
	delete_node(proot);
	printf("Trie successfully deleted.\n");

	return 0;
}

/* allocate new node on the heap
   output: pointer to new node (must be freed) */
struct s_trie_node * new_node(void) {
	/* TODO: allocate a new node on the heap, and
	   initialize all fields to default values */
	struct s_trie_node * new_node = (struct s_trie_node *)malloc(sizeof(struct s_trie_node));
	//new_node->translation = NULL;
	new_node->translation = NULL;
	for (int i = 0; i < (UCHAR_MAX+1); i++)
	{
		(new_node->children)[i] = NULL;
	}
	return new_node;
}

/* delete node and all its children
   input: pointer to node to delete
   postcondition: node and children are freed */
void delete_node(struct s_trie_node * pnode) {
	/* TODO: delete node and all its children
	   Be sure to free non-null translations!
	   Hint: use recursion
	 */
	if (!pnode)
		return;
	for(int i = 0; i < UCHAR_MAX; i++)
	{
		delete_node(pnode->children[i]);
	}
	free(pnode->translation);
	free(pnode);
	}

/* add word to trie, with translation
   input: word and translation
   output: non-zero if new node added, zero otherwise
   postcondition: word exists in trie */
int add_word(const char * word, char * translation) {
	/* TODO: add word to trie structure
	   If word exists, append translation to existing
	   string
	   Be sure to store a copy of translation, since
	   the string is reused by load_dictionary()
	 */
	//printf("%s\n",word);
	//printf("%s\n",translation);
	int count = 0; //count for strlen
	struct s_trie_node *ptr;
	ptr = proot;
	while(count < (strlen(word)))
	{
		if ((ptr->children)[(unsigned char)(word[count])] == NULL)
		{
			(ptr->children)[(unsigned char)(word[count])] = new_node();
			ptr = (ptr->children)[(unsigned char)(word[count])];
			//printf("%s\n",(ptr->translation));
		}
		else
		{
			//printf("%p\n",(ptr->children)[(word[count])]);
			ptr = (ptr->children)[(unsigned char)(word[count])];

		}
		//word++; //increment word to next letter
		count++; //increment count to next letter
	}
	int count2 = 0;
	int str_length = strlen(translation);
	ptr->translation = (char *)malloc(sizeof(char)*(str_length+1));
	while (count2 < str_length)
	{
	(ptr->translation)[count2] = *translation;
	translation++;
	count2++;
	}
	//printf("%lu\n",sizeof(*ptr));
	return 1;
}

/* delimiter for dictionary */
#define DELIMS "\t"

/* read dictionary file into trie structure */
unsigned int load_dictionary(const char * filename) {
	FILE * pfile;
	unsigned char line[LINE_MAX], * word, * translation;
	unsigned long int icount = 0;

	/* ensure file can be opened */
	if ( !(pfile = fopen(filename,"r")) )
	{
		printf("%s\n",filename);
		return icount;
	}
	long int x = 0;
	/* read lines */
	while (fgets(line, LINE_MAX, pfile)) {
		/* strip trailing newline */
		int len = strlen(line);
		if (len > 0 && line[len-1] == '\n') {
			line[len-1] = '\0';
			--len;
		}

		if (len == 0 || line[0] == '#')
			continue; /* ignore comment/empty lines */

		/* separate word and translation */
		word = line + strspn(line, DELIMS);
		if ( !word[0] )
			continue; /* no word in line */
		translation = word + strcspn(word, DELIMS);
		*translation++ = '\0';
		translation += strspn(translation, DELIMS);

		//add word to trie
		//printf("%s\t%s\n",word, translation);
		if (add_word(word, translation))
			icount++;
	}
	fclose(pfile);
	return icount;
}

/* search trie structure for word and return translations
   input: word to search
   output: translation, or NULL if not found */
char * lookup_word(const char * word) {
	/* TODO: search trie structure for word
	   return NULL if word is not found */
	int count = 0; //count of worldlen
	struct s_trie_node *ptr;
	ptr = proot;
	//printf("%s\n",word);
	while (count < (strlen(word)))
	{
		if ((ptr->children)[(word[count])] != NULL)
		{
			ptr = (ptr->children)[(word[count])];

		}
		else
		{
			return NULL;
		}
		count++; //increment count to next letter
	}
	return ptr->translation;
}



