/*
     Eth_lib.c     R. Hughes-Jones  The University of Manchester

     Ethernet library
*/
/*
   Copyright (c) 2002,2003,2004,2005,2006 Richard Hughes-Jones, University of Manchester
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
#include <string.h>
#include <errno.h>               /* in case of error */
#include <stdlib.h>
#include <netinet/in.h>          /* for htons() */
#include <sys/ioctl.h>
#include <net/if.h>              /* for ioctl calls */


#include "Eth_lib.h"             /* for Ethernet functions & defines */

void Eth_BuildEthFrame(struct EthFrame *ethframe, 
			unsigned char *dest_eth_add, 
			unsigned char *src_eth_add, 
			int ethpkt_type)
/* --------------------------------------------------------------------- */
{
    int ml;
    int type;

/* Put in MAC addresses */    
    for(ml = 0; ml < 6; ml++){ 
	ethframe->dest_hw_address[ml] = dest_eth_add[ml];
	ethframe->src_hw_address[ml] = src_eth_add[ml];
    }

/* Add packet type (network byte order) */ 
    type = htons(ethpkt_type);
    ethframe->ethpkt_type[0] = type & 0xff;
    ethframe->ethpkt_type[1] = ((type>>8) & 0xff);
    return ;
}

void Eth_Print_Frame(struct EthFrame *ethframe)
/* --------------------------------------------------------------------- */
{

    printf(" The frame destination   %02x:%02x:%02x:%02x:%02x:%02x\n", 
             ethframe->dest_hw_address[0],  ethframe->dest_hw_address[1],  ethframe->dest_hw_address[2], 
             ethframe->dest_hw_address[3],  ethframe->dest_hw_address[4],  ethframe->dest_hw_address[5] );
    printf(" The frame source        %02x:%02x:%02x:%02x:%02x:%02x\n", 
             ethframe->src_hw_address[0],  ethframe->src_hw_address[1],  ethframe->src_hw_address[2], 
             ethframe->src_hw_address[3],  ethframe->src_hw_address[4],  ethframe->src_hw_address[5] );
    printf(" Protocol %02x %02x \n", ethframe->ethpkt_type[0], ethframe->ethpkt_type[1]);

    return ;
}

int Eth_Get_HW_address(int soc, unsigned char *addr, char *hw_device)
/* --------------------------------------------------------------------- */
{
/* Use ioctl() to access the hardware address - Linux specific ? */

  struct ifreq req; 
  int err;

  strcpy(req.ifr_name, hw_device); 

  err = ioctl(soc, SIOCGIFHWADDR, &req);
  if (err == -1)  return err; 
    
  memcpy(addr, (char*)req.ifr_hwaddr.sa_data, 8);
  return 0;
}
 
int Eth_Get_if_index(int soc, int *if_index, char *hw_device)
/* --------------------------------------------------------------------- */
{
/* Use ioctl() to access the hardware address - Linux specific ? */

  struct ifreq req; 
  int err;

  strcpy(req.ifr_name, hw_device); 

  err = ioctl(soc, SIOCGIFINDEX, &req);
  if (err == -1)  return err; 
    
  *if_index = req.ifr_ifindex;
  return 0;
}

int Eth_Get_if_MTU(int soc, int *mtu, char *hw_device)
/* --------------------------------------------------------------------- */
{
/* Use ioctl() to access the hardware address - Linux specific ? */

  struct ifreq req; 
  int err;

  strcpy(req.ifr_name, hw_device); 

  err = ioctl(soc, SIOCGIFMTU, &req);
  if (err == -1)  return err; 
    
  *mtu = req.ifr_mtu;
  return 0;
}
