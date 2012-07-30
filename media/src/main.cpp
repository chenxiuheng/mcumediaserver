#include "log.h"
#include "xmlrpcserver.h"
#include "xmlhandler.h"
#include "xmlstreaminghandler.h"
#include "statushandler.h"
#include "audiomixer.h"
#include "rtmpserver.h"
#include "mcu.h"
#include "broadcaster.h"
#include "mediagateway.h"
#include "jsr309/JSR309Manager.h"
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
extern "C" {
#include "libavcodec/avcodec.h"
}
// rtsp addon by liuhong
//#include "live555MediaServer.h"

extern XmlHandlerCmd mcuCmdList[];
extern XmlHandlerCmd broadcasterCmdList[];
extern XmlHandlerCmd mediagatewayCmdList[];
extern XmlHandlerCmd jsr309CmdList[];

void log_ffmpeg(void* ptr, int level, const char* fmt, va_list vl)
{
	static int print_prefix = 1;
	char line[1024];

#ifndef MCUDEBUG
	if (level > AV_LOG_ERROR)
		return;
#endif

	//Format the
	av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &print_prefix);

	//Remove buffer errors
	if (strstr(line,"vbv buffer overflow")!=NULL)
		//exit
		return;
	//Log
	Log(line);
}

int lock_ffmpeg(void **param, enum AVLockOp op)
{
	//Get mutex pointer
	pthread_mutex_t* mutex = (pthread_mutex_t*)(*param);
	//Depending on the operation
	switch(op)
	 {
		case AV_LOCK_CREATE:
			//Create mutex
			mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
			//Init it
			pthread_mutex_init(mutex,NULL);
			//Store it
			*param = mutex;
			break;
		case AV_LOCK_OBTAIN:
			//Lock
			pthread_mutex_lock(mutex);
			break;
		case AV_LOCK_RELEASE:
			//Unlock
			pthread_mutex_unlock(mutex);
			break;
		case AV_LOCK_DESTROY:
			//Destroy mutex
			pthread_mutex_destroy(mutex);
			//Free memory
			free(mutex);
			//Clean
			*param = NULL;
			break;
	}
	return 0;
}

#ifdef FLASHSTREAMER
#include "flash.h"
extern XmlHandlerCmd flashCmdList[];
#endif

int main(int argc,char **argv)
{
	//Set default values
	bool forking = false;
	int port = 8080;
	int rtmpPort = 1935;
	const char *logfile = "mcu.log";
	const char *pidfile = "mcu.pid";

	//Get all
	for(int i=1;i<argc;i++)
	{
		//Check options
		if (strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0)
		{
			//Show usage
			printf("Usage: mcu [-h] [--help] [--mcu-log logfile] [--mcu-pid pidfile] [--http-port port] [--rtmp-port port]\r\n\r\n"
				"Options:\r\n"
				" -h,--help     Print help\r\n"
				" -f            Run as daemon in safe mode\r\n"
				" --mcu-log	Set mcu log file path (default: mcu.log)\r\n"
				" --mcu-pid	Set mcu pid file path (default: mcu.pid)\r\n"
				" --http-port   Set HTTP xmlrpc api port\r\n"
				" --rtmp-port   Set RTMP xmlrpc api port\r\n");
			//Exit
			return 0;
		} else if (strcmp(argv[i],"-f")==0)
			//Fork
			forking = true;
		else if (strcmp(argv[i],"--http-port")==0 && (i+1<argc))
			//Get port
			port = atoi(argv[++i]);
		else if (strcmp(argv[i],"--rtmp-port")==0 && (i+1<argc))
			//Get rtmp port
			rtmpPort = atoi(argv[++i]);
		else if (strcmp(argv[i],"--mcu-log")==0 && (i+1<argc))
			//Get rtmp port
			logfile = argv[++i];
		else if (strcmp(argv[i],"--mcu-pid")==0 && (i+1<argc))
			//Get rtmp port
			pidfile = argv[++i];
	}
	
	//Loop
	while(forking)
	{
		//Create the chld
		pid_t pid = fork();
		// fork error
		if (pid<0) exit(1);
		// parent exits
		if (pid>0) exit(0);

		//Log
		printf("MCU started\r\n");

		//Create the safe child
		pid = fork();

		//Check pid
		if (pid==0)
		{
			//It is the child obtain a new process group
			setsid();
			//for each descriptor
			for (int i=getdtablesize();i>=0;--i)
				//Close it
				close(i);
			//Redirect stdout and stderr
			int fd = open(logfile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			dup(fd);
			dup2(1,2);
			close(fd);
			//And continule
			break;
		} else if (pid<0)
			//Error
			return 0;

		//Pid string
		char spid[16];
		//Print it
		sprintf(spid,"%d",pid);

		//Write pid to file
		int pfd = open(pidfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		//Write it
		write(pfd,spid,strlen(spid));
		//Close it
		close(pfd);

		int status;

		do
		{
			//Wait for child
			if (waitpid(pid, &status, WUNTRACED | WCONTINUED)<0)
				return -1;
			//If it has exited or stopped
			if (WIFEXITED(status) || WIFSTOPPED(status))
				//Exit
				return 0;
			//If we have been  killed
			if (WIFSIGNALED(status) && WTERMSIG(status)==9)
				//Exit
				return 0;
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	//Dump core on fault
	rlimit l = {RLIM_INFINITY,RLIM_INFINITY};
	//Set new limit
        setrlimit(RLIMIT_CORE, &l);

	//Register mutext for ffmpeg
	av_lockmgr_register(lock_ffmpeg);

	//Set log level
	av_log_set_callback(log_ffmpeg);

	//Ignore SIGPIPE
	signal( SIGPIPE, SIG_IGN );

	//Create servers
	XmlRpcServer	server(port);
	RTMPServer	rtmpServer;
//	Live555MediaServer* rtspServer = Live555MediaServer::Instance();

	//Create services
	MCU		mcu;
	Broadcaster	broadcaster;
	MediaGateway	mediaGateway;
	JSR309Manager	jsr309Manager;

	//Create xml cmd handlers for the mcu and broadcaster
	XmlHandler xmlrpcmcu(mcuCmdList,(void*)&mcu);
	XmlHandler xmlrpcbroadcaster(broadcasterCmdList,(void*)&broadcaster);
	XmlHandler xmlrpcmediagateway(mediagatewayCmdList,(void*)&mediaGateway);
	XmlHandler xmlrpcjsr309(jsr309CmdList,(void*)&jsr309Manager);

	//Create http streaming for service events
	XmlStreamingHandler xmleventjsr309;
	XmlStreamingHandler xmleventmcu;

	//And default status hanlder
	StatusHandler status;

	//Init de mcu
	mcu.Init(&xmleventmcu);
	//Init the broadcaster
	broadcaster.Init();
	//Init the media gateway
	mediaGateway.Init();
	//INit the jsr309
	jsr309Manager.Init(&xmleventjsr309);

	//Add the rtmp application from the mcu to the rtmp server
	rtmpServer.AddApplication(L"mcu/",&mcu);
	//Add the rtmp applications from the broadcaster to the rmtp server
	rtmpServer.AddApplication(L"broadcaster/publish",&broadcaster);
	rtmpServer.AddApplication(L"broadcaster",&broadcaster);
	rtmpServer.AddApplication(L"streamer/mp4",&broadcaster);
	rtmpServer.AddApplication(L"streamer/flv",&broadcaster);
	//Add the rtmp applications from the media gateway
	rtmpServer.AddApplication(L"bridge/input",&mediaGateway);
	rtmpServer.AddApplication(L"bridge/output",&mediaGateway);
	
	//Append mcu cmd handler to the http server
	server.AddHandler(string("/mcu"),&xmlrpcmcu);
	server.AddHandler(string("/broadcaster"),&xmlrpcbroadcaster);
	server.AddHandler(string("/mediagateway"),&xmlrpcmediagateway);
	server.AddHandler(string("/jsr309"),&xmlrpcjsr309);
	server.AddHandler(string("/events/jsr309"),&xmleventjsr309);
	server.AddHandler(string("/events/mcu"),&xmleventmcu);
	
#ifdef FLASHSTREAMER
	Flash flash;
	flash.Init();
	XmlHandler xmlrpcFlash(flashCmdList,(void*)&flash);
	//Append flash cmldhandler
	server.AddHandler(string("/flash"),&xmlrpcFlash);
#endif

	//Add the html status handler
	server.AddHandler(string("/status"),&status);

	//Init the rtmp server
	rtmpServer.Init(rtmpPort);
	
	// rtsp addon by liuhong
	// Init and start rtsp server
	//rtspServer->Init();

	//Run it
	server.Start();

	//End the rtmp server
	rtmpServer.End();
	

	//End the mcu
	mcu.End();
	//End the broadcaster
	broadcaster.End();
	//End the media gateway
	mediaGateway.End();
	//End the jsr309
	jsr309Manager.End();

#ifdef FLASHSTREAMER
	//End flash player
	flash.End();
#endif
}

