/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Network Host Code

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

d3socket					server_host_socket;
bool						server_host_complete;
char						server_host_err_str[256];
pthread_t					server_host_thread;

/* =======================================================

      Initialize and Shutdown Host Networking
      
======================================================= */

bool host_initialize(char *err_str)
{
		// begin listener thread
		
	server_host_complete=FALSE;
	server_host_socket=D3_NULL_SOCKET;
	
	if (pthread_create(&server_host_thread,NULL,host_thread,NULL)!=0) {
		strcpy(err_str,"Networking: Could not start host listener thread");
		return(FALSE);
	}
	
		// wait for listenter thread to get into accept
		
	while (TRUE) {
		if (server_host_complete) break;
		usleep(100000);
	}
	
	if (server_host_err_str[0]!=0x0) {
		strcpy(err_str,server_host_err_str);
		return(FALSE);
	}

		// now listening on separate thread
		
	return(TRUE);
}

void host_shutdown(void)
{
		// did host never start?

	if (server_host_socket==D3_NULL_SOCKET) return;
	
		// shutdown socket and then wait for termination
		
	network_close_socket(&server_host_socket);
	pthread_join(server_host_thread,NULL);
}

/* =======================================================

      Host Networking Listener Thread
      
======================================================= */

void* host_thread(void *arg)
{
	d3socket			sock;
	int					err;
	socklen_t			addr_len;
	struct sockaddr		addr;
	pthread_t			message_thread;
	
		// use host err_str to flag if errors occured
		
	server_host_err_str[0]=0x0;
	
		// create host socket
		
	server_host_socket=network_open_socket();
	if (server_host_socket==D3_NULL_SOCKET) {
		strcpy(server_host_err_str,"Networking: Unable to open socket");
		server_host_complete=TRUE;
		pthread_exit(NULL);
		return(NULL);
	}
	
	network_socket_blocking(server_host_socket,TRUE);
		
		// bind

	if (!network_bind(server_host_socket,net_setup.host.ip_resolve,net_port_host,server_host_err_str)) {
		network_close_socket(&server_host_socket);
		server_host_complete=TRUE;
		pthread_exit(NULL);
		return(NULL);
	}

		// start listening
		
	err=listen(server_host_socket,256);
	if (err!=0) {
		network_close_socket(&server_host_socket);
		sprintf(server_host_err_str,"Networking: Could not start listener on %s (error: %d)",net_setup.host.ip_resolve,errno);
		server_host_complete=TRUE;
		pthread_exit(NULL);
		return(NULL);
	}

		// host is OK, free thread to run independantly
		
	server_host_complete=TRUE;
	
		// begin accepting
		
	addr_len=sizeof(struct sockaddr);
	
	while (TRUE) {
		sock=accept(server_host_socket,&addr,&addr_len);
		if (sock==-1) break;				// accept ending means socket has been closed and host shutting down
		
			// spawn thread for client
			
		network_socket_blocking(sock,FALSE);
		
		if (pthread_create(&message_thread,NULL,host_client_thread,(void*)sock)!=0) {
			network_close_socket(&sock);
		}
	}
	
	pthread_exit(NULL);
	return(NULL);
}

