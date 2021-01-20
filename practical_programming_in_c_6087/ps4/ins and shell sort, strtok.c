/*
 * ps4_1.c
 *
 *  Created on: Dec 1, 2019
 *      Author: Dell
 */

#include <stdio.h>
#include <stdlib.h>


int arr[100];

void shift_element(unsigned int i)
{
	int ivalue;
	for (ivalue = arr[i]; i && arr[i-1] > ivalue; i--)
		arr[i] = arr[i-1];
	arr[i] = ivalue;
}
void insertion_sort(void)
{
	unsigned int i, len = 5;
	for (i = 1; i < len; i++)
	{
		if (arr[i] < arr[i-1])
			shift_element(i);
	}

}
void pointer_func(int *ptr)
{
	for (int i = 0; i < 5; i++)
	{
		*ptr = *ptr + 1;
		ptr++;
	}
}
void shift_element_ptr(int *ptr)
{
	int ptr_value;
	for (ptr_value = *ptr; (ptr != arr) && *(ptr - 1) > ptr_value; ptr--)
		*ptr = *(ptr - 1);
	*ptr = ptr_value;
}
void print_list()
{
	for (int i = 0; i < 100; i++)
		printf("%d \n",arr[i]);
	printf("\n");
}
void insertion_sort_ptr(void)
{
	unsigned int len = 5, i;
	int *ptr;
	ptr = arr;
	for (i = 1; i < len; i++)
	{
		if((*(ptr + 1)) < (*ptr))
			shift_element_ptr((ptr + 1));
		ptr++;
	}

}

//problem 2 strtok functions

int strpos ( const char *str , const char ch )
{
	int index = 0;
	while (*str != '\0')
	{
		if (*str == ch)
			return index;
		else
		{
			index++;
			str++;
		}
	}
	return -1;
}
unsigned int strspn ( const char *str, const char * delims )
{
	int index = 0;
	while ((strpos(delims, *str) != -1) && (*str != '\0'))
	{
		str++;
		index++;
	}
	return index;
}

unsigned int strcspn ( const char *str, const char *delims )
{
	int index = 0;
	while ((strpos(delims, *str) == -1) && (*str != '\0'))
	{
		str++;
		index++;
	}
	return index;
}
char *strtok (char *text , const char *delims )
{
	static char *pnexttoken;
	if (!text)
		text = pnexttoken;
	text += strspn(text,delims);
	//printf("%s \n",text);
	if (*text == '\0')
		return NULL;
	pnexttoken = text + strcspn(text,delims);
	if (pnexttoken != '\0')
		*pnexttoken++ = '\0';
	return text;
}

//problem 3 shell sort functions

void shift_element_by_gap(unsigned int i, unsigned int gap)
{
	int ivalue;
	for (ivalue = arr[i]; i && arr[i-gap] > ivalue; i = i - gap)
		arr[i] = arr[i-gap];
	arr[i] = ivalue;
}

void shell_sort(void)
{
	unsigned int gap, i, len = 100;
	for (gap = len/2; gap > 0; gap /= 2)
	{
		for(i = gap; i < len;i += gap)
		{
			if(arr[i] < arr[i - gap])
				shift_element_by_gap(i,gap);
		}
	}

}
int main(void)
{
	char string[100];
	char delims[100];
	printf("Enter a string you want me to tokenize. When done, hit enter or EOF.\n");
	fflush(stdout);
	gets(string);
	printf("Now enter a set of delimiters to divide the string by (eg. space, tab, period)\n");
	fflush(stdout);
	gets(delims);
	char *x = strtok(string,delims);
	while (x != NULL)
	{
		printf("%s \n",x);
		x = strtok(NULL, delims);
	}

// problem 3 shell sort//
	printf("Press enter and I will sort 100 random numbers using shell sort.\n");
	fflush(stdout);
	gets(string);
	for (int i = 0; i < 100; i++)
	{
		arr[i] = rand();
	}
	shell_sort();
	print_list();
	return 0;
}
