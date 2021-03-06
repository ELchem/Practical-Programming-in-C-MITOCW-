/*
 * sqlprojLibrary.c
 *
 *  Created on: Mar 26, 2020
 *      Author: Dell
 */
/*
 * sqlproj.c
 *
 *  Created on: Mar 14, 2020
 *      Author: Dell
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* the include file for the SQLite library */
/* do not use sqlite.h or sqlite3ext.h */
#include "sqlite3.h"
#include "sqlprojLibrary.h"

/* T defines minimum number of children in non-root nodes
 * 2*T is maximum number of children in all nodes
 * 2*T-1 is maximum number of keys in all nodes */
#define T 3

typedef char * nodekey;

/* record structure */
typedef struct s_record {
	unsigned int irecord; /* index of record in main database */
	char * name; /* movie name used as key */
	char * category;
	unsigned int year;
	char * format;
	char * language;
	char * url;
} nodevalue;

/* B-tree node structure */
typedef struct s_tnode {
	nodekey keys[2*T-1]; /* keys */
	nodevalue * values[2*T-1]; /* values */
	unsigned int nkeys; /* number of keys, < 2*T */

	struct s_tnode * parent; /* pointer to parent */
	struct s_tnode * children[2*T]; /* pointers to children */
} * p_tnode;

/* static variables for the root of the tree and number of records */
static p_tnode ptreeroot = NULL;
static unsigned int record_count = 0;

/* alloc_record() - allocate a new record on the heap */
struct s_record * alloc_record() {
	struct s_record * precord = (struct s_record *)malloc(sizeof(struct s_record));
	precord->irecord = 0;
	precord->name = NULL; /* movie name used as key */
	precord->category = NULL;
	precord->year = 0;
	precord->format = NULL;
	precord->language = NULL;
	precord->url = NULL;
	return precord;
}

/* free_record() - frees the record structure and associated strings */
void free_record(struct s_record * precord) {
	if (precord->name) free(precord->name);
	if (precord->category) free(precord->category);
	if (precord->format) free(precord->format);
	if (precord->language) free(precord->language);
	if (precord->url) free(precord->url);
	free(precord);
}

/* display_record -- output record information to file pointer fp */
void display_record(struct s_record * precord, FILE * fp) {
	fprintf(fp, "[#%d] %s (%d): %s, %s, %s, <%s>\n", precord->irecord, precord->name, precord->year, precord->category, precord->format, precord->language, precord->url);
}

/* alloc_tnode() - creates a new B-tree node on the heap */
p_tnode alloc_tnode(void) {
	int n;
	p_tnode pnode = (p_tnode)malloc(sizeof(struct s_tnode));
	pnode->nkeys = 0;
	for (n = 0; n < 2*T; n++)
		pnode->children[n] = NULL;
	pnode->parent = NULL;
	return pnode;
}

/* free_tnode() - frees a node in the B-tree,
 * its associated record, and all its children from memory
 */
void free_tnode(p_tnode pnode) {
	unsigned int n;
	for (n = 0; n < 2*T; n++) {
		if (pnode->children[n] != NULL)
			free_tnode(pnode->children[n]);
	}

	for (n = 0; n < pnode->nkeys; n++) {
		if (pnode->values[n])
			free_record(pnode->values[n]);
	}

	free(pnode);
}

/* function for comparing two keys; simply calls the case-insensitive
 * string comparison function strcasecmp() declared in string.h.
 * Returns zero if both equal, positive if key1 > key2, negative if key1 < key2
 */
int key_compare(const nodekey key1, const nodekey key2) {
	return strcasecmp(key1, key2);
}

/* find_index() - performs binary search of list of keys in node
 * to find either the index of the existing key, or the insertion
 * point of a new key. Returns nonnegative index of insertion point
 * if new key, or -(index+1) for existing key found at index
 *
 * TODO: fill in the binary search while loop
 */
int find_index(nodekey key, p_tnode pnode) {
	/* find in between */
	int icmp, L = 0, R = (pnode->nkeys)-1, M;
	int ibetween = 0; /* index to return */

	/* TODO: complete binary search;
	 * use key_compare() to compare two keys. */
	while (L <= R) {
		M = (R + L)/2;
		icmp = key_compare(key,pnode->keys[M]);
		if(icmp > 0)
		{
			L = M+1;
			continue;
		}
		else if(icmp < 0)
		{
			R = M-1;
			continue;
		}
		else
		{
			ibetween = -(M + 1);
			return ibetween;
		}
	}
	if (key_compare(key,pnode->keys[M]) > 0)
		ibetween = (M+1);
	else
		ibetween = M;
	return ibetween;
	/*int icmp,i;
	for (i = 0; i < pnode->nkeys; i++){
	icmp = key_compare(key,pnode->keys[i]);
	if (icmp > 0)
		continue;
	else if (icmp < 0)
		return i;
	else if (icmp == 0)
		return -(i + 1);
	}
	return i;*/
}

/* split_node() - splits a full node in the B-tree into two separate nodes,
 * possibly creating a new root node in the process
 * should be no need to modify this function
 */
void split_node(p_tnode * ppnode, int * poffset) {
	p_tnode pnode = *ppnode;
	int median = pnode->nkeys>>1, i;
	p_tnode parent = pnode->parent, split = alloc_tnode();

	if (poffset != NULL) {
		/* update offset index and ppnode to point to new insertion point */
		if (*poffset > median) {
			/* move to new (split) node */
			*poffset -= (median+1);
			*ppnode = split;
		}
	}
	if (parent) {
		int insert = find_index(pnode->keys[median],parent);

		/* move median into parent */
		for (i = parent->nkeys; i > insert; i--) {
			parent->keys[i] = parent->keys[i-1];
			parent->values[i] = parent->values[i-1];
			parent->children[i+1] = parent->children[i];
		}
		parent->keys[insert] = pnode->keys[median];
		parent->values[insert] = pnode->values[median];
		parent->children[insert] = pnode;
		parent->nkeys++;

		/* move half from pnode into new node */
		for (i = median + 1; i < pnode->nkeys; i++) {
			split->keys[i-(median+1)] = pnode->keys[i];
			split->values[i-(median+1)] = pnode->values[i];
		}
		for (i = median + 1; i < pnode->nkeys+1; i++) {
			split->children[i-(median+1)] = pnode->children[i];
			if (pnode->children[i] != NULL)
				pnode->children[i]->parent = split;
			pnode->children[i] = NULL;
		}
		split->nkeys = pnode->nkeys - (median+1);
		pnode->nkeys = median;
		parent->children[insert+1] = split;
		split->parent = parent;
	} else {
		/* split root */
		parent = ptreeroot = alloc_tnode();
		parent->keys[0] = pnode->keys[median];
		parent->values[0] = pnode->values[median];
		parent->children[0] = pnode;
		parent->nkeys = 1;
		pnode->parent = parent;

		/* move half from pnode into new node */
		for (i = median + 1; i < pnode->nkeys; i++) {
			split->keys[i-(median+1)] = pnode->keys[i];
			split->values[i-(median+1)] = pnode->values[i];
		}
		for (i = median + 1; i < pnode->nkeys+1; i++) {
			split->children[i-(median+1)] = pnode->children[i];
			if (pnode->children[i] != NULL)
				pnode->children[i]->parent = split;
			pnode->children[i] = NULL;
		}
		split->nkeys = pnode->nkeys - (median+1);
		pnode->nkeys = median;
		parent->children[1] = split;
		split->parent = parent;
	}
}

/* add_element() - add the record with the specified key to the B-tree.
 * returns a pointer to an already existing record with the same key,
 * in which case, the new record was not inserted, or NULL on success
 * should be no need to modify this function
 */
nodevalue * add_element(nodekey key, nodevalue * pvalue) {
	/* find leaf */
	p_tnode pleaf = ptreeroot, parent = NULL;
	int ichild, i;

	while (pleaf != NULL) {
		ichild = find_index(key, pleaf);
		if (ichild < 0) /* already exists */
			return pleaf->values[-(ichild+1)];
		if (pleaf->nkeys == 2*T-1) {
			/* split leaf into two nodes */
			split_node(&pleaf, &ichild);
		}
		parent = pleaf;
		pleaf = pleaf->children[ichild];
	}

	pleaf = parent;
	record_count++; /* record actually added to tree */

	/* enough room, just add to leaf */
	for (i = pleaf->nkeys; i > ichild; i--) {
		pleaf->keys[i] = pleaf->keys[i-1];
		pleaf->values[i] = pleaf->values[i-1];
	}
	pleaf->keys[ichild] = key;
	pleaf->values[ichild] = pvalue;
	pleaf->nkeys++;

	//printf("added element");
	return NULL;
}

/* inorder_traversal() - traverse B-tree, in sorted order
 * similar to binary search tree traversal (except nodes have multiple values and children)
 * writes record info to file pointer fp
 *
 * TODO: fill in this function
 */
void inorder_traversal(p_tnode pnode, FILE * fp) {
	int n;
	for (n = 0; n <= pnode->nkeys; n++) {
		/* TODO: traverse children and keys, in order */
		if (pnode->children[n] != NULL)
		{
			inorder_traversal(pnode->children[n],fp);
			fputs(pnode->keys[n],fp);
			fputs("\n",fp);
		}
		else if (pnode->children[n] == NULL && n == pnode->nkeys - 1)
		{
			fputs(pnode->keys[n],fp);
			fputs("\n",fp);
			return;
		}
		else
		{
			fputs(pnode->keys[n],fp);
			fputs("\n",fp);
		}
	}
}

/* find_value() - locate value for specified key in B-tree
 * need to return pointer to record structure, or NULL if record not found
 * TODO: fill in this function
 */
nodevalue * find_value(const nodekey key) {
	int ichild, n, cmp, flag = 1;
	p_tnode pleaf = ptreeroot; /* start at root */

	/* TODO: iterate pleaf down to leaves of tree, or until key is found */
	while(pleaf != NULL)
	{
		//for (int x = 0; x < pleaf->nkeys; x++)
			//printf("pleaf->key[%d] = %s\n",x,pleaf->keys[x]);
		//printf("\n");
		for (n = 0; n < pleaf->nkeys; n++)
		{
			cmp = key_compare(key,pleaf->keys[n]);
			//printf("cmp = %d, pleaf->key[%d] = %s, key = %s\n",cmp,n,pleaf->keys[n], key);
			if(0 == strcmp(key,pleaf->keys[n]))
			{
				//printf("pleaf->keys[%d] = %s\nkey = %s\n",n,pleaf->keys[n],key);
				return pleaf->values[n];
			}
			else if(cmp > 0)
			{
				//printf("cmp > 0: %s\n",pleaf->keys[n]);
				continue;
			}
			else if(cmp < 0)
			{
				//printf("cmp < 0: %s\n",pleaf->keys[n]);
				pleaf = pleaf->children[n];
				flag = 0;
				break;
			}
		}
		if (flag)
		{
			pleaf = pleaf->children[n];
		}
		flag = 1;
	}
	return NULL; /* didn't find it */
}

/* display_result() - print information from the database
 * this function is a valid callback for use with sqlite3_exec()
 * pextra is unused
 *
 * needs to return zero (nonzero aborts SQL query)
 */
int display_result(void * pextra, int nfields, char ** arrvalues, char ** arrfieldnames) {
	int n;

	for (n = 0; n < nfields; n++) {
		printf("%s: [%s] ", arrfieldnames[n], arrvalues[n]);
	}
	printf("\n");
	return 0;
}

/* store_result() - store record from database in B-tree
 * this function is also a valid callback for use with sqlite3_exec()
 * pextra is again unused
 *
 * needs to return zero (nonzero aborts SQL query)
 */
int store_result(void * pextra, int nfields, char ** arrvalues, char ** arrfieldnames) {
	int n;

	/* allocate record on heap */
	struct s_record * prec = alloc_record();

	prec->irecord = record_count+1;
	for (n = 0; n < nfields; n++) {
		if (strcasecmp(arrfieldnames[n], "MovieTitle") == 0)
			prec->name = strdup(arrvalues[n]); /* key */
		else if (strcasecmp(arrfieldnames[n], "MovieCategory") == 0)
			prec->category = strdup(arrvalues[n]);
		else if (strcasecmp(arrfieldnames[n], "ProductionYear") == 0)
			prec->year = atoi(arrvalues[n]);
		else if (strcasecmp(arrfieldnames[n], "Format") == 0)
			prec->format = strdup(arrvalues[n]);
		else if (strcasecmp(arrfieldnames[n], "Language") == 0)
			prec->language = strdup(arrvalues[n]);
		else if (strcasecmp(arrfieldnames[n], "Web") == 0)
			prec->url = strdup(arrvalues[n]);
	}
	/*Set root*/
	if (ptreeroot == NULL)
	{
		ptreeroot = alloc_tnode();
		ptreeroot->keys[0] = prec->name;
		ptreeroot->values[0] = prec;
		ptreeroot->nkeys = 1;
		return 0;
	}
	/* add record to B-tree */
	if (add_element(prec->name, prec) != NULL) {
		/* element already exists -- don't add record */
		printf("Duplicate record exists: "); /* diagnostic value only */
		display_record(prec, stdout);
		free_record(prec);
	}

	return 0;
}


static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);

   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }

   printf("\n");
   return 0;
}

/*Start of functions used in SqlProjD.c*/
int initialize_db (const char *filename){
	sqlite3 *pSQL;
	int return_message;
	char *zErrMsg = 0;
	const char* data = "Callback function called";
	char *sqlC;
	//
	//open db file
	//
	return_message = sqlite3_open(filename,&pSQL);
	if (return_message == SQLITE_OK)
	{
		printf("Database file opened successfully.\n");
	}
	else
	{
		printf("Database file failed to open.\n");
		return 0;
	}
	//
	//read in data from db
	//
	sqlC = "SELECT * from MOVIES";
	return_message = sqlite3_exec(pSQL, sqlC, store_result, (void*)data, &zErrMsg);
	if (return_message != SQLITE_OK)
	{
	  fprintf(stderr, "SQL error: %s\n", zErrMsg);
	  sqlite3_free(zErrMsg);
	}
	else
	{
		printf("sqlite3_exec success\n");
	}
	//
	//close db file
	//
	return_message = sqlite3_close(pSQL);
	if(return_message == SQLITE_OK)
	{
		printf("close successful\n");
	}
	else
	{
		printf("error at close.");
	}
	return 0;
}
int locate_movie(const char * title)
{
	if(find_value(title))
		return 1;
	else
		return 0;
}
void dump_sorted_list(const char * filename)
{
	FILE *fp;
	fp = fopen(filename,"w+");
	inorder_traversal(ptreeroot,fp);
	if(!fclose(fp))
		printf("Write file .txt closed successfully.\n");
}
void cleanup()
{
	free_tnode(ptreeroot);
}
//int main(int argc, char * argv[]) {
//int main(){
	/* part (a): execute the first three SQL queries */
	//const char sql[] = "SELECT * FROM movies WHERE ProductionYear < 1950";
	/* const char sql[] = "SELECT * FROM movies WHERE Format == \"VHS\""; */
	/* const char sql[] = "SELECT * FROM movies WHERE Language == \"Spanish\""; */

	/* part (b, c, d): use this SQL query to read the entire table */
	/* const char sql[] = "SELECT * FROM movies"; */

	/*if (argc < 2) {
		fprintf(stderr,"Error: database name not specified!\n");
		return 1;
	}
	//char dbname[] = "C:\\Users\\Dell\\Desktop\\MIT_OCW\\practical_programming_in_c_6087\\database_ps\\movies.db";
	//char writefile[] = "C:\\Users\\Dell\\Desktop\\MIT_OCW\\practical_programming_in_c_6087\\database_ps\\sorted_movies.txt";
	FILE *fp;
	sqlite3 *pSQL;
	int return_message;
	char *zErrMsg = 0;
	const char* data = "Callback function called";
	char *sqlC;
	return_message = sqlite3_open(argv[1],&pSQL);
	//return_message = sqlite3_open(dbname,&pSQL);
	if (return_message == SQLITE_OK)
	{
		printf("Database file opened successfully.\n");
	}
	else
	{
		printf("Database file failed to open.\n");
		return 0;
	}
	sqlC = "SELECT * from MOVIES";
	return_message = sqlite3_exec(pSQL, sqlC, store_result, (void*)data, &zErrMsg);
	if (return_message != SQLITE_OK)
	{
	  fprintf(stderr, "SQL error: %s\n", zErrMsg);
	  sqlite3_free(zErrMsg);
	}
	else
	{
		printf("sqlite3_exec success\n");
	}
	//open write file//
	fp = fopen(argv[2],"w+");
	//fp = fopen(writefile,"w+");
	inorder_traversal(ptreeroot,fp);
	if(!fclose(fp))
		printf("Write file .txt closed successfully.\n");
	char str[50];
	int len;
	nodevalue * x;

	setbuf(stdout,NULL);
	printf("Enter value to search btree for :");
	while(*(fgets(str,50,stdin)) != '\n')
	{
		len = strlen(str);
		if (str[len-1] == '\n')
			str[len-1] = '\0';
		x = find_value(str);
		if(x)
			printf("\n%s was found in the tree\nEnter Value: ",x->name);
		else
			printf("not found\nEnter value: ");
	}


	return_message = sqlite3_close(pSQL);
	if(return_message == SQLITE_OK)
	{
		printf("close successful\n");
	}
	else
	{
		printf("error at close.");
	}
	return 0;
}*/

