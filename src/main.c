/*Bryan Martin Tostado
 *
 * creation: april 2014
 *
 * Description:
 *   entry point for compiler
 *
 * Notes:
 *
 *
 */

#include "include/lexer.h"
#include "./include/files.h"
#include "./include/syntax.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("usage:  ./sla file\n");
    return 0;
  }

  buffer_t buffer;
  if(fill_buff(argv[1],&buffer) == 0)
  {
    exit(1);
  }
  printf("   %s\n\n",buffer.buf);

  registerBuff(&buffer);

 exit( syntax() );
}






