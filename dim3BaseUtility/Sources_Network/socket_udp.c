/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Network UDP Socket Code

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif

/* =======================================================

      Network UDP Broadcast Sockets
      
======================================================= */

d3socket network_udp_open_socket(void)
{
	return(socket(AF_INET,SOCK_DGRAM,0));
}

bool network_udp_bind_broadcast(d3socket sock,int port,char *err_str)
{
	int					err;
	struct sockaddr_in	addr;
	
		// setup address
		// must be INADDR_ANY to receive broadcast messages
		
	memset(&addr,0x0,sizeof(struct sockaddr_in));
		
	addr.sin_family=AF_INET;
	addr.sin_port=htons((short)port);
	addr.sin_addr.s_addr=INADDR_ANY;
	
		// bind socket
		
	err=bind(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));
	if (err<0) {
		sprintf(err_str,"Networking: Could not bind to broadcast port %d",port);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Network UDP Broadcast Receive
      
======================================================= */

unsigned long network_udp_receive_broadcast(int sock)
{
	int					sz;
	char				msg[24];
	
		// wait for message
		
#ifndef D3_OS_WINDOWS
	sz=(int)recvfrom(sock,msg,24,MSG_WAITALL,NULL,0);
#else
	sz=(int)recvfrom(sock,msg,24,0,NULL,0);
#endif

	if (sz!=24) return(-1);
	
		// convert to long address
		
	return(inet_addr((char*)&msg[5]));
}

/* =======================================================

      Network UDP Broadcast Send
      
======================================================= */

void network_udp_set_broadcast_flag(d3socket sock)
{
#ifndef D3_OS_WINDOWS
	int				val;

	val=1;
	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&val,sizeof(int));
#else
	int				val;

	val=TRUE;
	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(const char*)&val,sizeof(BOOL));
#endif
}

bool network_udp_send_broadcast_single(d3socket sock,char *broad_ip,int broad_port,char *msg,int msg_len)
{
	struct sockaddr_in	addr;

	addr.sin_family=AF_INET;
	addr.sin_port=htons((short)broad_port);
	addr.sin_addr.s_addr=inet_addr(broad_ip);

	return(sendto(sock,msg,msg_len,0,(struct sockaddr*)&addr,sizeof(struct sockaddr_in))==msg_len);
}

bool network_udp_send_broadcast(char *ip,int port)
{
	char				msg[24];
	bool				snd_ok;
	d3socket			sock;
	
		// broadcast will be the IP of this machine
		// as a return address to reply to broadcasts
		
	bzero(msg,24);
	sprintf(msg,"dim3_%s",ip);
		
		// open socket and allow broadcasting
		
	sock=network_udp_open_socket();
	if (sock==D3_NULL_SOCKET) return(FALSE);

	network_udp_set_broadcast_flag(sock);

		// send broadcast to all address broadcast,
		// sub-address broadcasts, and this address
		// itself
		
	snd_ok=network_udp_send_broadcast_single(sock,"255.255.255.255",port,msg,24);

		// close socket
		
	network_close_socket(&sock);
	
	return(snd_ok);
}

