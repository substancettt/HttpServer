//============================================================================
// Name        : HttpServer.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "mongoose.h"
#include "weixin.h"

static int exit_flag;
static const char server_name[] = "RESTful Server for Weixin.";
static struct mg_server *server;
static const char listening_port[] = "80";
static const char token[] = "weixin";

int * EventHandler(mg_connection *conn, int ev);
#define EV_HANDLER EventHandler

WeixinInterface wx_interface;

static void signal_handler(int sig_num)
{
	// Reinstantiate signal handler
	signal(sig_num, signal_handler);

	if (sig_num == SIGCHLD)
	{
		do
		{
		} while (waitpid(-1, &sig_num, WNOHANG) > 0);
	}
	else
	{
		exit_flag = sig_num;
	}
}

static void *serving_thread_func(void *param)
{
	struct mg_server *srv = (struct mg_server *) param;
	while (exit_flag == 0)
	{
		mg_poll_server(srv, 1000);
	}
	return NULL;
}

static void start_mongoose(int argc, char *argv[])
{
	if ((server = mg_create_server(NULL, (mg_handler_t) EV_HANDLER)) == NULL)
	{
		printf("%s", "Failed to start Mongoose.");
	}
	mg_set_option(server, "listening_port", listening_port);
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
}

static void handleHttpMsg(mg_connection *conn)
{
	const char * response = NULL;

	WX_LOG(("HTTP Message revieved."));

	if (wx_interface.isTokenValidationUrl(conn->query_string))
	{
		response = wx_interface.wx_validate(conn->query_string);
	}
	else
	{
		response = wx_interface.wx_parseMsg(conn->content, conn->content_len);
	}

	if (NULL != response)
	{
		mg_RESTful_msg(conn, response, 200);
	}
}

int * EventHandler(mg_connection *conn, int ev)
{
	switch (ev)
	{
	case MG_POLL:
		WX_LOG(("POLL Event"));
		break;
	case MG_CONNECT:
		WX_LOG(("CONNECT Event"));
		break;
	case MG_AUTH:
		WX_LOG(("AUTH Event"));
		break;
	case MG_REQUEST:
		WX_LOG(("REQUEST Event"));
		break;
	case MG_REPLY:
		WX_LOG(("REPLY Event"));
		break;
	case MG_RECV:
		WX_LOG(("RECV Event"));
		break;
	case MG_CLOSE:
		WX_LOG(("CLOSE Event"));
		break;
	case MG_WS_HANDSHAKE:
		WX_LOG(("WS_HANDSHAKE Event"));
		break;
	case MG_WS_CONNECT:
		WX_LOG(("WS_CONNECT Event"));
		break;
	case MG_HTTP_ERROR:
		WX_LOG(("HTTP_ERROR Event"));
		break;
	case MG_HTTP_MSG:
		handleHttpMsg(conn);
		break;
	default:
		WX_LOG(("Invalid Event"));
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	WeixinInterface::Options opt;
	opt.token.assign(token);
	wx_interface.wx_init(opt);

	start_mongoose(argc, argv);
	printf("%s serving on port %s\n", server_name,
			mg_get_option(server, "listening_port"));
	fflush(stdout);  // Needed, Windows terminals might not be line-buffered
	serving_thread_func(server);
	printf("Exiting on signal %d ...", exit_flag);
	fflush(stdout);
	mg_destroy_server(&server);
	printf("%s\n", " done.");

	return EXIT_SUCCESS;
}
