/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Message Queues Utility Code

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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
	#include "dim3baseutility.h"
#endif

/* =======================================================

      Initialize and Shutdown Queues
      
======================================================= */

bool network_queue_initialize(network_message_queue *queue)
{
		// create queue
		
	queue->idx=0;
	queue->ptr=NULL;
	
	queue->ptr=(unsigned char*)valloc(net_max_client_msg_cache_size*net_max_client_msg_cache);
	if (queue->ptr==NULL) return(FALSE);
	
	bzero(queue->ptr,(net_max_client_msg_cache_size*net_max_client_msg_cache));
	
		// create queue lock

	pthread_mutex_init(&queue->lock,NULL);

	return(TRUE);
}

void network_queue_shutdown(network_message_queue *queue)
{
	free(queue->ptr);
	pthread_mutex_destroy(&queue->lock);
}

/* =======================================================

      Add/Replace Message In Queue
      
======================================================= */

bool network_queue_add_message_normal(network_message_queue *queue,int action,int remote_uid,unsigned char *data,int len)
{
	short			*sptr;

	pthread_mutex_lock(&queue->lock);

	if (queue->idx==net_max_client_msg_cache) {
		pthread_mutex_unlock(&queue->lock);
		return(FALSE);
	}
	
	sptr=(short*)(queue->ptr+(net_max_client_msg_cache_size*queue->idx));
	queue->idx++;
	
	*sptr++=(short)action;
	*sptr++=(short)remote_uid;
	*sptr++=(short)len;
	memmove(sptr,data,len);

	pthread_mutex_unlock(&queue->lock);

	return(TRUE);
}

bool network_queue_add_message_replace(network_message_queue *queue,int action,int remote_uid,unsigned char *data,int len)
{
	int				n,idx;
	short			*sptr;

	pthread_mutex_lock(&queue->lock);

		// check for replacable item

	idx=-1;

	for (n=0;n!=queue->idx;n++) {
		sptr=(short*)(queue->ptr+(net_max_client_msg_cache_size*n));
		if (*sptr==(short)action) {
			idx=n;
			break;
		}
	}

		// item to replace?

	if (idx!=-1) {
		sptr=(short*)(queue->ptr+(net_max_client_msg_cache_size*idx));
	}

		// or just an add?

	else {

		if (queue->idx==net_max_client_msg_cache) {
			pthread_mutex_unlock(&queue->lock);
			return(FALSE);
		}

		sptr=(short*)(queue->ptr+(net_max_client_msg_cache_size*queue->idx));
		queue->idx++;
	}
	
		// put in the data

	*sptr++=(short)action;
	*sptr++=(short)remote_uid;
	*sptr++=(short)len;
	memmove(sptr,data,len);

	pthread_mutex_unlock(&queue->lock);

	return(TRUE);
}

bool network_queue_add_message(network_message_queue *queue,int action,int queue_mode,int remote_uid,unsigned char *data,int len)
{
	if (queue_mode==net_queue_mode_replace) {
		return(network_queue_add_message_replace(queue,action,remote_uid,data,len));
	}
	
	return(network_queue_add_message_normal(queue,action,remote_uid,data,len));
}

/* =======================================================

      Get Message from Queue
      
======================================================= */

bool network_queue_check_message(network_message_queue *queue,int *action,int *from_remote_uid,unsigned char *data)
{
	int					len,sz;
	short				*sptr;
	
	pthread_mutex_lock(&queue->lock);
	
		// any messages?
	
	if (queue->idx==0) {
		pthread_mutex_unlock(&queue->lock);
		return(FALSE);
	}
	
		// get message from cache
		
	sptr=(short*)queue->ptr;
	
	*action=(int)*sptr++;
	*from_remote_uid=(int)*sptr++;
	len=(int)*sptr++;
	memmove(data,sptr,len);
	
		// delete retreived item
		
	if (queue->idx>1) {
		sz=net_max_client_msg_cache_size*(queue->idx-1);
		memmove(queue->ptr,(queue->ptr+net_max_client_msg_cache_size),sz);
	}
	
	queue->idx--;
		
	pthread_mutex_unlock(&queue->lock);
		
	return(TRUE);
}

