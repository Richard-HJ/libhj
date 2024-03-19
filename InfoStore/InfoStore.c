/*
	InfoStore.c	R.E. Hughes-Jones  	University of Manchester, ATLAS
*/
/*
   Copyright (c) 2022 Richard Hughes-Jones, University of Manchester
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
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "InfoStore.h"

void InfoStore_Init(struct InfoStore *info, char *title, int32 num_items, int32 num_lines )
{
    info->pTitle = title;
    info->pnum_items = num_items;
    info->pnum_lines = num_lines;
    info->pInfoData =  (int64 *)malloc(num_items*num_lines*8);  // *8 as int64_t is 8 bytes
    info->pInfo_end = info->pInfoData + (num_items*num_lines) -1;
    for (info->pInfo_index=info->pInfoData; info->pInfo_index<info->pInfo_end; info->pInfo_index++) *info->pInfo_index =0;
    info->pInfo_index = info->pInfoData;               // current item pointer

}

void InfoStore_Store(struct InfoStore *info, int64 value )
{
  if(info->pInfo_index < info->pInfo_end) {
      *info->pInfo_index = value;
      info->pInfo_index++;
  }
}

void InfoStore_Print(struct InfoStore *info)
{
	int32 count;
	int32 line_num =0;
	int64 *pInfo_ptr;
	
	printf("InfoStore; %s \n", info->pTitle);
	pInfo_ptr = info->pInfoData;
	while (pInfo_ptr < info->pInfo_end){
	    printf("%d ; ", line_num);
	    line_num++;
	    for( count = 0; count < info->pnum_items; ++count ) {
	        printf("%ld ; ", *pInfo_ptr );
			pInfo_ptr++;
	    }
	    printf("\n");
	}
}



