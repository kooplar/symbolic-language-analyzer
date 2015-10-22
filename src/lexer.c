/*Bryan Martin Tostado
 *   creation: april 2014
 *
 *   Description:
 *     Functions to perform lexcial analisis for Piton
 *
 *   Notes:
 *     After clasifying a token, if its an ID, then it is inserted into the current symtab. It is the 
 *     job of the syntactic analyzer to keep symtab scope correct and to clasify the ID token as VAR or FUNC as analyzed.
 *
 */

#include "include/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*static function prototypes*/

/* charToType
 *   Description:
 *    returns the numerical type of the char. see diagrams
 *   Returns:
 *    the type of the transition corresponding to the char
 */
static int charToType(char c);

/* rmWhites
*   Description:
*    removes all whites and leaves buffer.offset at the next non white.
*    lineNum is incremented at every new line found
*    charNum is incremented for every space or tab. it is set to = 1 after lineNum++
*   Returns:
*     nothing
*/
static void rmWhites(buffer_t **buffer);

/* IDType
 *  Description:
 *    Tells the type (keyword,logical operator/constant/) of the lexeme
 *  Returns:
 *   The type ( as define in lexer.h
 */
static int IDType(char *symbol);

/*global variables*/
buffer_t *buffer = NULL;

static int tranMat[23][17] = {
      //  0  , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 ,10 ,11, 12 ,13 ,14 ,15 ,16 
/*0 */   {0  ,  1,  2,  3, 15,  6,  9, 10, 11, 12, 19, 18, ER, ER, 20, 21, 22}, 
/*1 */   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*2 */   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*3 */   {0  , ER, ER, ER,  4, ER, ER, ER, ER, ER, ER, ER, ER, ER, ER, ER, ER},
/*4 */   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*5 */   {0  , AC, AC, AC,  8, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*6 */   {0  , AC, AC,  5, AC, AC, AC, AC, 16, AC, AC, AC, AC, AC, AC, AC, AC},
/*7 */   {0  ,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, //RETIRED
/*8 */   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*9 */   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*10*/   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*11*/   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*12*/   {0  , ER, ER, ER, ER, ER, ER, ER, 13, ER, ER, ER, ER, ER, ER, ER, ER},
/*13*/   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*14*/   {0  , 0 ,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, //RETIRED
/*15*/   {0  , AC, AC, AC, AC, AC, AC, AC, 17, AC, AC, AC, AC, AC, AC, AC, AC},
/*16*/   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*17*/   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*18*/   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, 18, 18, 18, 18, AC, AC, AC},
/*19*/   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, 19, 19, 19, 19, AC, AC, AC},
/*20*/   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*21*/   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
/*22*/   {0  , AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC, AC},
         };

static int charToType(char c)
{ 
  if(islower(c))
    return 10;
  else  if(isupper(c))
    return 11;
  else if(isdigit(c))
    return 12;
  
  switch(c)
  { 
    case '&': return 1;
    case '|': return 2;
    case '-': return 3;
    case '>': return 4;
    case '<': return 5;
    case '@': return 6;
    case '#': return 7;
    case '=': return 8;
    case '!': return 9;
//10 above
//11 above
//12 above
    case '_': return 13;
    case '(': return 14;
    case ')': return 15;
    case '~': return 16;
    case EOF: printf("EOF\n");exit(1);
    default: printf("Error, unrecognized symbol: %c (ascii %i)\n",c,c);exit(1);
  }
  return -1; 
}

void registerBuff(buffer_t *buff)
{
  buffer = buff;
}

token_t nexTok()
{
  if(buffer == NULL)
  {
    perror("Trying to call nexTok, but buffer is null!");
    return (token_t){-1,-1,NULL};
  }
  return nextTok(buffer);
}

token_t nextTok(buffer_t *buffer)
{  
  //printf("    %i | %i",buffer->offset+1, buffer->size);
  if(buffer->offset+1 >= buffer->size) //+1 needed because offset wont save EOF
  {
    return (token_t){-1,-1,NULL};
  }

  token_t token = {-1,-1, NULL};
  int lexeme_length = 0;
  int state = 0;
  int prevState = 0;
  char transition = 0; 
  rmWhites(&buffer);
  do
  {
    prevState = state;
    //protect from buffer over-offseting, for example by never ending string
    if((buffer->advance+1 >= buffer->size)  )
    {
      buffer->advance++;
      break;
    }
    transition = charToType(buffer->buf[buffer->advance++]);
    state = tranMat[state][transition];
    buffer->charNum++;
  }while(state != AC && state != ER);
  buffer->charNum--; //remove lookahead  

  //setup the token
  buffer->advance--;  
  lexeme_length = buffer->advance - buffer->offset ;
  token.lexeme = malloc((sizeof(char)) * (lexeme_length + 1)); 
  strncpy(token.lexeme,&((buffer->buf)[buffer->offset]), lexeme_length);
  token.lexeme[lexeme_length] = '\0';
  buffer->offset = buffer->advance;

  //classify token category and type
  switch(prevState)
  {
    case 1: token.type = AND; token.category = OP_LOG_BIN; break; 
    case 2: token.type = OR;  token.category = OP_LOG_BIN; break;
//
    case 4:  token.type = RA; token.category = OP_LOG_BIN; break;
    case 5: token.type = LA;  token.category = OP_LOG_BIN; break;
    case 6: token.type = LT; token.category = REL_OP; break;
//
    case 8: token.type = DA;  token.category = OP_LOG_BIN;break;
    case 9: token.type = UNIVERSAL;break;
    case 10: token.type = EXISTENTIAL;break;
    case 11: token.type = EQ; token.category = REL_OP; break;
//
    case 13: token.type = DIFF; token.category = REL_OP; break;
//
    case 15: token.type = GT; token.category = REL_OP; break;
    case 16: token.type = LTE; token.category = REL_OP; break;
    case 17: token.type = GTE; token.category = REL_OP; break;
    case 18: token.type = VAR;break;
    case 19: token.type = OBJ_FUNC;break;
    case 20: token.type = LP;break;
    case 21: token.type = RP;break;
    case 22: token.type = NEG;break;
   
//error states
    case 3: printf("ERROR invalid token -");exit(0); 
    case 7: printf("ERROR invalid state 7");exit(0);
    case 14: printf("ERROR invalid state 14");exit(0);
    default: printf(" This symbol is not  part of the language: %s\n",token.lexeme);
  }

  if(state == ER){
    return (token_t){-1,-1,token.lexeme};
  }
  //DONT RETURN TOKEN COMMENTS! maybe something can be done with these here..write to a file for a "javadoc" type of crap
  if(token.category == CMNT) 
  {
    free(token.lexeme);
    return nextTok(buffer);
  }
  else         
    return token;
}

static void rmWhites(buffer_t **buffer)
{
  while(((*buffer)->buf)[(*buffer)->offset] == ' ' ||
        ((*buffer)->buf)[(*buffer)->offset] == '\t'||
        ((*buffer)->buf)[(*buffer)->offset] == '\n')
  {
    if(((*buffer)->buf)[(*buffer)->offset] == '\n')
    {
        (*buffer)->lineNum++;
//printf("				 rm whites jumping! to: %i\n",(*buffer)->lineNum);
        (*buffer)->charNum = 0;
    }
    else
    {
      (*buffer)->charNum++;
    }
    ((*buffer)->offset)++;
  } 
  ((*buffer)->advance) = ((*buffer)->offset);
}


