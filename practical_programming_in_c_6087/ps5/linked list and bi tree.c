/*
 * ps5.c
 *
 *  Created on: Jan 6, 2020
 *      Author: Dell
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//linked list structure and methods
struct node
{
	int data;
	struct node *next;
};

struct node* nalloc(int data)
{

	struct node *p = (struct node*)malloc(sizeof(struct node));
	if (p != NULL)
	{
		p -> data = data;
		p -> next = NULL;
	}
	return p;
}
struct node* addfront(struct node* head, int data)
{
	struct node *p = nalloc(data);
	if (p == NULL) return head;
	p -> next = head;
	return p;
}

void display_list(struct node* head)
{
		printf("\n");
		do{
			printf("%d\n",head->data);
			head = head->next;
		}while(head->next != NULL);
		printf("%d\n",head->data);
}

struct node* addback(struct node* head, int data)
{
	struct node *p = nalloc(data);
	while((head->next) != NULL)
	{
		head = head->next;
	}
	head->next = p;
	p->next = NULL;
	return p;
}

struct node* find_node(struct node* head, int search_value)
{
	while(head->data != search_value)
	{
		head = head->next;
		if (head == NULL)
		{
			printf("Value you searched for not found.\n");
			return head; //returns NULL
		}

	}
	printf("Value found in linked list.\n");
	return head; //returns node containing the searched data
}
struct node* del_node(struct node* head, int data)
{
	struct node* pelement = find_node(head,data);
	struct node* temp = head;
	if(head == pelement)
	{
		head = head->next;
		free(pelement);
		return head;
	}
	while (temp->next != pelement)
	{
		if(!(temp->next))
		{
			printf("Item not found in list.\n");
			return head;
		}
		temp = temp->next;
	}
	temp->next = pelement->next;
	free(pelement);
	return head;
}

void del_list(struct node* head)
{
	struct node* temp;
	while(head != NULL)
	{
		 temp = head->next;
		 free(head);
		 head = temp;
	}
	free(head);
	free(temp);
}

void illustrate_list()
{
	struct node * head = NULL;
	char str[100];
	char * tok;
	printf("Enter a string of numbers separated by spaces and they will be stored in a linked list\n");
	fflush(stdout);
	gets(str);
	tok = strtok(str," ");
	head = nalloc(atoi(tok));
	while((tok = strtok(NULL," ")))
	{
		addback(head, atoi((tok)));
	}
	printf("You entered: \n");
	display_list(head);
	printf("\nEnter a value in the list you would like deleted\nor type 'end' to delete the whole list.");
	fflush(stdout);
	gets(str);
	while (1)
	{
		if(!strcmp("end",str))
		{
			del_list(head);
			printf("List deleted.\n");
			return;
		}
		head = del_node(head, atoi(str));
		printf("Remaining List:\n");
		display_list(head);
		printf("Enter a value to delete from list or type 'end' to delete whole list.\n");
		fflush(stdout);
		gets(str);
	}
}
//binary tree
struct tnode
{
	int data;
	struct tnode* left;
	struct tnode* right;
};
struct tnode* talloc(int data)
{
	struct tnode *p = (struct tnode*)malloc(sizeof(struct tnode));
	if (p != NULL)
	{
		p -> data = data;
		p -> left = NULL;
		p -> right = NULL;
	}
	return p;
}
struct tnode* addnode(struct tnode* root, int data)
{
	struct tnode* p=NULL;
	if (root==NULL)
	{
		p = talloc(data);
		return p;
	}
	else if (data<=root->data)
		root->left=addnode(root->left,data);
	else
		root->right=addnode(root->right,data);
	return root;
}
void preorder_print(struct tnode* root)
{
	if(root == NULL)
		return;
	printf("%d\n",root->data);
	preorder_print(root->left);
	preorder_print(root->right);
	return;
}
void inorder_print(struct tnode* root)
{
	if (root->left==NULL)
	{
		printf("%d\n",root->data);
	}
	else
	{
		inorder_print(root->left);
		printf("%d\n",root->data);
	}
	if (root->right==NULL)
	{
		return;
	}
	else
	{
		inorder_print(root->right);
		return;
	}


}
int main()
{
	illustrate_list();
	printf("Illustration of binary tree containing {3,6,0,2,8,6,5,9}.\n");
	struct tnode* root = NULL;
	int arr[8] = {3,6,0,2,8,6,5,9};
	root = addnode(root,arr[0]);
	for (int i = 1; i<8; i++)
	{
		addnode(root,arr[i]);
	}
	printf("preorder print binary tree.\n");
	preorder_print(root);
	printf("inorder print binary tree.\n");
	inorder_print(root);
	return 0;
}



