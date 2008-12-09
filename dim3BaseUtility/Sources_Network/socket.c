/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Network Socket Code

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

char		network_msg_tag[net_msg_synch_tag_len]={net_msg_synch_tag};

/* =======================================================

      Socket Debug Code
      
======================================================= */

void network_debug_printoutdata(unsigned char *data,int len)
{
	int				i;
	unsigned char	*uc;
	
	uc=data;
	
	for (i=0;i!=len;i++) {
		fprintf(stdout,"%.2X ",(int)*uc++);
		if ((i!=0) && ((i%20)==0)) fprintf(stdout,"\n");
	}
	
	fprintf(stdout,"\n");
}

/* =======================================================

      Socket Utilities
      
======================================================= */

d3socket network_open_socket(void)
{

	d3socket		sock;
#ifndef D3_OS_WINDOWS
	int				val;
#else
	BOOL			val;
#endif
	
		// get socket
		
	sock=socket(AF_INET,SOCK_STREAM,0);
	if (sock==D3_NULL_SOCKET) return(D3_NULL_SOCKET);
	
		// turn off nagel algorithm
	
#ifndef D3_OS_WINDOWS
	val=1;
	setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,&val,sizeof(int));
#else
	val=TRUE;
	setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(const char*)&val,sizeof(BOOL));
#endif

	return(sock);
}

void network_close_socket(d3socket *sock)
{
		// shutdown any more sending or receiving

#ifndef D3_OS_WINDOWS
	shutdown(*sock,SHUT_RDWR);
#else
	shutdown(*sock,SD_BOTH);
#endif

		// close socket

	closesocket(*sock);
	
	*sock=D3_NULL_SOCKET;
}

/* =======================================================

      Network Connect Utilities
      
======================================================= */

void network_socket_blocking(d3socket sock,bool blocking)
{
#ifndef D3_OS_WINDOWS

	long		opt;
	
	opt=fcntl(sock,F_GETFL,NULL);
	
	if (blocking) {
		opt&=(~O_NONBLOCK);
	}
	else {
		opt|=O_NONBLOCK;
	}
	
	fcntl(sock,F_SETFL,opt);

#else

	u_long		opt;

	opt=blocking?1:0;
	ioctlsocket(sock,FIONBIO,&opt);

#endif
}

bool network_connect_start(d3socket sock,char *ip,int port,char *err_str)
{
	int					err;
	unsigned long		ns_addr;
	bool				in_progress;
	struct sockaddr_in	addr;
	
		// setup host
		
	ns_addr=inet_addr(ip);
	if (ns_addr==INADDR_NONE) {
		sprintf(err_str,"Networking: Could not create address for %s",ip);
		return(FALSE);
	}

	memset(&addr,0x0,sizeof(struct sockaddr_in));
		
	addr.sin_family=AF_INET;
	addr.sin_port=htons((short)port);
	addr.sin_addr.s_addr=ns_addr;
	
		// put socket into non-blocking
		
	network_socket_blocking(sock,FALSE);
	
		// try to connect
	
	err=connect(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));
	if (err<0) {

#ifndef D3_OS_WINDOWS
		in_progress=(errno==EINPROGRESS);
#else
		in_progress=(WSAGetLastError()==WSAEINPROGRESS);
#endif

		if (!in_progress) {
			network_socket_blocking(sock,TRUE);
			sprintf(err_str,"Networking: Could not connect to %s:%d",ip,port);
			return(FALSE);
		}
	}
	
	return(TRUE);
}

bool network_connect_check(d3socket sock)
{
	socklen_t			len;
	struct sockaddr		name;
	
	len=sizeof(name);
	return(getpeername(sock,&name,&len)==0);
}

void network_connect_end(d3socket sock)
{
	network_socket_blocking(sock,TRUE);
}

bool network_connect_block(d3socket sock,char *ip,int port,int secs,char *err_str)
{
	int					count;
	bool				connect_ok;
	
	if (!network_connect_start(sock,ip,port,err_str)) return(FALSE);
	
	count=secs*1000;
	connect_ok=FALSE;
	
	while (count>0) {
	
		if (network_connect_check(sock)) {
			connect_ok=TRUE;
			break;
		}
	
		usleep(1000);
		count--;
	}

	network_connect_end(sock);
	
	if (!connect_ok) {
		sprintf(err_str,"Networking: No connection to %s:%d",ip,port);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Network Binds
      
======================================================= */

bool network_bind(d3socket sock,char *ip,int port,char *err_str)
{
	int					err;
	unsigned long		ns_addr;
	struct sockaddr_in	addr;
	
		// setup address
		
	ns_addr=inet_addr(ip);
	if (ns_addr==INADDR_NONE) {
		sprintf(err_str,"Networking: Could not create address for %s",ip);
		return(FALSE);
	}
		
	memset(&addr,0x0,sizeof(struct sockaddr_in));
		
	addr.sin_family=AF_INET;
	addr.sin_port=htons((short)port);
	addr.sin_addr.s_addr=ns_addr;
	
		// bind socket
		
	err=bind(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));
	if (err<0) {
		sprintf(err_str,"Networking: Could not bind to %s:%d",ip,port);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Network Status Utilities
      
======================================================= */
	
bool network_receive_ready(d3socket sock)
{
	fd_set					fd;
	struct timeval			timeout;

	if (sock==D3_NULL_SOCKET) return(FALSE);
	
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

bool network_send_ready(d3socket sock)
{
	fd_set					fd;
	struct timeval			timeout;

	if (sock==D3_NULL_SOCKET) return(FALSE);
	
	timeout.tv_sec=0;
	timeout.tv_usec=0;
	
	FD_ZERO(&fd);
	FD_SET(sock,&fd);
	
#ifndef D3_OS_WINDOWS
	select((sock+1),NULL,&fd,NULL,&timeout);
#else
	select(0,NULL,&fd,NULL,&timeout);
#endif

	return(FD_ISSET(sock,&fd));
}

/* =======================================================

      Network Receive Utilities
      
======================================================= */

int network_receive_data(d3socket sock,unsigned char *data,int len)
{
	int			recv_len,total_len,retry_count;

	retry_count=0;
	total_len=0;
	
	while (TRUE) {

			// if no data, retry a number of times

		if (!network_receive_ready(sock)) {
			retry_count++;
			if (retry_count>socket_no_data_retry) return(total_len);
			usleep(socket_no_data_u_wait);
			continue;
		}
	
			// grab the data
			
		recv_len=recv(sock,(data+total_len),(len-total_len),0);
		if (recv_len==-1) return(-1);
		
			// add up to next data
			
		total_len+=recv_len;
		if (total_len>=len) return(total_len);
	}
	
	return(total_len);
}

bool network_receive_get_next_tag(d3socket sock)
{
	int			tag_idx,recv_len,total_len,retry_count;
	char		byte;
	
		// this code exists because it's possible a
		// message could get broken up, delayed, smashed, etc

		// we tag all messages with these bytes, and always wait
		// until we see the bytes before considering we have
		// another message

	tag_idx=0;

	retry_count=0;
	total_len=0;

	while (TRUE) {

			// if no data, retry a number of times

		if (!network_receive_ready(sock)) {
			retry_count++;
			if (retry_count>socket_no_data_retry) return(total_len);
			usleep(socket_no_data_u_wait);
			continue;
		}

			// look for tag

		recv_len=recv(sock,&byte,1,0);
		if (recv_len==-1) return(-1);

		if (network_msg_tag[tag_idx]==byte) {
			tag_idx++;
			if (tag_idx==net_msg_synch_tag_len) return(TRUE);
		}

		tag_idx=0;
	}
}

bool network_receive_packet(d3socket sock,int *action,int *queue_mode,int *from_remote_uid,unsigned char *data,int *len)
{
	int					recv_len,data_len;
	network_header		head;
	
	if (sock==D3_NULL_SOCKET) return(FALSE);

	if (!network_receive_get_next_tag(sock)) return(FALSE);

	recv_len=network_receive_data(sock,(unsigned char*)&head,sizeof(network_header));
	if (recv_len<sizeof(network_header)) return(FALSE);		// didn't get all of network header or -1 error
	
	*len=data_len=(signed short)ntohs(head.len);
	*action=(signed short)ntohs(head.action);
	if (queue_mode!=NULL) *queue_mode=(signed short)ntohs(head.queue_mode);
	*from_remote_uid=(signed short)ntohs(head.from_remote_uid);
	
	if (data_len==0) return(TRUE);
	if (data_len<0) return(FALSE);
	if (data_len>net_max_msg_size) return(FALSE);	
	
		// get the data
		
	recv_len=network_receive_data(sock,data,data_len);
	return(recv_len==data_len);								// message OK only if entire data read
}

/* =======================================================

      Network Send Utilities
      
======================================================= */

int network_send_data(d3socket sock,unsigned char *data,int len)
{
	int				sent_len,total_len,retry_count;

	retry_count=0;
	total_len=0;

	while (TRUE) {

			// if not able to send, retry a number of times

		if (!network_send_ready(sock)) {
			retry_count++;
			if (retry_count>socket_no_data_retry) return(total_len);
			usleep(socket_no_data_u_wait);
			continue;
		}
	
			// send the data
			
		sent_len=send(sock,(data+total_len),(len-total_len),0);
		if (sent_len==-1) return(-1);
		
			// add up to next data
		
		total_len+=sent_len;
		if (total_len>=len) return(total_len);
	}
	
	return(total_len);
}

bool network_send_packet(d3socket sock,int action,int queue_mode,int from_remote_uid,unsigned char *data,int len)
{
	int					sent_len;
	network_header		head;
	
	if (sock==D3_NULL_SOCKET) return(FALSE);

		// message tag

	sent_len=network_send_data(sock,network_msg_tag,net_msg_synch_tag_len);
	if (sent_len<net_msg_synch_tag_len) return(FALSE);			// couldn't get out entire tag

		// header

	head.len=htons((short)len);
	head.action=htons((short)action);
	head.queue_mode=htons((short)queue_mode);
	head.from_remote_uid=htons((short)from_remote_uid);
	
	sent_len=network_send_data(sock,(unsigned char*)&head,sizeof(network_header));
	if (sent_len<sizeof(network_header)) return(FALSE);			// couldn't get out entire header

	if (data==NULL) return(TRUE);
	
		// data

	sent_len=network_send_data(sock,data,len);
	return(len==sent_len);										// only OK if entire data is sent
}
