/*
 * HashTable.c
 *
 *  Created on: Mar 8, 2020
 *      Author: Dell
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_BUCKETS 1000
#define MULTIPLIER 31
#define MAX_LEN    100

struct wordrec
{
  char* word;
  unsigned long count;
  struct wordrec* next;
};

/*
  @function walloc
  @desc     produces a new node
*/
struct wordrec* walloc(const char* str)
{
  struct wordrec* p=(struct wordrec*)malloc(sizeof(struct wordrec));
  if(p!=NULL)
  {
      p->count=0;
      p->word=strdup(str); /*creates a duplicate*/
      p->next=NULL;
  }
  return p;
}

/*hash bucket*/
struct wordrec* table[MAX_BUCKETS]; /* changed from MAX_LEN -- dsw */

/*
  @function hashstring
  @desc     produces hash code for a string
            multipliers 31,35 have been found to work well
*/
unsigned long hashstring(const char* str)
{
  unsigned long hash=0;
  while(*str)
    {
      hash= hash*MULTIPLIER+*str;
      str++;
    }
  return hash%MAX_BUCKETS;
}


/*
  @function lookup
  @desc     returns a pointer to the word or creates
            it if required
*/
struct wordrec*  lookup(const char* str,int create)
{
  unsigned long hash=hashstring(str);/*starting point*/
  struct wordrec* wp=table[hash];
  struct wordrec* curr=NULL;
  /*TODO: write code to
  follow the linked list to find str
  if found return pointer*/
  while(wp != NULL)
  {
	  if (!(strcmp(wp->word,str)))
		  return wp;
	  else
		  wp = wp->next;
  }

  /*if not found and create specified*/
   wp = table[hash];
   if(create)
    {
	   if (wp == NULL)
	   {
		   table[hash] = walloc(str);
		   curr = table[hash];
	   }
	   else
	   {
		   while(wp->next != NULL)
			   wp = wp->next;
		   wp->next = walloc(str);
		   curr = wp->next;
	   }
      /*TODO:write code to
       create new node
      update linked list*/
    }
  return curr;
}

/*
  @function cleartable()
  @desc     reclaims memory
*/
void cleartable()
{
  struct wordrec* wp=NULL,*p=NULL;
  int i=0;
  /*TODO: write code to
    reclaim memory
  */
  for (;i<MAX_BUCKETS;i++)
  {
	  wp = table[i];
	  p = wp->next;
	  free(wp);
	  while(p != NULL)
	  {
		  wp = p->next;
		  free(p);
		  p = wp;
	  }
  }
}

int main(int argc,char* argv[])
{
	//reads through book.txt file and counts how many times a given spelling of a word or prefix appears.
	//uses a basic hash table for efficient lookup of words.
  FILE* fp=fopen("C:\\Users\\Dell\\Desktop\\MIT_OCW\\practical_programming_in_c_6087\\ps6b\\HashTable\\book.txt","r");
  char  word[1024]; /*big enough*/
  struct wordrec* wp=NULL;
  int i=0;
  memset(table,0,sizeof(table));
  if (!fp)
  {
	  printf("file not found.\n");
	  return 0;
  }
  /*read from input*/
  while(1)
  {
    if(fscanf(fp,"%s",word)!=1)
      break;
    wp=lookup(word,1); /*create if doesn't exist*/
    wp->count++;
  }
  fclose(fp);

  /*
    print all words have frequency>100
   */
for(i=0;i<MAX_BUCKETS;i++)
    {
      for(wp=table[i];wp!=NULL;wp=wp->next)
	{
	  if(wp->count>100)
	    {
	      printf("%s-->%ld\n",wp->word,wp->count);
	    }
	}
    }
  cleartable();
  return 0;
}


