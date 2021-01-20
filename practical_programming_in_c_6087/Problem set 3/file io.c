/*
 * prob5.c
 *
 *  Created on: Nov 23, 2019
 *      Author: Dell
 */
#include <stdio.h>
#include <string.h>

int main()
{
	char str[] = "rip pop";
	FILE *fp;
	fp = fopen("C:\\Users\\Dell\\Desktop\\MIT_OCW\\practical_programming_in_c_6087\\Problem set 3\\stateoutflow0708.txt", "r");
	if (fp == NULL)
	{
		printf("file not found");
		return 0;
	}

	char sco[20];	//State_Code_Origin
	char sn[20];	 //and StateName(recieving state)
	int aagi, total = 0;	//aggregate_AGI and total emigration

	fscanf(fp, "%s", sco);
	while (strcmp(sco, "\"25\"") != 0)	//while not at massachucetts block
		fscanf(fp, "%*s %*s %*s %*s %*s %*s %*s %*s %s", sco);
	while (strcmp(sco,"\"25\"") == 0)	//while in massachucetts block
	{
		fscanf(fp, "%*s %*s %*s %*s %s %*s %*s %d %s",sn,&aagi,sco);	//capture variables
		printf("%s\t%d\n",sn,aagi);	//print recieving state and emigration number
		total += aagi;	//print total emigration
	}
	printf("Total = %d\n",total);
	fclose(fp);
	int count = 0;
	char * ptr;
	ptr = str;
	while(count < strlen(str)){
		printf("%c\n",*ptr);
		count++;
		ptr++;
	}
	return 0;
}

