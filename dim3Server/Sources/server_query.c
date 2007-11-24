/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Server Query Code

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
	#include "dim3server.h"
#endif

extern network_setup_type	net_setup;

d3socket					server_query_socket;
bool						server_query_complete;
char						server_query_err_str[256];
pthread_t					server_query_thread;

char						html_header_str[]="<html><head><title>dim3 Server</title></head><body>",
							html_footer_str[]="</body></html>";

extern void ui_quit(void);

/* =======================================================

      Initialize and Shutdown Query Networking
      
======================================================= */

bool server_query_initialize(char *err_str)
{
		// begin listener thread
		
	server_query_complete=FALSE;
	server_query_socket=D3_NULL_SOCKET;
	
	if (pthread_create(&server_query_thread,NULL,server_query_listen_thread,NULL)!=0) {
		strcpy(err_str,"Networking: Could not start host listener thread");
		return(FALSE);
	}
	
		// wait for listenter thread to get into accept
		
	while (TRUE) {
		if (server_query_complete) break;
		usleep(100000);
	}
	
	if (server_query_err_str[0]!=0x0) {
		strcpy(err_str,server_query_err_str);
		server_player_shutdown();
		return(FALSE);
	}

		// now listening on separate thread
		
	return(TRUE);
}

void server_query_shutdown(void)
{
		// did query never start?

	if (server_query_socket==D3_NULL_SOCKET) return;
	
		// shutdown socket and then wait for termination
	
	network_close_socket(&server_query_socket);
	pthread_join(server_query_thread,NULL);
}

/* =======================================================

      Query Networking Message Helper
      
======================================================= */

void server_query_client_send_error(d3socket sock,int err_num,char *err_str)
{
	char		str[1024];

	sprintf(str,"HTTP/1.1 %d %s\r\n",err_num,err_str);
	
	strcat(str,"Server: dim3Server\r\n");
	strcat(str,"Content-Type: text/html\r\n");
	strcat(str,"Accept-Ranges: bytes\r\n");
	strcat(str,"Connection: close\r\n");
	strcat(str,"\r\n");
	
	network_send_data(sock,(unsigned char*)str,strlen(str));
}

void server_query_client_send_reply(d3socket sock,char *html)
{
	int			str_sz,html_sz;
	char		str[1024],str2[256];
	char		*reply;

		// get some sizes

	html_sz=strlen(html_header_str)+strlen(html)+strlen(html_footer_str);

		// create header

	strcpy(str,"HTTP/1.1 200 OK\r\n");
	
	strcat(str,"Server: dim3Server\r\n");
	strcat(str,"Content-Type: text/html\r\n");
	
	sprintf(str2,"Content-Length: %d\r\n",html_sz);
	strcat(str,str2);

	strcat(str,"Accept-Ranges: bytes\r\n");
	strcat(str,"Connection: close\r\n");
	strcat(str,"Pragma: no-cache\r\n");
	strcat(str,"Cache-Control: no-cache\r\n");
	strcat(str,"Expires: -1\r\n");
	strcat(str,"\r\n");

		// data for http reply

	str_sz=strlen(str);

	reply=malloc(str_sz+html_sz+1);
	if (reply==NULL) {
		server_query_client_send_error(sock,500,"Internal Server Error");
		return;
	}

	strcpy(reply,str);
	strcat(reply,html_header_str);
	strcat(reply,html);
	strcat(reply,html_footer_str);
	
		// send header

	network_send_data(sock,(unsigned char*)reply,strlen(reply));

	free(reply);
}

bool server_query_client_find_parameter(char *cmd,char *name,char *value)
{
	int			n,len;
	char		*c,chk_name[32];
	
		// search for name
		// this is simplified as my html responses are predictable
		// all others are illegal and ignored

	sprintf(chk_name,"%s=",name);
	c=strstr(cmd,chk_name);
	if (c==NULL) return(FALSE);

		// get name

	strncpy(value,(c+strlen(chk_name)),32);
	value[31]=0x0;

		// break at next parameter

	c=strchr(value,'&');
	if (c!=NULL) *c=0x0;

	c=strchr(value,'\r');
	if (c!=NULL) *c=0x0;

	c=strchr(value,'\n');
	if (c!=NULL) *c=0x0;

		// fix name
		// URL encoding has more than this, but it's all we
		// need to do for this application

	c=value;
	len=strlen(value);

	for (n=0;n!=len;n++) {
		if (*c=='+') *c=' ';
		c++;
	}

	return(TRUE);
}

/* =======================================================

      Query Networking Message Thread
      
======================================================= */

bool server_query_reply(d3socket sock)
{
	int					sigpipeflag,idx;
	char				*c,*html;
	char				request[1024],cmd[256],password[32],action[32],index[32];

	int	rtn;

		// lets not raise sigpipes
		
	sigpipeflag=1;
#ifndef D3_OS_WINDOWS
	setsockopt(sock,SOL_SOCKET,SO_NOSIGPIPE,(void*)&sigpipeflag,sizeof(int));
#endif

		// grab the request

	memset(request,0x0,1024);
	
	rtn=network_receive_data(sock,(unsigned char*)request,1024);
	if (rtn==-1) {
		server_query_client_send_error(sock,401,"Access Denied");
		return(FALSE);
	}

	request[rtn]=0x0;
	
		// ignore everything but first line
		
	c=strchr(request,'\r');
	if (c!=NULL) *c=0x0;
	
	c=strchr(request,'\n');
	if (c!=NULL) *c=0x0;

		// reject everything but gets with
		// no paths and parameters

	if (memcmp(request,"GET /?",6)!=0) {
		server_query_client_send_error(sock,404,"Not Found");
		return(FALSE);
	}
	
		// reject gets without parameters
		
	c=strchr(request,'?');
	if (c==NULL) {
		server_query_client_send_error(sock,404,"Not Found");
		return(FALSE);
	}
	
		// get the commands

	strcpy(cmd,(c+1));
	c=strchr(cmd,' ');
	if (c!=NULL) *c=0x0;

		// check the password

	if (!server_query_client_find_parameter(cmd,"password",password)) {
		server_query_client_send_error(sock,401,"Access Denied");
		return(FALSE);
	}

	if (strcmp(password,net_setup.host.password)!=0) {
		server_query_client_send_error(sock,401,"Access Denied");
		return(FALSE);
	}

		// deal with commands

	if (server_query_client_find_parameter(cmd,"action",action)) {
	
			// game start command
			
		if (strcasecmp(action,"game")==0) {
			if (server_query_client_find_parameter(cmd,"index",index)) {
				idx=atoi(index);
				if ((idx>=0) && (idx<=net_setup.ngame)) {
					net_setup.host.game_idx=idx;
					server_player_send_all_packet(net_action_request_host_reset,net_queue_mode_normal,NULL,0,FALSE);		// force all players to reset
				}
			}
		}

			// quit command

		if (strcasecmp(action,"quit")==0) {
			server_query_client_send_reply(sock,"Server Shutdown");
			return(TRUE);
		}

	}

		// send the reply

	html=server_query_html_info();
	if (html==NULL) {
		server_query_client_send_error(sock,500,"Internal Server Error");
		return(FALSE);
	}

	server_query_client_send_reply(sock,html);
		
	free(html);
	
	return(FALSE);
}

/* =======================================================

      Query Networking Listener Thread
      
======================================================= */

void* server_query_listen_thread(void *arg)
{
	d3socket			sock;
	int					err;
	bool				quit_trigger;
	socklen_t			addr_len;
	struct sockaddr		addr;
	
		// use host err_str to flag if errors occured
		
	server_query_err_str[0]=0x0;
	
		// create host socket
		
	server_query_socket=network_open_socket();
	if (server_query_socket==D3_NULL_SOCKET) {
		strcpy(server_query_err_str,"Networking: Unable to open socket");
		server_query_complete=TRUE;
		pthread_exit(NULL);
		return(NULL);
	}
	
	network_socket_blocking(server_query_socket,TRUE);

		// bind
		
	if (!network_bind(server_query_socket,net_setup.host.ip_resolve,net_port_host_query,server_query_err_str)) {
		network_close_socket(&server_query_socket);
		server_query_complete=TRUE;
		pthread_exit(NULL);
		return(NULL);
	}

		// start listening
		
	err=listen(server_query_socket,256);
	if (err!=0) {
		network_close_socket(&server_query_socket);
		sprintf(server_query_err_str,"Networking: Could not start listener on %s (error: %d)",net_setup.host.ip_resolve,errno);
		server_query_complete=TRUE;
		pthread_exit(NULL);
		return(NULL);
	}

		// host is OK, free thread to run independantly
		
	server_query_complete=TRUE;
	
		// begin accepting
		
	while (TRUE) {
		addr_len=sizeof(struct sockaddr);
		sock=accept(server_query_socket,&addr,&addr_len);
		
		if (sock==-1) break;				// accept ending means socket has been closed and host shutting down
	
		network_socket_blocking(sock,FALSE);
		quit_trigger=server_query_reply(sock);
		network_close_socket(&sock);
	
		if (quit_trigger) break;
	}
	
	ui_quit();
	
	pthread_exit(NULL);
	return(NULL);
}

