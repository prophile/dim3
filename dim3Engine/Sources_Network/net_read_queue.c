/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Network Reading and Queues

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

/* =======================================================

      Initialize and Shutdown Queues
      
======================================================= */

bool net_queue_initialize(net_queue_type *queue)
{
		// create queue
		
	queue->len=0;
	queue->data=NULL;
	
	queue->data=(unsigned char*)malloc(net_queue_max_data_size);
	if (queue->data==NULL) return(FALSE);
	
	bzero(queue->data,net_queue_max_data_size);
	
		// create queue lock

	pthread_mutex_init(&queue->lock,NULL);

	return(TRUE);
}

void net_queue_shutdown(net_queue_type *queue)
{
	if (queue->data==NULL) return;
	
	free(queue->data);
	pthread_mutex_destroy(&queue->lock);
}

/* =======================================================

      Feed Data Into Queue
      
======================================================= */

inline bool net_queue_feed_socket_has_data(d3socket sock)
{
	fd_set					fd;
	struct timeval			timeout;

	timeout.tv_sec=0;
	timeout.tv_usec=0;
	
	FD_ZERO(&fd);
	FD_SET(sock,&fd);
	
#ifndef D3_OS_WINDOWS
	select((sock+1),&fd,NULL,NULL,&timeout);
#else
	select(0,&fd,NULL,NULL,&timeout);
#endif
	
	return(FD_ISSET(sock,&fd));
}

bool net_queue_feed(d3socket sock,net_queue_type *queue)
{
	int				len,read_sz;
	bool			err;

	pthread_mutex_unlock(&queue->lock);
	
		// read data until there is no more or
		// the queue is full
		
	err=FALSE;
		
	while (net_queue_feed_socket_has_data(sock)) {
	
		if (queue->len==net_queue_max_data_size) break;
	
			// read as much data as possible
			
		read_sz=net_queue_max_data_size-queue->len;
		len=recv(sock,(queue->data+queue->len),read_sz,0);
		
			// if len=0, then socket has closed on other end
			
		if (len==0) {
			err=TRUE;
			break;
		}
		
			// some error, skip and read again later
			
		if (len<0) break;
	
		queue->len+=len;
		
			// skip out if read stopped early, no more
			// data for now
			
		if (len<read_sz) break;
	}
	
	pthread_mutex_unlock(&queue->lock);
	
	return(!err);
}

/* =======================================================

      Push Message On Queue
      
======================================================= */

bool net_queue_push_message(net_queue_type *queue,int action,int remote_uid,unsigned char *msg_data,int msg_len)
{
	int					idx;
	unsigned char		tag[net_header_tag_size]=net_header_tag;
	network_header		head;

	pthread_mutex_lock(&queue->lock);
	
		// enough room on queue?
		
	if ((queue->len+(sizeof(network_header)+msg_len))>net_queue_max_data_size) {
		pthread_mutex_unlock(&queue->lock);
		return(FALSE);
	}
	
		// put message on queue
		
	idx=queue->len;
	
	memmove((queue->data+idx),tag,net_header_tag_size);
	idx+=net_header_tag_size;
	
	head.len=htons((short)msg_len);
	head.action=htons((short)action);
	head.from_remote_uid=htons((short)remote_uid);
	
	memmove((queue->data+idx),&head,sizeof(network_header));
	idx+=sizeof(network_header);
	
	if (msg_data!=NULL) {
		memmove((queue->data+idx),msg_data,msg_len);
		idx+=msg_len;
	}
	
	queue->len=idx;

	pthread_mutex_unlock(&queue->lock);

	return(TRUE);
}

/* =======================================================

      Get Next Message From Queue
      
======================================================= */

bool net_queue_check_message(net_queue_type *queue,int *action,int *from_remote_uid,unsigned char *msg_data,int *msg_data_len)
{
	int					idx,tag_idx,msg_len;
	bool				found;
	unsigned char		tag[net_header_tag_size]=net_header_tag;
	network_header		head;
	
	pthread_mutex_lock(&queue->lock);
	
		// search for next message start tag
		// this allows us to work around broken or garbled messages
		
	idx=0;
	tag_idx=0;
	
	found=FALSE;
	
	while (idx<queue->len) {
	
		if (*(queue->data+idx)==tag[tag_idx]) {
			tag_idx++;
			if (tag_idx==net_header_tag_size) {
				idx++;
				found=TRUE;
				break;
			}
		}
		else {
			tag_idx=0;
		}
		
		idx++;
	}
	
		// if we haven't found the tag, delete everything past
		// the furthest we got into tag, and return no messages
		
	if (!found) {
		
		if ((tag_idx!=0) && (idx>tag_idx)) memmove(queue->data,(queue->data+(idx-tag_idx)),tag_idx);
		queue->len=tag_idx;
		
		pthread_mutex_unlock(&queue->lock);
		
		return(FALSE);
	}
	
		// do we have enough to get header?
		
	if ((idx+sizeof(network_header))>queue->len) {
		pthread_mutex_unlock(&queue->lock);
		return(FALSE);
	}
	
		// get message header
		
	memmove(&head,(queue->data+idx),sizeof(network_header));
	idx+=sizeof(network_header);
	
	msg_len=(int)ntohs(head.len);
	*action=(int)ntohs(head.action);
	*from_remote_uid=(int)ntohs(head.from_remote_uid);
	
		// enough for data?
		
	if ((idx+msg_len)>queue->len) {
		pthread_mutex_unlock(&queue->lock);
		return(FALSE);
	}
	
	memmove(msg_data,(queue->data+idx),msg_len);
	idx+=msg_len;
	
	if (msg_data_len!=NULL) *msg_data_len=msg_len;
	
		// remove retreived bytes
		
	if (queue->len>idx) memmove(queue->data,(queue->data+idx),(queue->len-idx));
	queue->len-=idx;
		
	pthread_mutex_unlock(&queue->lock);
		
	return(TRUE);
}

/* =======================================================

      Single Message Queue
      
======================================================= */

bool net_queue_block_single_message(d3socket sock,int req_action,int req_remote_uid,unsigned char *req_msg_data,int req_msg_len,int rep_desired_action,unsigned char *rep_msg_data,int rep_msg_len)
{
	int						timeout_tick,action,remote_uid;
	unsigned char			data[net_max_msg_size];
	bool					read_ok;
	net_queue_type			queue;
	
	if (!net_queue_initialize(&queue)) return(FALSE);
	
		// send out the request
		
	if (req_action!=-1) {
		if (!net_send_message(sock,req_action,req_remote_uid,req_msg_data,req_msg_len)) return(FALSE);
	}
	
		// wait for response
		
	timeout_tick=time_get()+(client_timeout_wait_seconds*1000);
	
	read_ok=FALSE;
	action=-1;
	
	while (timeout_tick>time_get()) {
	
		if (!net_queue_feed(sock,&queue)) break;
	
		if (net_queue_check_message(&queue,&action,&remote_uid,data,NULL)) {
			if (action==rep_desired_action) {
				read_ok=TRUE;
				memmove(rep_msg_data,data,rep_msg_len);
				break;
			}
		}
		
		usleep(1000);
	}
	
	net_queue_shutdown(&queue);
	
	return(read_ok);
}


