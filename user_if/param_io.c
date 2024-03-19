/*
	param_io.c	R.E. Hughes-Jones  	University of Manchester, ATLAS
*/
/*
   Copyright (c) 2006,2007,2008 Richard Hughes-Jones, University of Manchester
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
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "stdio.h"
#include <string.h>


#include "param_io.h"
#include "user_if.h"

#define PARAM_IO_DELIM '='

void params_in(char *config_filename, struct Params_tbl *param_tbl_ptr)
/* --------------------------------------------------------------------- */
{

#define MAX_BUF 256  
    FILE *in_file;                              /* handle for file with config data */
    struct Params_tbl *local_tbl_ptr;
    char desc[32];
    int ip[4];
    int  mac[6];
    int ret;
    char in_txt[MAX_BUF];
    char *in_ptr;
    char *crnl_ptr;
    int line_num;

    /* open and read the config file */
    if((in_file = fopen(config_filename, "r") ) == NULL) {
      perror("open of config file failed :");
      exit(-1);
    }

    line_num =0;
    for(;;){
INPUT:
      ret = readline(in_txt, MAX_BUF, in_file);
      if(ret == 0) break;   /* found eof */
      sscanf(in_txt, "%s", &desc[0]);
      line_num++;

      for(local_tbl_ptr = param_tbl_ptr; local_tbl_ptr->address != NULL ;local_tbl_ptr++){
	if( strcmp(desc, local_tbl_ptr->desc) == 0){
	    /* look for : separater */
	    in_ptr = index(in_txt, PARAM_IO_DELIM) ;
	    if(in_ptr == NULL) printf("Bad parameter line %d - no :\n", line_num);
	    in_ptr++;
	    switch(local_tbl_ptr->type){
	        case CFG_DEC:
		  sscanf(in_ptr, "%d \n", (int*) (local_tbl_ptr->address) );
	          break;

	        case CFG_IP:
		  sscanf(in_ptr, "%d.%d.%d.%d \n",  &ip[0],&ip[1],&ip[2],&ip[3] );
		  *(int*) (local_tbl_ptr->address) = (ip[0]&0xff) | ((ip[1]&0xff)<<8)  | 
		                                     ((ip[2]&0xff)<<16) | ((ip[3]&0xff)<<24);
		  break;

	       case CFG_MAC:
		  sscanf(in_ptr,"%02x:%02x:%02x:%02x:%02x:%02x\n", 
			 &mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5] );
		  *(int*) (local_tbl_ptr->address) = (mac[0]&0xff) | ((mac[1]&0xff)<<8)  | 
		                                     ((mac[2]&0xff)<<16) | ((mac[3]&0xff)<<24);
		  local_tbl_ptr++;
		  *(int*) (local_tbl_ptr->address) = (mac[4]&0xff) | ((mac[5]&0xff)<<8); 
		  break;

	        case CFG_TXT:
		  /* remove \n and \r */
		  crnl_ptr = index(in_ptr, '\n') ;
		  if(crnl_ptr != NULL) *crnl_ptr =0;
		  crnl_ptr = index(in_ptr, '\r') ;
		  if(crnl_ptr != NULL) *crnl_ptr =0;
		  /* remove leading and trailing blanks */
		  while(*in_ptr==' '){ in_ptr++;}
		  crnl_ptr=in_ptr+strlen(in_ptr)-1;
		  while(*crnl_ptr==' '){
		    *crnl_ptr=0; 
		    crnl_ptr--;
		  }
		  strncpy( (char*) (local_tbl_ptr->address), in_ptr,32 );
	          break;


	    } /* end of switch */
	    goto INPUT;
	}  /* if (strcmp() ) */
      }

    } /* for(;;) */

      fclose(in_file);
}

void params_out(char *config_filename, struct Params_tbl *param_tbl_ptr)
/* --------------------------------------------------------------------- */
{
    FILE *out_file;                              /* handle for file with config data */
    int temp;
    int ip[4];
    int  mac[6];

    /* open and read the config file */
    if((out_file = fopen(config_filename, "w") ) == NULL) {
      perror("open of config file failed :");
      exit(-1);
    }

      for(; param_tbl_ptr->address != NULL ;param_tbl_ptr++){
	switch(param_tbl_ptr->type){
	case CFG_DEC:
	  fprintf(out_file, "%s %c %d \n", param_tbl_ptr->desc, PARAM_IO_DELIM, *(int*) (param_tbl_ptr->address));
	  break;
	case CFG_IP:
	  temp = *(int*) (param_tbl_ptr->address);
	  ip[0]= (temp &0xff);
	  ip[1]= ((temp>>8) &0xff);
	  ip[2]= ((temp>>16) &0xff);
	  ip[3]= ((temp>>24) &0xff);
	  fprintf(out_file, "%s %c %d.%d.%d.%d \n", param_tbl_ptr->desc, PARAM_IO_DELIM, ip[0],ip[1],ip[2],ip[3] );
	  break;
	case CFG_MAC:
	  temp = *(int*) (param_tbl_ptr->address);
	  mac[0]= (temp &0xff);
	  mac[1]= ((temp>>8) &0xff);
	  mac[2]= ((temp>>16) &0xff);
	  mac[3]= ((temp>>24) &0xff);
	  fprintf(out_file, "%s %c %02x:%02x:%02x:%02x:", param_tbl_ptr->desc, PARAM_IO_DELIM,
		  mac[0],mac[1],mac[2],mac[3] );
	  param_tbl_ptr++;
	  temp = *(int*) (param_tbl_ptr->address);
	  mac[4]= ((temp) &0xff);
	  mac[5]= ((temp>>8) &0xff);
	  fprintf(out_file, "%02x:%02x\n", mac[4],mac[5] );
	  break;
	case CFG_TXT:
	  fprintf(out_file, "%s %c %s \n", param_tbl_ptr->desc, PARAM_IO_DELIM, (char*) (param_tbl_ptr->address));
	  break;

	} /* end of switch */
      }
      fclose(out_file);
}
