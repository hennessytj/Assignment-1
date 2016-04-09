/******************************************************************************
cs2123p1Driver.c by Larry Clark
Purpose:
    This program reads queries and converts them from infix to postfix. 
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
    For each query, print the query and its corresponding prefix expression.
Returns:
    0 - normal
    901 - stack usage error (e.g., popping an empty stack)
    902 - Out overflow
    903 - algorithm error (see message for details)
Notes:
    1. This program uses an array to implement the stack.  It has a maximum of
       MAX_STACK_ELEM elements. 
    2. This program uses an Out array for the resulting postfix expression.
       It has a maximum of MAX_OUT_ITEM elements. 
*******************************************************************************/
// If compiling using visual studio, tell the compiler not to give its warnings
// about the safety of scanf and printf
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "cs2123p1.h"

// the following structure is used by the categorize function to categorize 
// tokens
static struct
{
    char szSymbol[MAX_TOKEN + 1];
    int iCategory;
    int iPrecedence;
} symbolDefM[] =
{
    "(",        CAT_LPAREN,   0
    , ")",      CAT_RPAREN,   0
    , "=",      CAT_OPERATOR, 2
    , "NOTANY", CAT_OPERATOR, 2
    , "ONLY",   CAT_OPERATOR, 2
    , "AND",    CAT_OPERATOR, 1
    , "OR",     CAT_OPERATOR, 1
    , "", 0, 0					// null terminating
};

// Stack implementation using arrays.  You are not required to document these.
/************************* push ***************************************
void push(Stack stack, Element value)
Purpose:
   This function places an object into an array.  It is designed
   to operate like a stack call.  The object is pushed onto the
   top of the stack.
Parameters:
   I/O Stack stack             A pointer to a stackImp structure
   I   Element value           A structure with data to be stacked
Returns:
   Nothing is returned functionally.  A new element is added into
   an array (which we will call a stack).
Notes:
   This function takes a pointer to a stackImp structure which
   as a member has an array of Element structures.  First, it 
   ensures that the call will not overflow the stack (i.e., number
   of elements in stack->element[] does not exceed allowed).  
   Program aborts if stack is full and cannot take additional
   elements (i.e., prevent stack overflow).  If stack is not full
   a new element is pushed on a stack and stack counter (array count)
   is incremented.
**********************************************************************/
void push(Stack stack, Element value)
{
    if (stack->iCount >= MAX_STACK_ELEM)
        ErrExit(ERR_STACK_USAGE
        , "Attempt to PUSH more than %d values on the array stack"
        , MAX_STACK_ELEM);
    else
    {
        stack->stackElementM[stack->iCount] = value;
        stack->iCount++;
    }
}
/********************************* pop *******************************
Element pop(Stack stack)
Purpose:
   Function simulates the stack operation pop which removes the top
   object from a stack.  In this case an array is used to represent
   the stack.
Parameters:
   I/O Stack stack          A pointer to a stackImp structure
Returns:
   The top element structure on the stack is returned.
Notes:
   First, the function calls isEmpty(stack) is called to determine
   if the stack is empty.  This prevents a stack condition known as
   underflow.  If an underflow or empty stack is encountered, the 
   program terminates.  If an underflow condition is not possible,
   the program decrements iCount (count of array) and returns the
   top element on the stack.  The function push() increments iCount
   after adding a new element to the stack which means that iCount
   is actually pointing to an empty array element.  This is the 
   reason it is decremented first.
*********************************************************************/
Element pop(Stack stack)
{
    if (isEmpty(stack))
        ErrExit(ERR_STACK_USAGE
        , "Attempt to POP an empty array stack");
    else
    {
        stack->iCount--;
        return stack->stackElementM[stack->iCount];
    }
}
/****************************** topElement ******************************
Element topElement(Stack stack)
Purpose:
   Function returns the top object (element) on the stack.
Parameters:
   I Stack stack            A pointer to a stackImp structure
Returns:
   Functionally, it returns the top element from the stack.
Notes:
   First, the function isEmpty(stack) is called to ensure the stack 
   is not empty.  If isEmpty returns FALSE the top element of the 
   stack is returned.  The variable iCount always points to an
   empty array element.  When it is decremented it points to the top
   of the stack.  In the case of this function, the top element is 
   returned without it being removed from the stack.  This is achieved
   by subtracting one from iCount ensuring it is not permanently 
   decremented.
************************************************************************/ 
Element topElement(Stack stack)
{
    if (isEmpty(stack))
        ErrExit(ERR_STACK_USAGE
        , "Attempt to examine topElement of an empty array stack");
    else
        return stack->stackElementM[stack->iCount-1];    // return the top
}
/****************************** isEmpty(Stack stack) ******************************
int isEmpty(Stack stack)
Purpose:
   Function determines if a given stack is empty.
Parameters:
   Stack stack             A pointer to a stackImp structure
Returns:
   An integer value is returned corresponding to TRUE or FALSE.  Any non-zero
   integer is considered TRUE in C.
Notes:
   If stack is empty an integer value corresponding to TRUE is returned else FALSE
   is returned.  If the array counter iCount is less than or equal to zero, TRUE
   is returned indicating the stack is empty otherwise FALSE is returned and the
   stack is not empty.
**********************************************************************************/
int isEmpty(Stack stack)
{
    return stack->iCount <= 0;
}
Stack newStack()
{
    Stack stack = (Stack) malloc(sizeof(StackImp));
    stack->iCount = 0;
    return stack;
}
void freeStack(Stack stack)
{
    free (stack);
}


// Main program for the driver

int main(int argc, char *argv[])
{
    Out out = malloc(sizeof(OutImp));
    char szInputBuffer[100];    // entire input line
    int rc;
    int icount = 1;
    
    // read text lines containing queries until EOF
	// in this case fgets will copy over up to 99 chars from
	// stdin stream and store them into szInputBuffer
    while (fgets(szInputBuffer, 100, stdin) != NULL)
    {
        printf("Query # %d: %s", icount, szInputBuffer);
        out->iOutCount = 0;   // reset out to empty
        rc = convertToPostFix(szInputBuffer, out);
        switch (rc)
        {
            case 0:
                printOut(out);
                break;
            case WARN_MISSING_LPAREN:
                printf("\tWarning: missing left parenthesis\n");
                break;
            case WARN_MISSING_RPAREN:
                printf("\tWarning: missing right parenthesis\n");
                break;
            default:
                printf("\t warning = %d\n", rc);
        }
        icount++;
    }
    free(out);
    printf("\n");
}

/******************** addOut **************************************
void addOut(Out out, Element element)
Purpose:
    Adds an element to out.  
Parameters:
    I/O Out out                 Stores the postfix expression 
    I Element element           Element structure to be added to out. 
Returns:
    n/a 
Notes:
    - Since out uses an array, addOut does a boundary check for overflow. 
**************************************************************************/
void addOut(Out out, Element element)
{
    if (out->iOutCount >= MAX_OUT_ITEM)
        ErrExit(ERR_OUT_OVERFLOW
        , "Overflow in the out array");
    out->outM[out->iOutCount++] = element;
}

/******************** printOut **************************************
void printOut(Out out)
Purpose:
    prints the contents of the out array to stdout 
Parameters:
    I Out out                 The postfx expression  
Returns:
    n/a 
Notes:
    - Prints 6 tokens from out per line
**************************************************************************/
void printOut(Out out)
{
    int i;
    printf("\t");
    // loop through each element in the out array
    for (i = 0; i < out->iOutCount; i++)
    {
        printf("%s ", out->outM[i].szToken);
        if ((i + 1) % 6 == 0)
            printf("\n\t");
    }
    printf("\n");
}

/******************** categorize **************************************
void categorize(Element *pElement)
Purpose:
    Categorizes a token providing its precedence (0 is low, higher 
    integers are a higher precedence) and category (operator, operand,
    left paren, right paren).  Since the category is an integer, it can
    be used in a switch statement.
Parameters:
    I/O Element *pElement       pointer to an element structure which
                                will be modified by this function
Returns:
    n/a 
Notes:
    - Uses the symbolDefM array to help categorize tokens 
**************************************************************************/
void categorize(Element *pElement)
{
    int i;
    // loop through the symbolDefM array until an empty symbol value is found
    // marking the end of the symbolDefM array
    for (i = 0; symbolDefM[i].szSymbol[0] != '\0'; i++) 
    {
        // does the element's token match the symbol in the symbolDefM array?
        if (strcmp(pElement->szToken, symbolDefM[i].szSymbol) == 0)
        {   // matched, so use its precedence and category
            pElement->iPrecedence = symbolDefM[i].iPrecedence;
            pElement->iCategory = symbolDefM[i].iCategory;
            return;
        }
    }
    // must be an operand
    pElement->iPrecedence = 0;
    pElement->iCategory = CAT_OPERAND;
}

/******************** ErrExit **************************************
  void ErrExit(int iexitRC, char szFmt[], ... )
Purpose:
    Prints an error message defined by the printf-like szFmt and the
    corresponding arguments to that function.  The number of 
    arguments after szFmt varies dependent on the format codes in
    szFmt.  
    It also exits the program with the specified exit return code.
Parameters:
    I   int iexitRC             Exit return code for the program
    I   char szFmt[]            This contains the message to be printed
                                and format codes (just like printf) for 
                                values that we want to print.
    I   ...                     A variable-number of additional arguments
                                which correspond to what is needed
                                by the format codes in szFmt. 
Returns:
    Returns a program exit return code:  the value of iexitRC.
Notes:
    - Prints "ERROR: " followed by the formatted error message specified 
      in szFmt. 
    - Prints the file path and file name of the program having the error.
      This is the file that contains this routine.
    - Requires including <stdarg.h>
**************************************************************************/
void ErrExit(int iexitRC, char szFmt[], ... )
{
    va_list args;               // This is the standard C variable argument list type
    va_start(args, szFmt);      // This tells the compiler where the variable arguments
                                // begins.  They begin after szFmt.
    printf("ERROR: ");
    vprintf(szFmt, args);       // vprintf receives a printf format string and  a
                                // va_list argument
    va_end(args);               // let the C environment know we are finished with the
                                // va_list argument
    printf("\n\tEncountered in file %s\n", __FILE__);  // this 2nd arg is filled in by
                                // the pre-compiler
    exit(iexitRC);
}
/******************** getToken **************************************
char * getToken (char *pszInputTxt, char szToken[], int iTokenSize)
Purpose:
    Examines the input text to return the next token.  It also
    returns the position in the text after that token.  This function
    does not skip over white space, but it assumes the input uses 
    spaces to separate tokens.
Parameters:
    I   char *pszInputTxt       input buffer to be parsed
    O   char szToken[]          Returned token.
    I   int iTokenSize          The size of the token variable.  This is used
                                to prevent overwriting memory.  The size
                                should be the memory size minus 1 (for
                                the zero byte).
Returns:
    Functionally:
        Pointer to the next character following the delimiter after the token.
        NULL - no token found.
    szToken parm - the returned token.  If not found, it will be an
        empty string.
Notes:
    - If the token is larger than the szToken parm, we return a truncated value.
    - If a token isn't found, szToken is set to an empty string
    - This function does not skip over white space occurring prior to the token.
**************************************************************************/
char * getToken(char *pszInputTxt, char szToken[], int iTokenSize)
{
    int iDelimPos;                      // found position of delim
    int iCopy;                          // number of characters to copy
    char szDelims[20] = " \n\r";        // delimiters
    szToken[0] = '\0';

    // check for NULL pointer 
    if (pszInputTxt == NULL)
        ErrExit(ERR_ALGORITHM
        , "getToken passed a NULL pointer");

    // Check for no token if at zero byte
    if (*pszInputTxt == '\0')
        return NULL;

    // get the position of the first delim
    iDelimPos = strcspn(pszInputTxt, szDelims);

    // if the delim position is at the first character, return no token.
    if (iDelimPos == 0)
        return NULL;

    // see if we have more characters than target token, if so, trunc
    if (iDelimPos > iTokenSize)
        iCopy = iTokenSize;             // truncated size
    else
        iCopy = iDelimPos;

    // copy the token into the target token variable
    memcpy(szToken, pszInputTxt, iCopy);
    szToken[iCopy] = '\0';              // null terminate

    // advance the position
    pszInputTxt += iDelimPos;
    if (*pszInputTxt == '\0')
        return pszInputTxt;
    else
        return pszInputTxt + 1;
}