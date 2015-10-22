/*
 * Bryan Martin Tostado
 * created  april 2014
 *
 * Descripticion:
 *  syntax! 
 *
 *
 * Notes:
 *
 */

#include <stdio.h>


#include <string.h>
#include <stdlib.h>
#include "./include/lexer.h"

#define TOKEN  tokens[toks_ptr]
#define TOKEN_P tokens[toks_ptr++]
#define P_TOKEN tokens[++toks_ptr]

/*global vars*/
//static token_t token;
static token_t tokens[10000];
static int toks_size = 0;
static int toks_ptr = 0;

static int funcion();
static int termino();
static int lista_terminos();
static int comparacion();
static int atomo();
static int cuantificacion();
static int exp_log();
static int printErr(char* msg,char*,int);

int syntax()
{ 
 do //fill token buffer ( for easy backtracking)
 {
   tokens[toks_size++] = nexTok();   
 }while(tokens[toks_size-1].lexeme != NULL);
 toks_size--;

 if(toks_size <=0)
 {
  printf("Empty file\n");exit(0);
 }

 for(;toks_ptr < toks_size; toks_ptr++)
 {
    if(toks_ptr < 9)
      printf("\t(0%i)[%s] ",toks_ptr+1,tokens[toks_ptr].lexeme);
    else
      printf("\t(%i)[%s] ",toks_ptr+1,tokens[toks_ptr].lexeme);
    if(toks_ptr  % 5 == 0 )
      printf("\n");
 }
 printf("\n");
 toks_ptr = 0;

 if(exp_log() && ( toks_ptr == toks_size-1) )
   { printf("\nPARSE SUCCESSFUL\n"); return 1; }
 else
   { printErr("PARSE FAILED!  expected NOTHING ",TOKEN.lexeme,toks_ptr+2); return 0; }

}

static int funcion()
{ 
  if (TOKEN.type != OBJ_FUNC)
  {
    return 0;
  }
  toks_ptr++;
  if(TOKEN.type != LP)
  {
   toks_ptr--;
   return 0;
  }
  toks_ptr++;
  lista_terminos();

  if(TOKEN.type != RP)
    printErr(" expected ')'",TOKEN.lexeme,toks_ptr+1); 

  return 1;
}

static int termino() //doesnt consume any tokens
{
  if( funcion() || TOKEN.type ==VAR || TOKEN.type == OBJ_FUNC)
    return 1;

  return 0; 

}

static int lista_terminos()
{
  if( !termino() )
    printErr(" expected funcion(..), variable, or const_obj",TOKEN.lexeme,toks_ptr+1); 

  toks_ptr++;
  while( termino() )
    toks_ptr++;
  
  return 1;
}

static int comparacion()
{ 
  if( !termino() )
     printErr("expected termino  ",TOKEN.lexeme,toks_ptr+1);
  
  toks_ptr++;
  if(TOKEN.category != REL_OP)
    {
        printErr("expected relational operator ( = | != | ‘<’ | ‘>’ | <= | >= ) ",TOKEN.lexeme,toks_ptr+1);
    }

  toks_ptr++;
  if( !termino() )
    printErr("expected termino ",TOKEN.lexeme,toks_ptr+1);
  
  return 1;
}

static int atomo()
{
  // ( funcion() || comparacion() ) is ambiguous since both call funcion(). fix is to call funcion and if parsed ok, is next tok is
  // a REL_OP, then it must be a comparision, if not, we backtrack the parse and try comparacion() 
  // (which will check for a funcion() or a variable as first token
  int save_toks_ptr = toks_ptr;
  if ( funcion() && ( tokens[toks_ptr+1].category != REL_OP) ) //if its just a funcion with no comparison next
    return 1;
  
  toks_ptr = save_toks_ptr;
  return ( comparacion() );

}

static int cuantificacion()
{
  if( !(TOKEN.type == UNIVERSAL || TOKEN.type == EXISTENTIAL) )
    return 0;
 
  toks_ptr++;
  if( TOKEN.type != VAR )
    printErr("expected variable",TOKEN.lexeme,toks_ptr+1);
   
  toks_ptr++; //consume the token
  return 1;
}

static int exp_log()
{
  if( TOKEN.type == NEG)
  {
    toks_ptr++;
    exp_log();
    return 1; //if you make it out, the string is parsed OK
  }
  else if( TOKEN.type == LP)
  {
    toks_ptr++; 
    
    exp_log();
    if(TOKEN.type != RP)
      printErr("expected ')'",TOKEN.lexeme,toks_ptr+1);
    return 1;
  }
  else if( TOKEN.type == UNIVERSAL || TOKEN.type == EXISTENTIAL)
  {
    cuantificacion();
    exp_log();
    return 1;
  }
  else //ambiguous derivation to atomo() or exp_log(). Taking advantage that atomo() must ahppen no matter what ( or fail)
  {    
    atomo(); 
    toks_ptr++;
    if(TOKEN.type == RP)// if th enext token is a '(', then this was the correct derivation
      return 1;

    if( TOKEN.category != OP_LOG_BIN)                                        
      printErr("expected a logical binary operator  & | ’|’ | -> | <- | <->  OR  ')' ", TOKEN.lexeme,toks_ptr+1);
    
    toks_ptr++;
    exp_log();

    return 1;
  }
  return 0; //shouldnt touch here, safekeeping
}

static int printErr(char* msg, char *got, int tok_num)
{
  printf("\nERROR: %s  got: [%s]\n token number: (%i) \n",msg,got,tok_num);
  exit(0);
}












