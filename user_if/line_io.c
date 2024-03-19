/*
	line_io.c	R.E. Hughes-Jones 
*/
/*
   Copyright (c) 2006,2007,2008 Richard Hughes-Jones,
   All rights reserved.

   Redistribution and use in source and binary forms, with or
   without modification, are permitted provided that the following
   conditions are met:

     o Redistributions of source code must retain the above
       copyright notice, this list of conditions and the following
       disclaimer. 
     o Redistributions in binary form must reproduce the above
       copyright notice, this list of conditions and the following
       disclaimer in the documentation and/or other materials
       provided with the distribution. 

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <ctype.h>
#include "stdio.h"

#include "user_if.h"

int GetLine(char *s, int lim, FILE *fp)
/* ------------------------------------------------------------------------ */
/* read line from stream fp
 * returns :
           +ve = number of characters, including newline char
	         i.e. just pressing <cr> will give a length of 1
           0   = <esc> or DOT <.> entered as 1st char 
	         - used to indicate acceptance of default or go up 1 level in menus
 * [modif from K&R page 67]

Mods for EB66:
	Use low level GetChar(), use getchar() on Linux
	deal with line edit cntl-h null
	echo chars
	at end of input, echo the line with PutChar() to tidy line - noot needed in Linux
*/
{
int s1  = ' '; 
int esc = 0x1b;
int dot = 0x2e;
int  i ,ii;
char c = 0;

   i = 0 ;
   while (--lim>0 && (c=getchar()) !='\r' && c != '\n')
   {
   ii= (int) c;
       switch (ii)
       {
       case 0177:   /* delete */
       case '\b':   /*  ^H    */
	  if(i == 0 ) break;
          printf ("\b \b");
          i--;
	  s--;
          break;

       default:    
	 //          putchar (c);  /* echo char and put in output */
	    if (i ==0) s1= (int)c;
 	    *s= c; s++; i++;
      }
   }

   if (c == '\n'){
     *s= c; 
     s++; 
     i++;
   }
   *s = '\0' ; 
/* check for ,esc> or <.> */
   if((s1 == esc) || (s1 == dot)) return (0);

   return(i) ;
}

int readline(char *s, int lim, FILE *fp)
/* ------------------------------------------------------------------------ */
/* read line from stream fp
 * returns :
           +ve = number of characters, including newline char
                 i.e. just pressing <cr> ie  \n 0xa will give a length of 1
           0   = EOF read
           *s contains characters read including the newline \n
[ not yet BUT \r 0xd are removed and NOT in the char count ]
 * [modif from K&R page 67]
 
*/
{
int  i;
int ii = 0;
 
   i = 0 ;
   while (--lim>0 && (ii=fgetc(fp)) != EOF && ii != 0xa)
   {
     /* reject the \r character */
        //      if(ii != 0xd) *s= char (ii); s++; i++;
	 *s= (char) ii; 
	 s++; 
	 i++;
  }
 
   if(ii == EOF ) return (0);
   if (ii == 0xa){
     *s= (char) ii; 
     s++; 
     i++;
   }
                                                                                                
   *s = '\0' ;
 
   return(i) ;
}
