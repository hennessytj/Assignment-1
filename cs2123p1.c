/**********************************************************************************
Program cs2123p1.c by Timothy Hennessy
Purpose:	
    Contains the convertToPostFix function and the functions it calls
    to perform the conversion from infix to postfix.
Command Parameters:
    n/a
Input:
    The standard input file stream contains queries (one per input text line).
    Tokens in the query will be separated by one space.  
    Some sample data:
        SMOKING = N AND GENDER = F
        SMOKING = N AND ( EXERCISE = HIKE OR EXERCISE = BIKE )
        GENDER = F AND EXERCISE NOTANY YOGA
        SMOKING = N AND EXERCISE = HIKE OR EXERCISE = BIKE
        ( BOOK = SCIFI )
        ( ( ( BOOK ONLY SCIFI ) ) )
Results:
  cs2123p1Driver.c calls converToPostFix and sends as its argument
  a line of text from an input file.  This program uses a conversion
  algorithm which will produce the below results: 
  Query # 1: SMOKING = N AND GENDER = F
          SMOKING N = GENDER F =
          AND
  Query # 2: SMOKING = N AND ( EXERCISE = HIKE OR EXERCISE = BIKE )
          SMOKING N = EXERCISE HIKE =
          EXERCISE BIKE = OR AND
  Query # 3: GENDER = F AND EXERCISE NOTANY YOGA
          GENDER F = EXERCISE YOGA NOTANY
          AND
Returns:
    0 - properly formatted input passed from cs2123p1Driver.c
  801 - improperly formatted input; missing right parenthesis
  802 - improperly formatted input; missing left parenthesis
Notes:
    1. This program uses an array to implement the stack.  It has a maximum of
       MAX_STACK_ELEM elements. 
    2. This program uses an Out array for the resulting postfix expression.
       It has a maximum of MAX_OUT_ITEM elements. 
    3. The errors found will be as a result of improper format and 
       different constant integer error codes are returned to the
       main function accordlingly.  When formatting errors are encountered
       program prints a warnring and skips to next iteration instead of 
       terminating execution.
********************************************************************************/

/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cs2123p1.h"

/******************** processRemString ***********************************
int processRemString(Stack stack, Out out)
Purpose:
	Will purge entire stack of all operators until the stack is empty.
    The elements purged from the stack will be sent to addOut.
Parameters:
    I/O Stack stack           pointer to a structure of type StackImp
    I/O Out out               pointer to a structure of type OutImp
Returns:
   TRUE  - if there are no errors 
   FALSE - left parenthesis was left on stack with no matching right
           parenthesis found
Notes:
    1. Program iterates through the remaining stack elements until
       it is empty or a left parenthesis is found.  If a left paren-
       thesis is found FALSE is returned to the convertToPostFix function.
    2. If no left parenthesis is found and the entire stack has been
       traversed, then the program returns true. 
**************************************************************************/
int processRemString(Stack stack, Out out)
{
	Element popElement;

	while (!isEmpty(stack))
	{
		popElement = pop(stack);
		
		if (popElement.iCategory == CAT_LPAREN)
			return FALSE;
		
		addOut(out, popElement);		
	} // end while 
	
	return TRUE;
}
/******************** processRightParen ***********************************
int processRightParen(Stack stack, Out out)
Purpose:
	Loop through stack elements until a corresponding left parenthesis is
    found otherwise all stack elements will be popped from stack and
    FALSE will be returned to convertToPostFix function.         
Parameters:
    I/O Stack stack           pointer to a structure of type StackImp
    I/O Out out               pointer to a structure of type OutImp
Returns:
    TRUE  - if a left parenthesis is found on stack
    FALSE - if no left parenthesis is found on stack
Notes:
    - Pops and stores elements in the out array until a left parenthesis
      is popped from the stack.  Loop execution is terminated by the
      return of TRUE to indicate a properly formatted query.
    - If no left parenthesis is found on stack then FALSE is returned.   
**************************************************************************/
int processRightParen(Stack stack, Out out)
{
	Element popElement;            // element popped from stack
	// while stack is not empty
	while (!isEmpty(stack))
	{
		popElement = pop(stack);
		// checks to see if popped stack element is a left paren
		if (popElement.iCategory == CAT_LPAREN)
			return TRUE;
		// popped stack element is not left paren
		// store it in out
		addOut(out, popElement);
	} // end while
	// left parenthesis was not found on stack
	return FALSE;
}
/******************** processOperator **************************************
void processOperator(Stack stack, Element newValue, Out out)
Purpose:
	Takes an operator (token stored inside of newValue) and processes it 
    according to the infix to postfix conversion algorithm.           
Parameters:
    I/O Stack stack           pointer to a structure of type StackImp
    I   Element newValue      pointer to a structure of type Element 
    O   Out out               pointer to a structure of type OutImp
Returns:
   n/a
Notes:
    - If operator token (newValue) has higher precedence than the current 
      operator on top of the stack, then the operator token 
      is pushed on top of the stack (becomes new top value).
    - Otherwise the newValue has a lower precedence than the current 
      operator on top of the stack, the stack is popped and the stack 
      operators are sent to addOut until the stack is empty or an 
      operator on the stack has higher precedence than newValue.
**************************************************************************/
void processOperator(Stack stack, Element newValue, Out out)
{
	Element popElement;                // used to store popped value from stack
	Element topStackElement;
	while (!isEmpty(stack))
	{
		topStackElement = topElement(stack);
		if (newValue.iPrecedence > topStackElement.iPrecedence)
			break;
		else 
		{
		   popElement = pop(stack);
		   addOut(out, popElement);		
		}
	} // end while
	push(stack, newValue);
}
/******************** convertToPostFix **************************************
int convertToPostFix(char *pszInfix, Out out)
Purpose:
	Takes a string from a redirected input file containing operands and 
    operators and converts the string into postfix form.
	It populates the out array using the addOut function (provided in the driver).
Parameters:
    I char *pszInfix       pointer to a string from input file
    O Out  out             pointer to a structure of type OutImp that will
	                       store tokens from the input string 
Returns:
    0   - conversion to postfix was successful
	801 - WARN_MISSING_RPAREN
	802 - WARN_MISSING_LPAREN
Notes:
    - Uses a while loop to traverse a line of text until their are no
      more tokens.
**************************************************************************/
int convertToPostFix(char *pszInfix, Out out)
{
	Stack stack = newStack();               // new dynamically allocated structure
	                                        // of type StackImp stored in a pointer
	                                        // named stack
	char *pszRemainingText;                 // stores address returned by getToken
	                                        // which points to next token in string
	                                        // after delimiter 
	char szfromGetToken[MAX_TOKEN];         // stores token from getToken
	Element element;                        // stores szfromGetToken 
	int bValid = FALSE;                     // stores TRUE or FALSE
	
	
	pszRemainingText = getToken(pszInfix, element.szToken, sizeof(element.szToken) - 1);
	
	while(pszRemainingText != NULL)
	{	
		categorize(&element);                           // argument to categorize function
	                                                    // is a pointer to element
		// element can now be used to convert to postfix
		// compare element to categories
		// if element stores an operand send it to out
		if (element.iCategory == CAT_OPERAND)
		{
			addOut(out, element);
		}
		// if element stores a left paren push it onto stack
		 else if (element.iCategory == CAT_LPAREN)
		{
			push(stack, element);                     // push element onto stack
		}
		// if element stores an operator (=, NOTANY, ONLY, AND, OR)
		else if (element.iCategory == CAT_OPERATOR)
		{
			processOperator(stack, element, out);      // process operator according
                                                       // to algorithm 
		}
		// if element stores a right paren 
		else if (element.iCategory == CAT_RPAREN)
		{
			// call function to process right paren
			bValid = processRightParen(stack, out);
			if (bValid == FALSE)
			{
				// left parenthesis was never found on stack
				freeStack(stack);
				return WARN_MISSING_LPAREN;
			}
		}
		// retrieve next token
		if (pszRemainingText != NULL)
			pszRemainingText = getToken(pszRemainingText, element.szToken, sizeof(element.szToken) - 1);
	} // end while
	// end of input string is reached
	// if stack is not empty
	// process remaining string
	bValid = processRemString(stack, out);
	freeStack(stack);
	// if right parenthesis is missing return warnring
	if (!bValid)
		return WARN_MISSING_RPAREN;	
	// if successful conversion occurred return 0
	return 0;								
}

