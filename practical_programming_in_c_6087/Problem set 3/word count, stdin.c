/*
 * prob4.c
 *
 *  Created on: Nov 17, 2019
 *      Author: Dell
 */


#include <stdio.h>
#include <stdlib.h>

int main(int argc,char* argv[])
{
    FILE* fp=NULL;
    int   nfiles =--argc; //ignore the name of the program itself
    int   argidx =1;      //ignore the name of the program itself
    char* currfile="";
    char  c;
    //count of words,lines,characters
    unsigned long nw=0,nl=0,nc=0;
    int char_flag = 0;

    if(nfiles==0)
    {
        char noFileOP[] = "stdin";
        currfile = noFileOP;
        printf("Enter a sentence and I will return # of chars\nafter I read EOF.\n");
        fflush(stdout);
        fp=stdin; //standard input
        nfiles++;
    }
    else //set to first
    {
        currfile=argv[argidx++];
        fp=fopen(currfile,"r");
    }
    while(nfiles>0) //files left >0
    {
       if(fp==NULL)
        {
            fprintf(stderr,"Unable to open input\n");
            exit(-1);
        }
        nc=nw=nl=0;
        while((c=getc(fp))!=EOF)
        {
	        //TODO:FILL HERE
		    //process the file using getc(fp)

        	if (c == ' ' || c == '\t' || c=='\n')
        	{
        		if (c == '\n')
        			nl++;
        		if (char_flag)
        		{
        			char_flag = 0;
        			nw++;
        		}
        		else
        			continue;
        	}
        	else
        	{
        		nc++;
        		char_flag = 1;
        	}
        }
        printf("\nThere were %ld chars entered in %s\n",nc,currfile);
        //next file if exists
        nfiles--;
        if(nfiles>0)
        {
            currfile=argv[argidx++];
            fp     =fopen(currfile,"r");
        }
    }
	return 0;
}


