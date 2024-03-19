/*
 	Eth_lib.h     R. Hughes-Jones  The University of Manchester
                  definitions for CPU or system control and query functions
*/
/*
   Copyright (c) 2006  Richard Hughes-Jones, University of Manchester
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
/*
     Modification: 

      Version 4.1.0      
        rich  15 May 06  
*/

#ifndef Eth_lib_h
#define Eth_lib_h

#define ETH_MACADDRSIZE   6
#define ETH_TYPESIZE   2
#define ETH_HDR_LEN   14
#define ETH_FRAMELEN   20000
#define ETH_PACKETLEN   ETH_FRAMELEN - ETH_HDR_LEN

struct EthFrame {
    unsigned char dest_hw_address[ETH_MACADDRSIZE];
    unsigned char src_hw_address[ETH_MACADDRSIZE];
    unsigned char ethpkt_type[ETH_TYPESIZE];
    unsigned char eth_packet[ETH_PACKETLEN];
};

/* A comment:
Standard Ethernet frame with a MTU 1500 bytes :
           | MAC link |  data                  | CRC |
len bytes:         14    46-1500                  4      

ethernet packet:
                      |  data                  |
len bytes:            |  46-1500               |

Note the 1500 increases for Jumbo packets

*/

void Eth_BuildEthFrame(struct EthFrame *ethframe, 
			unsigned char *dest_eth_add, 
			unsigned char *src_eth_add, 
		        int ethpkt_type);
/* create ethernet from from addresses and packet type */

void Eth_Print_Frame(struct EthFrame *ethframe);

int Eth_Get_HW_address(int soc, unsigned char *addr, char *hw_device);
/* lookup the MAC address of the interface */
 
int Eth_Get_if_index(int soc, int *if_index, char *hw_device);
/* lookup the index of the interface in the kernel structures */

int Eth_Get_if_MTU(int soc, int *mtu, char *hw_device);

#endif                 /*  Eth_lib_h */
