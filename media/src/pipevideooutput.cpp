#include "log.h"
#include "pipevideooutput.h"
#include <string.h>
#include <stdlib.h>

PipeVideoOutput::PipeVideoOutput(pthread_mutex_t* mutex, pthread_cond_t* cond)
{
	//Nos quedamos con los mutex
	videoMixerMutex = mutex;
	videoMixerCond  = cond;

	//E iniciamos el buffer
	buffer = NULL;
	bufferSize = 0;

	//Ponemos el cambio
	isChanged   = false;
	videoWidth  = 0;
	videoHeight = 0;
}

PipeVideoOutput::~PipeVideoOutput()
{
	//Si estaba reservado
	if (buffer!=NULL)
		//Liberamos memoria
		free(buffer);
}

int PipeVideoOutput::NextFrame(BYTE *pic)
{
	//Bloqueamos
	pthread_mutex_lock(videoMixerMutex);

	//Copiamos
	memcpy(buffer,pic,bufferSize);

	//Ponemos el cambio
	isChanged = true;

	//Se�alizamos
	pthread_cond_signal(videoMixerCond);

	//Y desbloqueamos
	pthread_mutex_unlock(videoMixerMutex);

	return true;
}

int PipeVideoOutput::SetVideoSize(int width,int height)
{
	//Check it it is the same size
	if ((videoWidth!=width) || (videoHeight!=height))
	{
		//Si habia buffer lo liberamos
		if (buffer)
			free(buffer);
		//Guardamos el tama�o
		videoWidth = width;
		videoHeight= height;

		//Calculamos la memoria
		bufferSize = (width*height*3)/2;
		//Alocamos
		buffer = (BYTE*)malloc(bufferSize);
	}

	return true;
}

BYTE* PipeVideoOutput::GetFrame()
{
	//QUitamos el cambio
	isChanged = false;

	//Y devolvemos el buffer
	return buffer;
}

int PipeVideoOutput::Init()
{
	Log(">PipeVideoOutput init\n");

	//Protegemos
	pthread_mutex_lock(videoMixerMutex);

	//Iniciamos
	inited = true;

	//Protegemos
	pthread_mutex_unlock(videoMixerMutex);

	Log("<PipeVideoOutput init\n");

	return true;
} 

int PipeVideoOutput::End()
{
	//Protegemos
	pthread_mutex_lock(videoMixerMutex);

	//Terminamos
	inited = false;

	//Se�alizamos la condicion
	pthread_cond_signal(videoMixerCond);

	//Protegemos
	pthread_mutex_unlock(videoMixerMutex);

	return true;
} 
