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
bool						client_complete;
pthread_t					client_thread;
net_queue_type				client_queue;

/* =======================================================

      Client Message Queue
      
======================================================= */

bool net_client_start_message_queue(char *err_str)
{
		// create cache

	if (!net_queue_initialize(&client_queue)) {
		strcpy(err_str,"Networking: Out of memory");
		return(FALSE);
	}
	
		// start receive thread
		
	if (pthread_create(&client_thread,NULL,net_client_receive_thread,NULL)==0) return(TRUE);
	
		// could not start thread
		
	net_queue_shutdown(&client_queue);
		
	strcpy(err_str,"Networking: Could not start client receive thread");
	return(FALSE);
}

void net_client_end_message_queue(void)
{
		// shutdown message handler
		
	client_complete=FALSE;
	pthread_join(client_thread,NULL);

		// free queue
		
	net_queue_shutdown(&client_queue);
}

void* net_client_receive_thread(void *arg)
{
	bool				client_err;
	
	client_err=FALSE;
	client_complete=TRUE;
		
	while (client_complete) {

			// feed the queues from the socket
			// if there's an error, break out of loop
			// and cancel network game
			
		if (!net_queue_feed(client_socket,&client_queue)) {
			client_err=TRUE;
			break;
		}
	
			// sleep so thread doesn't spike processor
			// as it only reads when data is ready
			
		usleep(10000);
	}

		// if there was an error, put a exit on
		// the queue
		
	if (client_err) net_queue_push_message(&client_queue,net_action_request_host_exit,0,NULL,0);
	
		// exit thread
	
	pthread_exit(NULL);
	
	return(0);
}

/* =======================================================

      Client Message Queue Local
      
======================================================= */

bool net_client_start_message_queue_local(char *err_str)
{
		// create cache

	if (!net_queue_initialize(&client_queue)) {
		strcpy(err_str,"Networking: Out of memory");
		return(FALSE);
	}

	return(TRUE);
}

void net_client_end_message_queue_local(void)
{
	net_queue_shutdown(&client_queue);
}

inline void net_client_push_queue_local(int action,int remote_uid,unsigned char *data,int len)
{
	net_queue_push_message(&client_queue,action,remote_uid,data,len);
}

/* =======================================================

      Check Message Queue
      
======================================================= */

inline bool net_client_check_message_queue(int *action,int *from_remote_uid,unsigned char *data)
{
	return(net_queue_check_message(&client_queue,action,from_remote_uid,data,NULL));
}

