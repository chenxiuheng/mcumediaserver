#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <errno.h>
#include <sys/poll.h>
#include <fcntl.h>
#include "tools.h"
#include "log.h"
#include "rtmpserver.h"

/************************
* RTMPServer
* 	Constructor
*************************/
RTMPServer::RTMPServer()
{
	//Y no tamos iniciados
	inited = 0;
	serverPort = 0;

	//Create mutx
	pthread_mutex_init(&sessionMutex,0);
}


/************************
* ~ RTMPServer
* 	Destructor
*************************/
RTMPServer::~RTMPServer()
{
	//Check we have been correctly ended
	if (inited)
		//End it anyway
		End();
	//Destroy mutex
	pthread_mutex_destroy(&sessionMutex);
}

/************************
* Init
* 	Open the listening server port
*************************/
int RTMPServer::Init(int port)
{
	sockaddr_in addr;

	Log(">Init RTMP Server\n");

	//Save server port
	serverPort = port;

	//Create socket
	server = socket(AF_INET, SOCK_STREAM, 0);

	//Set SO_REUSEADDR on a socket to true (1):
	int optval = 1;
	setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	
	//Bind to first available port
	memset(&addr,0,sizeof(addr));
	addr.sin_family 	= AF_INET;
	addr.sin_addr.s_addr 	= INADDR_ANY;
	addr.sin_port 		= htons(serverPort);
	
	//Bind
     	if (bind(server, (sockaddr *) &addr, sizeof(addr)) < 0) 
		//Error 
		return Error("Can't bind server socket\n");
	
	//Listen for connections
	if (listen(server,5)<0)
		//Error 
		return Error("Can't listen on  server socket\n");


	//I am inited
	inited = 1;

	//Create threads
	createPriorityThread(&serverThread,run,this,0);

	Log("<Init RTMP Server [%d]\n",serverPort);

	//Return server port
	return serverPort;
}

/***************************
 * Run
 * 	Server running thread 
 ***************************/
int RTMPServer::Run()
{
	pollfd ufds[1];

	Log(">Run RTMP Server [%p]\n",this);

	//Set values for polling
	ufds[0].fd = server;
	ufds[0].events = POLLIN | POLLHUP | POLLERR ;

	//Set non blocking so we can get an error when we are closed by end
	int fsflags = fcntl(server,F_GETFL,0);
	fsflags |= O_NONBLOCK;
	fcntl(server,F_SETFL,fsflags);

	//Run until ended
	while(inited)
	{
		Log("-Accepting connections [%d]\n",inited);

		//Wait for events
		if (poll(ufds,1,-1)<0)
			//Check again
			continue;

		//Chek events, will fail if closed by End() so we can exit
		if (ufds[0].revents!=POLLIN)
		{
			//Error
			Log("Pool error event [%d]\n",ufds[0].revents);
			//Exit
			break;
		}


		//Accpept incoming connections
		int fd = accept(server,NULL,0);

		//If error
		if (fd<0)
		{
			Log("Error accepting new connection [%d]\n",errno);
			//Exit
			continue;
		}

		//Set non blocking again
		fsflags = fcntl(fd,F_GETFL,0);
		fsflags |= O_NONBLOCK;
		fcntl(fd,F_SETFL,fsflags);

		//Create the connection
		CreateConnection(fd);

	}

	Log("<Run RTMP Server\n");
}

/*************************
 * CreateConnection
 * 	Create new RTMP Connection for socket
 *************************/
void RTMPServer::CreateConnection(int fd)
{
	Log(">Creating connection\n");

	//Create new RTMP connection
	RTMPConnection* rtmp = new RTMPConnection(this);

	Log("-Incoming connection [%d,%p]\n",fd,rtmp);

	//Init connection
	rtmp->Init(fd);

	//Lock list
	pthread_mutex_lock(&sessionMutex);

	//Append
	lstConnections.push_back(rtmp);

	//Unlock
	pthread_mutex_unlock(&sessionMutex);

	Log("<Creating connection [0x%x]\n",rtmp);
}

/**************************
 * DeleteConnection
 * 	DeleteConnection
 **************************/
void RTMPServer::DeleteConnection(RTMPConnection *rtmp)
{

	Log(">Delete connection [0x%x]\n",rtmp);

	//Lock list
	pthread_mutex_lock(&sessionMutex);

	//End connection
	rtmp->End();
	
	//Remove from list
	lstConnections.remove(rtmp);
	
	//Unlock list
	pthread_mutex_unlock(&sessionMutex);

	Log("<Delete connection\n");
}

/*********************
 * DeleteAllConnections
 *	End all connections and clean list
 *********************/
void RTMPServer::DeleteAllConnections()
{
	LstConnections::iterator it;

	Log(">Delete all connections\n");

	//Lock list
	pthread_mutex_lock(&sessionMutex);

	//Connection iterator
	for (it=lstConnections.begin();it!=lstConnections.end();++it)
	{
		//End connection
		(*it)->End();
		//Delete connection
		delete *it;
	}

	//Clear connections
	lstConnections.clear();

	//Unlock list
	pthread_mutex_unlock(&sessionMutex);

	Log("<Delete all connections\n");

}

/***********************
* run
*       Helper thread function
************************/
void * RTMPServer::run(void *par)
{
        Log("-RTMP Server Thread [%d]\n",getpid());

        //Obtenemos el parametro
        RTMPServer *ses = (RTMPServer *)par;

        //Bloqueamos las señales
        blocksignals();

        //Ejecutamos
        pthread_exit((void *)ses->Run());
}


/************************
* End
* 	End server and close all connections
*************************/
int RTMPServer::End()
{
	Log(">End RTMP Server\n");

	//Check we have been inited
	if (!inited)
		//Do nothing
		return 0;

	//Stop thread
	inited = 0;

	//Close server socket
	shutdown(server,SHUT_RDWR);
	//Will cause poll function to exit
	close(server);

	//Wait for server thread to close
        Log("Joining server thread [%d,%d]\n",serverThread,inited);
        pthread_join(serverThread,NULL);
        Log("Joined server thread [%d]\n",serverThread);

	//Delete connections
	DeleteAllConnections();

	Log("<End RTMP Server\n");
}

/**********************************
 * AddApplication
 *   Set a handler for an application
 *********************************/
int RTMPServer::AddApplication(const wchar_t* name,RTMPApplication *app)
{
	Log("-Adding rtmp application %ls\n",name);
	//Store
	applications[std::wstring(name)] = app;

	//Exit
	return 1;
}

/**************************************
 * OnConnect
 *   Event launched from RTMPConnection to indicate a net connection stream
 *   Should return the RTMPStream associated to the url
 *************************************/
RTMPApplication* RTMPServer::OnConnect(const std::wstring &appName)
{
	//Recorremos la lista
	for (ApplicationMap::iterator it=applications.begin(); it!=applications.end(); ++it)
	{
		//Si la uri empieza por la base del handler
		if (appName.find(it->first)==0)
			//Ejecutamos el handler
			return it->second;
	}

	//Not found
	return NULL;
}
/**************************************
 * OnDisconnect
 *   Event launched from RTMPConnection to indicate that the connection stream has been disconnected
  *************************************/
void RTMPServer::onDisconnect(RTMPConnection *con)
{
	Log("-onDisconnected [%p]\n",con);
	//Delete connection
	DeleteConnection(con);
}
