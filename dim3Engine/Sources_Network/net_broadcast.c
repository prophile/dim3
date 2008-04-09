/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Broadcast Listener Code

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

extern network_setup_type	net_setup;

d3socket					broadcast_listen_socket;
bool						broadcast_listen_complete;
char						broadcast_listen_err_str[256];
pthread_t					broadcast_listen_thread;

extern void net_host_client_handle_info(int sock);

/* =======================================================

      Initialize and Shutdown Broadcast Listener
      
======================================================= */

bool net_host_broadcast_initialize(char *err_str)
{
		// begin listener thread
		
	broadcast_listen_complete=FALSE;
	broadcast_listen_socket=D3_NULL_SOCKET;
	
	if (pthread_create(&broadcast_listen_thread,NULL,net_host_broadcast_thread,NULL)!=0) {
		strcpy(err_str,"Networking: Could not start broadcast listener thread");
		return(FALSE);
	}
	
		// wait for listenter thread to get into accept
		
	while (TRUE) {
		if (broadcast_listen_complete) break;
		usleep(100000);
	}
	
	if (broadcast_listen_err_str[0]!=0x0) {
		strcpy(err_str,broadcast_listen_err_str);
		return(FALSE);
	}

		// now listening on separate thread

	return(TRUE);
}

void net_host_broadcast_shutdown(void)
{
		// did broadcast listener never start?

	if (broadcast_listen_socket==D3_NULL_SOCKET) return;
	
		// shutdown socket and then wait for termination
		
	network_close_socket(&broadcast_listen_socket);
	pthread_join(broadcast_listen_thread,NULL);
}

/* =======================================================

      Broadcast Listener Thread
      
======================================================= */

void* net_host_broadcast_thread(void *arg)
{
	char				ip[32],err_str[256];
	unsigned char		*uc_ptr;
	unsigned long		r_addr;
	d3socket			sock;
	
		// use broadcast err_str to flag if errors occured
		
	broadcast_listen_err_str[0]=0x0;
	
		// create host socket
		
	broadcast_listen_socket=network_udp_open_socket();
	if (broadcast_listen_socket==D3_NULL_SOCKET) {
		strcpy(broadcast_listen_err_str,"Networking: Unable to open socket");
		broadcast_listen_complete=TRUE;
		pthread_exit(NULL);
		return(NULL);
	}
		
	network_socket_blocking(broadcast_listen_socket,TRUE);

		// bind to the "any" IP to gather
		// any broadcast messages

	if (!network_udp_bind_broadcast(broadcast_listen_socket,net_port_host_broadcast,broadcast_listen_err_str)) {
		network_close_socket(&broadcast_listen_socket);
		broadcast_listen_complete=TRUE;
		pthread_exit(NULL);
		return(NULL);
	}

		// listener is OK, free thread to run independantly
		
	broadcast_listen_complete=TRUE;

		// start listening
		
	while (TRUE) {
		r_addr=network_udp_receive_broadcast(broadcast_listen_socket);
		if (r_addr==-1) break;
		
			// connect and reply that we are a server
		
		uc_ptr=(unsigned char*)&r_addr;
		
		sock=network_open_socket();
		if (sock==D3_NULL_SOCKET) continue;
		
		uc_ptr=(unsigned char*)&r_addr;
		sprintf(ip,"%d.%d.%d.%d",uc_ptr[0],uc_ptr[1],uc_ptr[2],uc_ptr[3]);
		
		if (network_connect_block(sock,ip,net_port_host_broadcast_reply,5,err_str)) {
			net_host_client_handle_info(sock);
		}
		
		network_close_socket(&sock);
	}
	
	pthread_exit(NULL);
	return(NULL);
}

