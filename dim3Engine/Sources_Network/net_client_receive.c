/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Receive Message from Host

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

#include "objects.h"
#include "interfaces.h"

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;

extern bool					join_thread_quit;

void* net_client_receive_thread(void *arg);			// forward reference

d3socket					client_socket;
bool						client_complete,client_msg_handler_active;
pthread_t					client_thread;
network_message_queue		client_queue;

/* =======================================================

      Client Message Queue
      
======================================================= */

bool net_client_start_message_queue(char *err_str)
{
		// create cache

	if (!network_queue_initialize(&client_queue)) {
		strcpy(err_str,"Networking: Out of memory");
		return(FALSE);
	}
	
		// start receive thread
		
	if (pthread_create(&client_thread,NULL,net_client_receive_thread,NULL)==0) {
		client_msg_handler_active=TRUE;
		return(TRUE);
	}
	
		// could not start thread
		
	network_queue_shutdown(&client_queue);
		
	strcpy(err_str,"Networking: Could not start client receive thread");
	return(FALSE);
}

void net_client_end_message_queue(void)
{
		// handler hasn't started yet
	
	if (!client_msg_handler_active) return;
	
		// shutdown message handler
		
	client_complete=FALSE;
	
		// wait for thread to exit
		
	while (TRUE) {
		if (client_complete) break;
		usleep(1000);
	}

		// free queue
		
	network_queue_shutdown(&client_queue);
}

void* net_client_receive_thread(void *arg)
{
	int					action,queue_mode,from_remote_uid,len,
						queue_count;
	unsigned char		data[net_max_msg_size];
	
	client_complete=TRUE;
		
	while (client_complete) {
	
			// is a message ready?
		
		if (!network_receive_ready(client_socket)) {
			usleep(10000);		// no packet, wait 1/100th of a second and try again
			continue;
		}
	
			// read message
			
		if (!network_receive_packet(client_socket,&action,&queue_mode,&from_remote_uid,data,&len)) {
			usleep(10000);		// wait 1/100th of a second between errors
			continue;
		}

			// add to queue
			// if the queue if full, retry a number of times and then
			// give up and ignore the message

		queue_count=0;

		while (client_complete) {
			if (network_queue_add_message(&client_queue,action,queue_mode,from_remote_uid,data,len)) break;

			queue_count++;
			if (queue_count>10) break;		// give up

			usleep(10000);		// queue if full, wait 1/100th of a second and try again
		}
	}
	
	client_complete=TRUE;
	
	pthread_exit(NULL);
	
	return(0);
}

/* =======================================================

      Client Message Queue Local
      
======================================================= */

bool net_client_start_message_queue_local(char *err_str)
{
		// create cache

	if (!network_queue_initialize(&client_queue)) {
		strcpy(err_str,"Networking: Out of memory");
		return(FALSE);
	}

	return(TRUE);
}

void net_client_end_message_queue_local(void)
{
	network_queue_shutdown(&client_queue);
}

/* =======================================================

      Check Message Queue
      
======================================================= */

bool net_client_check_message_queue(int *action,int *from_remote_uid,unsigned char *data)
{
	return(network_queue_check_message(&client_queue,action,from_remote_uid,data));
}

