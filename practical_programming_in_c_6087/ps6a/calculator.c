/*
 * ps6.c
 *
 *  Created on: Jan 26, 2020
 *      Author: Dell
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* maximum length of input string (including newline character) */
#define INPUT_MAX 2048

/* enable (1) or disable (0) parentheses checking in parsing strings */
/* leave disabled for part (a); enable for part (b) */
#define PARSE_PARENS 1

/* type of token */
enum token_type {
	OPERAND, /* number */
	OPERATOR, /* operator: +, -, *, / */
#if PARSE_PARENS
	LPARENS, /* left parentheses ( */
	RPARENS /* right parentheses ) */
#endif
};

/* operator identifiers (opcodes) */
enum op {
	ADD, /* a+b */
	SUBTRACT, /* a-b (binary) */
	MULTIPLY, /* a*b */
	DIVIDE, /* a/b */
	NEGATE /* -a (unary) */
};

/* direction of evaluation (associativity) */
enum assoc {
	LEFT, /* left-to-right (+, binary -, *, /) */
	RIGHT /* right-to-left (unary -) */
};

/* number of operands for each operator type */
const unsigned int op_operands[] = {2, 2, 2, 2, 1};

/* order-of-operations (precedence) (0 = evaluated last) */
const int op_precedences[] = {0, 0, 1, 1, 2, -1};

/* evaluation direction (associativity) for each precedence level */
const enum assoc op_associativity[] = {LEFT, LEFT, RIGHT};

/* contains value of token */
union token_value {
	double operand; /* numeric value for operand */
	enum op op_code; /* opcode for operators */
};

/* data structure for token */
typedef struct s_expr_token {
	union token_value value; /* numeric value or opcode */
	enum token_type type; /* type of token */

	struct s_expr_token * linked_token; /* linked token in stack/queue */
} * p_expr_token; /* p_expr_token is shorthand for "struct s_expr_token *" */

/* data structure for queue */
struct token_queue {
	p_expr_token front; /* front of queue, where tokens are dequeued */
	p_expr_token back; /* back of queue, where tokens are added */
};

/* queue functions - enqueue and dequeue */
void enqueue(struct token_queue * pqueue, const p_expr_token ptoken);
p_expr_token dequeue(struct token_queue * pqueue);

/* stack functions - push and pop */
void push(p_expr_token * ptop, const p_expr_token ptoken);
p_expr_token pop(p_expr_token * ptop);

/* creates a new token in dynamic memory (using malloc()) */
p_expr_token new_token(const enum token_type type, const union token_value value);

/* constructs a queue of tokens in infix order from a space-delimited string */
struct token_queue expr_to_infix(char * str);

/* creates a queue of tokens in postfix order from a queue of tokens in infix order */
/* postcondition: returned queue contains all the tokens, and pqueue_infix should be
   empty */
struct token_queue infix_to_postfix(struct token_queue * pqueue_infix);

/* evalutes the postfix expression stored in the queue */
/* postcondition: returned value is final answer, and pqueue_postfix should be empty */
double evaluate_postfix(struct token_queue * pqueue_postfix);

/* handles evaluation process (calls above functions) for expression string str */
double evaluate(const char * str);

int main(void) {
	char input[INPUT_MAX];
	double ans;
	unsigned int len;
	printf("Enter an expression to evaluate below\n(Note: You must put a space between each\nterm of expression eg. ( 4 * 6 ))\n");
	do {
		printf("Enter an expression:");
		fflush(stdout);
		if (!fgets(input, INPUT_MAX, stdin))
			abort(); /* failed to read stdin */

		len = strlen(input);
		/* remove trailing newline character */
		if (len > 0 && input[len-1] == '\n') {
			input[len-1] = '\0';
			--len;
		}
		if (len == 0) /* empty expression signals exit */
			break;

		/* call evaluation functions */
		ans = evaluate(input);

		/* write result to stdout */
		printf("%s => %g\n", input, ans);
	} while (1);

	return 0;
}

/* enqueue (add) token to end of queue
   input: pqueue - pointer to queue
          ptoken - token pointer to add
   postcondition: token added to end of queue */
void enqueue(struct token_queue * pqueue, const p_expr_token ptoken) {
	ptoken->linked_token = NULL;
	if (pqueue->back)
		pqueue->back->linked_token = ptoken;
	else /* empty */
		pqueue->front = ptoken;
	pqueue->back = ptoken;
}

/* dequeue (remove) token from front of queue
   input: pointer to queue
   output: front token pointer (or NULL, if queue was empty)
   postcondition: token removed from queue */
p_expr_token dequeue(struct token_queue * pqueue) {
	p_expr_token ptoken = pqueue->front;
	if (pqueue->front) {
		pqueue->front = ptoken->linked_token;
		if (ptoken == pqueue->back) /* at end */
			pqueue->back = NULL;
		ptoken->linked_token = NULL;
	}
	return ptoken;
}

/* push (add) token to top of stack
   input: ptop - pointer to top token pointer of stack
          ptoken - token pointer to add
   postcondition: ptop points to the added token */
void push(p_expr_token * ptop, const p_expr_token ptoken) {
	ptoken->linked_token = *ptop;
	*ptop = ptoken;
}

/* pop (remove) token from top of stack
   input: pointer to top token pointer of stack
   output: top token pointer (or NULL, if stack was empty)
   postcondition: ptop points to next token in stack */
p_expr_token pop(p_expr_token * ptop) {
	p_expr_token ptoken;
	if ( (ptoken = *ptop) ) {
		*ptop = ptoken->linked_token;
		ptoken->linked_token = NULL;
	}
	return ptoken;
}

/* allocate new token on heap, with specified type and value; the token is initially
   un-linked (field "linked_token" == NULL)
   note: token must be freed using free() after use */
p_expr_token new_token(const enum token_type type, const union token_value value) {
	p_expr_token ptoken = (p_expr_token)malloc(sizeof(struct s_expr_token));
	ptoken->type = type;
	ptoken->value = value;
	ptoken->linked_token = NULL;
	return ptoken;
}

/* handles evaluation process (calls above functions) for expression string str */
/* returns the final answer */
double evaluate(const char * str) {
	char * strbuffer; /* mutable buffer for string (modified in calls to strtok()) */
	double ans; /* answer to return */
	struct token_queue queue_infix, queue_postfix;

	/* copy str into mutable buffer */
	strbuffer = strcpy((char *)malloc(strlen(str)+1),str);

	/* get queue of tokens in infix order from string buffer */
	queue_infix = expr_to_infix(strbuffer);

	/* get queue of tokens in postfix order from infix-ordered queue */
	queue_postfix = infix_to_postfix(&queue_infix);
	//printf("postfix expr: %s\n");

	/* get answer from postfix-ordered queue */
	ans = evaluate_postfix(&queue_postfix);

	free(strbuffer); /* free memory from heap */
	return ans;
}

/* constructs a queue of tokens in infix order from a space-delimited string */
struct token_queue expr_to_infix(char * str) {
	struct token_queue queue_infix; /* queue with infix ordering */
	enum token_type type = OPERATOR;
	union token_value value;

	/* initialize the queue to empty */
	queue_infix.front = NULL;
	queue_infix.back = NULL;

	/* delimiter string for strtok() -- contains whitespace characters */
#define DELIMS_STR " \n\r\t"
	for (str = strtok(str, DELIMS_STR); str; str = strtok(NULL, DELIMS_STR)) {
		/* parse token */
		if (strlen(str) == 1) { /* operators are all 1 character */
			switch (str[0]) {
			case '+':
				type = OPERATOR;
				value.op_code = ADD;
				break;
			case '-':
				/* check previous token to distinguish between
				   negate (unary) and subtract (binary) */
				if (type == OPERATOR)
				{
					value.op_code = NEGATE; /* unary */
				}
#if PARSE_PARENS
				else if (type == LPARENS)
				{
					value.op_code = NEGATE; /* unary */
				}
#endif
				else
				{
					value.op_code = SUBTRACT; /* binary */
				}
				type = OPERATOR;
				break;
			case '*':
				type = OPERATOR;
				value.op_code = MULTIPLY;
				break;
			case '/':
				type = OPERATOR;
				value.op_code = DIVIDE;
				break;
#if PARSE_PARENS
			case '(':
				type = LPARENS;
				value.operand = 0;
				value.op_code = 5;
				break;
			case ')':
				type = RPARENS;
				value.operand = 0;
				value.op_code = 100;
				break;
#endif
			default:
				/* not an operator */
				type = OPERAND;
				value.operand = strtod(str, NULL);
			}
		} else {
			type = OPERAND;
			value.operand = strtod(str, NULL);
		}
		/* add token with parsed type and value to end of queue */
		enqueue(&queue_infix, new_token(type, value));
	}
	return queue_infix;
}

/* creates a queue of tokens in postfix order from a queue of tokens in infix order */
/* postcondition: returned queue contains all the tokens, and pqueue_infix should be
   empty */
struct token_queue infix_to_postfix(struct token_queue * pqueue_infix) {
	/*  construct postfix-ordered queue from infix-ordered queue;
	   all tokens from infix queue should be added to postfix queue or freed */
	struct token_queue queue_postfix;
	struct token_queue operator_stack;
	p_expr_token ptoken;

	/* initialize the queue to empty */
	queue_postfix.front = NULL;
	queue_postfix.back = NULL;
	operator_stack.front = NULL;

	struct s_expr_token **dptr;
	while ((ptoken = dequeue(pqueue_infix)))
	{
		if (ptoken->type == OPERATOR)
		{
			if(operator_stack.front == NULL)
			{
				dptr = &(operator_stack.front);
				push(dptr, ptoken);
			}
			else if (op_precedences[((operator_stack.front)->value).op_code] >= op_precedences[(ptoken->value).op_code])
			{
				while(*(dptr = &(operator_stack.front)))
				{
				if ((*dptr)->type == LPARENS)
					break;
				enqueue(&queue_postfix, pop(dptr));
				}
				push(dptr, ptoken);
			}
			else
			{
				dptr = &(operator_stack.front);
				push(dptr, ptoken);
			}
		}
		else if ((ptoken->type) == LPARENS)
		{
			dptr = &(operator_stack.front);
			push(dptr, ptoken);
		}
		else if ((ptoken->type) == RPARENS)
		{
			dptr = &(operator_stack.front);
			while(((operator_stack.front)->type) != LPARENS)
			{
			enqueue(&queue_postfix, pop(dptr));
			dptr = &(operator_stack.front);
			}
			//enqueue(&queue_postfix, pop(dptr));
			//dptr = &(operator_stack.front);
			free(pop(dptr)); //get rid of parenthesis token in postfix notation
		}
		else
		{
			enqueue(&queue_postfix, ptoken);
		}
	}
	while(*(dptr = &(operator_stack.front)))
	{
	enqueue(&queue_postfix, pop(dptr));
	}
	/*p_expr_token ptemp = queue_postfix.front;
	while(ptemp)
	{
		if (ptemp->type == OPERAND)
			printf("%f\n",(ptemp->value).operand);
		else if (ptemp->type == RPARENS)
			printf("shoot RPARENS\n");
		else if (ptemp->type == LPARENS)
			printf("darn LPARENS\n");
		else
			printf("%d\n",(ptemp->value).op_code);
		ptemp = ptemp->linked_token;
	}*/
	//for debugging
	/*ptoken = queue_postfix.front;
	printf("\n\n\n");
	while(ptoken)
	{
		if (ptoken->type == OPERAND)
			printf("%f\n",(ptoken->value).operand);
		else
			printf("(%d)\n",(ptoken->value).op_code);
		ptoken = ptoken->linked_token;
	}*/
	//for debugging
	return queue_postfix;
}

/* evalutes the postfix expression stored in the queue */
/* postcondition: returned value is final answer, and pqueue_postfix should be empty */
double evaluate_postfix(struct token_queue * pqueue_postfix) {
	/* TODO: process postfix-ordered queue and return final answer;
	   all tokens from postfix-ordered queue is freed */
	struct token_queue ans_stack;
	struct s_expr_token **dptr = &ans_stack.front;
	p_expr_token ptoken, operand_1, operand_2, operand_final;
	double val_1, val_2;
	union token_value temp_ans;
	while((ptoken = dequeue(pqueue_postfix)))
	{
		if (ptoken->type == OPERAND )
		{
			push(dptr,ptoken);
			dptr = &ans_stack.front;
		}
		else if((ptoken->value).op_code == NEGATE)
		{
			operand_1 = pop(dptr);
			dptr = &ans_stack.front;

			val_1 = (operand_1->value).operand;
			temp_ans.operand = -val_1;
			push(dptr, new_token(OPERAND,temp_ans));
		}
		else
		{
			operand_1 = pop(dptr);
			dptr = &ans_stack.front;
			operand_2 = pop(dptr);
			dptr = &ans_stack.front;

			val_1 = (operand_1->value).operand;
			free(operand_1);
			val_2 = (operand_2->value).operand;
			free(operand_2);

			switch ((ptoken->value).op_code)
			{
				case 0:
					temp_ans.operand = val_2 + val_1;
					push(dptr,new_token(OPERAND,temp_ans));
					dptr = &ans_stack.front;
					break;
				case 1:
					temp_ans.operand = val_2 - val_1;
					push(dptr,new_token(OPERAND,temp_ans));
					dptr = &ans_stack.front;
					break;
				case 2:
					temp_ans.operand = val_2 * val_1;
					push(dptr,new_token(OPERAND,temp_ans));
					dptr = &ans_stack.front;
					break;
				case 3:
					temp_ans.operand = val_2 / val_1;
					push(dptr,new_token(OPERAND,temp_ans));
					dptr = &ans_stack.front;
					break;
				case 4:
					break;
			}
		}
	}
	operand_final = pop(dptr);
	return (operand_final->value).operand;
}
